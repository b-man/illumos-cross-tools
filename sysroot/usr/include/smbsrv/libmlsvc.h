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
 * Copyright (c) 2007, 2010, Oracle and/or its affiliates. All rights reserved.
 * Copyright 2019 Nexenta Systems, Inc.  All rights reserved.
 */

#ifndef	_LIBMLSVC_H
#define	_LIBMLSVC_H

#include <sys/param.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/ksynch.h>
#include <uuid/uuid.h>

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <netdb.h>
#include <libuutil.h>

#include <smb/wintypes.h>
#include <libmlrpc/libmlrpc.h>

#include <smbsrv/hash_table.h>
#include <smbsrv/smb_token.h>
#include <smbsrv/smb_privilege.h>
#include <smbsrv/smb_share.h>
#include <smbsrv/smb_xdr.h>
#include <smbsrv/smb_dfs.h>
#include <smbsrv/libsmb.h>

/*
 * XXX: Some temporary left-overs from the old ntstatus.h
 * Should eliminate uses of these macros when convenient.
 */
/* This used to OR in the severity bits. */
#define	NT_SC_ERROR(S)		(S)
/* This used to mask off the severity bits. */
#define	NT_SC_VALUE(S)		(S)
/* XXX end of temporary left-overs. */

#ifdef	__cplusplus
extern "C" {
#endif

uint32_t lsa_lookup_name(char *, uint16_t, smb_account_t *);
uint32_t lsa_lookup_lname(char *, uint16_t, smb_account_t *);
uint32_t lsa_lookup_sid(smb_sid_t *, smb_account_t *);
uint32_t lsa_lookup_lsid(smb_sid_t *, smb_account_t *);

/*
 * SMB domain API to discover a domain controller and obtain domain
 * information.
 */

extern boolean_t smb_locate_dc(char *, smb_domainex_t *);
uint32_t smb_ddiscover_dns(char *, smb_domainex_t *);
extern void smb_ddiscover_bad_dc(char *);
extern void smb_ddiscover_refresh(void);

extern int dssetup_check_service(void);
extern void dssetup_clear_domain_info(void);
extern void mlsvc_disconnect(const char *);
extern int mlsvc_init(void);
extern void mlsvc_fini(void);
extern DWORD mlsvc_netlogon(char *, char *);
extern void mlsvc_join(smb_joininfo_t *, smb_joinres_t *);

extern void smb_logon_domain(smb_logon_t *, smb_token_t *);
extern uint32_t smb_decode_krb5_pac(smb_token_t *, char *, uint_t);
extern boolean_t smb_token_setup_common(smb_token_t *);


/*
 * The maximum number of domains (NT limit).
 */
#define	MLSVC_DOMAIN_MAX		32

#define	MLSVC_ANON_USER			"IPC$"

char *mlsvc_ipc_name(int ipc_type, char *username);

/*
 * Passthrough negotiation and authentication interface.
 *
 * NT supports two forms of password: a Lanman (case-insensitive)
 * password and an NT (case-sensitive) password. If either of the
 * passwords is not available its pointer and length should be set
 * to zero. The session key and vc number are required to validate
 * the encrypted passwords.
 */

void mlsvc_nt_password_hash(char *result, char *password);
int mlsvc_encrypt_nt_password(char *password, char *key, int keylen, char *out,
    int outmax);

#define	SMB_AUTOHOME_FILE	"smbautohome"
#define	SMB_AUTOHOME_PATH	"/etc"

typedef struct smb_autohome {
	struct smb_autohome *ah_next;
	uint32_t ah_hits;
	time_t ah_timestamp;
	char *ah_name;		/* User account name */
	char *ah_path;		/* Home directory path */
	char *ah_container;	/* ADS container distinguished name */
} smb_autohome_t;

extern void smb_autohome_add(const smb_token_t *);
extern void smb_autohome_remove(const char *);

/*
 * A local unique id (LUID) is an opaque id used by servers to identify
 * local resources, such as privileges.  A client will use lookup
 * functions to translate the LUID to a more general, machine independent
 * form; such as a string.
 */
typedef struct ms_luid {
	uint32_t low_part;
	uint32_t high_part;
} ms_luid_t;

/*
 * A client_t is created while binding a client connection to hold the
 * context for calls made using that connection.
 *
 * Handles are RPC call specific and we use an inheritance mechanism to
 * ensure that each handle has a pointer to the client_t.  When the top
 * level (bind) handle is released, we close the connection.
 */
typedef struct mlrpc_handle mlsvc_handle_t;

/* mlsvc_client.c */
void ndr_rpc_init(void);
void ndr_rpc_fini(void);
uint32_t ndr_rpc_bind(mlsvc_handle_t *, char *, char *, char *, const char *);
void ndr_rpc_unbind(mlsvc_handle_t *);
void ndr_rpc_status(mlsvc_handle_t *, int, uint32_t);

/* These three get info about the connected client. */
boolean_t ndr_is_admin(ndr_xa_t *);
boolean_t ndr_is_poweruser(ndr_xa_t *);
int32_t ndr_native_os(ndr_xa_t *);

/* SRVSVC */
int srvsvc_net_remote_tod(char *, char *, struct timeval *, struct tm *);

/* SVCCTL service */
/*
 * Calculate the wide-char equivalent string length required to
 * store a string - including the terminating null wide-char.
 */
#define	SVCCTL_WNSTRLEN(S)	((strlen((S)) + 1) * sizeof (smb_wchar_t))

/* An AVL-storable node representing each service in the SCM database. */
typedef struct svcctl_svc_node {
	uu_avl_node_t		sn_node;
	char			*sn_name;	/* Service Name (Key) */
	char			*sn_fmri;	/* Display Name (FMRI) */
	char			*sn_desc;	/* Description */
	char			*sn_state;	/* State */
} svcctl_svc_node_t;

/* This structure provides context for each svcctl_s_OpenManager call. */
typedef struct svcctl_manager_context {
	scf_handle_t		*mc_scf_hdl;	  /* SCF handle */
	scf_propertygroup_t	*mc_scf_gpg;	  /* Property group */
	scf_property_t		*mc_scf_gprop;	  /* Property */
	scf_value_t		*mc_scf_gval;	  /* Value */
	uint32_t		mc_scf_numsvcs;   /* Number of SMF services */
	ssize_t			mc_scf_max_fmri_len;  /* Max FMRI length */
	ssize_t			mc_scf_max_value_len; /* Max Value length */
	uint32_t		mc_bytes_needed;  /* Number of bytes needed */
	uu_avl_pool_t		*mc_svcs_pool;	  /* AVL pool */
	uu_avl_t		*mc_svcs;	  /* AVL tree of SMF services */
} svcctl_manager_context_t;

/* This structure provides context for each svcctl_s_OpenService call. */
typedef struct svcctl_service_context {
	ndr_hdid_t		*sc_mgrid;	/* Manager ID */
	char			*sc_svcname;    /* Service Name */
} svcctl_service_context_t;

typedef enum {
	SVCCTL_MANAGER_CONTEXT = 0,
	SVCCTL_SERVICE_CONTEXT
} svcctl_context_type_t;

/* This structure provides abstraction for service and manager context call. */
typedef struct svcctl_context {
	svcctl_context_type_t	c_type;
	union {
		svcctl_manager_context_t *uc_mgr;
		svcctl_service_context_t *uc_svc;
		void *uc_cp;
	} c_ctx;
} svcctl_context_t;

/* Service Control Manager (SCM) functions */
void svcctl_init(void);
void svcctl_fini(void);
int svcctl_scm_init(svcctl_manager_context_t *);
void svcctl_scm_fini(svcctl_manager_context_t *);
int svcctl_scm_scf_handle_init(svcctl_manager_context_t *);
void svcctl_scm_scf_handle_fini(svcctl_manager_context_t *);
int svcctl_scm_refresh(svcctl_manager_context_t *);
uint32_t svcctl_scm_enum_services(svcctl_manager_context_t *, uint8_t *,
    size_t, uint32_t *, boolean_t);
uint32_t svcctl_scm_validate_service(svcctl_manager_context_t *, char *);
svcctl_svc_node_t *svcctl_scm_find_service(svcctl_manager_context_t *, char *);
uint32_t svcctl_scm_map_status(const char *);

/* LOGR service */
#define	LOGR_APPLICATION_LOG		"Application"
#define	LOGR_SECURITY_LOG		"Security"
#define	LOGR_SYSTEM_LOG			"System"
#define	LOGR_NMSGMASK			1023
#define	LOGR_MAXMSGLEN			800

typedef struct logr_entry {
	struct timeval	le_timestamp;			/* Time of log entry */
	int		le_pri;				/* Message priority */
	char		le_hostname[MAXHOSTNAMELEN];	/* Log hostname */
	char		le_msg[LOGR_MAXMSGLEN];		/* Log message text */
} logr_entry_t;

typedef struct logr_info {
	logr_entry_t	li_entry[LOGR_NMSGMASK+1];	/* Array of log entry */
	int		li_idx;				/* Index */
} logr_info_t;

typedef struct logr_read_data {
	int		rd_tot_recnum;		/* Total no. of record read */
	int		rd_last_sentrec;	/* Last sentence read */
	char		rd_first_read;		/* First sentence read */
	logr_info_t	*rd_log;		/* Log information read */
} logr_read_data_t;

/* This structure provides the context for eventlog calls from clients. */
typedef struct logr_context {
	logr_read_data_t *lc_cached_read_data;
	char *lc_source_name;
} logr_context_t;

void logr_init(void);
void logr_fini(void);
boolean_t logr_is_supported(char *);
int logr_get_snapshot(logr_context_t *);

/* User and Group quotas */
uint32_t smb_quota_query(smb_quota_query_t *, smb_quota_response_t *);
uint32_t smb_quota_set(smb_quota_set_t *);
void smb_quota_free(smb_quota_response_t *);

uint32_t dfs_get_referrals(const char *, dfs_reftype_t, dfs_info_t *);
void dfs_info_free(dfs_info_t *);

/*
 * The spoolss installable copyfile API.
 */
typedef void (*spoolss_copyfile_t)(smb_inaddr_t *, char *, char *, char *);
void spoolss_register_copyfile(spoolss_copyfile_t);

#ifdef	__cplusplus
}
#endif

#endif	/* _LIBMLSVC_H */
