/*
 *    RS-PostgreSQL.c
 *
 * Last Modified: $Date$
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

struct data_types RS_PostgreSQL_dataTypes[] = {

    {"BIGINT", 20},         /* ALSO KNOWN AS INT8 */
    {"DECIMAL", 1700},      /* ALSO KNOWN  AS NUMERIC */
    {"FLOAT8", 701},        /* DOUBLE PRECISION */
    {"FLOAT", 700},         /* ALSO CALLED FLOAT4 (SINGLE PRECISION) */
    {"INTEGER", 23},        /*ALSO KNOWN AS INT 4 */
    {"SMALLINT", 21},       /* ALSO KNOWN AS INT2 */
    {"MONEY", 790},         /* MONEY (8 bytes) */
    
    {"CHAR", 1042},         /* FIXED LENGTH STRING-BLANK PADDED */
    {"VARCHAR", 1043},      /* VARIABLE LENGTH STRING WITH SPECIFIED LIMIT */
    {"TEXT", 25},           /* VARIABLE LENGTH STRING */
    
    {"DATE", 1082},
    {"TIME", 1083},
    {"TIMESTAMP", 1114},
    {"TIMESTAMPTZOID", 1184},
    {"INTERVAL", 1186},
    {"TIMETZOID", 1266},

    {"BOOL", 16},           /* BOOLEAN */
    
    {"BYTEA", 17},          /* USED FOR STORING RAW DATA */
    
    {"OID", 26},
    
    {"NULL", 2278},

    {(char *) 0, -1}
};



#ifndef USING_R
#  error("the function RS_DBI_invokeBeginGroup() has not been implemented in S")
#  error("the function RS_DBI_invokeEndGroup()   has not been implemented in S")
#  error("the function RS_DBI_invokeNewRecord()  has not been implemented in S")
#endif

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


Mgr_Handle *
RS_PostgreSQL_init(s_object * config_params, s_object * reload)
{
    S_EVALUATOR
        /* Currently we can specify the defaults for 2 parameters, max num of
         * connections, and max of records per fetch (this can be over-ridden
         * explicitly in the S call to fetch).
         */
        Mgr_Handle * mgrHandle;
    Sint fetch_default_rec, force_reload, max_con;
    const char *drvName = "PostgreSQL";


    max_con = INT_EL(config_params, 0);
    fetch_default_rec = INT_EL(config_params, 1);
    force_reload = LGL_EL(reload, 0);

    mgrHandle = RS_DBI_allocManager(drvName, max_con, fetch_default_rec, force_reload);

    return mgrHandle;
}


s_object *
RS_PostgreSQL_closeManager(Mgr_Handle * mgrHandle)
{
    S_EVALUATOR RS_DBI_manager * mgr;
    s_object *status;

    mgr = RS_DBI_getManager(mgrHandle);
    if (mgr->num_con) {
        RS_DBI_errorMessage("There are opened connections -- close them first", RS_DBI_ERROR);
    }
    RS_DBI_freeManager(mgrHandle);

    MEM_PROTECT(status = NEW_LOGICAL((Sint) 1));
    LGL_EL(status, 0) = TRUE;
    MEM_UNPROTECT(1);
    return status;
}



/* open a connection with the same parameters used for in
 *  conHandle
 */
Con_Handle *
RS_PostgreSQL_cloneConnection(Con_Handle * conHandle)
{
    S_EVALUATOR Mgr_Handle * mgrHandle;
    RS_DBI_connection *con;
    RS_PostgreSQL_conParams *conParams;
    s_object *con_params;

    /* get connection params used to open existing connection */
    con = RS_DBI_getConnection(conHandle);
    conParams = con->conParams;

    mgrHandle = RS_DBI_asMgrHandle(MGR_ID(conHandle));


    /* Connection parameters need to be put into a 8-element character
     * vector to be passed to the RS_PostgreSQL_newConnection() function.
     */

    MEM_PROTECT(con_params = NEW_CHARACTER((Sint) 7));
    SET_CHR_EL(con_params, 0, C_S_CPY(conParams->user));
    SET_CHR_EL(con_params, 1, C_S_CPY(conParams->password));
    SET_CHR_EL(con_params, 2, C_S_CPY(conParams->host));
    SET_CHR_EL(con_params, 3, C_S_CPY(conParams->dbname));
    SET_CHR_EL(con_params, 4, C_S_CPY(conParams->port));
    SET_CHR_EL(con_params, 5, C_S_CPY(conParams->tty));
    SET_CHR_EL(con_params, 6, C_S_CPY(conParams->options));

    MEM_UNPROTECT(1);

    return RS_PostgreSQL_newConnection(mgrHandle, con_params);
}



RS_PostgreSQL_conParams *
RS_postgresql_allocConParams(void)
{
    RS_PostgreSQL_conParams *conParams;

    conParams = (RS_PostgreSQL_conParams *)
        malloc(sizeof(RS_PostgreSQL_conParams));
    if (!conParams) {
        RS_DBI_errorMessage("could not malloc space for connection params", RS_DBI_ERROR);
    }
    return conParams;
}



void
RS_PostgreSQL_freeConParams(RS_PostgreSQL_conParams * conParams)
{
    if (conParams->host) {
        free(conParams->host);
    }
    if (conParams->dbname) {
        free(conParams->dbname);
    }
    if (conParams->user) {
        free(conParams->user);
    }
    if (conParams->password) {
        free(conParams->password);
    }
    if (conParams->port) {
        free(conParams->port);
    }
    if (conParams->tty) {
        free(conParams->tty);
    }
    if (conParams->options) {
        free(conParams->options);
    }

    free(conParams);
    return;
}


Con_Handle *
RS_PostgreSQL_newConnection(Mgr_Handle * mgrHandle, s_object * con_params)
{
    S_EVALUATOR RS_DBI_connection * con;
    RS_PostgreSQL_conParams *conParams;
    Con_Handle *conHandle;
    PGconn *my_connection;

    char *user = NULL, *password = NULL, *host = NULL, *dbname = NULL, *port = NULL, *tty = NULL, *options = NULL;

    if (!is_validHandle(mgrHandle, MGR_HANDLE_TYPE)) {
        RS_DBI_errorMessage("invalid PostgreSQLManager", RS_DBI_ERROR);
    }

#define IS_EMPTY(s1)   !strcmp((s1), "")

    if (!IS_EMPTY(CHR_EL(con_params, 0))) {
        user = (char *) CHR_EL(con_params, 0);
    }
    if (!IS_EMPTY(CHR_EL(con_params, 1))) {
        password = (char *) CHR_EL(con_params, 1);
    }
    if (!IS_EMPTY(CHR_EL(con_params, 2))) {
        host = (char *) CHR_EL(con_params, 2);
    }
    if (!IS_EMPTY(CHR_EL(con_params, 3))) {
        dbname = (char *) CHR_EL(con_params, 3);
    }
    if (!IS_EMPTY(CHR_EL(con_params, 4))) {
        port = (char *) CHR_EL(con_params, 4);
    }
    if (!IS_EMPTY(CHR_EL(con_params, 5))) {
        tty = (char *) CHR_EL(con_params, 5);
    }
    if (!IS_EMPTY(CHR_EL(con_params, 6))) {
        options = (char *) CHR_EL(con_params, 6);
    }

    my_connection = PQsetdbLogin(host, port, options, tty, dbname, user, password);

    if (PQstatus(my_connection) != CONNECTION_OK) {
        char buf[1000];
	sprintf(buf, "could not connect %s@%s on dbname \"%s\"\n", PQuser(my_connection), host?host:"local", PQdb(my_connection));
        RS_DBI_errorMessage(buf, RS_DBI_ERROR);
    }

    conParams = RS_postgresql_allocConParams();

    /* save actual connection parameters */
    conParams->user = RS_DBI_copyString(PQuser(my_connection));
    conParams->password = RS_DBI_copyString(PQpass(my_connection));
    {
	const char *tmphost = PQhost(my_connection);
	if (tmphost) {
	    conParams->host = RS_DBI_copyString(tmphost);
	} else {
	    conParams->host = RS_DBI_copyString("");
	}
    }
    conParams->dbname = RS_DBI_copyString(PQdb(my_connection));
    conParams->port = RS_DBI_copyString(PQport(my_connection));
    conParams->tty = RS_DBI_copyString(PQtty(my_connection));
    conParams->options = RS_DBI_copyString(PQoptions(my_connection));

    conHandle = RS_DBI_allocConnection(mgrHandle, (Sint) 1);
    con = RS_DBI_getConnection(conHandle);
    if (!con) {
        PQfinish(my_connection);
        RS_PostgreSQL_freeConParams(conParams);
        conParams = (RS_PostgreSQL_conParams *) NULL;
        RS_DBI_errorMessage("could not alloc space for connection object", RS_DBI_ERROR);
    }
    con->drvConnection = (void *) my_connection;
    con->conParams = (void *) conParams;

    return conHandle;
}


s_object *
RS_PostgreSQL_closeConnection(Con_Handle * conHandle)
{
    S_EVALUATOR RS_DBI_connection * con;
    PGconn *my_connection;
    s_object *status;

    con = RS_DBI_getConnection(conHandle);
    if (con->num_res > 0) {
        RS_DBI_errorMessage("close the pending result sets before closing this connection", RS_DBI_ERROR);
    }
    /* make sure we first free the conParams and postgresql connection from
     * the RS-RBI connection object.
     */
    if (con->conParams) {
        RS_PostgreSQL_freeConParams(con->conParams);
        con->conParams = (RS_PostgreSQL_conParams *) NULL;
    }
    my_connection = (PGconn *) con->drvConnection;

    PQfinish(my_connection);
    con->drvConnection = (void *) NULL;

    RS_DBI_freeConnection(conHandle);

    MEM_PROTECT(status = NEW_LOGICAL((Sint) 1));
    LGL_EL(status, 0) = TRUE;
    MEM_UNPROTECT(1);

    return status;
}

/* Execute (currently) one sql statement (INSERT, DELETE, SELECT, etc.),
 * set coercion type mappings between the server internal data types and
 * S classes.   Returns  an S handle to a resultSet object.
 */

Res_Handle *
RS_PostgreSQL_exec(Con_Handle * conHandle, s_object * statement)
{
    S_EVALUATOR RS_DBI_connection * con;
    Res_Handle *rsHandle;
    RS_DBI_resultSet *result;
    PGconn *my_connection;
    PGresult *my_result;
 
    Sint res_id, is_select=0;
    char *dyn_statement;

    con = RS_DBI_getConnection(conHandle);
    my_connection = (PGconn *) con->drvConnection;
    dyn_statement = RS_DBI_copyString(CHR_EL(statement, 0));

    /* Do we have a pending resultSet in the current connection?
     * PostgreSQL only allows  one resultSet per connection.
     */
    if (con->num_res > 0) {
        res_id = (Sint) con->resultSetIds[0];   /* recall, PostgreSQL has only 1 res */
        rsHandle = RS_DBI_asResHandle(MGR_ID(conHandle), CON_ID(conHandle), res_id);
        result = RS_DBI_getResultSet(rsHandle);
        if (result->completed == 0) {
            free(dyn_statement);
            RS_DBI_errorMessage("connection with pending rows, close resultSet before continuing", RS_DBI_ERROR);
        }
        else {
            RS_PostgreSQL_closeResultSet(rsHandle);
        }
    }

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


    /* ExecStatusType PQresultStatus(const PGresult *res); */

    if (PQresultStatus(my_result) == PGRES_TUPLES_OK) {
        is_select = (Sint) TRUE;
    }
    if (PQresultStatus(my_result) == PGRES_COMMAND_OK) {
        is_select = (Sint) FALSE;
    }

    /* char *PQresultErrorMessage(const PGresult *res); */

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

    /* we now create the wrapper and copy values */
    rsHandle = RS_DBI_allocResultSet(conHandle);
    result = RS_DBI_getResultSet(rsHandle);
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
        result->fields = RS_PostgreSQL_createDataMappings(rsHandle);
    }
    free(dyn_statement);
    return rsHandle;
}


RS_DBI_fields *
RS_PostgreSQL_createDataMappings(Res_Handle * rsHandle)
{
    PGresult *my_result;

    RS_DBI_connection *con;
    RS_DBI_resultSet *result;
    RS_DBI_fields *flds;

    int j, num_fields, internal_type;
    char errMsg[128];

    result = RS_DBI_getResultSet(rsHandle);
    my_result = (PGresult *) result->drvResultSet;

    con = RS_DBI_getConnection(rsHandle);
    num_fields = PQnfields(my_result);

    flds = RS_DBI_allocFields(num_fields);

    char buff[1000];            /* Buffer to hold the sql query to check whether the given column is nullable */
    PGconn *conn;
    PGresult *res;
    conn = (PGconn *) con->drvConnection;

    for (j = 0; j < num_fields; j++) {

        flds->name[j] = RS_DBI_copyString(PQfname(my_result, j));

        flds->type[j] = (int) PQftype(my_result, j);

        flds->length[j] = (Sint) PQfsize(my_result, j);

        /* NOTE: PQfmod is -1 incase of no information */
        flds->precision[j] = (Sint) PQfmod(my_result, j);

        flds->scale[j] = (Sint) - 1;

        /* PQftablecol returns the column number (within its table) of
         * the column making up the specified query result column.Zero
         * is returned if the column number is out of range, or if the
         * specified column is not a simple reference to a table
         * column, or when using pre-3.0 protocol. So
         * "if(PQftablecol(my_result,j) !=0)" checks whether the
         * particular colomn in the result set is column of table or
         * not. Or else there is no meaning in checking whether a
         * column is nullable or not if it does not belong to the
         * table.
         */

        flds->nullOk[j] = (Sint) INT_MIN; /* This should translate to NA in R */

        if (PQftablecol(my_result, j) != 0) {
            /* Code to find whether a row can be nullable or not */
            /* we might better just store the table id and column number 
               for lazy evaluation at dbColumnInfo call*/
            /* although the database structure can change, we are not in transaction anyway 
               and there is no guarantee in current code */
            snprintf(buff, 1000, "select attnotnull from pg_attribute where attrelid=%d and attnum='%d'",
                    PQftable(my_result, j), PQftablecol(my_result, j));
            res = PQexec(conn, buff);

	    if (res && (PQntuples(res) > 0)){
                const char * attnotnull = PQgetvalue(res, 0, 0);
		if(strcmp(attnotnull, "f") == 0) {
		    flds->nullOk[j] = (Sint) 1; /* nollOK is TRUE when attnotnull is f*/
                }
		if(strcmp(attnotnull, "t") == 0) {
		    flds->nullOk[j] = (Sint) 0; /* nollOK is FALSE when attnotnull is t*/
                }
	    }
            PQclear(res);
        }

        internal_type = (int) PQftype(my_result, j);

        switch (internal_type) {
        case BOOLOID:
            flds->Sclass[j] = LOGICAL_TYPE;
            break;
        case BPCHAROID:
            flds->Sclass[j] = CHARACTER_TYPE;
            flds->isVarLength[j] = (Sint) 0;
            break;
        case VARCHAROID:
        case TEXTOID:
        case BYTEAOID:
        case NAMEOID:
            flds->Sclass[j] = CHARACTER_TYPE;
            flds->isVarLength[j] = (Sint) 1;
            break;
        case INT2OID:
        case INT4OID:
        case OIDOID:
            flds->Sclass[j] = INTEGER_TYPE;
            break;
        case INT8OID:
            if (sizeof(Sint) >= 8) {
                flds->Sclass[j] = INTEGER_TYPE;
            }
            else {
                flds->Sclass[j] = NUMERIC_TYPE;
            }
            break;
        case NUMERICOID:
        case FLOAT8OID:
        case FLOAT4OID:
            flds->Sclass[j] = NUMERIC_TYPE;
            break;
        case DATEOID:
        case TIMEOID:
        case TIMETZOID:
        case TIMESTAMPOID:
        case TIMESTAMPTZOID:
        case INTERVALOID:
            flds->Sclass[j] = CHARACTER_TYPE;
            /*flds->isVarLength[j] = (Sint) 1; */
            break;
        default:
            flds->Sclass[j] = CHARACTER_TYPE;
            flds->isVarLength[j] = (Sint) 1;
            snprintf(errMsg, 128, "unrecognized PostgreSQL field type %d in column %d", internal_type, j);
            RS_DBI_errorMessage(errMsg, RS_DBI_WARNING);
            break;
        }
    }
    return flds;
}

s_object *                      /* output is a named list */
RS_PostgreSQL_fetch(s_object * rsHandle, s_object * max_rec)
{
    S_EVALUATOR RS_DBI_manager * mgr;
    RS_DBI_resultSet *result;
    RS_DBI_fields *flds;
    PGresult *my_result;
    s_object *output, *s_tmp;
#ifndef USING_R
    s_object *raw_obj, *raw_container;
#endif
    /* unsigned long  *lens;  */
    int i, j, null_item, expand;
    Sint *fld_nullOk, completed;
    Stype *fld_Sclass;
    Sint num_rec;
    int num_fields;
    int num_rows;               /*num_rows added to count number of rows */
    int k;                      /* This takes care of pointer to the required row */

    result = RS_DBI_getResultSet(rsHandle);
    flds = result->fields;

    if (result->isSelect != 1) {
        RS_DBI_errorMessage("resultSet does not correspond to a SELECT statement", RS_DBI_ERROR);
    }
    if (!flds) {
        RS_DBI_errorMessage("corrupt resultSet, missing fieldDescription", RS_DBI_ERROR);
    }
    num_rec = INT_EL(max_rec, 0);
    expand = (num_rec < 0);     /* dyn expand output to accommodate all rows */
    if (expand || num_rec == 0) {
        mgr = RS_DBI_getManager(rsHandle);
        /* num_rec contains "default num of records per fetch"  */
        num_rec = mgr->fetch_default_rec;
    }
    num_fields = flds->num_fields;
    MEM_PROTECT(output = NEW_LIST((Sint) num_fields));
    RS_DBI_allocOutput(output, flds, num_rec, 0);
#ifndef USING_R
    if (IS_LIST(output)) {
        output = AS_LIST(output);
    }
    else {
        RS_DBI_errorMessage("internal error: could not alloc output list", RS_DBI_ERROR);
    }
#endif
    fld_Sclass = flds->Sclass;
    fld_nullOk = flds->nullOk;



    /* actual fetching.... */
    my_result = (PGresult *) result->drvResultSet;

    num_rows = PQntuples(my_result);

    k = result->rowCount;       /* ADDED */

    completed = (Sint) 0;
    for (i = 0;; i++, k++) {
        if (k >= num_rows) {
            completed = 1;
            break;
        }

        if (i == num_rec) {     /* exhausted the allocated space */
            if (expand) {       /* do we extend or return the records fetched so far */
                num_rec = 2 * num_rec;
                RS_DBI_allocOutput(output, flds, num_rec, expand);
#ifndef USING_R
                if (IS_LIST(output)) {
                    output = AS_LIST(output);
                }
                else {
                    RS_DBI_errorMessage("internal error: could not alloc output list", RS_DBI_ERROR);
                }
#endif
            }
            else
                break;          /* okay, no more fetching for now */
        }

        /* PQgetlength (Returns the actual length of a field value in bytes)  is used instead of lens
         * Syntax: int PQgetlength(const PGresult *res, int row_number, int column_number);
         */


        if (i == num_rows) {    /* we finish  or encounter an error */
            RS_DBI_connection *con;
            con = RS_DBI_getConnection(rsHandle);

            if (strcmp(PQerrorMessage((PGconn *) con->drvConnection), "") == 0) {
                completed = 1;
            }
            else {
                completed = -1;
            }
            break;

        }


        for (j = 0; j < num_fields; j++) {


            /* Testing a field for a null value ...
             * Syntax: int PQgetisnull(const PGresult *res, int row_number, int column_number);
             * This function returns 1 if the field is null and 0 if it contains a non-null value.
             */

            null_item = PQgetisnull(my_result, k, j);

            switch ((int) fld_Sclass[j]) {

            case LOGICAL_TYPE:
                if (null_item) {
                    NA_SET(&(LST_INT_EL(output, j, i)), LOGICAL_TYPE);
                }
                else if (strcmp(PQgetvalue(my_result, k, j), "f") == 0) {
                    LST_LGL_EL(output, j, i) = (Sint) 0;        /* FALSE */
                }
                else if (strcmp(PQgetvalue(my_result, k, j), "t") == 0) {
                    LST_LGL_EL(output, j, i) = (Sint) 1;        /* TRUE */
                }
                break;

            case INTEGER_TYPE:
                if (null_item) {
                    NA_SET(&(LST_INT_EL(output, j, i)), INTEGER_TYPE);
                }
                else {
                    LST_INT_EL(output, j, i) = (Sint) atol(PQgetvalue(my_result, k, j));        /* NOTE: changed */
                }
                break;

            case CHARACTER_TYPE:
                if (null_item) {
#ifdef USING_R
                    SET_LST_CHR_EL(output, j, i, NA_STRING);
#else
                    NA_CHR_SET(LST_CHR_EL(output, j, i));
#endif
                }
                else {
                    SET_LST_CHR_EL(output, j, i, C_S_CPY(PQgetvalue(my_result, k, j)));
                }
                break;

            case NUMERIC_TYPE:
                if (null_item) {
                    NA_SET(&(LST_NUM_EL(output, j, i)), NUMERIC_TYPE);
                }
                else {
                    LST_NUM_EL(output, j, i) = (double) atof(PQgetvalue(my_result, k, j));
                }
                break;

#ifndef USING_R
            case SINGLE_TYPE:
                if (null_item) {
                    NA_SET(&(LST_FLT_EL(output, j, i)), SINGLE_TYPE);
                }
                else {
                    LST_FLT_EL(output, j, i) = (float) atof(PQgetvalue(my_result, k, j));
                }
                break;

            case RAW_TYPE:     /* these are blob's */
                raw_obj = NEW_RAW((Sint) PQgetlength(my_result, k, j));
                memcpy(RAW_DATA(raw_obj), PQgetvalue(my_result, k, j), PQgetlength(my_result, k, j));
                raw_container = LST_EL(output, j);
                SET_ELEMENT(raw_container, (Sint) i, raw_obj);
                SET_ELEMENT(output, (Sint) j, raw_container);
                break;
#endif
            default:
                if (null_item) {
#ifdef USING_R
                    SET_LST_CHR_EL(output, j, i, NA_STRING);
#else
                    NA_CHR_SET(LST_CHR_EL(output, j, i));
#endif
                }
                else {
                    char warn[64];
                    snprintf(warn, 64, "unrecognized field type %d in column %d", (int) fld_Sclass[j], (int) j);
                    RS_DBI_errorMessage(warn, RS_DBI_WARNING);
                    SET_LST_CHR_EL(output, j, i, C_S_CPY(PQgetvalue(my_result, k, j))); /* NOTE: changed */
                }
                break;
            }
        }
    }

    /* actual number of records fetched */
    if (i < num_rec) {
        num_rec = i;
        /* adjust the length of each of the members in the output_list */
        for (j = 0; j < num_fields; j++) {
            s_tmp = LST_EL(output, j);
            MEM_PROTECT(SET_LENGTH(s_tmp, num_rec));
            SET_ELEMENT(output, j, s_tmp);
            MEM_UNPROTECT(1);
        }
    }
    if (completed < 0) {
        RS_DBI_errorMessage("error while fetching rows", RS_DBI_WARNING);
    }
    result->rowCount += num_rec;
    result->completed = (int) completed;

    MEM_UNPROTECT(1);
    return output;
}

/* return a 2-elem list with the last exception number and
 * exception message on a given connection.
 */
s_object *
RS_PostgreSQL_getException(s_object * conHandle)
{
    S_EVALUATOR PGconn * my_connection;
    s_object *output;
    RS_DBI_connection *con;
    Sint n = 2;
    char *exDesc[] = { "errorNum", "errorMsg" };
    Stype exType[] = { INTEGER_TYPE, CHARACTER_TYPE };
    Sint exLen[] = { 1, 1 };

    con = RS_DBI_getConnection(conHandle);
    if (!con->drvConnection) {
        RS_DBI_errorMessage("internal error: corrupt connection handle", RS_DBI_ERROR);
    }
    output = RS_DBI_createNamedList(exDesc, exType, exLen, n);
#ifndef USING_R
    if (IS_LIST(output)) {
        output = AS_LIST(output);
    }
    else {
        RS_DBI_errorMessage("internal error: could not allocate named list", RS_DBI_ERROR);
    }
#endif

    my_connection = (PGconn *) con->drvConnection;

    LST_INT_EL(output, 0, 0) = 0;

    /* PQerrorMessage: Returns the error message most recently generated by an  * operation on the connection.
     * char *PQerrorMessage(const PGconn *conn);
     */

    if (strcmp(PQerrorMessage(my_connection), "") == 0) {
        SET_LST_CHR_EL(output, 1, 0, C_S_CPY("OK"));
    }
    else {
        SET_LST_CHR_EL(output, 1, 0, C_S_CPY(PQerrorMessage(my_connection)));
    }
    return output;
}


s_object *
RS_PostgreSQL_closeResultSet(s_object * resHandle)
{
    S_EVALUATOR RS_DBI_resultSet * result;
    PGresult *my_result;
    s_object *status;

    result = RS_DBI_getResultSet(resHandle);

    my_result = (PGresult *) result->drvResultSet;

    PQclear(my_result);

    /* need to NULL drvResultSet, otherwise can't free the rsHandle */
    result->drvResultSet = (void *) NULL;
    RS_DBI_freeResultSet(resHandle);

    MEM_PROTECT(status = NEW_LOGICAL((Sint) 1));
    LGL_EL(status, 0) = TRUE;
    MEM_UNPROTECT(1);

    return status;
}

s_object *
RS_PostgreSQL_managerInfo(Mgr_Handle * mgrHandle)
{
    S_EVALUATOR RS_DBI_manager * mgr;
    s_object *output;
    Sint i, num_con, max_con, *cons, ncon;
    Sint j, n = 7;
    char *mgrDesc[] = { "drvName", "connectionIds", "fetch_default_rec",
        "managerId", "length", "num_con",
        "counter"               /*,   "clientVersion" */
    };
    Stype mgrType[] = { CHARACTER_TYPE, INTEGER_TYPE, INTEGER_TYPE,
        INTEGER_TYPE, INTEGER_TYPE, INTEGER_TYPE,
        INTEGER_TYPE            /*,   CHARACTER_TYPE */
    };
    Sint mgrLen[] = { 1, 1, 1, 1, 1, 1, 1 /*, 1 */  };

    mgr = RS_DBI_getManager(mgrHandle);
    if (!mgr) {
        RS_DBI_errorMessage("driver not loaded yet", RS_DBI_ERROR);
    }
    num_con = (Sint) mgr->num_con;
    max_con = (Sint) mgr->length;
    mgrLen[1] = num_con;

    output = RS_DBI_createNamedList(mgrDesc, mgrType, mgrLen, n);
#ifndef USING_R
    if (IS_LIST(output)) {
        output = AS_LIST(output);
    }
    else {
        RS_DBI_errorMessage("internal error: could not alloc named list", RS_DBI_ERROR);
    }
#endif
    j = (Sint) 0;
    if (mgr->drvName) {
        SET_LST_CHR_EL(output, j++, 0, C_S_CPY(mgr->drvName));
    }
    else {
        SET_LST_CHR_EL(output, j++, 0, C_S_CPY(""));
    }
    cons = (Sint *) S_alloc((long) max_con, (int) sizeof(Sint));
    ncon = RS_DBI_listEntries(mgr->connectionIds, mgr->length, cons);
    if (ncon != num_con) {
        RS_DBI_errorMessage("internal error: corrupt RS_DBI connection table", RS_DBI_ERROR);
    }
    for (i = 0; i < num_con; i++) {
        LST_INT_EL(output, j, i) = cons[i];
    }
    j++;
    LST_INT_EL(output, j++, 0) = mgr->fetch_default_rec;
    LST_INT_EL(output, j++, 0) = mgr->managerId;
    LST_INT_EL(output, j++, 0) = mgr->length;
    LST_INT_EL(output, j++, 0) = mgr->num_con;
    LST_INT_EL(output, j++, 0) = mgr->counter;

    return output;
}

s_object *
RS_PostgreSQL_connectionInfo(Con_Handle * conHandle)
{
    S_EVALUATOR PGconn * my_con;
    RS_PostgreSQL_conParams *conParams;
    RS_DBI_connection *con;
    s_object *output;
    Sint i, n = 7 /*8 */ , *res, nres;
    char *conDesc[] = { "host", "user", "dbname",
        "serverVersion", "protocolVersion",
        "backendPId", "rsId"
    };
    Stype conType[] = { CHARACTER_TYPE, CHARACTER_TYPE, CHARACTER_TYPE,
        /* CHARACTER_TYPE, */ CHARACTER_TYPE, INTEGER_TYPE,
        INTEGER_TYPE, INTEGER_TYPE
    };
    Sint conLen[] = { 1, 1, 1 /*, 1 */ , 1, 1, 1, 1 };

    con = RS_DBI_getConnection(conHandle);
    conLen[6 /*7 */ ] = con->num_res;   /* num of open resultSets */
    my_con = (PGconn *) con->drvConnection;
    output = RS_DBI_createNamedList(conDesc, conType, conLen, n);
#ifndef USING_R
    if (IS_LIST(output)) {
        output = AS_LIST(output);
    }
    else {
        RS_DBI_errorMessage("internal error: could not alloc named list", RS_DBI_ERROR);
    }
#endif
    conParams = (RS_PostgreSQL_conParams *) con->conParams;

    SET_LST_CHR_EL(output, 0, 0, C_S_CPY(conParams->host));
    SET_LST_CHR_EL(output, 1, 0, C_S_CPY(conParams->user));
    SET_LST_CHR_EL(output, 2, 0, C_S_CPY(conParams->dbname));

    /* PQserverVersion: Returns an integer representing the backend version.
     * Syntax: int PQserverVersion(const PGconn *conn);
     */
    /*Long int is taken because in some Operating sys int is 2 bytes which will not be enough for     * server version number
     */
    long int sv = PQserverVersion(my_con);

    int major = (int) sv / 10000;
    int minor = (int) (sv % 10000) / 100;
    int revision_num = (int) (sv % 10000) % 100;

    char buf1[50];

    sprintf(buf1, "%d.%d.%d", major, minor, revision_num);

    SET_LST_CHR_EL(output, 3, 0, C_S_CPY(buf1));

    /* PQprotocolVersion: Interrogates the frontend/backend protocol being used.
     * int PQprotocolVersion(const PGconn *conn);
     */
    LST_INT_EL(output, 4 /*5 */ , 0) = (Sint) PQprotocolVersion(my_con);

    /* PQbackendPID: Returns the process ID (PID) of the backend server process handling     * this connection.
     * Syntax: int PQbackendPID(const PGconn *conn);
     */

    LST_INT_EL(output, 5 /*6 */ , 0) = (Sint) PQbackendPID(my_con);

    res = (Sint *) S_alloc((long) con->length, (int) sizeof(Sint));
    nres = RS_DBI_listEntries(con->resultSetIds, con->length, res);
    if (nres != con->num_res) {
        RS_DBI_errorMessage("internal error: corrupt RS_DBI resultSet table", RS_DBI_ERROR);
    }

    for (i = 0; i < con->num_res; i++) {
        LST_INT_EL(output, 6, i) = (Sint) res[i];
    }

    return output;
}

s_object *
RS_PostgreSQL_resultSetInfo(Res_Handle * rsHandle)
{
    S_EVALUATOR RS_DBI_resultSet * result;
    s_object *output, *flds;
    Sint n = 6;
    char *rsDesc[] = { "statement", "isSelect", "rowsAffected",
        "rowCount", "completed", "fieldDescription"
    };
    Stype rsType[] = { CHARACTER_TYPE, INTEGER_TYPE, INTEGER_TYPE,
        INTEGER_TYPE, INTEGER_TYPE, LIST_TYPE
    };
    Sint rsLen[] = { 1, 1, 1, 1, 1, 1 };

    result = RS_DBI_getResultSet(rsHandle);
    if (result->fields) {
        flds = RS_DBI_getFieldDescriptions(result->fields);
    }
    else {
        flds = S_NULL_ENTRY;
    }
    output = RS_DBI_createNamedList(rsDesc, rsType, rsLen, n);
#ifndef USING_R
    if (IS_LIST(output)) {
        output = AS_LIST(output);
    }
    else {
        RS_DBI_errorMessage("internal error: could not alloc named list", RS_DBI_ERROR);
    }
#endif
    SET_LST_CHR_EL(output, 0, 0, C_S_CPY(result->statement));
    LST_INT_EL(output, 1, 0) = result->isSelect;
    LST_INT_EL(output, 2, 0) = result->rowsAffected;
    LST_INT_EL(output, 3, 0) = result->rowCount;
    LST_INT_EL(output, 4, 0) = result->completed;
    if (flds != S_NULL_ENTRY) {
        SET_ELEMENT(LST_EL(output, 5), (Sint) 0, flds);
    }
    return output;
}

s_object *
RS_PostgreSQL_typeNames(s_object * type)
{
    s_object *typeNames;
    Sint n, *typeCodes;
    int i;

    n = LENGTH(type);
    typeCodes = INTEGER_DATA(type);
    MEM_PROTECT(typeNames = NEW_CHARACTER(n));
    for (i = 0; i < n; i++) {
        SET_CHR_EL(typeNames, i, C_S_CPY(RS_DBI_getTypeName(typeCodes[i], RS_PostgreSQL_dataTypes)));
    }
    MEM_UNPROTECT(1);
    return typeNames;
}

/*
 * RS_PostgreSQL_dbApply.
 *
 * R/S: dbApply(rs, INDEX, FUN, group.begin, group.end, end, ...)
 *
 * This first implementation of R's dbApply()
 * extracts rows from an open result set rs and applies functions
 * to those rows of each group.  This is how it works: it keeps tracks of
 * the values of the field pointed by "group" and it identifies events:
 * BEGIN_GROUP (just read the first row of a different group),
 * NEW_RECORD (every record fetched generates this event),
 * and END_GROUP (just finished with the current group). At these points
 * we invoke the R functions group.end() and group.begin() in the
 * environment() of dbApply
 * [should it be the environment where dbApply was called from (i.e.,
 * dbApply's parent's * frame)?]
 * Except for the very first group, the order of invocation is
 * end.group() followed by begin.group()
 *
 * NOTE: We're thinking of groups as commonly defined in awk scripts
 * (but also in SAP's ABAP/4) were rows are assumed to be sorted by
 * the "group" fields and we detect a different (new) group when any of
 * the "group" fields changes.  Our implementation does not require
 * the result set to be sorted by group, but for performance-sake,
 * it better be.
 *
 * TODO: 1. Notify the reason for exiting (normal, exhausted maxBatches, etc.)
 *       2. Allow INDEX to be a list, as in tapply().
 *       3. Handle NA's (SQL NULL's) in the INDEX and/or data fields.
 *          Currently they are ignored, thus effectively causing a
 *          new BEGIN_GROUP event.
 *       4. Re-write fetch() in terms of events (END_OF_DATA,
 *          EXHAUST_DATAFRAME, DB_ERROR, etc.)
 *       5. Create a table of R callback functions indexed by events,
 *          then a handle_event() could conveniently handle all the events.
 */

s_object *expand_list(s_object * old, Sint new_len);
void add_group(s_object * group_names, s_object * data, Stype * fld_Sclass, Sint group, Sint ngroup, Sint i);
unsigned int check_groupEvents(s_object * data, Stype fld_Sclass[], Sint row, Sint col);

/* The following are the masks for the events/states we recognize as we
 * bring rows from the result set/cursor
 */
#define NEVER           0
#define BEGIN           1       /* prior to reading 1st row from the resultset */
#define END             2       /* after reading last row from the result set  */
#define BEGIN_GROUP     4       /* just read in 1'st row for a different group */
#define END_GROUP       8       /* just read the last row of the current group */
#define NEW_RECORD     16       /* uninteresting */
#define PARTIAL_GROUP  32       /* too much data (>max_rex) partial buffer     */

/* the following are non-grouping events (e.g., db errors, memory) */
#define EXHAUSTED_DF   64       /* exhausted the allocated data.frame  */
#define EXHAUSTED_OUT 128       /* exhausted the allocated output list */
#define END_OF_DATA   256       /* end of data from the result set     */
#define DBMS_ERROR    512       /* error in remote dbms                */

/* beginGroupFun takes only one arg: the name of the current group */
s_object *
RS_DBI_invokeBeginGroup(s_object * callObj,     /* should be initialized */
                        const char *group_name, /* one string */
                        s_object * rho)
{
    S_EVALUATOR s_object * s_group_name, *val;

    /* make a copy of the argument */
    MEM_PROTECT(s_group_name = NEW_CHARACTER((Sint) 1));
    SET_CHR_EL(s_group_name, 0, C_S_CPY(group_name));

    /* and stick into call object */
    SETCADR(callObj, s_group_name);
    val = EVAL_IN_FRAME(callObj, rho);
    MEM_UNPROTECT(1);

    return S_NULL_ENTRY;
}

s_object *
RS_DBI_invokeNewRecord(s_object * callObj,      /* should be initialized already */
                       s_object * new_record,   /* a 1-row data.frame */
                       s_object * rho)
{
    S_EVALUATOR s_object * df, *val;

    /* make a copy of the argument */
    MEM_PROTECT(df = COPY_ALL(new_record));

    /* and stick it into the call object */
    SETCADR(callObj, df);
    val = EVAL_IN_FRAME(callObj, rho);
    MEM_UNPROTECT(1);

    return S_NULL_ENTRY;
}

/* endGroupFun takes two args: a data.frame and the group name */
s_object *
RS_DBI_invokeEndGroup(s_object * callObj, s_object * data, const char *group_name, s_object * rho)
{
    S_EVALUATOR s_object * s_x, *s_group_name, *val;

    /* make copies of the arguments */
    MEM_PROTECT(callObj = duplicate(callObj));
    MEM_PROTECT(s_x = COPY_ALL(data));
    MEM_PROTECT(s_group_name = NEW_CHARACTER((Sint) 1));
    SET_CHR_EL(s_group_name, 0, C_S_CPY(group_name));

    /* stick copies of args into the call object */
    SETCADR(callObj, s_x);
    SETCADDR(callObj, s_group_name);
    SETCADDDR(callObj, R_DotsSymbol);

    val = EVAL_IN_FRAME(callObj, rho);

    MEM_UNPROTECT(3);
    return val;
}

s_object *                      /* output is a named list */
RS_PostgreSQL_dbApply(s_object * rsHandle,      /* resultset handle */
                      s_object * s_group_field, /* this is a 0-based field number */
                      s_object * s_funs,        /* a 5-elem list with handler funs */
                      s_object * rho,   /* the env where to run funs */
                      s_object * s_batch_size,  /* alloc these many rows */
                      s_object * s_max_rec)
{                               /* max rows per group */
    S_EVALUATOR RS_DBI_resultSet * result;
    RS_DBI_fields *flds;

    PGresult *my_result;
    /* POSTGRESQL_ROW  row;   NOTE: REMOVED  ths.... because it is MySQL specific */

    int row_counter = -1;       /* NOTE: added this.... to maintain a counter for the rows */
    int row_max;                /* NOTE: added this.... fetch the maximum number of rows in the resultset */

    s_object *data, *cur_rec, *out_list, *group_names, *val;
#ifndef USING_R
    s_object *raw_obj, *raw_container;
#endif
    /*  unsigned long  *lens = (unsigned long *)0; NOTE: not being used */
    Stype *fld_Sclass;
    Sint i, j, null_item, expand, *fld_nullOk, completed;
    Sint num_rec, num_groups;
    int num_fields;
    Sint max_rec = INT_EL(s_max_rec, 0);        /* max rec per group */
    Sint ngroup = 0, group_field = INT_EL(s_group_field, 0);
    long total_records;
    Sint pushed_back = FALSE;

    unsigned int event = NEVER;
    int np = 0;                 /* keeps track of MEM_PROTECT()'s */
    s_object *beginGroupCall, *beginGroupFun = LST_EL(s_funs, 2);
    s_object *endGroupCall, *endGroupFun = LST_EL(s_funs, 3);
    s_object *newRecordCall, *newRecordFun = LST_EL(s_funs, 4);
    int invoke_beginGroup = (GET_LENGTH(beginGroupFun) > 0);
    int invoke_endGroup = (GET_LENGTH(endGroupFun) > 0);
    int invoke_newRecord = (GET_LENGTH(newRecordFun) > 0);

    /* row = NULL;     NOTE: REMOVED  ths.... because it is MySQL specific */

    beginGroupCall = R_NilValue;        /* -Wall */
    if (invoke_beginGroup) {
        MEM_PROTECT(beginGroupCall = lang2(beginGroupFun, R_NilValue));
        ++np;
    }
    endGroupCall = R_NilValue;  /* -Wall */
    if (invoke_endGroup) {
        /* TODO: append list(...) to the call object */
        MEM_PROTECT(endGroupCall = lang4(endGroupFun, R_NilValue, R_NilValue, R_NilValue));
        ++np;
    }
    newRecordCall = R_NilValue; /* -Wall */
    if (invoke_newRecord) {
        MEM_PROTECT(newRecordCall = lang2(newRecordFun, R_NilValue));
        ++np;
    }

    result = RS_DBI_getResultSet(rsHandle);
    flds = result->fields;
    if (!flds) {
        RS_DBI_errorMessage("corrupt resultSet, missing fieldDescription", RS_DBI_ERROR);
    }
    num_fields = flds->num_fields;
    fld_Sclass = flds->Sclass;
    fld_nullOk = flds->nullOk;
    MEM_PROTECT(data = NEW_LIST((Sint) num_fields));    /* buffer records */
    MEM_PROTECT(cur_rec = NEW_LIST((Sint) num_fields)); /* current record */
    np += 2;
    RS_DBI_allocOutput(cur_rec, flds, (Sint) 1, 1);
    RS_DBI_makeDataFrame(cur_rec);

    num_rec = INT_EL(s_batch_size, 0);  /* this is num of rec per group! */
    max_rec = INT_EL(s_max_rec, 0);     /* max rec **per group**         */
    num_groups = num_rec;
    MEM_PROTECT(out_list = NEW_LIST(num_groups));
    MEM_PROTECT(group_names = NEW_CHARACTER(num_groups));
    np += 2;

    /* set conversion for group names */

    if (result->rowCount == 0) {
        event = BEGIN;
        /* here we could invoke the begin function */
    }

    /* actual fetching.... */

    my_result = (PGresult *) result->drvResultSet;
    completed = (Sint) 0;

    row_max = PQntuples(my_result);

    total_records = 0;
    expand = 0;                 /* expand or init each data vector? */
    i = 0;                      /* index into row number **within** groups */
    while (1) {

        if (i == 0 || i == num_rec) {   /* BEGIN, EXTEND_DATA, BEGIN_GROUP */

            /* reset num_rec upon a new group, double it if needs to expand */
            num_rec = (i == 0) ? INT_EL(s_batch_size, 0) : 2 * num_rec;
            if (i < max_rec) {
                RS_DBI_allocOutput(data, flds, num_rec, expand++);
            }
            else {
                break;          /* ok, no more fetching for now (pending group?) */
            }
        }

        if (!pushed_back) {
            /*   row = postgresql_fetch_row(my_result); Removed.... MYSQL specific */
            ++row_counter;
        }
        if (row_counter == row_max) {   /*finish  *//*NOTE:Changed */

            completed = (Sint) 1;
            /* TODO: error handling has to be done */
            break;

            /*  NOTE: Removed
               unsigned int  err_no;
               RS_DBI_connection   *con;
               con = RS_DBI_getConnection(rsHandle);
               err_no = postgresql_errno((POSTGRESQL *) con->drvConnection);
               completed = (Sint) (err_no ? -1 : 1);
               break;
             */
        }

        if (!pushed_back) {     /* recompute fields lengths? */
            /* lens = postgresql_fetch_lengths(my_result);  NOTE: NOT required *//* lengths for each field */
            ++total_records;
        }
        else {
            pushed_back = FALSE;
        }
        /* coerce each entry row[j] to an R/S type according to its Sclass.
         * TODO:  converter functions are badly needed.
         */
        for (j = 0; j < num_fields; j++) {

            /*   null_item = (row[j] == NULL); NOTE: REMOVED */

            null_item = PQgetisnull(my_result, row_counter, j);

            switch ((int) fld_Sclass[j]) {

            case INTEGER_TYPE:
                if (null_item)
                    NA_SET(&(LST_INT_EL(data, j, i)), INTEGER_TYPE);
                else
                    LST_INT_EL(data, j, i) = atol(PQgetvalue(my_result, row_counter, j));       /* NOTE: changed */
                LST_INT_EL(cur_rec, j, 0) = LST_INT_EL(data, j, i);
                break;

            case CHARACTER_TYPE:
                /* BUG: I need to verify that a TEXT field (which is stored as
                 * a BLOB by PostgreSQL!) is indeed char and not a true
                 * Binary obj (PostgreSQL does not truly distinguish them). This
                 * test is very gross.
                 */
                if (null_item) {
#ifdef USING_R
                    SET_LST_CHR_EL(data, j, i, NA_STRING);
#else
                    NA_CHR_SET(LST_CHR_EL(data, j, i));
#endif
                }
                else {
                    if ((size_t) PQfsize(my_result, j) != strlen(PQgetvalue(my_result, row_counter, j))) {      /* NOTE: changed */
                        char warn[128];
                        snprintf(warn, 128, "internal error: row %ld field %ld truncated", (long) i, (long) j);
                        RS_DBI_errorMessage(warn, RS_DBI_WARNING);
                    }
                    SET_LST_CHR_EL(data, j, i, C_S_CPY(PQgetvalue(my_result, row_counter, j))); /* NOTE: changed */
                }
                SET_LST_CHR_EL(cur_rec, j, 0, C_S_CPY(LST_CHR_EL(data, j, i)));
                break;

            case NUMERIC_TYPE:
                if (null_item) {
                    NA_SET(&(LST_NUM_EL(data, j, i)), NUMERIC_TYPE);
                }
                else {
                    LST_NUM_EL(data, j, i) = (double) atof(PQgetvalue(my_result, row_counter, j));      /* NOTE: changed */
                }
                LST_NUM_EL(cur_rec, j, 0) = LST_NUM_EL(data, j, i);
                break;

#ifndef USING_R
            case SINGLE_TYPE:
                if (null_item) {
                    NA_SET(&(LST_FLT_EL(data, j, i)), SINGLE_TYPE);
                }
                else {
                    LST_FLT_EL(data, j, i) = (float) atof(PQgetvalue(my_result, row_counter, j));       /* NOTE: changed */
                }
                LST_FLT_EL(cur_rec, j, 0) = LST_FLT_EL(data, j, i);
                break;

            case RAW_TYPE:     /* these are blob's */
                raw_obj = NEW_RAW((Sint) PQfsize(my_result, j));        /* NOTE: changed */
                memcpy(RAW_DATA(raw_obj), PQgetvalue(my_result, row_counter, j), PQfsize(my_result, j));        /* NOTE: changed */
                raw_container = LST_EL(data, j);        /* get list of raw objects */
                SET_ELEMENT(raw_container, (Sint) i, raw_obj);
                SET_ELEMENT(data, (Sint) j, raw_container);
                break;
#endif

            default:           /* error, but we'll try the field as character (!) */
                if (null_item) {
#ifdef USING_R
                    SET_LST_CHR_EL(data, j, i, NA_STRING);
#else
                    NA_CHR_SET(LST_CHR_EL(data, j, i));
#endif
                }
                else {
                    char warn[64];
                    snprintf(warn, 64, "unrecognized field type %d in column %d", (int) fld_Sclass[j], (int) j);
                    RS_DBI_errorMessage(warn, RS_DBI_WARNING);
                    SET_LST_CHR_EL(data, j, i, C_S_CPY(PQgetvalue(my_result, row_counter, j)));
                }
                SET_LST_CHR_EL(cur_rec, j, 0, C_S_CPY(LST_CHR_EL(data, j, i)));
                break;
            }
        }

        /* We just finished processing the new record, now we check
         * for some events (in addition to NEW_RECORD, of course).
         */
        event = check_groupEvents(data, fld_Sclass, i, group_field);

        if (BEGIN_GROUP & event) {
            if (ngroup == num_groups) { /* exhausted output list? */
                num_groups = 2 * num_groups;
                MEM_PROTECT(SET_LENGTH(out_list, num_groups));
                MEM_PROTECT(SET_LENGTH(group_names, num_groups));
                np += 2;
            }
            if (invoke_beginGroup) {
                RS_DBI_invokeBeginGroup(beginGroupCall, CHR_EL(group_names, ngroup), rho);
            }
        }

        if (invoke_newRecord) {
            RS_DBI_invokeNewRecord(newRecordCall, cur_rec, rho);
        }

        if (END_GROUP & event) {

            add_group(group_names, data, fld_Sclass, group_field, ngroup, i - 1);

            RS_DBI_allocOutput(data, flds, i, expand++);
            RS_DBI_makeDataFrame(data);

            val = RS_DBI_invokeEndGroup(endGroupCall, data, CHR_EL(group_names, ngroup), rho);
            SET_ELEMENT(out_list, ngroup, val);

            /* set length of data to zero to force initialization
             * for next group
             */
            RS_DBI_allocOutput(data, flds, (Sint) 0, (Sint) 1);
            i = 0;              /* flush */
            ++ngroup;
            pushed_back = TRUE;
            continue;
        }
        i++;
    }

    /* we fetched all the rows we needed/could; compute actual number of
     * records fetched.
     * TODO: What should we return in the case of partial groups???
     */
    if (completed < 0) {
        RS_DBI_errorMessage("error while fetching rows", RS_DBI_WARNING);
    }
    else if (completed) {
        event = (END_GROUP | END);
    }
    else {
        event = PARTIAL_GROUP;
    }

    /* wrap up last group */
    if ((END_GROUP & event) || (PARTIAL_GROUP & event)) {

        add_group(group_names, data, fld_Sclass, group_field, ngroup, i - i);

        if (i < num_rec) {
            RS_DBI_allocOutput(data, flds, i, expand++);
            RS_DBI_makeDataFrame(data);
        }
        if (invoke_endGroup) {
            val = RS_DBI_invokeEndGroup(endGroupCall, data, CHR_EL(group_names, ngroup), rho);
            SET_ELEMENT(out_list, ngroup++, val);
        }
        if (PARTIAL_GROUP & event) {
            char buf[512];
            (void) strcpy(buf, "exhausted the pre-allocated storage. The last ");
            (void) strcat(buf, "output group was computed with partial data. ");
            (void) strcat(buf, "The remaining data were left un-read in the ");
            (void) strcat(buf, "result set.");
            RS_DBI_errorMessage(buf, RS_DBI_WARNING);
        }
    }

    /* set the correct length of output list */
    if (GET_LENGTH(out_list) != ngroup) {
        MEM_PROTECT(SET_LENGTH(out_list, ngroup));
        MEM_PROTECT(SET_LENGTH(group_names, ngroup));
        np += 2;
    }

    result->rowCount += total_records;
    result->completed = (int) completed;

    SET_NAMES(out_list, group_names);   /* do I need to PROTECT? */
#ifndef USING_R
    out_list = AS_LIST(out_list);       /* for S4/Splus[56]'s sake */
#endif

    MEM_UNPROTECT(np);
    return out_list;
}

unsigned int
check_groupEvents(s_object * data, Stype fld_Sclass[], Sint irow, Sint jcol)
{
    if (irow == 0) {              /* Begin */
        return (BEGIN | BEGIN_GROUP);
    }
    switch (fld_Sclass[jcol]) {

    case LOGICAL_TYPE:
        if (LST_LGL_EL(data, jcol, irow) != LST_LGL_EL(data, jcol, irow - 1)) {
            return (END_GROUP | BEGIN_GROUP);
        }
        break;

    case INTEGER_TYPE:
        if (LST_INT_EL(data, jcol, irow) != LST_INT_EL(data, jcol, irow - 1)) {
            return (END_GROUP | BEGIN_GROUP);
        }
        break;

    case NUMERIC_TYPE:
        if (LST_NUM_EL(data, jcol, irow) != LST_NUM_EL(data, jcol, irow - 1)) {
            return (END_GROUP | BEGIN_GROUP);
        }
        break;

#ifndef USING_R
    case SINGLE_TYPE:
        if (LST_FLT_EL(data, jcol, irow) != LST_FLT_EL(data, jcol, irow - 1)) {
            return (END_GROUP | BEGIN_GROUP);
        }
        break;
#endif

    case CHARACTER_TYPE:
        if (strcmp(LST_CHR_EL(data, jcol, irow), LST_CHR_EL(data, jcol, irow - 1))) {
            return (END_GROUP | BEGIN_GROUP);
        }
        break;

    default:
        PROBLEM "un-regongnized R/S data type %d", fld_Sclass[jcol] ERROR;
        break;
    }

    return NEW_RECORD;
}

/* append current group (as character) to the vector of group names */
void
add_group(s_object * group_names, s_object * data, Stype * fld_Sclass, Sint group_field, Sint ngroup, Sint i)
{
    char buff[1024];

    switch ((int) fld_Sclass[group_field]) {

    case LOGICAL_TYPE:
        (void) sprintf(buff, "%ld", (long) LST_LGL_EL(data, group_field, i));
        break;
    case INTEGER_TYPE:
        (void) sprintf(buff, "%ld", (long) LST_INT_EL(data, group_field, i));
        break;
#ifndef USING_R
    case SINGLE_TYPE:
        (void) sprintf(buff, "%f", (double) LST_FLT_EL(data, group_field, i));
        break;
#endif
    case NUMERIC_TYPE:
        (void) sprintf(buff, "%f", (double) LST_NUM_EL(data, group_field, i));
        break;
    case CHARACTER_TYPE:
        strcpy(buff, LST_CHR_EL(data, group_field, i));
        break;
    default:
        RS_DBI_errorMessage("unrecognized R/S type for group", RS_DBI_ERROR);
        break;
    }
    SET_CHR_EL(group_names, ngroup, C_S_CPY(buff));
    return;
}
