/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _NFS_ACL_H_RPCGEN
#define	_NFS_ACL_H_RPCGEN

#include <rpc/rpc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define	NFS_ACL_MAX_ENTRIES 1024

typedef int uid;

typedef u_short o_mode;

struct aclent {
	int type;
	uid id;
	o_mode perm;
};
typedef struct aclent aclent;
#define	NA_USER_OBJ 0x1
#define	NA_USER 0x2
#define	NA_GROUP_OBJ 0x4
#define	NA_GROUP 0x8
#define	NA_CLASS_OBJ 0x10
#define	NA_OTHER_OBJ 0x20
#define	NA_ACL_DEFAULT 0x1000
#define	NA_READ 0x4
#define	NA_WRITE 0x2
#define	NA_EXEC 0x1

struct secattr {
	u_int mask;
	int aclcnt;
	struct {
		u_int aclent_len;
		aclent *aclent_val;
	} aclent;
	int dfaclcnt;
	struct {
		u_int dfaclent_len;
		aclent *dfaclent_val;
	} dfaclent;
};
typedef struct secattr secattr;
#define	NA_ACL 0x1
#define	NA_ACLCNT 0x2
#define	NA_DFACL 0x4
#define	NA_DFACLCNT 0x8

struct GETACL2args {
	fhandle_t fh;
	u_int mask;
};
typedef struct GETACL2args GETACL2args;

struct GETACL2resok {
	struct nfsfattr attr;
	secattr acl;
};
typedef struct GETACL2resok GETACL2resok;

struct GETACL2res {
	nfsstat status;
	union {
		GETACL2resok resok;
	} GETACL2res_u;
};
typedef struct GETACL2res GETACL2res;

struct SETACL2args {
	fhandle_t fh;
	secattr acl;
};
typedef struct SETACL2args SETACL2args;

struct SETACL2resok {
	struct nfsfattr attr;
};
typedef struct SETACL2resok SETACL2resok;

struct SETACL2res {
	nfsstat status;
	union {
		SETACL2resok resok;
	} SETACL2res_u;
};
typedef struct SETACL2res SETACL2res;

struct GETATTR2args {
	fhandle_t fh;
};
typedef struct GETATTR2args GETATTR2args;

struct GETATTR2resok {
	struct nfsfattr attr;
};
typedef struct GETATTR2resok GETATTR2resok;

struct GETATTR2res {
	nfsstat status;
	union {
		GETATTR2resok resok;
	} GETATTR2res_u;
};
typedef struct GETATTR2res GETATTR2res;

struct ACCESS2args {
	fhandle_t fh;
	uint32 access;
};
typedef struct ACCESS2args ACCESS2args;
#define	ACCESS2_READ 0x1
#define	ACCESS2_LOOKUP 0x2
#define	ACCESS2_MODIFY 0x4
#define	ACCESS2_EXTEND 0x8
#define	ACCESS2_DELETE 0x10
#define	ACCESS2_EXECUTE 0x20

struct ACCESS2resok {
	struct nfsfattr attr;
	uint32 access;
};
typedef struct ACCESS2resok ACCESS2resok;

struct ACCESS2res {
	nfsstat status;
	union {
		ACCESS2resok resok;
	} ACCESS2res_u;
};
typedef struct ACCESS2res ACCESS2res;

struct GETXATTRDIR2args {
	fhandle_t fh;
	bool_t create;
};
typedef struct GETXATTRDIR2args GETXATTRDIR2args;

struct GETXATTRDIR2resok {
	fhandle_t fh;
	struct nfsfattr attr;
};
typedef struct GETXATTRDIR2resok GETXATTRDIR2resok;

struct GETXATTRDIR2res {
	nfsstat status;
	union {
		GETXATTRDIR2resok resok;
	} GETXATTRDIR2res_u;
};
typedef struct GETXATTRDIR2res GETXATTRDIR2res;

struct GETACL3args {
	nfs_fh3 fh;
	u_int mask;
};
typedef struct GETACL3args GETACL3args;

struct GETACL3resok {
	post_op_attr attr;
	secattr acl;
};
typedef struct GETACL3resok GETACL3resok;

struct GETACL3resfail {
	post_op_attr attr;
};
typedef struct GETACL3resfail GETACL3resfail;

struct GETACL3res {
	nfsstat3 status;
	union {
		GETACL3resok resok;
		GETACL3resfail resfail;
	} GETACL3res_u;
};
typedef struct GETACL3res GETACL3res;

struct SETACL3args {
	nfs_fh3 fh;
	secattr acl;
};
typedef struct SETACL3args SETACL3args;

struct SETACL3resok {
	post_op_attr attr;
};
typedef struct SETACL3resok SETACL3resok;

struct SETACL3resfail {
	post_op_attr attr;
};
typedef struct SETACL3resfail SETACL3resfail;

struct SETACL3res {
	nfsstat3 status;
	union {
		SETACL3resok resok;
		SETACL3resfail resfail;
	} SETACL3res_u;
};
typedef struct SETACL3res SETACL3res;

struct GETXATTRDIR3args {
	nfs_fh3 fh;
	bool_t create;
};
typedef struct GETXATTRDIR3args GETXATTRDIR3args;

struct GETXATTRDIR3resok {
	nfs_fh3 fh;
	post_op_attr attr;
};
typedef struct GETXATTRDIR3resok GETXATTRDIR3resok;

struct GETXATTRDIR3res {
	nfsstat3 status;
	union {
		GETXATTRDIR3resok resok;
	} GETXATTRDIR3res_u;
};
typedef struct GETXATTRDIR3res GETXATTRDIR3res;

struct GETACL4args {
	nfs_fh4 fh;
	u_int mask;
};
typedef struct GETACL4args GETACL4args;

struct GETACL4resok {
	post_op_attr attr;
	secattr acl;
};
typedef struct GETACL4resok GETACL4resok;

struct GETACL4resfail {
	post_op_attr attr;
};
typedef struct GETACL4resfail GETACL4resfail;

struct GETACL4res {
	nfsstat3 status;
	union {
		GETACL4resok resok;
		GETACL4resfail resfail;
	} GETACL4res_u;
};
typedef struct GETACL4res GETACL4res;

struct SETACL4args {
	nfs_fh4 fh;
	secattr acl;
};
typedef struct SETACL4args SETACL4args;

struct SETACL4resok {
	post_op_attr attr;
};
typedef struct SETACL4resok SETACL4resok;

struct SETACL4resfail {
	post_op_attr attr;
};
typedef struct SETACL4resfail SETACL4resfail;

struct SETACL4res {
	nfsstat3 status;
	union {
		SETACL4resok resok;
		SETACL4resfail resfail;
	} SETACL4res_u;
};
typedef struct SETACL4res SETACL4res;
#define	NFS_ACL_PORT 2049

#define	NFS_ACL_PROGRAM	100227
#define	NFS_ACL_V2	2

#if defined(__STDC__) || defined(__cplusplus)
#define	ACLPROC2_NULL	0
extern  void * aclproc2_null_2(void *, CLIENT *);
extern  void * aclproc2_null_2_svc(void *, struct svc_req *);
#define	ACLPROC2_GETACL	1
extern  GETACL2res * aclproc2_getacl_2(GETACL2args *, CLIENT *);
extern  GETACL2res * aclproc2_getacl_2_svc(GETACL2args *, struct svc_req *);
#define	ACLPROC2_SETACL	2
extern  SETACL2res * aclproc2_setacl_2(SETACL2args *, CLIENT *);
extern  SETACL2res * aclproc2_setacl_2_svc(SETACL2args *, struct svc_req *);
#define	ACLPROC2_GETATTR	3
extern  GETATTR2res * aclproc2_getattr_2(GETATTR2args *, CLIENT *);
extern  GETATTR2res * aclproc2_getattr_2_svc(GETATTR2args *, struct svc_req *);
#define	ACLPROC2_ACCESS	4
extern  ACCESS2res * aclproc2_access_2(ACCESS2args *, CLIENT *);
extern  ACCESS2res * aclproc2_access_2_svc(ACCESS2args *, struct svc_req *);
#define	ACLPROC2_GETXATTRDIR	5
extern  GETXATTRDIR2res * aclproc2_getxattrdir_2(GETXATTRDIR2args *, CLIENT *);
extern  GETXATTRDIR2res * aclproc2_getxattrdir_2_svc(GETXATTRDIR2args *, struct svc_req *);
extern int nfs_acl_program_2_freeresult(SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define	ACLPROC2_NULL	0
extern  void * aclproc2_null_2();
extern  void * aclproc2_null_2_svc();
#define	ACLPROC2_GETACL	1
extern  GETACL2res * aclproc2_getacl_2();
extern  GETACL2res * aclproc2_getacl_2_svc();
#define	ACLPROC2_SETACL	2
extern  SETACL2res * aclproc2_setacl_2();
extern  SETACL2res * aclproc2_setacl_2_svc();
#define	ACLPROC2_GETATTR	3
extern  GETATTR2res * aclproc2_getattr_2();
extern  GETATTR2res * aclproc2_getattr_2_svc();
#define	ACLPROC2_ACCESS	4
extern  ACCESS2res * aclproc2_access_2();
extern  ACCESS2res * aclproc2_access_2_svc();
#define	ACLPROC2_GETXATTRDIR	5
extern  GETXATTRDIR2res * aclproc2_getxattrdir_2();
extern  GETXATTRDIR2res * aclproc2_getxattrdir_2_svc();
extern int nfs_acl_program_2_freeresult();
#endif /* K&R C */
#define	NFS_ACL_V3	3

#if defined(__STDC__) || defined(__cplusplus)
#define	ACLPROC3_NULL	0
extern  void * aclproc3_null_3(void *, CLIENT *);
extern  void * aclproc3_null_3_svc(void *, struct svc_req *);
#define	ACLPROC3_GETACL	1
extern  GETACL3res * aclproc3_getacl_3(GETACL3args *, CLIENT *);
extern  GETACL3res * aclproc3_getacl_3_svc(GETACL3args *, struct svc_req *);
#define	ACLPROC3_SETACL	2
extern  SETACL3res * aclproc3_setacl_3(SETACL3args *, CLIENT *);
extern  SETACL3res * aclproc3_setacl_3_svc(SETACL3args *, struct svc_req *);
#define	ACLPROC3_GETXATTRDIR	3
extern  GETXATTRDIR3res * aclproc3_getxattrdir_3(GETXATTRDIR3args *, CLIENT *);
extern  GETXATTRDIR3res * aclproc3_getxattrdir_3_svc(GETXATTRDIR3args *, struct svc_req *);
extern int nfs_acl_program_3_freeresult(SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define	ACLPROC3_NULL	0
extern  void * aclproc3_null_3();
extern  void * aclproc3_null_3_svc();
#define	ACLPROC3_GETACL	1
extern  GETACL3res * aclproc3_getacl_3();
extern  GETACL3res * aclproc3_getacl_3_svc();
#define	ACLPROC3_SETACL	2
extern  SETACL3res * aclproc3_setacl_3();
extern  SETACL3res * aclproc3_setacl_3_svc();
#define	ACLPROC3_GETXATTRDIR	3
extern  GETXATTRDIR3res * aclproc3_getxattrdir_3();
extern  GETXATTRDIR3res * aclproc3_getxattrdir_3_svc();
extern int nfs_acl_program_3_freeresult();
#endif /* K&R C */
#define	NFS_ACL_V4	4

#if defined(__STDC__) || defined(__cplusplus)
#define	ACLPROC4_NULL	0
extern  void * aclproc4_null_4(void *, CLIENT *);
extern  void * aclproc4_null_4_svc(void *, struct svc_req *);
#define	ACLPROC4_GETACL	1
extern  GETACL4res * aclproc4_getacl_4(GETACL4args *, CLIENT *);
extern  GETACL4res * aclproc4_getacl_4_svc(GETACL4args *, struct svc_req *);
#define	ACLPROC4_SETACL	2
extern  SETACL4res * aclproc4_setacl_4(SETACL4args *, CLIENT *);
extern  SETACL4res * aclproc4_setacl_4_svc(SETACL4args *, struct svc_req *);
extern int nfs_acl_program_4_freeresult(SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define	ACLPROC4_NULL	0
extern  void * aclproc4_null_4();
extern  void * aclproc4_null_4_svc();
#define	ACLPROC4_GETACL	1
extern  GETACL4res * aclproc4_getacl_4();
extern  GETACL4res * aclproc4_getacl_4_svc();
#define	ACLPROC4_SETACL	2
extern  SETACL4res * aclproc4_setacl_4();
extern  SETACL4res * aclproc4_setacl_4_svc();
extern int nfs_acl_program_4_freeresult();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_uid(XDR *, uid*);
extern  bool_t xdr_o_mode(XDR *, o_mode*);
extern  bool_t xdr_aclent(XDR *, aclent*);
extern  bool_t xdr_secattr(XDR *, secattr*);
extern  bool_t xdr_GETACL2args(XDR *, GETACL2args*);
extern  bool_t xdr_GETACL2resok(XDR *, GETACL2resok*);
extern  bool_t xdr_GETACL2res(XDR *, GETACL2res*);
extern  bool_t xdr_SETACL2args(XDR *, SETACL2args*);
extern  bool_t xdr_SETACL2resok(XDR *, SETACL2resok*);
extern  bool_t xdr_SETACL2res(XDR *, SETACL2res*);
extern  bool_t xdr_GETATTR2args(XDR *, GETATTR2args*);
extern  bool_t xdr_GETATTR2resok(XDR *, GETATTR2resok*);
extern  bool_t xdr_GETATTR2res(XDR *, GETATTR2res*);
extern  bool_t xdr_ACCESS2args(XDR *, ACCESS2args*);
extern  bool_t xdr_ACCESS2resok(XDR *, ACCESS2resok*);
extern  bool_t xdr_ACCESS2res(XDR *, ACCESS2res*);
extern  bool_t xdr_GETXATTRDIR2args(XDR *, GETXATTRDIR2args*);
extern  bool_t xdr_GETXATTRDIR2resok(XDR *, GETXATTRDIR2resok*);
extern  bool_t xdr_GETXATTRDIR2res(XDR *, GETXATTRDIR2res*);
extern  bool_t xdr_GETACL3args(XDR *, GETACL3args*);
extern  bool_t xdr_GETACL3resok(XDR *, GETACL3resok*);
extern  bool_t xdr_GETACL3resfail(XDR *, GETACL3resfail*);
extern  bool_t xdr_GETACL3res(XDR *, GETACL3res*);
extern  bool_t xdr_SETACL3args(XDR *, SETACL3args*);
extern  bool_t xdr_SETACL3resok(XDR *, SETACL3resok*);
extern  bool_t xdr_SETACL3resfail(XDR *, SETACL3resfail*);
extern  bool_t xdr_SETACL3res(XDR *, SETACL3res*);
extern  bool_t xdr_GETXATTRDIR3args(XDR *, GETXATTRDIR3args*);
extern  bool_t xdr_GETXATTRDIR3resok(XDR *, GETXATTRDIR3resok*);
extern  bool_t xdr_GETXATTRDIR3res(XDR *, GETXATTRDIR3res*);
extern  bool_t xdr_GETACL4args(XDR *, GETACL4args*);
extern  bool_t xdr_GETACL4resok(XDR *, GETACL4resok*);
extern  bool_t xdr_GETACL4resfail(XDR *, GETACL4resfail*);
extern  bool_t xdr_GETACL4res(XDR *, GETACL4res*);
extern  bool_t xdr_SETACL4args(XDR *, SETACL4args*);
extern  bool_t xdr_SETACL4resok(XDR *, SETACL4resok*);
extern  bool_t xdr_SETACL4resfail(XDR *, SETACL4resfail*);
extern  bool_t xdr_SETACL4res(XDR *, SETACL4res*);

#else /* K&R C */
extern bool_t xdr_uid();
extern bool_t xdr_o_mode();
extern bool_t xdr_aclent();
extern bool_t xdr_secattr();
extern bool_t xdr_GETACL2args();
extern bool_t xdr_GETACL2resok();
extern bool_t xdr_GETACL2res();
extern bool_t xdr_SETACL2args();
extern bool_t xdr_SETACL2resok();
extern bool_t xdr_SETACL2res();
extern bool_t xdr_GETATTR2args();
extern bool_t xdr_GETATTR2resok();
extern bool_t xdr_GETATTR2res();
extern bool_t xdr_ACCESS2args();
extern bool_t xdr_ACCESS2resok();
extern bool_t xdr_ACCESS2res();
extern bool_t xdr_GETXATTRDIR2args();
extern bool_t xdr_GETXATTRDIR2resok();
extern bool_t xdr_GETXATTRDIR2res();
extern bool_t xdr_GETACL3args();
extern bool_t xdr_GETACL3resok();
extern bool_t xdr_GETACL3resfail();
extern bool_t xdr_GETACL3res();
extern bool_t xdr_SETACL3args();
extern bool_t xdr_SETACL3resok();
extern bool_t xdr_SETACL3resfail();
extern bool_t xdr_SETACL3res();
extern bool_t xdr_GETXATTRDIR3args();
extern bool_t xdr_GETXATTRDIR3resok();
extern bool_t xdr_GETXATTRDIR3res();
extern bool_t xdr_GETACL4args();
extern bool_t xdr_GETACL4resok();
extern bool_t xdr_GETACL4resfail();
extern bool_t xdr_GETACL4res();
extern bool_t xdr_SETACL4args();
extern bool_t xdr_SETACL4resok();
extern bool_t xdr_SETACL4resfail();
extern bool_t xdr_SETACL4res();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_NFS_ACL_H_RPCGEN */
