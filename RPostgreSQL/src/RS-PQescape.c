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
 */
SEXP
RS_PostgreSQL_escape_bytea(SEXP conHandle, SEXP raw_data)
{
    S_EVALUATOR PGconn * my_connection;
    RS_DBI_connection *con;
    SEXP output;
    size_t length;
    const char *statement_cstr;
    char *escapedstring;
    size_t escaped_length;
    con = RS_DBI_getConnection(conHandle);
    my_connection = (PGconn *) con->drvConnection;
    length = LENGTH(raw_data);
    escapedstring = PQescapeByteaConn(my_connection, RAW(raw_data), length, &escaped_length);
    PROTECT(output = allocVector(STRSXP, 1));
    SET_STRING_ELT(output, 0, mkChar(escapedstring));
    free(escapedstring);
    UNPROTECT(1);
    return output;
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
    unsigned char* rawbuffer;
    rawbuffer = PQunescapeBytea(CHAR(STRING_ELT(escapedstring, 0)),  &raw_length);
    output = allocVector(RAWSXP, raw_length);
    memcpy(RAW(output), rawbuffer, raw_length);
    free(rawbuffer);
    return output;
}


