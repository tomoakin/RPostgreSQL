/*
 *    RS-pgsql-copy.c
 *
 *    $Id$
 */

#include "RS-PostgreSQL.h"
#define COPY_IN_BUFSIZE 8192

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


s_object *
RS_PostgreSQL_getResult(Con_Handle * conHandle)
{
    S_EVALUATOR RS_DBI_connection * con;
    S_EVALUATOR RS_DBI_resultSet * result;
    PGconn *my_connection;
    Res_Handle *rsHandle;
    Sint res_id;
 
    PGresult *my_result;
   
    con = RS_DBI_getConnection(conHandle);
    my_connection = (PGconn *) con->drvConnection;

    if (con->num_res > 0) {
        res_id = (Sint) con->resultSetIds[0];
        rsHandle = RS_DBI_asResHandle(MGR_ID(conHandle), CON_ID(conHandle), res_id);
        result = RS_DBI_getResultSet(rsHandle);
        if (result->completed == 0) {
            RS_DBI_errorMessage("connection with pending rows, close resultSet before continuing", RS_DBI_ERROR);
        }
        else {
            RS_PostgreSQL_closeResultSet(rsHandle);
        }
    }

    my_result = PQgetResult(my_connection);
    if(my_result == NULL)
       return S_NULL_ENTRY;
    if (strcmp(PQresultErrorMessage(my_result), "") != 0) {
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

    /* we now create the wrapper and copy values */
    rsHandle = RS_DBI_allocResultSet(conHandle);
    result = RS_DBI_getResultSet(rsHandle);
    result->drvResultSet = (void *) my_result;
    result->rowCount = (Sint) 0;
    result->isSelect = 0;
    result->rowsAffected = 0;
    result->completed = 1;
    return rsHandle;
}

