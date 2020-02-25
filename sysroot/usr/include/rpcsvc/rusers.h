/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _RUSERS_H_RPCGEN
#define	_RUSERS_H_RPCGEN

#include <rpc/rpc.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Find out about remote users
 */
#define	RUSERS_MAXUSERLEN 32
#define	RUSERS_MAXLINELEN 32
#define	RUSERS_MAXHOSTLEN 257

struct rusers_utmp {
	char *ut_user;
	char *ut_line;
	char *ut_host;
	int ut_type;
	int ut_time;
	u_int ut_idle;
};
typedef struct rusers_utmp rusers_utmp;

typedef struct {
	u_int utmp_array_len;
	rusers_utmp *utmp_array_val;
} utmp_array;

/*
 * Values for ut_type field above.
 */
#define	RUSERS_EMPTY 0
#define	RUSERS_RUN_LVL 1
#define	RUSERS_BOOT_TIME 2
#define	RUSERS_OLD_TIME 3
#define	RUSERS_NEW_TIME 4
#define	RUSERS_INIT_PROCESS 5
#define	RUSERS_LOGIN_PROCESS 6
#define	RUSERS_USER_PROCESS 7
#define	RUSERS_DEAD_PROCESS 8
#define	RUSERS_ACCOUNTING 9



/*
 * The following structures are used by version 2 of the rusersd protocol.
 * They were not developed with rpcgen, so they do not appear as RPCL.
 */

#define RUSERSVERS_IDLE 2
#define RUSERSVERS 3 /* current version */
#define MAXUSERS 100

/*
 * This is the structure used in version 2 of the rusersd RPC service.
 * It corresponds to the utmp structure for BSD systems.
 */
struct ru_utmp {
 char ut_line[8]; /* tty name */
 char ut_name[8]; /* user id */
 char ut_host[16]; /* host name, if remote */
 time_t ut_time; /* time on */
};

struct utmpidle {
 struct ru_utmp ui_utmp;
 unsigned ui_idle;
};

struct utmpidlearr {
 struct utmpidle **uia_arr;
 int uia_cnt;
};

int xdr_utmpidlearr();

#if defined(__STDC__) || defined(__cplusplus)
enum clnt_stat rusers(char *host, struct utmpidlearr *up);
int rnusers(char *host);
#else
enum clnt_stat rusers();
int rnusers();
#endif


#define	RUSERSPROG	100002
#define	RUSERSVERS_3	3

#if defined(__STDC__) || defined(__cplusplus)
#define	RUSERSPROC_NUM	1
extern  int * rusersproc_num_3(void *, CLIENT *);
extern  int * rusersproc_num_3_svc(void *, struct svc_req *);
#define	RUSERSPROC_NAMES	2
extern  utmp_array * rusersproc_names_3(void *, CLIENT *);
extern  utmp_array * rusersproc_names_3_svc(void *, struct svc_req *);
#define	RUSERSPROC_ALLNAMES	3
extern  utmp_array * rusersproc_allnames_3(void *, CLIENT *);
extern  utmp_array * rusersproc_allnames_3_svc(void *, struct svc_req *);
extern int rusersprog_3_freeresult(SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define	RUSERSPROC_NUM	1
extern  int * rusersproc_num_3();
extern  int * rusersproc_num_3_svc();
#define	RUSERSPROC_NAMES	2
extern  utmp_array * rusersproc_names_3();
extern  utmp_array * rusersproc_names_3_svc();
#define	RUSERSPROC_ALLNAMES	3
extern  utmp_array * rusersproc_allnames_3();
extern  utmp_array * rusersproc_allnames_3_svc();
extern int rusersprog_3_freeresult();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_rusers_utmp(XDR *, rusers_utmp*);
extern  bool_t xdr_utmp_array(XDR *, utmp_array*);

#else /* K&R C */
extern bool_t xdr_rusers_utmp();
extern bool_t xdr_utmp_array();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_RUSERS_H_RPCGEN */
