/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 * Copyright (c) 2008, 2010, Oracle and/or its affiliates. All rights reserved.
 * Copyright 2019 Nexenta by DDN, Inc. All rights reserved.
 */

/*
 * Structures and type definitions for the SMB module.
 */

#ifndef _SMBSRV_SMB_KTYPES_H
#define	_SMBSRV_SMB_KTYPES_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <sys/note.h>
#include <sys/systm.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/synch.h>
#include <sys/taskq.h>
#include <sys/socket.h>
#include <sys/acl.h>
#include <sys/sdt.h>
#include <sys/stat.h>
#include <sys/vnode.h>
#include <sys/cred.h>
#include <netinet/in.h>
#include <sys/ksocket.h>
#include <sys/fem.h>
#include <smbsrv/smb.h>
#include <smbsrv/smb2.h>
#include <smbsrv/smbinfo.h>
#include <smbsrv/mbuf.h>
#include <smbsrv/smb_sid.h>
#include <smbsrv/smb_xdr.h>
#include <smbsrv/netbios.h>
#include <smbsrv/smb_vops.h>
#include <smbsrv/smb_kstat.h>

struct __door_handle;	/* <sys/door.h> */
struct edirent;		/* <sys/extdirent.h> */
struct nvlist;

struct smb_disp_entry;
struct smb_request;
struct smb_server;
struct smb_event;
struct smb_export;

/*
 * Accumulated time and queue length statistics.
 *
 * Accumulated time statistics are kept as a running sum of "active" time.
 * Queue length statistics are kept as a running sum of the product of queue
 * length and elapsed time at that length -- i.e., a Riemann sum for queue
 * length integrated against time.  (You can also think of the active time as a
 * Riemann sum, for the boolean function (queue_length > 0) integrated against
 * time, or you can think of it as the Lebesgue measure of the set on which
 * queue_length > 0.)
 *
 *		^
 *		|			_________
 *		8			| i4	|
 *		|			|	|
 *	Queue	6			|	|
 *	Length	|	_________	|	|
 *		4	| i2	|_______|	|
 *		|	|	    i3		|
 *		2_______|			|
 *		|    i1				|
 *		|_______________________________|
 *		Time->	t1	t2	t3	t4
 *
 * At each change of state (entry or exit from the queue), we add the elapsed
 * time (since the previous state change) to the active time if the queue length
 * was non-zero during that interval; and we add the product of the elapsed time
 * times the queue length to the running length*time sum.
 *
 * This method is generalizable to measuring residency in any defined system:
 * instead of queue lengths, think of "outstanding RPC calls to server X".
 *
 * A large number of I/O subsystems have at least two basic "lists" of
 * transactions they manage: one for transactions that have been accepted for
 * processing but for which processing has yet to begin, and one for
 * transactions which are actively being processed (but not done). For this
 * reason, two cumulative time statistics are defined here: wait (pre-service)
 * time, and run (service) time.
 *
 * All times are 64-bit nanoseconds (hrtime_t), as returned by gethrtime().
 *
 * The units of cumulative busy time are accumulated nanoseconds. The units of
 * cumulative length*time products are elapsed time times queue length.
 *
 * Updates to the fields below are performed implicitly by calls to
 * these functions:
 *
 *	smb_srqueue_init()
 *	smb_srqueue_destroy()
 *	smb_srqueue_waitq_enter()
 *	smb_srqueue_runq_exit()
 *	smb_srqueue_waitq_to_runq()
 *	smb_srqueue_update()
 *
 * These fields should never be updated by any other means.
 */
typedef struct smb_srqueue {
	kmutex_t	srq_mutex;
	hrtime_t	srq_wlastupdate;
	hrtime_t	srq_wtime;
	hrtime_t	srq_wlentime;
	hrtime_t	srq_rlastupdate;
	hrtime_t	srq_rtime;
	hrtime_t	srq_rlentime;
	uint32_t	srq_wcnt;
	uint32_t	srq_rcnt;
} smb_srqueue_t;

/*
 * The fields with the prefix 'ly_a' contain the statistics collected since the
 * server was last started ('a' for 'aggregated'). The fields with the prefix
 * 'ly_d' contain the statistics collected since the last snapshot ('d' for
 * 'delta').
 */
typedef struct smb_latency {
	kmutex_t	ly_mutex;
	uint64_t	ly_a_nreq;
	hrtime_t	ly_a_sum;
	hrtime_t	ly_a_mean;
	hrtime_t	ly_a_stddev;
	uint64_t	ly_d_nreq;
	hrtime_t	ly_d_sum;
	hrtime_t	ly_d_mean;
	hrtime_t	ly_d_stddev;
} smb_latency_t;

typedef struct smb_disp_stats {
	volatile uint64_t sdt_txb;
	volatile uint64_t sdt_rxb;
	smb_latency_t	sdt_lat;
} smb_disp_stats_t;

int smb_noop(void *, size_t, int);

#define	SMB_AUDIT_STACK_DEPTH	16
#define	SMB_AUDIT_BUF_MAX_REC	16
#define	SMB_AUDIT_NODE		0x00000001

/*
 * Maximum number of records returned in SMBsearch, SMBfind
 * and SMBfindunique response. Value set to 10 for compatibility
 * with Windows.
 */
#define	SMB_MAX_SEARCH		10

#define	SMB_SEARCH_ATTRIBUTES    \
	(FILE_ATTRIBUTE_HIDDEN | \
	FILE_ATTRIBUTE_SYSTEM |  \
	FILE_ATTRIBUTE_DIRECTORY)

#define	SMB_SEARCH_HIDDEN(sattr) ((sattr) & FILE_ATTRIBUTE_HIDDEN)
#define	SMB_SEARCH_SYSTEM(sattr) ((sattr) & FILE_ATTRIBUTE_SYSTEM)
#define	SMB_SEARCH_DIRECTORY(sattr) ((sattr) & FILE_ATTRIBUTE_DIRECTORY)
#define	SMB_SEARCH_ALL(sattr) ((sattr) & SMB_SEARCH_ATTRIBUTES)

typedef struct {
	uint32_t		anr_refcnt;
	int			anr_depth;
	pc_t			anr_stack[SMB_AUDIT_STACK_DEPTH];
} smb_audit_record_node_t;

typedef struct {
	int			anb_index;
	int			anb_max_index;
	smb_audit_record_node_t	anb_records[SMB_AUDIT_BUF_MAX_REC];
} smb_audit_buf_node_t;

/*
 * Thread State Machine
 * --------------------
 *
 *			    T5			   T0
 * smb_thread_destroy()	<-------+		+------- smb_thread_init()
 *                              |		|
 *				|		v
 *			+-----------------------------+
 *			|   SMB_THREAD_STATE_EXITED   |<---+
 *			+-----------------------------+	   |
 *				      | T1		   |
 *				      v			   |
 *			+-----------------------------+	   |
 *			|  SMB_THREAD_STATE_STARTING  |	   |
 *			+-----------------------------+	   |
 *				     | T2		   | T4
 *				     v			   |
 *			+-----------------------------+	   |
 *			|  SMB_THREAD_STATE_RUNNING   |	   |
 *			+-----------------------------+	   |
 *				     | T3		   |
 *				     v			   |
 *			+-----------------------------+	   |
 *			|  SMB_THREAD_STATE_EXITING   |----+
 *			+-----------------------------+
 *
 * Transition T0
 *
 *    This transition is executed in smb_thread_init().
 *
 * Transition T1
 *
 *    This transition is executed in smb_thread_start().
 *
 * Transition T2
 *
 *    This transition is executed by the thread itself when it starts running.
 *
 * Transition T3
 *
 *    This transition is executed by the thread itself in
 *    smb_thread_entry_point() just before calling thread_exit().
 *
 *
 * Transition T4
 *
 *    This transition is executed in smb_thread_stop().
 *
 * Transition T5
 *
 *    This transition is executed in smb_thread_destroy().
 */
typedef enum smb_thread_state {
	SMB_THREAD_STATE_STARTING = 0,
	SMB_THREAD_STATE_RUNNING,
	SMB_THREAD_STATE_EXITING,
	SMB_THREAD_STATE_EXITED,
	SMB_THREAD_STATE_FAILED
} smb_thread_state_t;

struct _smb_thread;

typedef void (*smb_thread_ep_t)(struct _smb_thread *, void *ep_arg);

#define	SMB_THREAD_MAGIC	0x534D4254	/* SMBT */

typedef struct _smb_thread {
	uint32_t		sth_magic;
	char			sth_name[32];
	smb_thread_state_t	sth_state;
	kthread_t		*sth_th;
	kt_did_t		sth_did;
	smb_thread_ep_t		sth_ep;
	void			*sth_ep_arg;
	pri_t			sth_pri;
	boolean_t		sth_kill;
	kmutex_t		sth_mtx;
	kcondvar_t		sth_cv;
} smb_thread_t;

/*
 * Pool of IDs
 * -----------
 *
 *    A pool of IDs is a pool of 16 bit numbers. It is implemented as a bitmap.
 *    A bit set to '1' indicates that that particular value has been allocated.
 *    The allocation process is done shifting a bit through the whole bitmap.
 *    The current position of that index bit is kept in the smb_idpool_t
 *    structure and represented by a byte index (0 to buffer size minus 1) and
 *    a bit index (0 to 7).
 *
 *    The pools start with a size of 8 bytes or 64 IDs. Each time the pool runs
 *    out of IDs its current size is doubled until it reaches its maximum size
 *    (8192 bytes or 65536 IDs). The IDs 0 and 65535 are never given out which
 *    means that a pool can have a maximum number of 65534 IDs available.
 */
#define	SMB_IDPOOL_MAGIC	0x4944504C	/* IDPL */
#define	SMB_IDPOOL_MIN_SIZE	64	/* Number of IDs to begin with */
#define	SMB_IDPOOL_MAX_SIZE	64 * 1024

typedef struct smb_idpool {
	uint32_t	id_magic;
	kmutex_t	id_mutex;
	uint8_t		*id_pool;
	uint32_t	id_size;
	uint32_t	id_maxsize;
	uint8_t		id_bit;
	uint8_t		id_bit_idx;
	uint32_t	id_idx;
	uint32_t	id_idx_msk;
	uint32_t	id_free_counter;
	uint32_t	id_max_free_counter;
} smb_idpool_t;

/*
 * Maximum size of a Transport Data Unit when CAP_LARGE_READX and
 * CAP_LARGE_WRITEX are not set.  CAP_LARGE_READX/CAP_LARGE_WRITEX
 * allow the payload to exceed the negotiated buffer size.
 *     4 --> NBT/TCP Transport Header.
 *    32 --> SMB Header
 *     1 --> Word Count byte
 *   510 --> Maximum Number of bytes of the Word Table (2 * 255)
 *     2 --> Byte count of the data
 * 65535 --> Maximum size of the data
 * -----
 * 66084
 */
#define	SMB_REQ_MAX_SIZE	66560		/* 65KB */
#define	SMB_XPRT_MAX_SIZE	(SMB_REQ_MAX_SIZE + NETBIOS_HDR_SZ)

#define	SMB_TXREQ_MAGIC		0X54524251	/* 'TREQ' */
typedef struct {
	list_node_t	tr_lnd;
	uint32_t	tr_magic;
	int		tr_len;
	uint8_t		tr_buf[SMB_XPRT_MAX_SIZE];
} smb_txreq_t;

#define	SMB_TXLST_MAGIC		0X544C5354	/* 'TLST' */
typedef struct {
	uint32_t	tl_magic;
	kmutex_t	tl_mutex;
	kcondvar_t	tl_wait_cv;
	boolean_t	tl_active;
} smb_txlst_t;

/*
 * Maximum buffer size for NT is 37KB.  If all clients are Windows 2000, this
 * can be changed to 64KB.  37KB must be used with a mix of NT/Windows 2000
 * clients because NT loses directory entries when values greater than 37KB are
 * used.
 *
 * Note: NBT_MAXBUF will be subtracted from the specified max buffer size to
 * account for the NBT header.
 */
#define	NBT_MAXBUF		8
#define	SMB_NT_MAXBUF		(37 * 1024)

#define	OUTBUFSIZE		(65 * 1024)
#define	SMBHEADERSIZE		32
#define	SMBND_HASH_MASK		(0xFF)
#define	MAX_IOVEC		512
#define	MAX_READREF		(8 * 1024)

#define	SMB_WORKER_MIN		4
#define	SMB_WORKER_DEFAULT	64
#define	SMB_WORKER_MAX		1024

/*
 * Destructor object used in the locked-list delete queue.
 */
#define	SMB_DTOR_MAGIC		0x44544F52	/* DTOR */
#define	SMB_DTOR_VALID(d)	\
    ASSERT(((d) != NULL) && ((d)->dt_magic == SMB_DTOR_MAGIC))

typedef void (*smb_dtorproc_t)(void *);

typedef struct smb_dtor {
	list_node_t	dt_lnd;
	uint32_t	dt_magic;
	void		*dt_object;
	smb_dtorproc_t	dt_proc;
} smb_dtor_t;

typedef struct smb_llist {
	krwlock_t	ll_lock;
	list_t		ll_list;
	uint32_t	ll_count;
	uint64_t	ll_wrop;
	kmutex_t	ll_mutex;
	list_t		ll_deleteq;
	uint32_t	ll_deleteq_count;
	boolean_t	ll_flushing;
} smb_llist_t;

typedef struct smb_bucket {
	smb_llist_t	b_list;
	uint32_t	b_max_seen;
} smb_bucket_t;

typedef struct smb_hash {
	uint32_t	rshift;
	uint32_t	num_buckets;
	smb_bucket_t	*buckets;
} smb_hash_t;

typedef struct smb_slist {
	kmutex_t	sl_mutex;
	kcondvar_t	sl_cv;
	list_t		sl_list;
	uint32_t	sl_count;
	boolean_t	sl_waiting;
} smb_slist_t;

/*
 * smb_avl_t State Machine
 * --------------------
 *
 *                      +-----------------------------+
 *                      |     SMB_AVL_STATE_START     |
 *                      +-----------------------------+
 *                                    | T0
 *                                    v
 *                      +-----------------------------+
 *                      |     SMB_AVL_STATE_READY     |
 *                      +-----------------------------+
 *                                    | T1
 *                                    v
 *                      +-----------------------------+
 *                      |  SMB_AVL_STATE_DESTROYING   |
 *                      +-----------------------------+
 *
 * Transition T0
 *
 *    This transition is executed in smb_avl_create().
 *
 * Transition T1
 *
 *    This transition is executed in smb_avl_destroy().
 *
 */
typedef enum {
	SMB_AVL_STATE_START = 0,
	SMB_AVL_STATE_READY,
	SMB_AVL_STATE_DESTROYING
} smb_avl_state_t;

typedef struct smb_avl_nops {
	int		(*avln_cmp) (const void *, const void *);
	void		(*avln_hold)(const void *);
	boolean_t	(*avln_rele)(const void *);
	void		(*avln_destroy)(void *);
} smb_avl_nops_t;

typedef struct smb_avl_cursor {
	void		*avlc_next;
	uint32_t	avlc_sequence;
} smb_avl_cursor_t;

typedef struct smb_avl {
	krwlock_t	avl_lock;
	avl_tree_t	avl_tree;
	kmutex_t	avl_mutex;
	kcondvar_t	avl_cv;
	smb_avl_state_t	avl_state;
	uint32_t	avl_refcnt;
	uint32_t	avl_sequence;
	const smb_avl_nops_t	*avl_nops;
} smb_avl_t;

typedef struct {
	kcondvar_t	rwx_cv;
	kmutex_t	rwx_mutex;
	krwlock_t	rwx_lock;
	boolean_t	rwx_waiting;
} smb_rwx_t;

typedef struct smb_export {
	kmutex_t	e_mutex;
	boolean_t	e_ready;
	smb_avl_t	e_share_avl;
	smb_slist_t	e_unexport_list;
	smb_thread_t	e_unexport_thread;
} smb_export_t;

/*
 * NOTIFY CHANGE, a.k.a. File Change Notification (FCN)
 */

/*
 * These FCN filter mask values are not from MS-FSCC, but
 * must not overlap with any FILE_NOTIFY_VALID_MASK values.
 */
#define	FILE_NOTIFY_CHANGE_EV_SUBDIR	0x00010000
#define	FILE_NOTIFY_CHANGE_EV_DELETE	0x00020000
#define	FILE_NOTIFY_CHANGE_EV_CLOSED	0x00040000
#define	FILE_NOTIFY_CHANGE_EV_OVERFLOW	0x00080000

/*
 * Note: These FCN action values are not from MS-FSCC, but must
 * follow in sequence from FILE_ACTION_MODIFIED_STREAM.
 *
 * FILE_ACTION_SUBDIR_CHANGED is used internally for
 * "watch tree" support, posted to all parents of a
 * directory that had one of the changes above.
 *
 * FILE_ACTION_DELETE_PENDING is used internally to tell
 * notify change requests when the "delete-on-close" flag
 * has been set on the directory being watched.
 *
 * FILE_ACTION_HANDLE_CLOSED is used to wakeup notify change
 * requests when the watched directory handle is closed.
 */
#define	FILE_ACTION_SUBDIR_CHANGED	0x00000009
#define	FILE_ACTION_DELETE_PENDING	0x0000000a
#define	FILE_ACTION_HANDLE_CLOSED	0x0000000b

/*
 * Sub-struct within smb_ofile_t
 */
typedef struct smb_notify {
	list_t			nc_waiters; /* Waiting SRs */
	mbuf_chain_t		nc_buffer;
	uint32_t		nc_filter;
	uint32_t		nc_events;
	int			nc_last_off;
	boolean_t		nc_subscribed;
} smb_notify_t;

/*
 * SMB operates over a NetBIOS-over-TCP transport (NBT) or directly
 * over TCP, which is also known as direct hosted NetBIOS-less SMB
 * or SMB-over-TCP.
 *
 * NBT messages have a 4-byte header that defines the message type
 * (8-bits), a 7-bit flags field and a 17-bit length.
 *
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |      TYPE     |     FLAGS   |E|            LENGTH             |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * 8-bit type      Defined in RFC 1002
 * 7-bit flags     Bits 0-6 reserved (must be 0)
 *                 Bit 7: Length extension bit (E)
 * 17-bit length   Includes bit 7 of the flags byte
 *
 *
 * SMB-over-TCP is defined to use a modified version of the NBT header
 * containing an 8-bit message type and 24-bit message length.
 *
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |      TYPE     |                  LENGTH                       |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * 8-bit type      Must be 0
 * 24-bit length
 *
 * The following structure is used to represent a generic, in-memory
 * SMB transport header; it is not intended to map directly to either
 * of the over-the-wire formats.
 */
typedef struct {
	uint8_t		xh_type;
	uint32_t	xh_length;
} smb_xprt_t;

int MBC_LENGTH(struct mbuf_chain *);
int MBC_MAXBYTES(struct mbuf_chain *);
void MBC_SETUP(struct mbuf_chain *, uint32_t);
void MBC_INIT(struct mbuf_chain *, uint32_t);
void MBC_FLUSH(struct mbuf_chain *);
void MBC_ATTACH_MBUF(struct mbuf_chain *, struct mbuf *);
void MBC_APPEND_MBUF(struct mbuf_chain *, struct mbuf *);
void MBC_ATTACH_BUF(struct mbuf_chain *MBC, unsigned char *BUF, int LEN);
int MBC_SHADOW_CHAIN(struct mbuf_chain *SUBMBC, struct mbuf_chain *MBC,
    int OFF, int LEN);

#define	MBC_ROOM_FOR(b, n) (((b)->chain_offset + (n)) <= (b)->max_bytes)

/*
 * Per smb_node oplock state
 */
typedef struct smb_oplock {
	kmutex_t		ol_mutex;
	boolean_t		ol_fem;		/* fem monitor installed? */
	struct smb_ofile	*excl_open;
	uint32_t		ol_state;
	int32_t			cnt_II;
	int32_t			cnt_R;
	int32_t			cnt_RH;
	int32_t			cnt_RHBQ;
	int32_t			waiters;
	kcondvar_t		WaitingOpenCV;
} smb_oplock_t;

/*
 * Per smb_ofile oplock state
 */
typedef struct smb_oplock_grant {
	/* smb protocol-level state */
	uint32_t		og_state;	/* latest sent to client */
	uint32_t		og_breaking;	/* BREAK_TO... flags */
	uint16_t		og_dialect;	/* how to send breaks */
	/* File-system level state */
	uint8_t			onlist_II;
	uint8_t			onlist_R;
	uint8_t			onlist_RH;
	uint8_t			onlist_RHBQ;
	uint8_t			BreakingToRead;
} smb_oplock_grant_t;

#define	SMB_LEASE_KEY_SZ	16

typedef struct smb_lease {
	list_node_t		ls_lnd;		/* sv_lease_ht */
	kmutex_t		ls_mutex;
	smb_llist_t		*ls_bucket;
	struct smb_node		*ls_node;
	/*
	 * With a lease, just one ofile has the oplock.
	 * This (used only for comparison) identifies which.
	 */
	void			*ls_oplock_ofile;
	uint32_t		ls_refcnt;
	uint32_t		ls_state;
	uint32_t		ls_breaking;	/* BREAK_TO... flags */
	uint16_t		ls_epoch;
	uint16_t		ls_version;
	uint8_t			ls_key[SMB_LEASE_KEY_SZ];
	uint8_t			ls_clnt[SMB_LEASE_KEY_SZ];
} smb_lease_t;

#define	SMB_NODE_MAGIC		0x4E4F4445	/* 'NODE' */
#define	SMB_NODE_VALID(p)	ASSERT((p)->n_magic == SMB_NODE_MAGIC)

typedef enum {
	SMB_NODE_STATE_AVAILABLE = 0,
	SMB_NODE_STATE_DESTROYING
} smb_node_state_t;

/*
 * waiting_event        # of clients requesting FCN
 * n_timestamps         cached timestamps
 * n_allocsz            cached file allocation size
 * n_dnode              directory node
 * n_unode              unnamed stream node
 * delete_on_close_cred credentials for delayed delete
 */
typedef struct smb_node {
	list_node_t		n_lnd;
	uint32_t		n_magic;
	krwlock_t		n_lock;
	kmutex_t		n_mutex;
	smb_node_state_t	n_state;
	uint32_t		n_refcnt;
	uint32_t		n_hashkey;
	smb_llist_t		*n_hash_bucket;
	uint32_t		n_open_count;
	uint32_t		n_opening_count;
	smb_llist_t		n_ofile_list;
	/* If entering both, go in order n_lock_list, n_wlock_list */
	smb_llist_t		n_lock_list;	/* active locks */
	smb_llist_t		n_wlock_list;	/* waiting locks */
	volatile int		flags;
	u_offset_t		n_allocsz;
	uint32_t		n_fcn_count;
	smb_oplock_t		n_oplock;
	struct smb_node		*n_dnode;
	struct smb_node		*n_unode;
	cred_t			*delete_on_close_cred;
	uint32_t		n_delete_on_close_flags;
	char			od_name[MAXNAMELEN];
	vnode_t			*vp;
	smb_audit_buf_node_t	*n_audit_buf;
} smb_node_t;

#define	NODE_FLAGS_REPARSE		0x00001000
#define	NODE_FLAGS_DFSLINK		0x00002000
#define	NODE_FLAGS_VFSROOT		0x00004000
#define	NODE_FLAGS_SYSTEM		0x00008000
#define	NODE_FLAGS_WRITE_THROUGH	0x00100000
#define	NODE_XATTR_DIR			0x01000000
#define	NODE_FLAGS_DELETE_COMMITTED	0x20000000
#define	NODE_FLAGS_DELETE_ON_CLOSE	0x40000000
#define	NODE_FLAGS_EXECUTABLE		0x80000000

#define	SMB_NODE_VFS(node)	((node)->vp->v_vfsp)
#define	SMB_NODE_FSID(node)	((node)->vp->v_vfsp->vfs_fsid)

/* Maximum buffer size for encryption key */
#define	SMB_ENCRYPT_KEY_MAXLEN		32

#define	SMB_SHARE_MAGIC		0x4B534852	/* KSHR */

typedef struct smb_kshare {
	uint32_t	shr_magic;
	avl_node_t	shr_link;
	kmutex_t	shr_mutex;
	kcondvar_t	shr_cv;
	char		*shr_name;
	char		*shr_path;
	char		*shr_cmnt;
	char		*shr_container;
	char		*shr_oemname;
	uint32_t	shr_flags;
	uint32_t	shr_type;
	uint32_t	shr_refcnt;
	uint32_t	shr_autocnt;
	uid_t		shr_uid;
	gid_t		shr_gid;
	char		*shr_access_none;
	char		*shr_access_ro;
	char		*shr_access_rw;
	smb_node_t	*shr_root_node;
	smb_node_t	*shr_ca_dir;
	void		*shr_import_busy;
	smb_cfg_val_t	shr_encrypt; /* Share.EncryptData */
} smb_kshare_t;


typedef struct smb_arg_negotiate {
	char		*ni_name;
	int		ni_dialect;
	int		ni_index;
	uint32_t	ni_capabilities;
	uint16_t	ni_maxmpxcount;
	int16_t		ni_tzcorrection;
	uint8_t		ni_keylen;
	uint8_t		ni_key[SMB_ENCRYPT_KEY_MAXLEN];
	timestruc_t	ni_servertime;
} smb_arg_negotiate_t;

typedef enum {
	SMB_SSNSETUP_PRE_NTLM012 = 1,
	SMB_SSNSETUP_NTLM012_NOEXT,
	SMB_SSNSETUP_NTLM012_EXTSEC
} smb_ssnsetup_type_t;

typedef struct smb_arg_sessionsetup {
	smb_ssnsetup_type_t ssi_type;
	char		*ssi_user;
	char		*ssi_domain;
	/* LM password hash, f.k.a. case-insensitive p/w */
	uint16_t	ssi_lmpwlen;
	uint8_t		*ssi_lmpwd;
	/* NT password hash, f.k.a. case-sensitive p/w */
	uint16_t	ssi_ntpwlen;
	uint8_t		*ssi_ntpwd;
	/* Incoming security blob */
	uint16_t	ssi_iseclen;
	uint8_t		*ssi_isecblob;
	/* Incoming security blob */
	uint16_t	ssi_oseclen;
	uint8_t		*ssi_osecblob;
	/* parameters */
	uint16_t	ssi_maxbufsize;
	uint16_t	ssi_maxmpxcount;
	uint32_t	ssi_capabilities;
	int		ssi_native_os;
	int		ssi_native_lm;
} smb_arg_sessionsetup_t;

typedef struct tcon {
	char		*name;
	char		*path;
	char		*service;
	int		pwdlen;
	char		*password;
	uint16_t	flags;
	uint16_t	optional_support;
	smb_kshare_t	*si;
} smb_arg_tcon_t;

/*
 * Based on section 2.6.1.2 (Connection Management) of the June 13,
 * 1996 CIFS spec, a server may terminate the transport connection
 * due to inactivity. The client software is expected to be able to
 * automatically reconnect to the server if this happens. Like much
 * of the useful background information, this section appears to
 * have been dropped from later revisions of the document.
 *
 * Each session has an activity timestamp that's updated whenever a
 * request is dispatched. If the session is idle, i.e. receives no
 * requests, for SMB_SESSION_INACTIVITY_TIMEOUT minutes it will be
 * closed.
 *
 * Each session has an I/O semaphore to serialize communication with
 * the client. For example, after receiving a raw-read request, the
 * server is not allowed to send an oplock break to the client until
 * after it has sent the raw-read data.
 */
#define	SMB_SESSION_INACTIVITY_TIMEOUT		(15 * 60)

/* SMB1 signing */
struct smb_sign {
	unsigned int flags;
	uint32_t seqnum;
	uint_t mackey_len;
	uint8_t *mackey;
};

/*
 * SMB2 signing
 */
struct smb_key {
	uint_t len;
	uint8_t key[SMB2_SESSION_KEY_LEN];
};

#define	SMB_SIGNING_ENABLED	1
#define	SMB_SIGNING_CHECK	2

/*
 * Locking notes:
 * If you hold the mutex/lock on an object, don't flush the deleteq
 * of the objects directly below it in the logical hierarchy
 * (i.e. via smb_llist_exit()). I.e. don't drop s_tree_list when
 * you hold u_mutex, because deleted trees need u_mutex to
 * lower the refcnt.
 *
 * Note that this also applies to u_mutex and t_ofile_list.
 */

/*
 * The "session" object.
 *
 * Note that the smb_session_t object here corresponds to what MS-SMB2
 * calls a "connection".  Adding to the confusion, what MS calls a
 * "session" corresponds to our smb_user_t (below).
 */

/*
 * Session State Machine
 * ---------------------
 *
 *
 * +-----------------------------+	    +----------------------------+
 * | SMB_SESSION_STATE_CONNECTED |	    | SMB_SESSION_STATE_SHUTDOWN |
 * +-----------------------------+	    +----------------------------+
 *		  |					     ^
 *		  |					     |T6
 *		  |			    +------------------------------+
 *		  |			    | SMB_SESSION_STATE_TERMINATED |
 *		T0|			    +------------------------------+
 *		  +--------------------+		     ^
 *		  v		       |T4                   |T5
 * +-------------------------------+   |    +--------------------------------+
 * | SMB_SESSION_STATE_ESTABLISHED |---+--->| SMB_SESSION_STATE_DISCONNECTED |
 * +-------------------------------+        +--------------------------------+
 *		T1|				^
 *		  +----------+			|T3
 *                           v			|
 *                  +------------------------------+
 *                  | SMB_SESSION_STATE_NEGOTIATED |
 *                  +------------------------------+
 *
 *
 * Transition T0
 *
 *
 *
 * Transition T1
 *
 *
 *
 * Transition T2
 *
 *
 *
 * Transition T3
 *
 *
 *
 * Transition T4
 *
 *
 *
 * Transition T5
 *
 *
 *
 * Transition T6
 *
 *
 *
 */
#define	SMB_SESSION_MAGIC	0x53455353	/* 'SESS' */
#define	SMB_SESSION_VALID(p)	\
    ASSERT(((p) != NULL) && ((p)->s_magic == SMB_SESSION_MAGIC))

#define	SMB_CHALLENGE_SZ	8

typedef enum {
	SMB_SESSION_STATE_INITIALIZED = 0,
	SMB_SESSION_STATE_DISCONNECTED,
	SMB_SESSION_STATE_CONNECTED,
	SMB_SESSION_STATE_ESTABLISHED,
	SMB_SESSION_STATE_NEGOTIATED,
	SMB_SESSION_STATE_TERMINATED,
	SMB_SESSION_STATE_SHUTDOWN,
	SMB_SESSION_STATE_SENTINEL
} smb_session_state_t;

/* Bits in s_flags below */
#define	SMB_SSN_AAPL_CCEXT	1	/* Saw "AAPL" create ctx. ext. */
#define	SMB_SSN_AAPL_READDIR	2	/* Wants MacOS ext. readdir */

typedef struct smb_session {
	list_node_t		s_lnd;
	uint32_t		s_magic;
	smb_rwx_t		s_lock;
	uint64_t		s_kid;
	smb_session_state_t	s_state;
	uint32_t		s_flags;
	taskqid_t		s_receiver_tqid;
	kthread_t		*s_thread;
	kt_did_t		s_ktdid;
	int	(*newrq_func)(struct smb_request *);
	struct smb_server	*s_server;
	smb_kmod_cfg_t		s_cfg;
	int32_t			s_gmtoff;
	uint32_t		keep_alive;
	uint64_t		opentime;
	uint16_t		s_local_port;
	uint16_t		s_remote_port;
	smb_inaddr_t		ipaddr;
	smb_inaddr_t		local_ipaddr;
	int			dialect;
	int			native_os;
	int			native_lm;

	kmutex_t		s_credits_mutex;
	uint16_t		s_cur_credits;
	uint16_t		s_max_credits;

	uint32_t		capabilities;
	uint32_t		srv_cap;

	struct smb_sign		signing;	/* SMB1 */
	void			*sign_mech;	/* mechanism info */
	void			*enc_mech;

	/* SMB2/SMB3 signing support */
	int			(*sign_calc)(struct smb_request *,
					struct mbuf_chain *, uint8_t *);
	void			(*sign_fini)(struct smb_session *);

	ksocket_t		sock;

	smb_slist_t		s_req_list;
	smb_llist_t		s_xa_list;
	smb_llist_t		s_user_list;
	smb_llist_t		s_tree_list;
	smb_idpool_t		s_uid_pool;
	smb_idpool_t		s_tid_pool;
	smb_txlst_t		s_txlst;

	volatile uint32_t	s_tree_cnt;
	volatile uint32_t	s_file_cnt;
	volatile uint32_t	s_dir_cnt;

	uint16_t		cli_secmode;
	uint16_t		srv_secmode;
	uint32_t		sesskey;
	uint32_t		challenge_len;
	unsigned char		challenge_key[SMB_CHALLENGE_SZ];
	int64_t			activity_timestamp;
	timeout_id_t		s_auth_tmo;

	/*
	 * Maximum negotiated buffer sizes between SMB client and server
	 * in SMB_SESSION_SETUP_ANDX
	 */
	int			cmd_max_bytes;
	int			reply_max_bytes;
	uint16_t		smb_msg_size;
	uint16_t		smb_max_mpx;
	smb_srqueue_t		*s_srqueue;
	uint64_t		start_time;
	unsigned char		MAC_key[44];
	char			ip_addr_str[INET6_ADDRSTRLEN];
	uint8_t			clnt_uuid[16];
	char			workstation[SMB_PI_MAX_HOST];
} smb_session_t;

/*
 * The "user" object.
 *
 * Note that smb_user_t object here corresponds to what MS-SMB2 calls
 * a "session".  (Our smb_session_t is something else -- see above).
 */

#define	SMB_USER_MAGIC 0x55534552	/* 'USER' */
#define	SMB_USER_VALID(u)	\
    ASSERT(((u) != NULL) && ((u)->u_magic == SMB_USER_MAGIC))

/* These flags are all <= 0x00000010 */
#define	SMB_USER_FLAG_GUEST			SMB_ATF_GUEST
#define	SMB_USER_FLAG_ANON			SMB_ATF_ANON
#define	SMB_USER_FLAG_ADMIN			SMB_ATF_ADMIN
#define	SMB_USER_FLAG_POWER_USER		SMB_ATF_POWERUSER
#define	SMB_USER_FLAG_BACKUP_OPERATOR		SMB_ATF_BACKUPOP

#define	SMB_USER_IS_ADMIN(U)	(((U)->u_flags & SMB_USER_FLAG_ADMIN) != 0)
#define	SMB_USER_IS_GUEST(U)	(((U)->u_flags & SMB_USER_FLAG_GUEST) != 0)

/*
 * Internal privilege flags derived from smb_privilege.h numbers
 * Would rather not include that in this file.
 */
#define	SMB_USER_PRIV_SECURITY		(1<<8)	/* SE_SECURITY_LUID */
#define	SMB_USER_PRIV_TAKE_OWNERSHIP	(1<<9)	/* SE_TAKE_OWNERSHIP_LUID */
#define	SMB_USER_PRIV_BACKUP		(1<<17)	/* SE_BACKUP_LUID */
#define	SMB_USER_PRIV_RESTORE		(1<<18)	/* SE_RESTORE_LUID */
#define	SMB_USER_PRIV_CHANGE_NOTIFY	(1<<23)	/* SE_CHANGE_NOTIFY_LUID */
#define	SMB_USER_PRIV_READ_FILE		(1<<25)	/* SE_READ_FILE_LUID */
#define	SMB_USER_PRIV_WRITE_FILE	(1<<26)	/* SE_WRITE_FILE_LUID */

/*
 * See the long "User State Machine" comment in smb_user.c
 */
typedef enum {
	SMB_USER_STATE_LOGGING_ON = 0,
	SMB_USER_STATE_LOGGED_ON,
	SMB_USER_STATE_LOGGING_OFF,
	SMB_USER_STATE_LOGGED_OFF,
	SMB_USER_STATE_SENTINEL
} smb_user_state_t;

typedef enum {
	SMB2_DH_PRESERVE_NONE = 0,
	SMB2_DH_PRESERVE_SOME,
	SMB2_DH_PRESERVE_ALL
} smb_preserve_type_t;

typedef struct smb_user {
	list_node_t		u_lnd;
	uint32_t		u_magic;
	kmutex_t		u_mutex;
	smb_user_state_t	u_state;

	struct smb_server	*u_server;
	smb_session_t		*u_session;
	ksocket_t		u_authsock;
	timeout_id_t		u_auth_tmo;
	uint16_t		u_name_len;
	char			*u_name;
	uint16_t		u_domain_len;
	char			*u_domain;
	time_t			u_logon_time;
	cred_t			*u_cred;
	cred_t			*u_privcred;

	uint64_t		u_ssnid;	/* unique server-wide */
	uint32_t		u_refcnt;
	uint32_t		u_flags;
	smb_preserve_type_t	preserve_opens;
	uint32_t		u_privileges;
	uint16_t		u_uid;		/* unique per-session */
	uint32_t		u_audit_sid;

	uint32_t		u_sign_flags;
	struct smb_key		u_sign_key;	/* SMB2 signing */

	struct smb_key		u_enc_key;
	struct smb_key		u_dec_key;
	volatile uint64_t	u_nonce_cnt;
	uint8_t			u_nonce_fixed[4];
	uint64_t		u_salt;
	smb_cfg_val_t		u_encrypt;
} smb_user_t;

#define	SMB_TREE_MAGIC			0x54524545	/* 'TREE' */
#define	SMB_TREE_VALID(p)	\
    ASSERT((p != NULL) && ((p)->t_magic == SMB_TREE_MAGIC))

#define	SMB_TYPENAMELEN			_ST_FSTYPSZ
#define	SMB_VOLNAMELEN			32

#define	SMB_TREE_READONLY		0x00000001
#define	SMB_TREE_SUPPORTS_ACLS		0x00000002
#define	SMB_TREE_STREAMS		0x00000004
#define	SMB_TREE_CASEINSENSITIVE	0x00000008
#define	SMB_TREE_NO_CASESENSITIVE	0x00000010
#define	SMB_TREE_NO_EXPORT		0x00000020
#define	SMB_TREE_OPLOCKS		0x00000040
#define	SMB_TREE_SHORTNAMES		0x00000080
#define	SMB_TREE_XVATTR			0x00000100
#define	SMB_TREE_DIRENTFLAGS		0x00000200
#define	SMB_TREE_ACLONCREATE		0x00000400
#define	SMB_TREE_ACEMASKONACCESS	0x00000800
#define	SMB_TREE_NFS_MOUNTED		0x00001000
#define	SMB_TREE_UNICODE_ON_DISK	0x00002000
#define	SMB_TREE_CATIA			0x00004000
#define	SMB_TREE_ABE			0x00008000
#define	SMB_TREE_QUOTA			0x00010000
#define	SMB_TREE_DFSROOT		0x00020000
#define	SMB_TREE_SPARSE			0x00040000
#define	SMB_TREE_TRAVERSE_MOUNTS	0x00080000
#define	SMB_TREE_FORCE_L2_OPLOCK	0x00100000
#define	SMB_TREE_CA			0x00200000
/* Note: SMB_TREE_... in the mdb module too. */

/*
 * See the long "Tree State Machine" comment in smb_tree.c
 */
typedef enum {
	SMB_TREE_STATE_CONNECTED = 0,
	SMB_TREE_STATE_DISCONNECTING,
	SMB_TREE_STATE_DISCONNECTED,
	SMB_TREE_STATE_SENTINEL
} smb_tree_state_t;

typedef struct smb_tree {
	list_node_t		t_lnd;
	uint32_t		t_magic;
	kmutex_t		t_mutex;
	smb_tree_state_t	t_state;

	struct smb_server	*t_server;
	smb_session_t		*t_session;
	/*
	 * user whose uid was in the tree connect message
	 * ("owner" in MS-CIFS parlance, see section 2.2.1.6 definition of FID)
	 */
	smb_user_t		*t_owner;
	smb_node_t		*t_snode;

	smb_llist_t		t_ofile_list;
	smb_idpool_t		t_fid_pool;

	smb_llist_t		t_odir_list;
	smb_idpool_t		t_odid_pool;

	uint32_t		t_refcnt;
	uint32_t		t_flags;
	int32_t			t_res_type;
	uint16_t		t_tid;
	uint16_t		t_umask;
	char			t_sharename[MAXNAMELEN];
	char			t_resource[MAXPATHLEN];
	char			t_typename[SMB_TYPENAMELEN];
	char			t_volume[SMB_VOLNAMELEN];
	acl_type_t		t_acltype;
	uint32_t		t_access;
	uint32_t		t_execflags;
	time_t			t_connect_time;
	volatile uint32_t	t_open_files;
	smb_cfg_val_t		t_encrypt; /* Share.EncryptData */
} smb_tree_t;

#define	SMB_TREE_VFS(tree)	((tree)->t_snode->vp->v_vfsp)
#define	SMB_TREE_FSID(tree)	((tree)->t_snode->vp->v_vfsp->vfs_fsid)

#define	SMB_TREE_IS_READONLY(sr)					\
	((sr) != NULL && (sr)->tid_tree != NULL &&			\
	!((sr)->tid_tree->t_access & ACE_ALL_WRITE_PERMS))

#define	SMB_TREE_IS_CASEINSENSITIVE(sr)                                 \
	(((sr) && (sr)->tid_tree) ?                                     \
	smb_tree_has_feature((sr)->tid_tree, SMB_TREE_CASEINSENSITIVE) : 0)

#define	SMB_TREE_HAS_ACCESS(sr, acemask)				\
	((sr) == NULL ? ACE_ALL_PERMS : (				\
	(((sr) && (sr)->tid_tree) ?					\
	(((sr)->tid_tree->t_access) & (acemask)) : 0)))

#define	SMB_TREE_SUPPORTS_CATIA(sr)					\
	(((sr) && (sr)->tid_tree) ?                                     \
	smb_tree_has_feature((sr)->tid_tree, SMB_TREE_CATIA) : 0)

#define	SMB_TREE_SUPPORTS_ABE(sr)					\
	(((sr) && (sr)->tid_tree) ?                                     \
	smb_tree_has_feature((sr)->tid_tree, SMB_TREE_ABE) : 0)

#define	SMB_TREE_IS_DFSROOT(sr)						\
	(((sr) && (sr)->tid_tree) ?                                     \
	smb_tree_has_feature((sr)->tid_tree, SMB_TREE_DFSROOT) : 0)

#define	SMB_TREE_SUPPORTS_SHORTNAMES(sr)				\
	(((sr) && (sr)->tid_tree) ?					\
	smb_tree_has_feature((sr)->tid_tree, SMB_TREE_SHORTNAMES) : 0)

/*
 * SMB_TREE_CONTAINS_NODE is used to check if a node is on the same
 * file system as the tree's root filesystem, or if mount point traversal
 * should be allowed.  Note that this is also called in some cases with
 * sr=NULL, where it is expected to evaluate to TRUE.
 */

#define	SMB_TREE_CONTAINS_NODE(sr, node)                                \
	((sr) == NULL || (sr)->tid_tree == NULL ||                      \
	SMB_TREE_VFS((sr)->tid_tree) == SMB_NODE_VFS(node) ||           \
	smb_tree_has_feature((sr)->tid_tree, SMB_TREE_TRAVERSE_MOUNTS))

/*
 * SMB_PATHFILE_IS_READONLY indicates whether or not a file is
 * readonly when the caller has a path rather than an ofile.
 */
#define	SMB_PATHFILE_IS_READONLY(sr, node)			\
	(SMB_TREE_IS_READONLY((sr)) ||				\
	smb_node_file_is_readonly((node)))

#define	SMB_ODIR_MAGIC		0x4F444952	/* 'ODIR' */
#define	SMB_ODIR_VALID(p)	\
    ASSERT((p != NULL) && ((p)->d_magic == SMB_ODIR_MAGIC))

#define	SMB_ODIR_BUFSIZE	(8 * 1024)

#define	SMB_ODIR_FLAG_WILDCARDS		0x0001
#define	SMB_ODIR_FLAG_IGNORE_CASE	0x0002
#define	SMB_ODIR_FLAG_XATTR		0x0004
#define	SMB_ODIR_FLAG_EDIRENT		0x0008
#define	SMB_ODIR_FLAG_CATIA		0x0010
#define	SMB_ODIR_FLAG_ABE		0x0020
#define	SMB_ODIR_FLAG_SHORTNAMES	0x0040

typedef enum {
	SMB_ODIR_STATE_OPEN = 0,
	SMB_ODIR_STATE_IN_USE,
	SMB_ODIR_STATE_CLOSING,
	SMB_ODIR_STATE_CLOSED,
	SMB_ODIR_STATE_SENTINEL
} smb_odir_state_t;

typedef enum {
	SMB_ODIR_RESUME_CONT,
	SMB_ODIR_RESUME_IDX,
	SMB_ODIR_RESUME_COOKIE,
	SMB_ODIR_RESUME_FNAME
} smb_odir_resume_type_t;

typedef struct smb_odir_resume {
	smb_odir_resume_type_t	or_type;
	int			or_idx;
	uint32_t		or_cookie;
	char			*or_fname;
} smb_odir_resume_t;

/*
 * Flags used when opening an odir
 */
#define	SMB_ODIR_OPENF_BACKUP_INTENT	0x01

typedef struct smb_odir {
	list_node_t		d_lnd;
	uint32_t		d_magic;
	kmutex_t		d_mutex;
	smb_odir_state_t	d_state;
	smb_session_t		*d_session;
	smb_user_t		*d_user;
	smb_tree_t		*d_tree;
	smb_node_t		*d_dnode;
	cred_t			*d_cred;
	uint32_t		d_opened_by_pid;
	uint16_t		d_odid;
	uint16_t		d_sattr;
	uint32_t		d_refcnt;
	uint32_t		d_flags;
	boolean_t		d_eof;
	int			d_bufsize;
	uint64_t		d_offset;
	union {
		char		*u_bufptr;
		struct edirent	*u_edp;
		struct dirent64	*u_dp;
	} d_u;
	uint32_t		d_last_cookie;
	uint32_t		d_cookies[SMB_MAX_SEARCH];
	char			d_pattern[MAXNAMELEN];
	char			d_buf[SMB_ODIR_BUFSIZE];
	char			d_last_name[MAXNAMELEN];
} smb_odir_t;
#define	d_bufptr	d_u.u_bufptr
#define	d_edp		d_u.u_edp
#define	d_dp		d_u.u_dp

typedef struct smb_odirent {
	char		od_name[MAXNAMELEN];	/* on disk name */
	ino64_t		od_ino;
	uint32_t	od_eflags;
} smb_odirent_t;

#define	SMB_OPIPE_MAGIC		0x50495045	/* 'PIPE' */
#define	SMB_OPIPE_VALID(p)	\
    ASSERT(((p) != NULL) && (p)->p_magic == SMB_OPIPE_MAGIC)
#define	SMB_OPIPE_MAXNAME	32

/*
 * Data structure for SMB_FTYPE_MESG_PIPE ofiles, which is used
 * at the interface between SMB and NDR RPC.
 */
typedef struct smb_opipe {
	uint32_t		p_magic;
	kmutex_t		p_mutex;
	kcondvar_t		p_cv;
	struct smb_ofile	*p_ofile;
	struct smb_server	*p_server;
	uint32_t		p_refcnt;
	ksocket_t		p_socket;
	/* This is the "flat" name, without path prefix */
	char			p_name[SMB_OPIPE_MAXNAME];
} smb_opipe_t;

/*
 * The of_ftype	of an open file should contain the SMB_FTYPE value
 * returned when the file/pipe was opened. The following
 * assumptions are currently made:
 *
 * File Type	    Node       PipeInfo
 * ---------	    --------   --------
 * SMB_FTYPE_DISK       Valid      Null
 * SMB_FTYPE_BYTE_PIPE  Undefined  Undefined
 * SMB_FTYPE_MESG_PIPE  Null       Valid
 * SMB_FTYPE_PRINTER    Undefined  Undefined
 * SMB_FTYPE_UNKNOWN    Undefined  Undefined
 */

/*
 * Some flags for ofile structure
 *
 *	SMB_OFLAGS_SET_DELETE_ON_CLOSE
 *   Set this flag when the corresponding open operation whose
 *   DELETE_ON_CLOSE bit of the CreateOptions is set. If any
 *   open file instance has this bit set, the NODE_FLAGS_DELETE_ON_CLOSE
 *   will be set for the file node upon close.
 */

/*	SMB_OFLAGS_READONLY		0x0001 (obsolete) */
#define	SMB_OFLAGS_EXECONLY		0x0002
#define	SMB_OFLAGS_SET_DELETE_ON_CLOSE	0x0004
#define	SMB_OFLAGS_LLF_POS_VALID	0x0008

#define	SMB_OFILE_MAGIC		0x4F464C45	/* 'OFLE' */
#define	SMB_OFILE_VALID(p)	\
    ASSERT((p != NULL) && ((p)->f_magic == SMB_OFILE_MAGIC))

/*
 * This is the size of the per-handle "Lock Sequence" array.
 * See LockSequenceIndex in [MS-SMB2] 2.2.26, and smb2_lock.c
 */
#define	SMB_OFILE_LSEQ_MAX		64

/* {arg_open,ofile}->dh_vers values */
typedef enum {
	SMB2_NOT_DURABLE = 0,
	SMB2_DURABLE_V1,
	SMB2_DURABLE_V2,
	SMB2_RESILIENT,
} smb_dh_vers_t;

/*
 * See the long "Ofile State Machine" comment in smb_ofile.c
 */
typedef enum {
	SMB_OFILE_STATE_ALLOC = 0,
	SMB_OFILE_STATE_OPEN,
	SMB_OFILE_STATE_SAVE_DH,
	SMB_OFILE_STATE_SAVING,
	SMB_OFILE_STATE_CLOSING,
	SMB_OFILE_STATE_CLOSED,
	SMB_OFILE_STATE_ORPHANED,
	SMB_OFILE_STATE_RECONNECT,
	SMB_OFILE_STATE_EXPIRED,
	SMB_OFILE_STATE_SENTINEL
} smb_ofile_state_t;

typedef struct smb_ofile {
	list_node_t		f_tree_lnd;	/* t_ofile_list */
	list_node_t		f_node_lnd;	/* n_ofile_list */
	list_node_t		f_dh_lnd;	/* sv_persistid_ht */
	uint32_t		f_magic;
	kmutex_t		f_mutex;
	smb_ofile_state_t	f_state;

	struct smb_server	*f_server;
	smb_session_t		*f_session;
	smb_user_t		*f_user;
	smb_tree_t		*f_tree;
	smb_node_t		*f_node;
	smb_odir_t		*f_odir;
	smb_opipe_t		*f_pipe;

	kcondvar_t		f_cv;
	/*
	 * Note: f_persistid == 0 means this ofile has no persistid
	 * (same interpretation at the protocol level).  IFF non-zero,
	 * this ofile is linked in the sv_persistid_ht hash table.
	 */
	uint64_t		f_persistid;
	uint32_t		f_uniqid;
	uint32_t		f_refcnt;
	uint64_t		f_seek_pos;
	uint32_t		f_flags;
	uint32_t		f_granted_access;
	uint32_t		f_share_access;
	uint32_t		f_create_options;
	uint32_t		f_opened_by_pid;
	uint16_t		f_fid;
	uint16_t		f_ftype;
	uint64_t		f_llf_pos;
	int			f_mode;
	cred_t			*f_cr;
	pid_t			f_pid;
	smb_attr_t		f_pending_attr;
	boolean_t		f_written;
	smb_oplock_grant_t	f_oplock;
	uint8_t			TargetOplockKey[SMB_LEASE_KEY_SZ];
	uint8_t			ParentOplockKey[SMB_LEASE_KEY_SZ];
	struct smb_lease	*f_lease;

	smb_notify_t		f_notify;

	smb_dh_vers_t		dh_vers;
	hrtime_t		dh_timeout_offset; /* time offset for timeout */
	hrtime_t		dh_expire_time; /* time the handle expires */
	boolean_t		dh_persist;
	kmutex_t		dh_nvlock;
	struct nvlist		*dh_nvlist;
	smb_node_t		*dh_nvfile;

	uint8_t			dh_create_guid[16];
	char			f_quota_resume[SMB_SID_STRSZ];
	uint8_t			f_lock_seq[SMB_OFILE_LSEQ_MAX];
} smb_ofile_t;

typedef struct smb_fileinfo {
	char		fi_name[MAXNAMELEN];
	char		fi_shortname[SMB_SHORTNAMELEN];
	uint32_t	fi_cookie;	/* Dir offset (of next entry) */
	uint32_t	fi_dosattr;	/* DOS attributes */
	uint64_t	fi_nodeid;	/* file system node id */
	uint64_t	fi_size;	/* file size in bytes */
	uint64_t	fi_alloc_size;	/* allocation size in bytes */
	timestruc_t	fi_atime;	/* last access */
	timestruc_t	fi_mtime;	/* last modification */
	timestruc_t	fi_ctime;	/* last status change */
	timestruc_t	fi_crtime;	/* file creation */
} smb_fileinfo_t;

typedef struct smb_streaminfo {
	uint64_t	si_size;
	uint64_t	si_alloc_size;
	char		si_name[MAXPATHLEN];
} smb_streaminfo_t;

#define	SMB_LOCK_MAGIC	0x4C4F434B	/* 'LOCK' */

typedef struct smb_lock {
	list_node_t		l_lnd;
	uint32_t		l_magic;
	kmutex_t		l_mutex;
	kcondvar_t		l_cv;

	smb_ofile_t		*l_file;

	struct smb_lock		*l_blocked_by; /* Debug info only */

	uint32_t		l_conflicts;
	uint32_t		l_flags;
	uint32_t		l_pid;
	uint32_t		l_type;
	uint64_t		l_start;
	uint64_t		l_length;
	clock_t			l_end_time;
} smb_lock_t;

#define	SMB_LOCK_FLAG_INDEFINITE	0x0004
#define	SMB_LOCK_FLAG_CLOSED		0x0008
#define	SMB_LOCK_FLAG_CANCELLED		0x0010

#define	SMB_LOCK_TYPE_READWRITE		101
#define	SMB_LOCK_TYPE_READONLY		102

typedef struct vardata_block {
	uint8_t			vdb_tag;
	uint32_t		vdb_len;
	struct uio		vdb_uio;
	struct iovec		vdb_iovec[MAX_IOVEC];
} smb_vdb_t;

#define	SMB_WRMODE_WRITE_THRU	0x0001
#define	SMB_WRMODE_IS_STABLE(M)	((M) & SMB_WRMODE_WRITE_THRU)

#define	SMB_RW_MAGIC		0x52445257	/* 'RDRW' */

typedef struct smb_rw_param {
	uint32_t rw_magic;
	smb_vdb_t rw_vdb;
	uint64_t rw_offset;
	uint32_t rw_last_write;
	uint16_t rw_mode;
	uint32_t rw_count;		/* bytes in this request */
	uint16_t rw_mincnt;
	uint32_t rw_total;		/* total bytes (write-raw) */
	uint16_t rw_dsoff;		/* SMB data offset */
	uint8_t rw_andx;		/* SMB secondary andx command */
} smb_rw_param_t;

typedef struct smb_pathname {
	char	*pn_path;
	char	*pn_pname;
	char	*pn_fname;
	char	*pn_sname;
	char	*pn_stype;
} smb_pathname_t;

/*
 * fs_query_info
 */
typedef struct smb_fqi {
	smb_pathname_t	fq_path;
	uint16_t	fq_sattr;
	smb_node_t	*fq_dnode;
	smb_node_t	*fq_fnode;
	smb_attr_t	fq_fattr;
	char		fq_last_comp[MAXNAMELEN];
} smb_fqi_t;

typedef struct dirop {
	smb_fqi_t	fqi;
	smb_fqi_t	dst_fqi;
	uint16_t	info_level;
	uint16_t	flags;
} smb_arg_dirop_t;

typedef struct smb_queryinfo {
	smb_node_t	*qi_node;	/* NULL for pipes */
	uint8_t qi_InfoType;
	uint8_t qi_InfoClass;
	uint8_t	qi_delete_on_close;
	uint8_t qi_isdir;
	uint32_t qi_AddlInfo;
	uint32_t qi_Flags;
	mbuf_chain_t in_data;
	smb_attr_t	qi_attr;
	uint32_t	qi_namelen;
	char		qi_shortname[SMB_SHORTNAMELEN];
	char		qi_name[MAXPATHLEN];
} smb_queryinfo_t;

typedef struct smb_setinfo {
	smb_node_t *si_node;
	mbuf_chain_t si_data;
	smb_attr_t si_attr;
} smb_setinfo_t;

/*
 * smb_fssize_t
 * volume_units and volume avail are the total allocated and
 * available units on the volume.
 * caller_units and caller_avail are the allocated and available
 * units on the volume for the user associated with the calling
 * thread.
 */
typedef struct smb_fssize {
	uint64_t	fs_volume_units;
	uint64_t	fs_volume_avail;
	uint64_t	fs_caller_units;
	uint64_t	fs_caller_avail;
	uint32_t	fs_sectors_per_unit;
	uint32_t	fs_bytes_per_sector;
} smb_fssize_t;

/*
 * SMB FsCtl operations (SMB2 Ioctl, and some SMB1 trans calls)
 */
typedef struct {
	uint32_t CtlCode;
	uint32_t InputCount;
	uint32_t OutputCount;
	uint32_t MaxOutputResp;
	mbuf_chain_t *in_mbc;
	mbuf_chain_t *out_mbc;
} smb_fsctl_t;

typedef struct {
	uint64_t	persistent;
	uint64_t	temporal;
} smb2fid_t;

typedef struct {
	uint32_t status;
	uint16_t errcls;
	uint16_t errcode;
} smb_error_t;

typedef struct open_param {
	smb_fqi_t	fqi;
	uint16_t	omode;
	uint16_t	ofun;
	uint32_t	nt_flags;
	uint32_t	timeo;
	uint32_t	dattr;
	timestruc_t	crtime;
	timestruc_t	mtime;
	timestruc_t	timewarp;
	/*
	 * Careful: dsize is the desired (allocation) size before the
	 * common open function, and the actual size afterwards.
	 */
	uint64_t	dsize;	/* alloc size, actual size */
	uint32_t	desired_access;
	uint32_t	maximum_access;
	uint32_t	share_access;
	uint32_t	create_options;
	uint32_t	create_disposition;
	boolean_t	create_timewarp;
	boolean_t	created_readonly;
	uint32_t	ftype;
	uint32_t	devstate;
	uint32_t	action_taken;
	uint64_t	fileid;
	uint32_t	rootdirfid;
	fsid_t		op_fsid;
	smb_ofile_t	*dir;
	smb_opipe_t	*pipe;	/* for smb_opipe_open */
	struct smb_sd	*sd;	/* for NTTransactCreate */
	void		*create_ctx;

	uint8_t		op_oplock_level;	/* requested/granted level */
	uint32_t	op_oplock_state;	/* internal type+level */
	uint32_t	lease_state;		/* SMB2_LEASE_... */
	uint32_t	lease_flags;
	uint16_t	lease_epoch;
	uint16_t	lease_version;		/* 1 or 2 */
	uint8_t		lease_key[SMB_LEASE_KEY_SZ];	/* from client */
	uint8_t		parent_lease_key[SMB_LEASE_KEY_SZ]; /* for V2 */

	smb_dh_vers_t	dh_vers;
	smb2fid_t	dh_fileid;		/* for durable reconnect */
	uint8_t		create_guid[16];
	uint32_t	dh_v2_flags;
	uint32_t	dh_timeout;
} smb_arg_open_t;

typedef struct smb_arg_lock {
	void		*lvec;
	uint32_t	lcnt;
	uint32_t	lseq;
} smb_arg_lock_t;

typedef struct smb_arg_olbrk {
	uint32_t	NewLevel;
	boolean_t	AckRequired;
} smb_arg_olbrk_t;

/*
 * SMB Request State Machine
 * -------------------------
 *
 *                  T4               +------+		T0
 *      +--------------------------->| FREE |---------------------------+
 *      |                            +------+                           |
 * +-----------+                                                        |
 * | COMPLETED |                                                        |
 * +-----------+
 *      ^                                                               |
 *      | T15                      +-----------+                        v
 * +------------+        T6        |           |                +--------------+
 * | CLEANED_UP |<-----------------| CANCELLED |                | INITIALIZING |
 * +------------+                  |           |                +--------------+
 *      |    ^                     +-----------+                        |
 *      |    |                        ^  ^ ^ ^                          |
 *      |    |          +-------------+  | | |                          |
 *      |    |    T3    |                | | |               T13        | T1
 *      |    +-------------------------+ | | +----------------------+   |
 *      +----------------------------+ | | |                        |   |
 *         T16          |            | | | +-----------+            |   |
 *                      |           \/ | | T5          |            |   v
 * +-----------------+  |   T12     +--------+         |     T2    +-----------+
 * | EVENT_OCCURRED  |------------->| ACTIVE |<--------------------| SUBMITTED |
 * +-----------------+  |           +--------+         |           +-----------+
 *        ^             |              | ^ |           |
 *        |             |           T8 | | |  T7       |
 *        | T10      T9 |   +----------+ | +-------+   |  T11
 *        |             |   |            +-------+ |   |
 *        |             |   |               T14  | |   |
 *        |             |   v                    | v   |
 *      +----------------------+                +--------------+
 *	|     WAITING_EVENT    |                | WAITING_LOCK |
 *      +----------------------+                +--------------+
 *
 *
 *
 *
 *
 * Transition T0
 *
 * This transition occurs when the request is allocated and is still under the
 * control of the session thread.
 *
 * Transition T1
 *
 * This transition occurs when the session thread dispatches a task to treat the
 * request.
 *
 * Transition T2
 *
 *
 *
 * Transition T3
 *
 * A request completes and smbsr_cleanup is called to release resources
 * associated with the request (but not the smb_request_t itself).  This
 * includes references on smb_ofile_t, smb_node_t, and other structures.
 * CLEANED_UP state exists to detect if we attempt to cleanup a request
 * multiple times and to allow us to detect that we are accessing a
 * request that has already been cleaned up.
 *
 * Transition T4
 *
 *
 *
 * Transition T5
 *
 *
 *
 * Transition T6
 *
 *
 *
 * Transition T7
 *
 *
 *
 * Transition T8
 *
 *
 *
 * Transition T9
 *
 *
 *
 * Transition T10
 *
 *
 *
 * Transition T11
 *
 *
 *
 * Transition T12
 *
 *
 *
 * Transition T13
 *
 *
 *
 * Transition T14
 *
 *
 *
 * Transition T15
 *
 * Request processing is completed (control returns from smb_dispatch)
 *
 * Transition T16
 *
 * Multipart (andx) request was cleaned up with smbsr_cleanup but more "andx"
 * sections remain to be processed.
 *
 */

#define	SMB_REQ_MAGIC		0x534D4252	/* 'SMBR' */
#define	SMB_REQ_VALID(p)	ASSERT((p)->sr_magic == SMB_REQ_MAGIC)

typedef enum smb_req_state {
	SMB_REQ_STATE_FREE = 0,
	SMB_REQ_STATE_INITIALIZING,
	SMB_REQ_STATE_SUBMITTED,
	SMB_REQ_STATE_ACTIVE,
	SMB_REQ_STATE_WAITING_AUTH,
	SMB_REQ_STATE_WAITING_FCN1,
	SMB_REQ_STATE_WAITING_FCN2,
	SMB_REQ_STATE_WAITING_LOCK,
	SMB_REQ_STATE_WAITING_PIPE,
	SMB_REQ_STATE_COMPLETED,
	SMB_REQ_STATE_CANCEL_PENDING,
	SMB_REQ_STATE_CANCELLED,
	SMB_REQ_STATE_CLEANED_UP,
	SMB_REQ_STATE_SENTINEL
} smb_req_state_t;

typedef struct smb_request {
	list_node_t		sr_session_lnd;
	uint32_t		sr_magic;
	kmutex_t		sr_mutex;
	smb_req_state_t		sr_state;
	struct smb_server	*sr_server;
	pid_t			*sr_pid;
	int32_t			sr_gmtoff;
	smb_session_t		*session;
	smb_kmod_cfg_t		*sr_cfg;
	void			(*cancel_method)(struct smb_request *);
	void			*cancel_arg2;

	/* Queue used by smb_request_append_postwork. */
	struct smb_request	*sr_postwork;

	list_node_t		sr_waiters;	/* smb_notify.c */

	/* Info from session service header */
	uint32_t		sr_req_length; /* Excluding NBT header */

	/* Request buffer excluding NBT header */
	void			*sr_request_buf;

	struct mbuf_chain	command;
	struct mbuf_chain	reply;
	struct mbuf_chain	raw_data;
	list_t			sr_storage;
	struct smb_xa		*r_xa;
	int			andx_prev_wct;
	int			cur_reply_offset;
	int			orig_request_hdr;
	unsigned int		reply_seqnum;	/* reply sequence number */
	unsigned char		first_smb_com;	/* command code */
	unsigned char		smb_com;	/* command code */

	uint8_t			smb_rcls;	/* error code class */
	uint8_t			smb_reh;	/* rsvd (AH DOS INT-24 ERR) */
	uint16_t		smb_err;	/* error code */
	smb_error_t		smb_error;

	uint8_t			smb_flg;	/* flags */
	uint16_t		smb_flg2;	/* flags */
	unsigned char		smb_sig[8];	/* signiture */
	uint16_t		smb_tid;	/* tree id #  */
	uint32_t		smb_pid;	/* caller's process id # */
	uint16_t		smb_uid;	/* local (smb1) user id # */
	uint16_t		smb_mid;	/* mutiplex id #  */
	unsigned char		smb_wct;	/* count of parameter words */
	uint16_t		smb_bcc;	/* data byte count */

	/*
	 * Beginning offsets (in the mbuf chain) for the
	 * command and reply headers, and the next reply.
	 */
	uint32_t		smb2_cmd_hdr;
	uint32_t		smb2_reply_hdr;
	uint32_t		smb2_next_reply;

	/*
	 * SMB2 header fields.  [MS-SMB2 2.2.1.2]
	 * XXX: Later do a union w smb1 members
	 */
	uint16_t		smb2_credit_charge;
	uint16_t		smb2_chan_seq;	/* cmd only */
	uint32_t		smb2_status;
	uint16_t		smb2_cmd_code;
	uint16_t		smb2_credit_request;
	uint16_t		smb2_credit_response;
	uint16_t		smb2_total_credits; /* in compound */
	uint32_t		smb2_hdr_flags;
	uint32_t		smb2_next_command;
	uint64_t		smb2_messageid;
	uint64_t		smb2_first_msgid;
	/* uint32_t		smb2_pid; use smb_pid */
	/* uint32_t		smb2_tid; use smb_tid */
	uint64_t		smb2_ssnid;	/* See u_ssnid */
	uint8_t			smb2_sig[16];	/* signature */

	/*
	 * SMB3 transform header fields. [MS-SMB2 2.2.41]
	 */
	uint64_t		smb3_tform_ssnid;
	smb_user_t		*tform_ssn;
	uint32_t		msgsize;
	uint8_t			nonce[16];

	boolean_t		encrypted;
	boolean_t		dh_nvl_dirty;

	boolean_t		smb2_async;
	uint64_t		smb2_async_id;
	/* Parameters */
	struct mbuf_chain	smb_vwv;	/* variable width value */

	/* Data */
	struct mbuf_chain	smb_data;

	uint16_t		smb_fid;	/* not in hdr, but common */

	unsigned char		andx_com;
	uint16_t		andx_off;

	struct smb_tree		*tid_tree;
	struct smb_ofile	*fid_ofile;
	smb_user_t		*uid_user;

	cred_t			*user_cr;
	kthread_t		*sr_worker;
	hrtime_t		sr_time_submitted;
	hrtime_t		sr_time_active;
	hrtime_t		sr_time_start;
	int32_t			sr_txb;
	uint32_t		sr_seqnum;

	union {
		smb_arg_negotiate_t	*negprot;
		smb_arg_sessionsetup_t	*ssetup;
		smb_arg_tcon_t		tcon;
		smb_arg_dirop_t		dirop;
		smb_arg_open_t		open;
		smb_arg_lock_t		lock;
		smb_arg_olbrk_t		olbrk;	/* for async oplock break */
		smb_rw_param_t		*rw;
		int32_t			timestamp;
		void			*other;
	} arg;
} smb_request_t;

#define	sr_ssetup	arg.ssetup
#define	sr_negprot	arg.negprot
#define	sr_tcon		arg.tcon
#define	sr_dirop	arg.dirop
#define	sr_open		arg.open
#define	sr_rw		arg.rw
#define	sr_timestamp	arg.timestamp

#define	SMB_READ_PROTOCOL(hdr) \
	LE_IN32(((smb_hdr_t *)(hdr))->protocol)

#define	SMB_PROTOCOL_MAGIC_INVALID(rd_sr) \
	(SMB_READ_PROTOCOL((rd_sr)->sr_request_buf) != SMB_PROTOCOL_MAGIC)

#define	SMB_READ_COMMAND(hdr) \
	(((smb_hdr_t *)(hdr))->command)

#define	SMB_IS_NT_CANCEL(rd_sr) \
	(SMB_READ_COMMAND((rd_sr)->sr_request_buf) == SMB_COM_NT_CANCEL)

#define	SMB_IS_SESSION_SETUP_ANDX(rd_sr) \
	(SMB_READ_COMMAND((rd_sr)->sr_request_buf) == \
	    SMB_COM_SESSION_SETUP_ANDX)

#define	SMB_IS_NT_NEGOTIATE(rd_sr) \
	(SMB_READ_COMMAND((rd_sr)->sr_request_buf) == SMB_COM_NEGOTIATE)

#define	SMB_IS_TREE_CONNECT_ANDX(rd_sr) \
	(SMB_READ_COMMAND((rd_sr)->sr_request_buf) == SMB_COM_TREE_CONNECT_ANDX)

#define	SMB_XA_FLAG_OPEN	0x0001
#define	SMB_XA_FLAG_CLOSE	0x0002
#define	SMB_XA_FLAG_COMPLETE	0x0004
#define	SMB_XA_CLOSED(xa) (!((xa)->xa_flags & SMB_XA_FLAG_OPEN))

#define	SMB_XA_MAGIC		0x534D4258	/* 'SMBX' */

typedef struct smb_xa {
	list_node_t		xa_lnd;
	uint32_t		xa_magic;
	kmutex_t		xa_mutex;

	uint32_t		xa_refcnt;
	uint32_t		xa_flags;

	struct smb_session	*xa_session;

	unsigned char		smb_com;	/* which TRANS type */
	unsigned char		smb_flg;	/* flags */
	uint16_t		smb_flg2;	/* flags */
	uint16_t		smb_tid;	/* tree id number */
	uint32_t		smb_pid;	/* caller's process id */
	uint16_t		smb_uid;	/* user id number */
	uint32_t		smb_func;	/* NT_TRANS function */

	uint16_t		xa_smb_mid;	/* mutiplex id number */
	uint16_t		xa_smb_fid;	/* TRANS2 secondary */

	unsigned int		reply_seqnum;	/* reply sequence number */

	uint32_t	smb_tpscnt;	/* total parameter bytes being sent */
	uint32_t	smb_tdscnt;	/* total data bytes being sent */
	uint32_t	smb_mprcnt;	/* max parameter bytes to return */
	uint32_t	smb_mdrcnt;	/* max data bytes to return */
	uint32_t	smb_msrcnt;	/* max setup words to return */
	uint32_t	smb_flags;	/* additional information: */
				/*  bit 0 - if set, disconnect TID in smb_tid */
				/*  bit 1 - if set, transaction is one way */
				/*  (no final response) */
	int32_t	smb_timeout;	/* number of milliseconds to await completion */
	uint32_t	smb_suwcnt;	/* set up word count */

	char			*xa_pipe_name;

	/*
	 * These are the param and data count received so far,
	 * used to decide if the whole trans is here yet.
	 */
	int			req_disp_param;
	int			req_disp_data;

	struct mbuf_chain	req_setup_mb;
	struct mbuf_chain	req_param_mb;
	struct mbuf_chain	req_data_mb;

	struct mbuf_chain	rep_setup_mb;
	struct mbuf_chain	rep_param_mb;
	struct mbuf_chain	rep_data_mb;
} smb_xa_t;


#define	SDDF_NO_FLAGS			0
#define	SDDF_SUPPRESS_TID		0x0001
#define	SDDF_SUPPRESS_UID		0x0002

/*
 * SMB dispatch return codes.
 */
typedef enum {
	SDRC_SUCCESS = 0,
	SDRC_ERROR,
	SDRC_DROP_VC,
	SDRC_NO_REPLY,
	SDRC_SR_KEPT,
	SDRC_NOT_IMPLEMENTED
} smb_sdrc_t;

#define	VAR_BCC		((short)-1)

#define	SMB_SERVER_MAGIC	0x53534552	/* 'SSER' */
#define	SMB_SERVER_VALID(s)	\
    ASSERT(((s) != NULL) && ((s)->sv_magic == SMB_SERVER_MAGIC))

#define	SMB_LISTENER_MAGIC	0x4C53544E	/* 'LSTN' */
#define	SMB_LISTENER_VALID(ld)	\
    ASSERT(((ld) != NULL) && ((ld)->ld_magic == SMB_LISTENER_MAGIC))

typedef struct {
	uint32_t		ld_magic;
	struct smb_server	*ld_sv;
	smb_thread_t		ld_thread;
	ksocket_t		ld_so;
	in_port_t		ld_port;
	int			ld_family;
	struct sockaddr_in	ld_sin;
	struct sockaddr_in6	ld_sin6;
} smb_listener_daemon_t;

#define	SMB_SSETUP_CMD			"authentication"
#define	SMB_TCON_CMD			"share mapping"
#define	SMB_OPIPE_CMD			"pipe open"
#define	SMB_THRESHOLD_REPORT_THROTTLE	50
typedef struct smb_cmd_threshold {
	char			*ct_cmd;
	kmutex_t		ct_mutex;
	volatile uint32_t	ct_active_cnt;
	volatile uint32_t	ct_blocked_cnt;
	uint32_t		ct_threshold;
	uint32_t		ct_timeout; /* milliseconds */
	kcondvar_t		ct_cond;
} smb_cmd_threshold_t;

typedef struct {
	kstat_named_t		ls_files;
	kstat_named_t		ls_trees;
	kstat_named_t		ls_users;
} smb_server_legacy_kstat_t;

typedef enum smb_server_state {
	SMB_SERVER_STATE_CREATED = 0,
	SMB_SERVER_STATE_CONFIGURED,
	SMB_SERVER_STATE_RUNNING,
	SMB_SERVER_STATE_STOPPING,
	SMB_SERVER_STATE_DELETING,
	SMB_SERVER_STATE_SENTINEL
} smb_server_state_t;

typedef struct {
	/* protected by sv_mutex */
	kcondvar_t		sp_cv;
	uint32_t		sp_cnt;
	smb_llist_t		sp_list;
	smb_llist_t		sp_fidlist;
} smb_spool_t;

#define	SMB_SERVER_STATE_VALID(S)               \
    ASSERT(((S) == SMB_SERVER_STATE_CREATED) || \
	    ((S) == SMB_SERVER_STATE_CONFIGURED) || \
	    ((S) == SMB_SERVER_STATE_RUNNING) ||    \
	    ((S) == SMB_SERVER_STATE_STOPPING) ||   \
	    ((S) == SMB_SERVER_STATE_DELETING))

typedef struct smb_server {
	list_node_t		sv_lnd;
	uint32_t		sv_magic;
	kcondvar_t		sv_cv;
	kmutex_t		sv_mutex;
	smb_server_state_t	sv_state;
	uint32_t		sv_refcnt;
	pid_t			sv_pid;
	zoneid_t		sv_zid;
	smb_listener_daemon_t	sv_nbt_daemon;
	smb_listener_daemon_t	sv_tcp_daemon;
	krwlock_t		sv_cfg_lock;
	smb_kmod_cfg_t		sv_cfg;
	smb_session_t		*sv_session;
	smb_user_t		*sv_rootuser;
	smb_llist_t		sv_session_list;
	smb_hash_t		*sv_persistid_ht;
	smb_hash_t		*sv_lease_ht;

	smb_export_t		sv_export;
	struct __door_handle	*sv_lmshrd;

	/* Internal door for up-calls to smbd */
	struct __door_handle	*sv_kdoor_hd;
	int			sv_kdoor_id; /* init -1 */
	uint64_t		sv_kdoor_ncall;
	kmutex_t		sv_kdoor_mutex;
	kcondvar_t		sv_kdoor_cv;

	int32_t			si_gmtoff;

	smb_thread_t		si_thread_timers;

	taskq_t			*sv_worker_pool;
	taskq_t			*sv_receiver_pool;

	smb_node_t		*si_root_smb_node;
	smb_llist_t		sv_opipe_list;
	smb_llist_t		sv_event_list;

	/* Statistics */
	hrtime_t		sv_start_time;
	kstat_t			*sv_ksp;
	volatile uint32_t	sv_nbt_sess;
	volatile uint32_t	sv_tcp_sess;
	volatile uint32_t	sv_users;
	volatile uint32_t	sv_trees;
	volatile uint32_t	sv_files;
	volatile uint32_t	sv_pipes;
	volatile uint64_t	sv_txb;
	volatile uint64_t	sv_rxb;
	volatile uint64_t	sv_nreq;
	smb_srqueue_t		sv_srqueue;
	smb_spool_t		sp_info;
	smb_cmd_threshold_t	sv_ssetup_ct;
	smb_cmd_threshold_t	sv_tcon_ct;
	smb_cmd_threshold_t	sv_opipe_ct;
	kstat_t			*sv_legacy_ksp;
	kmutex_t		sv_legacy_ksmtx;
	smb_disp_stats_t	*sv_disp_stats1;
	smb_disp_stats_t	*sv_disp_stats2;
} smb_server_t;

#define	SMB_EVENT_MAGIC		0x45564E54	/* EVNT */
#define	SMB_EVENT_TIMEOUT	45		/* seconds */
#define	SMB_EVENT_VALID(e)	\
    ASSERT(((e) != NULL) && ((e)->se_magic == SMB_EVENT_MAGIC))
typedef struct smb_event {
	list_node_t		se_lnd;
	uint32_t		se_magic;
	kmutex_t		se_mutex;
	kcondvar_t		se_cv;
	smb_server_t		*se_server;
	uint32_t		se_txid;
	boolean_t		se_notified;
	int			se_waittime;
	int			se_timeout;
	int			se_errno;
} smb_event_t;

typedef struct smb_kspooldoc {
	list_node_t	sd_lnd;
	uint32_t	sd_magic;
	smb_inaddr_t	sd_ipaddr;
	uint32_t	sd_spool_num;
	uint16_t	sd_fid;
	char		sd_username[MAXNAMELEN];
	char		sd_path[MAXPATHLEN];
} smb_kspooldoc_t;

typedef struct smb_spoolfid {
	list_node_t	sf_lnd;
	uint32_t	sf_magic;
	uint16_t	sf_fid;
} smb_spoolfid_t;

#define	SMB_INFO_NETBIOS_SESSION_SVC_RUNNING	0x0001
#define	SMB_INFO_NETBIOS_SESSION_SVC_FAILED	0x0002
#define	SMB_INFO_USER_LEVEL_SECURITY		0x40000000
#define	SMB_INFO_ENCRYPT_PASSWORDS		0x80000000

#define	SMB_IS_STREAM(node) ((node)->n_unode)

typedef struct smb_tsd {
	void (*proc)();
	void *arg;
	char name[100];
} smb_tsd_t;

typedef struct smb_disp_entry {
	char		sdt_name[KSTAT_STRLEN];
	smb_sdrc_t	(*sdt_pre_op)(smb_request_t *);
	smb_sdrc_t	(*sdt_function)(smb_request_t *);
	void		(*sdt_post_op)(smb_request_t *);
	uint8_t		sdt_com;
	char		sdt_dialect;
	uint8_t		sdt_flags;
} smb_disp_entry_t;

typedef struct smb_xlate {
	int	code;
	char	*str;
} smb_xlate_t;

/*
 * This structure is a helper for building RAP NetShareEnum response
 *
 * es_posix_uid UID of the user requesting the shares list which
 *              is used to detect if the user has any autohome
 * es_bufsize   size of the response buffer
 * es_buf       pointer to the response buffer
 * es_ntotal    total number of shares exported by server which
 *              their OEM names is less then 13 chars
 * es_nsent     number of shares that can fit in the specified buffer
 * es_datasize  actual data size (share's data) which was encoded
 *              in the response buffer
 */
typedef struct smb_enumshare_info {
	uid_t		es_posix_uid;
	uint16_t	es_bufsize;
	char		*es_buf;
	uint16_t	es_ntotal;
	uint16_t	es_nsent;
	uint16_t	es_datasize;
} smb_enumshare_info_t;

#ifdef	__cplusplus
}
#endif

#endif /* _SMBSRV_SMB_KTYPES_H */
