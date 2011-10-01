/*
 *    RS-pgsql-copy.c
 *
 *    $Id$
 */

#include "RS-PostgreSQL.h"
#include <Rinternals.h>  
#include <R_ext/RS.h>  /* for Calloc/Free */
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#define COPY_IN_BUFSIZE 8192

typedef struct {
 char *data;
 size_t bufsize;
 size_t defaultSize;
} R_StringBuffer;

/* adapter for PQputCopyData and PQputCopyEnd 
   which is used in conjunction with COPY table from STDIN */

/*
 * Copies all content of the file specified with filename to the conHandle which
 * has opened connection previously issued the COPY table from STDIN query
 * the data is read from file sent to the database with PQputCopyData
 * in chunks of COPY_IN_BUFSIZE.
 * The copy ends when 0 byte could be read from the file and then the
 * PQputCopyEnd is called to complete the copying.
 */


static inline void chkpqcopydataerr(PGconn *, int);
s_object *
RS_PostgreSQL_CopyIn(Con_Handle * conHandle, s_object * filename)
{
    S_EVALUATOR RS_DBI_connection * con;
    PGconn *my_connection;
 
    char *dyn_filename;
    char copybuf[COPY_IN_BUFSIZE];
    FILE* filehandle;
    size_t len;
    int pqretcode;

    con = RS_DBI_getConnection(conHandle);
    my_connection = (PGconn *) con->drvConnection;
    dyn_filename = RS_DBI_copyString(CHR_EL(filename, 0));

    filehandle=fopen(dyn_filename, "r");
    if(filehandle == NULL){
        char errmsg[1024];
        snprintf(errmsg, 1024, "could not open file: %s", dyn_filename);
        RS_DBI_errorMessage(dyn_filename, RS_DBI_ERROR);
        return S_NULL_ENTRY;
    }

    while((len = fread(copybuf,1,COPY_IN_BUFSIZE, filehandle))){
        pqretcode = PQputCopyData(my_connection, copybuf, len);
        chkpqcopydataerr(my_connection, pqretcode);
        
    }
    PQputCopyEnd(my_connection, NULL);
    fclose(filehandle);

    free(dyn_filename);
    return S_NULL_ENTRY;
}


void *R_AllocStringBuffer(size_t blen, R_StringBuffer *buf)
{
    size_t blen1, bsize = buf->defaultSize;

    if(blen < buf->bufsize) return buf->data;
    blen1 = blen = (blen + 1);
    blen = (blen / bsize) * bsize;
    if(blen < blen1) blen += bsize;

    if(buf->data == NULL) {
        buf->data = (char *) malloc(blen);
        buf->data[0] = '\0';
    } else
        buf->data = (char *) realloc(buf->data, blen);
    buf->bufsize = blen;
    if(!buf->data) {
        buf->bufsize = 0;
        /* don't translate internal error message */
        error("could not allocate memory (%u Mb) in C function 'R_AllocStringBuffer'",
              (unsigned int) blen/1024/1024);
    }
    return buf->data;
}


void
R_FreeStringBuffer(R_StringBuffer *buf)
{
    if (buf->data != NULL) {
        free(buf->data);
        buf->bufsize = 0;
        buf->data = NULL;
    }
}


static Rboolean isna(SEXP x, int indx)
{
    Rcomplex rc;
    switch(TYPEOF(x)) {
    case LGLSXP:
        return LOGICAL(x)[indx] == NA_LOGICAL;
        break;
    case INTSXP:
        return INTEGER(x)[indx] == NA_INTEGER;
        break;
    case REALSXP:
        return ISNAN(REAL(x)[indx]);
        break;
    case STRSXP:
        return STRING_ELT(x, indx) == NA_STRING;
        break;
    case CPLXSXP:
        rc = COMPLEX(x)[indx];
        return ISNAN(rc.r) || ISNAN(rc.i);
        break;
    default:
        break;
    }
    return FALSE;
}

/* a version of EncodeElement with different escaping of char strings */
static const char
*EncodeElementS(SEXP x, int indx,
                R_StringBuffer *buff, char cdec)
{
    int w, d, e, wi, di, ei;
    const char *res;

    switch(TYPEOF(x)) {
       case STRSXP:
       {
	    const char *s = translateCharUTF8(STRING_ELT(x, indx));
	    char *u, *cbuf;
            int j, len, blen, offset;
	    len = strlen(s);
	    blen = len * 2 + 1;
            R_AllocStringBuffer(blen, buff);
	    u = cbuf = buff->data;
	    offset = 0;
	    for (j = 0; j < len; j++){
                switch(s[offset+j]){
/* http://www.postgresql.org/docs/8.1/static/sql-copy.html */
                    case '\b':
                        *u++ = '\\';
                        *u++ = 'b';
                        break;
                    case '\f':
                        *u++ = '\\';
                        *u++ = 'f';
                        break;
                    case '\n':
                        *u++ = '\\';
                        *u++ = 'n';
                        break;
                    case '\r':
                        *u++ = '\\';
                        *u++ = 'r';
                        break;
                    case '\t':
                        *u++ = '\\';
                        *u++ = 't';
                        break;
                    case '\v':
                        *u++ = '\\';
                        *u++ = 'v';
                        break;
                    case '\\':
                        *u++ = '\\';
                        *u++ = '\\';
                        break;
                    default:
		        *u++ = s[offset+j];
                }
            }
            *u = '\0';
            return buff->data;
        }
        case LGLSXP:{
            int value;
            value = LOGICAL(x)[indx];
            if(value == TRUE) return "true";
            if(value == FALSE) return "false";
            return "\\N";
        }
        case INTSXP:{
            int value;
            value = INTEGER(x)[indx];
            if(ISNA(value)) return "\\N";
            snprintf(buff->data, buff->bufsize, "%d", value);
            return buff->data;
        }
        case REALSXP:{
            double value = REAL(x)[indx];
            if (!R_FINITE(value)) {
                if(ISNA(value)) return "\\N";
                else if(ISNAN(value)) return "NaN";
                else if(value > 0) return "Inf";
                else return "-Inf";
            }
            snprintf(buff->data, buff->bufsize, "%.15g", value);
            return buff->data;
        }
        default:
            return NULL; 
    }
    return NULL; 
}

static inline void
chkpqcopydataerr(PGconn *my_connection, int pqretcode)
{
    if(pqretcode == -1){
        char * pqerrmsg = PQerrorMessage(my_connection);
        char * rserrmsg;
        char * format = "PQputCopyData failed: %s";
        size_t len = strlen(pqerrmsg) + strlen(format) + 1;
        rserrmsg = malloc(len);
        if(rserrmsg){
             snprintf(rserrmsg, len, format, pqerrmsg);
             RS_DBI_errorMessage(rserrmsg, RS_DBI_ERROR);
        }else{
             RS_DBI_errorMessage("malloc failed while reporting error in PQputCopyData", RS_DBI_ERROR);
        }
    }
}

SEXP
RS_PostgreSQL_CopyInDataframe(Con_Handle * conHandle, SEXP x, SEXP nrow, SEXP ncol)
{
    S_EVALUATOR RS_DBI_connection * con;
    int nr, nc, i, j;
    const char *cna ="\\N", *tmp=NULL /* -Wall */;
    char cdec = '.';

    PGconn *my_connection;
    int pqretcode;
    nr = asInteger(nrow);
    nc = asInteger(ncol);
    const int buff_threshold = 8000;

    con = RS_DBI_getConnection(conHandle);
    my_connection = (PGconn *) con->drvConnection;

    if(isVectorList(x)) { /* A data frame */
        R_StringBuffer rstrbuf = {NULL, 0, 10000};
        void *p;
        
        char *strBuf  = Calloc(buff_threshold * 2 + 2, char); /* + 2 for '\t' or '\n' plus '\0'*/
        char *strendp = strBuf;
        SEXP *levels;
        *strendp = '\0';

        p = R_AllocStringBuffer(10000, &rstrbuf);
	/* handle factors internally, check integrity */
	levels = (SEXP *) R_alloc(nc, sizeof(SEXP));
	for(j = 0; j < nc; j++) {
            SEXP xj;
	    xj = VECTOR_ELT(x, j);
	    if(LENGTH(xj) != nr)
		error(("corrupt data frame -- length of column %d does not not match nrows"), j+1);
	    if(inherits(xj, "factor")) {
		levels[j] = getAttrib(xj, R_LevelsSymbol);
	    } else levels[j] = R_NilValue;
	}

	for(i = 0; i < nr; i++) {
	    for(j = 0; j < nc; j++) {
                SEXP xj;
		xj = VECTOR_ELT(x, j);
		if(j > 0){
                    *strendp++ =  '\t';/*need no size count check here*/
                }
		if(isna(xj, i)) tmp = cna;
		else {
		    if(!isNull(levels[j])) {
			/* We cannot assume factors have integer levels */
			if(TYPEOF(xj) == INTSXP){
                            tmp = EncodeElementS(levels[j], INTEGER(xj)[i] - 1,
                                                 &rstrbuf, cdec);
			}else if(TYPEOF(xj) == REALSXP){
                            tmp = EncodeElementS(levels[j], REAL(xj)[i] - 1,
                                                 &rstrbuf, cdec);
			}else
			    error("column %s claims to be a factor but does not have numeric codes", j+1);
		    } else {
			tmp = EncodeElementS(xj, i, 
					     &rstrbuf, cdec);
		    }
		}
                {
                    size_t n;
                    size_t len = strendp - strBuf;
                    n = strlen(tmp);
                    if (len + n < buff_threshold){
                        memcpy(strendp, tmp, n);/* we already know the length */
                        strendp += n;
                    }else if(n < buff_threshold){ /*copy and flush*/
                        memcpy(strendp, tmp, n);/* we already know the length */
                        pqretcode = PQputCopyData(my_connection, strBuf, len + n);
              	        chkpqcopydataerr(my_connection, pqretcode);
                        strendp = strBuf;
                    }else{ /*flush and copy current*/
                        if(len > 0){
                            pqretcode = PQputCopyData(my_connection, strBuf, len);
                            chkpqcopydataerr(my_connection, pqretcode);
                            strendp = strBuf;
                        }
                        pqretcode = PQputCopyData(my_connection, tmp, n);
                        chkpqcopydataerr(my_connection, pqretcode);
                    }
                }
	    }
            *strendp = '\n'; strendp +=1; *strendp='\0';
	}
        pqretcode = PQputCopyData(my_connection, strBuf, strendp - strBuf);
        chkpqcopydataerr(my_connection, pqretcode);
        Free(strBuf);
        R_FreeStringBuffer(&rstrbuf);
    }
    PQputCopyEnd(my_connection, NULL);
    return R_NilValue;
}


