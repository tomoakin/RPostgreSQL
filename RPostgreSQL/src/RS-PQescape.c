/*
 *  RS-PQescape.c
 *
 *  $Id$
 */

#include "RS-PostgreSQL.h"

/* 
 * Adapter function to PQescapeStringConn()
 * This function should properly escape the string argument 
 * appropriately depending on the encoding etc. that is specific to 
 * connection.
 * Note the single quote is not attached in the return val.
 */
SEXP
RS_PostgreSQL_escape(SEXP conHandle, SEXP preescapestring)
{
    S_EVALUATOR PGconn * my_connection;
    RS_DBI_connection *con;
    SEXP output;
    size_t length;
    const char *statement_cstr;
    char *escapedstring;
    con = RS_DBI_getConnection(conHandle);
    my_connection = (PGconn *) con->drvConnection;
    statement_cstr = CHR_EL(preescapestring, 0);
    length = strlen(statement_cstr);
    escapedstring = R_alloc(length * 2 + 1, 1);
    PQescapeStringConn(my_connection, escapedstring, statement_cstr, length, NULL);
    output = allocVector(STRSXP, 1);
    SET_STRING_ELT(output, 0, mkChar(escapedstring));
    return output;
}

/* 
 * Adapter function to PQescapeByteaConn()
 * This function should properly escape the raw argument 
 * appropriately depending on connection.
 * Note the single quote is not attached in the return val.
 * The returned string could differ depending on the environment.
 * Especially standard_conforming_strings parameter 
 * is possibly influencial.
 * http://www.postgresql.org/docs/9.3/static/sql-syntax-lexical.html#SQL-SYNTAX-STRINGS
 */
SEXP
RS_PostgreSQL_escape_bytea(SEXP conHandle, SEXP raw_data)
{
    S_EVALUATOR PGconn * my_connection;
    RS_DBI_connection *con;
    SEXP output;
    size_t length;
    char *escapedstring;
    size_t escaped_length;
    con = RS_DBI_getConnection(conHandle);
    my_connection = (PGconn *) con->drvConnection;
    length = LENGTH(raw_data);
    escapedstring = (char *)PQescapeByteaConn(my_connection, RAW(raw_data), length, &escaped_length);
    /* explicit cast to make clang silent for difference in signedness*/
    PROTECT(output = allocVector(STRSXP, 1));
    SET_STRING_ELT(output, 0, mkChar(escapedstring));
    free(escapedstring);
    UNPROTECT(1);
    return output;
}

static inline unsigned char
hex2n(unsigned char h)
{
  if (h >= '0' && h <= '9') return h-'0';
  if (h >= 'a' && h <= 'f') return h-'a' + 10;
  if (h >= 'A' && h <= 'F') return h-'A' + 10;
  return h;
}
/* 
 * Adapter function to PQunescapeBytea()
 * This function should properly unescape the charactor representation
 * of the binary data returned from PostgreSQL and return raw binary data.
 */
SEXP
RS_PostgreSQL_unescape_bytea(SEXP escapedstring)
{
    SEXP output;
    size_t raw_length;
    const char* strbuffer;
    unsigned char* rawbuffer;
    strbuffer = CHAR(STRING_ELT(escapedstring, 0));
    if(!strbuffer) RS_DBI_errorMessage("strbuffer is NULL!", RS_DBI_ERROR);
    if (strbuffer[0] == '\\' && strbuffer[1] == 'x'){
      /* the new hex fomat */
        int i;
        size_t enc_length = LENGTH(STRING_ELT(escapedstring, 0));
        raw_length = enc_length / 2 - 1;
        output = allocVector(RAWSXP, raw_length);
        rawbuffer = RAW(output);
        for(i = 0; i < raw_length; i++){
          rawbuffer[i] = (hex2n(strbuffer[2+ i*2]) << 4) + hex2n(strbuffer[2+i*2+1]);
        }
        return output;
    }else{ /* the old escape format */
        rawbuffer = PQunescapeBytea((const unsigned char*) strbuffer,  &raw_length);
        /* explicit cast to suppress warning on signedness by clang */
        if(!rawbuffer) RS_DBI_errorMessage("PQunescapeByea Failed", RS_DBI_ERROR);
        output = allocVector(RAWSXP, raw_length);
        memcpy(RAW(output), rawbuffer, raw_length);
        free(rawbuffer);
        return output;
    }
}


