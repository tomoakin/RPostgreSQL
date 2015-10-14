/*
 * RS-pgsql-pqexecparam.c
 *
 * $Id: RS-pgsql-pqexecparam.c $
 *
 */

#include <limits.h>
#include "RS-PostgreSQL.h"
#include <Rinternals.h>


/* Execute (currently) one sql statement (INSERT, DELETE, SELECT, etc.),
 * set coercion type mappings between the server internal data types and
 * S classes.  Don't drag the return value.
 */

/* should call with .External */
SEXP
RS_PostgreSQL_pqexecparams(SEXP args)
{
    S_EVALUATOR RS_DBI_connection * con;
    SEXP retval;
    PGconn *my_connection;
    PGresult *my_result;
    R_len_t nparams;
    Con_Handle * conHandle;
    s_object * statement;
    s_object * params;
    Sint is_select=0;
    const char *dyn_statement;
    const char ** pqparams;
    RS_DBI_resultSet *result;

    conHandle = CADR(args);
    statement = CADDR(args);
    params = CADDDR(args);

    con = RS_DBI_getConnection(conHandle);
    my_connection = (PGconn *) con->drvConnection;
    dyn_statement = CHR_EL(statement, 0);
    nparams = length(params);
    pqparams = Calloc(nparams, const char*);
    
    for (R_len_t i = 0; i < nparams; i++){
        pqparams[i] = CHR_EL(params, i);
    }

/* http://www.postgresql.org/docs/9.2/static/libpq-exec.html
 * PGresult *PQexecParams(PGconn *conn,
 *                      const char *command,
 *                      int nParams,
 *                      const Oid *paramTypes,
 *                      const char * const *paramValues,
 *                      const int *paramLengths,
 *                      const int *paramFormats,
 *                      int resultFormat);
 */
    my_result = PQexecParams(my_connection, dyn_statement, nparams, NULL, pqparams, NULL, NULL, 0);
    Free(pqparams);
    if (my_result == NULL) {
        char *errMsg;
        const char *omsg;
        size_t len;
        omsg = PQerrorMessage(my_connection);
        len = strlen(omsg);
        errMsg = R_alloc(len + 80, 1); /* 80 should be larger than the length of "could not ..."*/
        snprintf(errMsg, len + 80,  "could not run statement: %s", omsg);
        RS_DBI_errorMessage(errMsg, RS_DBI_ERROR);
    }


    if (PQresultStatus(my_result) == PGRES_TUPLES_OK) {
        is_select = (Sint) TRUE;
    }
    if (PQresultStatus(my_result) == PGRES_COMMAND_OK) {
        is_select = (Sint) FALSE;
    }

    if (strcmp(PQresultErrorMessage(my_result), "") != 0) {

        char *errResultMsg;
        const char *omsg;
        size_t len;
        omsg = PQerrorMessage(my_connection);
        len = strlen(omsg);
        errResultMsg = R_alloc(len + 80, 1); /* 80 should be larger than the length of "could not ..."*/
        snprintf(errResultMsg, len + 80, "could not Retrieve the result : %s", omsg);
        /*  Frees the storage associated with a PGresult.
         *  void PQclear(PGresult *res);   */
        PQclear(my_result);
        RS_DBI_errorMessage(errResultMsg, RS_DBI_ERROR);
    }

    /* we now create the wrapper and copy values */
    PROTECT(retval = RS_DBI_allocResultSet(conHandle));
    result = RS_DBI_getResultSet(retval);
    result->statement = RS_DBI_copyString(dyn_statement);
    result->drvResultSet = (void *) my_result;
    result->rowCount = (Sint) 0;
    result->isSelect = is_select;

    /*  Returns the number of rows affected by the SQL command.
     *  char *PQcmdTuples(PGresult *res);
     */

    if (!is_select) {
        result->rowsAffected = (Sint) atoi(PQcmdTuples(my_result));
        result->completed = 1;
    }
    else {
        result->rowsAffected = (Sint) - 1;
        result->completed = 0;
    }

    if (is_select) {
        result->fields = RS_PostgreSQL_createDataMappings(retval);
    }
    UNPROTECT(1);
    return retval;
}

