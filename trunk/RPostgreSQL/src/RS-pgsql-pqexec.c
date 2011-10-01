/*
 * RS-PostgreSQL.c
 *
 * $Id$
 *
 * This package was developed as a part of Summer of Code program organized by Google.
 * Thanks to David A. James & Saikat DebRoy, the authors of RMySQL package.
 * Code from RMySQL package was reused with the permission from the authors.
 * Also Thanks to my GSoC mentor Dirk Eddelbuettel for helping me in the development.
 *
 * Source Processed with:
 * indent -br -i4 -nut --line-length120 --comment-line-length120 --leave-preprocessor-space -npcs RS-PostgreSQL.c
 *
 */

#include <limits.h>
#include "RS-PostgreSQL.h"


/*   R and S DataBase Interface to PostgreSQL
 *
 * C function library which can be used to run SQL queries
 * from inside of S4, Splus5.x, or R.
 * This Driver hooks R/S and PostgreSQL and implements the
 * the proposed RS-DBI generic database interface.
 *
 * For details refer
 * On R,
 *      "R extensions" manual
 * On PostgreSQL,
 *      "PostgreSQL 8.3.1 documentation"
 */


/* Execute (currently) one sql statement (INSERT, DELETE, SELECT, etc.),
 * set coercion type mappings between the server internal data types and
 * S classes.  Don't drag the return value.
 */

SEXP
RS_PostgreSQL_pqexec(Con_Handle * conHandle, s_object * statement)
{
    S_EVALUATOR RS_DBI_connection * con;
    SEXP retval;
    RS_DBI_resultSet *result;
    PGconn *my_connection;
    PGresult *my_result;
 
    Sint res_id, is_select=0;
    char *dyn_statement;

    con = RS_DBI_getConnection(conHandle);
    my_connection = (PGconn *) con->drvConnection;
    dyn_statement = RS_DBI_copyString(CHR_EL(statement, 0));

    /* Here is where we actually run the query */

    /* Example: PGresult *PQexec(PGconn *conn, const char *command); */

    my_result = PQexec(my_connection, dyn_statement);
    if (my_result == NULL) {
        char *errMsg;
        const char *omsg;
        size_t len;
        omsg = PQerrorMessage(my_connection);
        len = strlen(omsg);
        free(dyn_statement);
        errMsg = malloc(len + 80); /* 80 should be larger than the length of "could not ..."*/
        snprintf(errMsg, len + 80,  "could not run statement: %s", omsg);
        RS_DBI_errorMessage(errMsg, RS_DBI_ERROR);
        free(errMsg);
    }


    if (PQresultStatus(my_result) == PGRES_TUPLES_OK) {
        is_select = (Sint) TRUE;
    }
    if (PQresultStatus(my_result) == PGRES_COMMAND_OK) {
        is_select = (Sint) FALSE;
    }

    if (strcmp(PQresultErrorMessage(my_result), "") != 0) {

        free(dyn_statement);
        char *errResultMsg;
        const char *omsg;
        size_t len;
        omsg = PQerrorMessage(my_connection);
        len = strlen(omsg);
        errResultMsg = malloc(len + 80); /* 80 should be larger than the length of "could not ..."*/
        snprintf(errResultMsg, len + 80, "could not Retrieve the result : %s", omsg);
        RS_DBI_errorMessage(errResultMsg, RS_DBI_ERROR);
        free(errResultMsg);

        /*  Frees the storage associated with a PGresult.
         *  void PQclear(PGresult *res);   */
        PQclear(my_result);
    }

    free(dyn_statement);
    PROTECT(retval = allocVector(LGLSXP, 1));
    LOGICAL(retval)[0] = is_select;
    UNPROTECT(1);
    return retval;
}

