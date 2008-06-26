#ifndef _RS_POSTGRESQL_H
#define _RS_POSTGRESQL_H 1

/* 
 *    RS-PostgreSQL.h     Last Modified:
 *
 * This package was developed as a part of Summer of Code program organized by Google.
 * Thanks to David A. James & Saikat DebRoy, the authors of RMySQL package.
 * Code from RMySQL package was reused with the permission from the authors.
 * Also Thanks to my GSoC mentor Dirk Eddelbuettel for helping me in the development.
 */


#ifdef _cplusplus
extern  "C" {
#endif

/* We use HAVE_GETOPT_LONG to signal we can use getopt_long() 
 * (by default we assume we're running on a GNU-aware system)
 */


#include "libpq-fe.h"
#include <string.h>
#include "RS-DBI.h"


/* Note that the default number of maximum connections to the PostgreSQL server is typically 100
 *  connections, but may be less if your kernel settings will not support it (as determined during initdb)
* Refer to: http://www.postgresql.org/docs/8.2/interactive/runtime-config-resource.html for details
 */


typedef struct st_sdbi_conParams {
char *user;
char *password;
char *host;
char *dbname;
char *port;
char *tty;
char *options;
}RS_PostgreSQL_conParams;

RS_PostgreSQL_conParams *RS_PostgreSQL_allocConParams(void);

void RS_PostgreSQL_freeConParams(RS_PostgreSQL_conParams *conParams);

/* The following functions are the S/R entry points into the C implementation
 * (i.e., these are the only ones visible from R/S) we use the prefix
 * "RS_PostgreSQL" in function names to denote this.
 * These functions are  built on top of the underlying RS_DBI manager, 
 * connection, and resultsets structures and functions (see RS-DBI.h).
 * 
 * Note: A handle is just an R/S object (see RS-DBI.h for details), i.e.,
 *       Mgr_Handle, Con_Handle, Res_Handle, Db_Handle are s_object 
 *       (integer vectors, to be precise).
 */

/* dbManager */
Mgr_Handle *RS_PostgreSQL_init(s_object *config_params, s_object *reload);
s_object   *RS_PostgreSQL_close(Mgr_Handle *mgrHandle);

/* dbConnection */
Con_Handle *RS_PostgreSQL_newConnection(Mgr_Handle *mgrHandle,
				   s_object *con_params);  /*NOTE: groups & default_files removed because they are MySQL specific */
Con_Handle *RS_PostgreSQL_cloneConnection(Con_Handle *conHandle);
s_object   *RS_PostgreSQL_closeConnection(Con_Handle *conHandle);
s_object   *RS_PostgreSQL_getException(Con_Handle *conHandle);    /* err No, Msg */

/* dbResultSet */
Res_Handle *RS_PostgreSQL_exec(Con_Handle *conHandle, s_object *statement);
s_object   *RS_PostgreSQL_fetch(Res_Handle *rsHandle, s_object *max_rec);
s_object   *RS_PostgreSQL_closeResultSet(Res_Handle *rsHandle);

s_object   *RS_PostgreSQL_validHandle(Db_Handle *handle);      /* boolean */

RS_DBI_fields *RS_PostgreSQL_createDataMappings(Res_Handle *resHandle);
/* the following funs return named lists with meta-data for 
 * the manager, connections, and  result sets, respectively.
 */
s_object *RS_PostgreSQL_managerInfo(Mgr_Handle *mgrHandle);
s_object *RS_PostgreSQL_connectionInfo(Con_Handle *conHandle);
s_object *RS_PostgreSQL_resultSetInfo(Res_Handle *rsHandle);





/*  OID"S mapping taken from pg_type.h */
#define BOOLOID			16
#define BYTEAOID		17
#define CHAROID			18
#define NAMEOID			19
#define INT8OID			20
#define INT2OID			21
#define INT2VECTOROID	        22
#define INT4OID			23
#define REGPROCOID		24
#define TEXTOID			25
#define OIDOID			26
#define TIDOID		 	27
#define XIDOID 			28
#define CIDOID 			29
#define OIDVECTOROID		30
#define PG_TYPE_RELTYPE_OID 	71
#define PG_ATTRIBUTE_RELTYPE_OID 75
#define PG_PROC_RELTYPE_OID 	81
#define PG_CLASS_RELTYPE_OID 	83
#define XMLOID 			142
#define POINTOID		600
#define LSEGOID			601
#define PATHOID			602
#define BOXOID			603
#define POLYGONOID		604
#define LINEOID			628
#define FLOAT4OID 		700
#define FLOAT8OID 		701
#define ABSTIMEOID		702
#define RELTIMEOID		703
#define TINTERVALOID		704
#define UNKNOWNOID		705
#define CIRCLEOID		718
#define CASHOID 		790
#define MACADDROID 		829
#define INETOID 		869
#define CIDROID 		650
#define INT4ARRAYOID		1007
#define FLOAT4ARRAYOID 		1021
#define ACLITEMOID		1033
#define CSTRINGARRAYOID		1263
#define BPCHAROID		1042
#define VARCHAROID		1043
#define DATEOID			1082
#define TIMEOID			1083
#define TIMESTAMPOID		1114
#define TIMESTAMPTZOID		1184
#define INTERVALOID		1186
#define TIMETZOID		1266
#define BITOID			1560
#define VARBITOID	  	1562
#define NUMERICOID		1700
#define REFCURSOROID		1790
#define REGPROCEDUREOID 	2202
#define REGOPEROID		2203
#define REGOPERATOROID		2204
#define REGCLASSOID		2205
#define REGTYPEOID		2206
#define REGTYPEARRAYOID 	2211
#define TSVECTOROID		3614
#define GTSVECTOROID		3642
#define TSQUERYOID		3615
#define REGCONFIGOID		3734
#define REGDICTIONARYOID	3769
#define RECORDOID		2249
#define CSTRINGOID		2275
#define ANYOID			2276
#define ANYARRAYOID		2277
#define VOIDOID			2278
#define TRIGGEROID		2279
#define LANGUAGE_HANDLEROID	2280
#define INTERNALOID		2281
#define OPAQUEOID		2282
#define ANYELEMENTOID		2283
#define ANYNONARRAYOID		2776
#define ANYENUMOID		3500



static struct data_types RS_PostgreSQL_dataTypes[] = {




     {"BIGINT",      20				}, /* ALSO KNOWN AS INT8 */
     {"DECIMAL",     1700			}, /* ALSO KNOWN  AS NUMERIC */
     {"FLOAT8",      701			},  /* DOUBLE PRECISION */
     {"FLOAT" ,      700			}, /* ALSO CALLED FLOAT4 (SINGLE PRECISION) */
     {"INTEGER",     23				}, /*ALSO KNOWN AS INT 4 */
     {"SMALLINT",    21				}, /* ALSO KNOWN AS INT2 */

     {"CHAR",        1042	                }, /* FIXED LENGTH STRING-BLANK PADDED */
     {"VARCHAR",     1043		        }, /* VARIABLE LENGTH STRING WITH SPECIFIED LIMIT*/
     {"TEXT",	     25				}, /* VARIABLE LENGTH STRING */

     {"DATE",	     1082			},
     {"TIME",        1083	                },
     {"TIMESTAMP",    1114		        },
     {"INTERVAL",    1186			},

     {"BOOL", 	     16				}, /* BOOLEAN */

     {"BYTEA",	     17				},  /* USED FOR STORING RAW DATA */

     {"OID",	     26				},

     {"NULL",	     2278			},

     { (char *) 0,              -1              }
};
 

s_object *RS_PostgreSQL_typeNames(s_object *typeIds);
extern const struct data_types RS_dataTypeTable[];

#ifdef _cplusplus
}
#endif

#endif   /* _RS_PostgreSQL_H */

