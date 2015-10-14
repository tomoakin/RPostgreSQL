/*
 * RS-DBI.c
 *
 * $Id$
 *
 * This package was developed as a part of Summer of Code program organized by Google.
 * Thanks to David A. James & Saikat DebRoy, the authors of RMySQL package.
 * Code from RMySQL package was reused with the permission from the authors.
 * Also Thanks to my GSoC mentor Dirk Eddelbuettel for helping me in the development.
 *
 * Source processed by:
 * indent -br -i4 -nut --line-length120 --comment-line-length120 --leave-preprocessor-space -npcs RS-DBI.c
 *
 */


#include "RS-DBI.h"

/* TODO: monitor memory/object size consumption against S limits
 *       in $SHOME/include/options.h we find "max_memory". We then
 *       mem_size to make sure we're not bumping into problems.
 *       But, is mem_size() reliable?  How should we do this?
 *
 * TODO: invoke user-specified generators
 *
 * TODO: Implement exception objects for each dbObject.
 */

static RS_DBI_manager *dbManager = NULL;

Mgr_Handle *
RS_DBI_allocManager(const char *drvName, Sint max_con, Sint fetch_default_rec, Sint force_realloc)
{
    /* Currently, the dbManager is a singleton (therefore we don't
     * completly free all the space).  Here we alloc space
     * for the dbManager and return its mgrHandle.  force_realloc
     * means to re-allocate number of connections, etc. (in this case
     * we require to have all connections closed).  (Note that if we
     * re-allocate, we don't re-set the counter, and thus we make sure
     * we don't recycle connection Ids in a giver S/R session).
     */
    Mgr_Handle *mgrHandle;
    RS_DBI_manager *mgr;
    Sint counter;
    Sint mgr_id = (Sint) getpid();
    int i;

    PROTECT(mgrHandle = RS_DBI_asMgrHandle(mgr_id));

    if (!dbManager) {           /* alloc for the first time */
        counter = 0;            /* connections handled so far */
        mgr = (RS_DBI_manager *) malloc(sizeof(RS_DBI_manager));
    }
    else {                      /* we're re-entering */
        if (dbManager->connections) {   /* and mgr is valid */
            if (!force_realloc) {
                UNPROTECT(1);
                return mgrHandle;
            }
            else {
                RS_DBI_freeManager(mgrHandle);  /* i.e., free connection arrays */
            }
        }
        counter = dbManager->counter;
        mgr = dbManager;
    }
    /* Ok, we're here to expand number of connections, etc. */
    if (!mgr) {
        RS_DBI_errorMessage("could not malloc the dbManger", RS_DBI_ERROR);
    }
    mgr->drvName = RS_DBI_copyString(drvName);
    mgr->drvData = (void *) NULL;
    mgr->managerId = mgr_id;
    mgr->connections = (RS_DBI_connection **) calloc((size_t) max_con, sizeof(RS_DBI_connection));
    if (!mgr->connections) {
        free(mgr->drvName);
        free(mgr);
        RS_DBI_errorMessage("could not calloc RS_DBI_connections", RS_DBI_ERROR);
    }
    mgr->connectionIds = (Sint *) calloc((size_t) max_con, sizeof(Sint));
    if (!mgr->connectionIds) {
        free(mgr->drvName);
        free(mgr->connections);
        free(mgr);
        RS_DBI_errorMessage("could not calloc vector of connection Ids", RS_DBI_ERROR);
    }
    mgr->counter = counter;
    mgr->length = max_con;
    mgr->num_con = (Sint) 0;
    mgr->fetch_default_rec = fetch_default_rec;
    for (i = 0; i < max_con; i++) {
        mgr->connectionIds[i] = -1;
        mgr->connections[i] = (RS_DBI_connection *) NULL;
    }

    dbManager = mgr;
    UNPROTECT(1);
    return mgrHandle;
}

/* We don't want to completely free the dbManager, but rather we
 * re-initialize all the fields except for mgr->counter to ensure we don't
 * re-cycle connection ids across R/S DBI sessions in the the same pid
 * (S/R session).
 */
void
RS_DBI_freeManager(Mgr_Handle * mgrHandle)
{
    RS_DBI_manager *mgr;
    int i;

    mgr = RS_DBI_getManager(mgrHandle);
    if (mgr->num_con > 0) {
        char *errMsg = "all opened connections were forcebly closed";
        RS_DBI_errorMessage(errMsg, RS_DBI_WARNING);
    }
    if (mgr->drvData) {
        char *errMsg = "mgr->drvData was not freed (some memory leaked)";
        RS_DBI_errorMessage(errMsg, RS_DBI_WARNING);
    }
    if (mgr->drvName) {
        free(mgr->drvName);
        mgr->drvName = (char *) NULL;
    }
    if (mgr->connections) {
        for (i = 0; i < mgr->num_con; i++) {
            if (mgr->connections[i]) {
                free(mgr->connections[i]);
            }
        }
        free(mgr->connections);
        mgr->connections = (RS_DBI_connection **) NULL;
    }
    if (mgr->connectionIds) {
        free(mgr->connectionIds);
        mgr->connectionIds = (Sint *) NULL;
    }
    return;
}

Con_Handle *
RS_DBI_allocConnection(Mgr_Handle * mgrHandle, Sint max_res)
{
    RS_DBI_manager *mgr;
    RS_DBI_connection *con;
    Con_Handle *conHandle;
    Sint i, indx, con_id;

    mgr = RS_DBI_getManager(mgrHandle);
    indx = RS_DBI_newEntry(mgr->connectionIds, mgr->length);
    if (indx < 0) {
        char buf[128], msg[128];
        (void) strcat(msg, "cannot allocate a new connection -- maximum of ");
        (void) strcat(msg, "%d connections already opened");
        (void) sprintf(buf, msg, (int) mgr->length);
        RS_DBI_errorMessage(buf, RS_DBI_ERROR);
    }
    con = (RS_DBI_connection *) malloc(sizeof(RS_DBI_connection));
    if (!con) {
        char *errMsg = "could not malloc dbConnection";
        RS_DBI_freeEntry(mgr->connectionIds, indx);
        RS_DBI_errorMessage(errMsg, RS_DBI_ERROR);
    }
    con->managerId = MGR_ID(mgrHandle);
    con_id = mgr->counter;
    con->connectionId = con_id;
    con->drvConnection = (void *) NULL;
    con->drvData = (void *) NULL;       /* to be used by the driver in any way */
    con->conParams = (void *) NULL;
    con->counter = (Sint) 0;
    con->length = max_res;      /* length of resultSet vector */

    /* result sets for this connection */
    con->resultSets = (RS_DBI_resultSet **)
        calloc((size_t) max_res, sizeof(RS_DBI_resultSet));
    if (!con->resultSets) {
        char *errMsg = "could not calloc resultSets for the dbConnection";
        RS_DBI_freeEntry(mgr->connectionIds, indx);
        free(con);
        RS_DBI_errorMessage(errMsg, RS_DBI_ERROR);
    }
    con->num_res = (Sint) 0;
    con->resultSetIds = (Sint *) calloc((size_t) max_res, sizeof(Sint));
    if (!con->resultSetIds) {
        char *errMsg = "could not calloc vector of resultSet Ids";
        free(con->resultSets);
        free(con);
        RS_DBI_freeEntry(mgr->connectionIds, indx);
        RS_DBI_errorMessage(errMsg, RS_DBI_ERROR);
    }
    for (i = 0; i < max_res; i++) {
        con->resultSets[i] = (RS_DBI_resultSet *) NULL;
        con->resultSetIds[i] = -1;
    }

    /* Finally, update connection table in mgr */
    mgr->num_con += (Sint) 1;
    mgr->counter += (Sint) 1;
    mgr->connections[indx] = con;
    mgr->connectionIds[indx] = con_id;
    conHandle = RS_DBI_asConHandle(MGR_ID(mgrHandle), con_id);
    return conHandle;
}

/* the invoking (freeing) function must provide a function for
 * freeing the conParams, and by setting the (*free_drvConParams)(void *)
 * pointer.
 */

void
RS_DBI_freeConnection(Con_Handle * conHandle)
{
    RS_DBI_connection *con;
    RS_DBI_manager *mgr;
    Sint indx;

    con = RS_DBI_getConnection(conHandle);
    mgr = RS_DBI_getManager(conHandle);

    /* Are there open resultSets? If so, free them first */
    if (con->num_res > 0) {
        char *errMsg = "opened resultSet(s) forcebly closed";
        int i;
        Res_Handle *rsHandle;

        for (i = 0; i < con->num_res; i++) {
            rsHandle = RS_DBI_asResHandle(con->managerId, con->connectionId, (Sint) con->resultSetIds[i]);
            RS_DBI_freeResultSet(rsHandle);
        }
        RS_DBI_errorMessage(errMsg, RS_DBI_WARNING);
    }
    if (con->drvConnection) {
        char *errMsg =
            "internal error in RS_DBI_freeConnection: driver might have left open its connection on the server";
        RS_DBI_errorMessage(errMsg, RS_DBI_WARNING);
    }
    if (con->conParams) {
        char *errMsg = "internal error in RS_DBI_freeConnection: non-freed con->conParams (tiny memory leaked)";
        RS_DBI_errorMessage(errMsg, RS_DBI_WARNING);
    }
    if (con->drvData) {
        char *errMsg = "internal error in RS_DBI_freeConnection: non-freed con->drvData (some memory leaked)";
        RS_DBI_errorMessage(errMsg, RS_DBI_WARNING);
    }
    /* delete this connection from manager's connection table */
    if (con->resultSets) {
        free(con->resultSets);
    }
    if (con->resultSetIds) {
        free(con->resultSetIds);
    }

    /* update the manager's connection table */
    indx = RS_DBI_lookup(mgr->connectionIds, mgr->length, con->connectionId);
    RS_DBI_freeEntry(mgr->connectionIds, indx);
    mgr->connections[indx] = (RS_DBI_connection *) NULL;
    mgr->num_con -= (Sint) 1;

    free(con);
    con = (RS_DBI_connection *) NULL;

    return;
}

Res_Handle *
RS_DBI_allocResultSet(Con_Handle * conHandle)
{
    RS_DBI_connection *con = NULL;
    RS_DBI_resultSet *result = NULL;
    Res_Handle *rsHandle;
    Sint indx, res_id;

    con = RS_DBI_getConnection(conHandle);
    indx = RS_DBI_newEntry(con->resultSetIds, con->length);
    if (indx < 0) {
        char msg[128], fmt[128];
        (void) strcpy(fmt, "cannot allocate a new resultSet -- ");
        (void) strcat(fmt, "maximum of %d resultSets already reached");
        (void) sprintf(msg, fmt, con->length);
        RS_DBI_errorMessage(msg, RS_DBI_ERROR);
    }

    result = (RS_DBI_resultSet *) malloc(sizeof(RS_DBI_resultSet));
    if (!result) {
        char *errMsg = "could not malloc dbResultSet";
        RS_DBI_freeEntry(con->resultSetIds, indx);
        RS_DBI_errorMessage(errMsg, RS_DBI_ERROR);
    }
    result->drvResultSet = (void *) NULL;       /* driver's own resultSet (cursor) */
    result->drvData = (void *) NULL;    /* this can be used by driver */
    result->statement = (char *) NULL;
    result->managerId = MGR_ID(conHandle);
    result->connectionId = CON_ID(conHandle);
    result->resultSetId = con->counter;
    result->isSelect = (Sint) - 1;
    result->rowsAffected = (Sint) - 1;
    result->rowCount = (Sint) 0;
    result->completed = (Sint) - 1;
    result->fields = (RS_DBI_fields *) NULL;

    /* update connection's resultSet table */
    res_id = con->counter;
    con->num_res += (Sint) 1;
    con->counter += (Sint) 1;
    con->resultSets[indx] = result;
    con->resultSetIds[indx] = res_id;

    rsHandle = RS_DBI_asResHandle(MGR_ID(conHandle), CON_ID(conHandle), res_id);
    return rsHandle;
}

void
RS_DBI_freeResultSet(Res_Handle * rsHandle)
{
    RS_DBI_resultSet *result;
    RS_DBI_connection *con;
    Sint indx;

    con = RS_DBI_getConnection(rsHandle);
    result = RS_DBI_getResultSet(rsHandle);

    if (result->drvResultSet) {
        char *errMsg = "internal error in RS_DBI_freeResultSet: non-freed result->drvResultSet (some memory leaked)";
        RS_DBI_errorMessage(errMsg, RS_DBI_ERROR);
    }
    if (result->drvData) {
        char *errMsg = "internal error in RS_DBI_freeResultSet: non-freed result->drvData (some memory leaked)";
        RS_DBI_errorMessage(errMsg, RS_DBI_WARNING);
    }
    if (result->statement) {
        free(result->statement);
    }
    if (result->fields) {
        RS_DBI_freeFields(result->fields);
    }
    free(result);
    result = (RS_DBI_resultSet *) NULL;

    /* update connection's resultSet table */
    indx = RS_DBI_lookup(con->resultSetIds, con->length, RES_ID(rsHandle));
    RS_DBI_freeEntry(con->resultSetIds, indx);
    con->resultSets[indx] = (RS_DBI_resultSet *) NULL;
    con->num_res -= (Sint) 1;

    return;
}

RS_DBI_fields *
RS_DBI_allocFields(int num_fields)
{
    RS_DBI_fields *flds;
    size_t n;

    flds = (RS_DBI_fields *) malloc(sizeof(RS_DBI_fields));
    if (!flds) {
        char *errMsg = "could not malloc RS_DBI_fields";
        RS_DBI_errorMessage(errMsg, RS_DBI_ERROR);
    }
    n = (size_t) num_fields;
    flds->num_fields = num_fields;
    flds->name = (char **) calloc(n, sizeof(char *));
    flds->type = (Sint *) calloc(n, sizeof(Sint));
    flds->length = (Sint *) calloc(n, sizeof(Sint));
    flds->precision = (Sint *) calloc(n, sizeof(Sint));
    flds->scale = (Sint *) calloc(n, sizeof(Sint));
    flds->nullOk = (Sint *) calloc(n, sizeof(Sint));
    flds->isVarLength = (Sint *) calloc(n, sizeof(Sint));
    flds->Sclass = (Stype *) calloc(n, sizeof(Stype));

    return flds;
}

void
RS_DBI_freeFields(RS_DBI_fields * flds)
{
    int i;
    if (flds->name) {           /* (as per Jeff Horner's patch) */
        for (i = 0; i < flds->num_fields; i++) {
            if (flds->name[i]) {
                free(flds->name[i]);
            }
        }
        free(flds->name);
    }
    if (flds->type) {
        free(flds->type);
    }
    if (flds->length) {
        free(flds->length);
    }
    if (flds->precision) {
        free(flds->precision);
    }
    if (flds->scale) {
        free(flds->scale);
    }
    if (flds->nullOk) {
        free(flds->nullOk);
    }
    if (flds->isVarLength) {
        free(flds->isVarLength);
    }
    if (flds->Sclass) {
        free(flds->Sclass);
    }
    free(flds);
    flds = (RS_DBI_fields *) NULL;
    return;
}

/* Make a data.frame from a named list by adding row.names, and class
 * attribute.  Use "1", "2", .. as row.names.
 * NOTE: Only tested  under R (not tested at all under S4 or Splus5+).
 */
void
RS_DBI_makeDataFrame(s_object * data)
{
    S_EVALUATOR s_object *row_names, *df_class_name;
#ifndef USING_R
    s_object *S_RowNamesSymbol; /* mimic Rinternal.h R_RowNamesSymbol */
    s_object *S_ClassSymbol;
#endif
    Sint i, n;
    char buf[1024];

#ifndef USING_R
    if (IS_LIST(data)) {
        data = AS_LIST(data);
    }
    else {
        RS_DBI_errorMessage
            ("internal error in RS_DBI_makeDataFrame: could not corce named-list into data.frame", RS_DBI_ERROR);
    }
#endif

    MEM_PROTECT(data);
    MEM_PROTECT(df_class_name = NEW_CHARACTER((Sint) 1));
    SET_CHR_EL(df_class_name, 0, C_S_CPY("data.frame"));

    /* row.names */
    n = GET_LENGTH(LST_EL(data, 0));    /* length(data[[1]]) */
    MEM_PROTECT(row_names = NEW_CHARACTER(n));
    for (i = 0; i < n; i++) {
        (void) sprintf(buf, "%d", i + 1);
        SET_CHR_EL(row_names, i, C_S_CPY(buf));
    }
#ifdef USING_R
    SET_ROWNAMES(data, row_names);
    SET_CLASS_NAME(data, df_class_name);
#else
    /* untested S4/Splus code */
    MEM_PROTECT(S_RowNamesSymbol = NEW_CHARACTER((Sint) 1));
    SET_CHR_EL(S_RowNamesSymbol, 0, C_S_CPY("row.names"));

    MEM_PROTECT(S_ClassSymbol = NEW_CHARACTER((Sint) 1));
    SET_CHR_EL(S_ClassSymbol, 0, C_S_CPY("class"));
    /* Note: the fun attribute() is just an educated guess as to
     * which function to use for setting attributes (see S.h)
     */
    (void) attribute(data, S_ClassSymbol, df_class_name);
    MEM_UNPROTECT(2);
#endif
    MEM_UNPROTECT(3);
    return;
}

void
RS_DBI_allocOutput(s_object * output, RS_DBI_fields * flds, Sint num_rec, Sint expand)
{
    s_object *names, *s_tmp;
    Sint j;
    int num_fields;
    Stype *fld_Sclass;

#ifndef USING_R
    if (IS_LIST(output)) {
        output = AS_LIST(output);
    }
    else {
        RS_DBI_errorMessage("internal error in RS_DBI_allocOutput: could not (re)allocate output list", RS_DBI_ERROR);
    }
#endif

    MEM_PROTECT(output);

    num_fields = flds->num_fields;
    if (expand) {
        for (j = 0; j < (Sint) num_fields; j++) {
            /* Note that in R-1.2.3 (at least) we need to protect SET_LENGTH */
            s_tmp = LST_EL(output, j);
            MEM_PROTECT(SET_LENGTH(s_tmp, num_rec));
            SET_ELEMENT(output, j, s_tmp);
            MEM_UNPROTECT(1);
        }
#ifndef USING_R
        output = AS_LIST(output);       /* this is only for S4's sake */
#endif
        MEM_UNPROTECT(1);
        return;
    }

    fld_Sclass = flds->Sclass;
    for (j = 0; j < (Sint) num_fields; j++) {
        switch ((int) fld_Sclass[j]) {
        case LOGICAL_TYPE:
            SET_ELEMENT(output, j, NEW_LOGICAL(num_rec));
            break;
        case CHARACTER_TYPE:
            SET_ELEMENT(output, j, NEW_CHARACTER(num_rec));
            break;
        case INTEGER_TYPE:
            SET_ELEMENT(output, j, NEW_INTEGER(num_rec));
            break;
        case NUMERIC_TYPE:
            SET_ELEMENT(output, j, NEW_NUMERIC(num_rec));
            break;
        case LIST_TYPE:
            SET_ELEMENT(output, j, NEW_LIST(num_rec));
            break;
#ifndef USING_R
        case RAW:              /* we use a list as a container for raw objects */
            SET_ELEMENT(output, j, NEW_LIST(num_rec));
            break;
#endif
        default:
            RS_DBI_errorMessage("unsupported data type in allocOutput", RS_DBI_ERROR);
        }
    }

    MEM_PROTECT(names = NEW_CHARACTER((Sint) num_fields));
    for (j = 0; j < (Sint) num_fields; j++) {
        SET_CHR_EL(names, j, C_S_CPY(flds->name[j]));
    }
    SET_NAMES(output, names);
#ifndef USING_R
    output = AS_LIST(output);   /* again this is required only for S4 */
#endif

    MEM_UNPROTECT(2);

    return;
}

s_object *                      /* boolean */
RS_DBI_validHandle(Db_Handle * handle)
{
    S_EVALUATOR s_object *valid;
    int handleType = 0;

    switch ((int) GET_LENGTH(handle)) {
    case MGR_HANDLE_TYPE:
        handleType = MGR_HANDLE_TYPE;
        break;
    case CON_HANDLE_TYPE:
        handleType = CON_HANDLE_TYPE;
        break;
    case RES_HANDLE_TYPE:
        handleType = RES_HANDLE_TYPE;
        break;
    }
    MEM_PROTECT(valid = NEW_LOGICAL((Sint) 1));
    LGL_EL(valid, 0) = (Sint) is_validHandle(handle, handleType);
    MEM_UNPROTECT(1);
    return valid;
}

void
RS_DBI_setException(Db_Handle * handle, DBI_EXCEPTION exceptionType, int errorNum, const char *errorMsg)
{
    HANDLE_TYPE handleType;

    handleType = (int) GET_LENGTH(handle);
    if (handleType == MGR_HANDLE_TYPE) {
        RS_DBI_manager *obj;
        obj = RS_DBI_getManager(handle);
        obj->exception->exceptionType = exceptionType;
        obj->exception->errorNum = errorNum;
        obj->exception->errorMsg = RS_DBI_copyString(errorMsg);
    }
    else if (handleType == CON_HANDLE_TYPE) {
        RS_DBI_connection *obj;
        obj = RS_DBI_getConnection(handle);
        obj->exception->exceptionType = exceptionType;
        obj->exception->errorNum = errorNum;
        obj->exception->errorMsg = RS_DBI_copyString(errorMsg);
    }
    else {
        RS_DBI_errorMessage("internal error in RS_DBI_setException: could not setException", RS_DBI_ERROR);
    }
    return;
}

void
RS_DBI_errorMessage(char *msg, DBI_EXCEPTION exception_type)
{
    char *driver = "RS-DBI";    /* TODO: use the actual driver name */

    switch (exception_type) {
    case RS_DBI_MESSAGE:
        PROBLEM "%s driver message: (%s)", driver, msg WARN;    /* was PRINT_IT */
        break;
    case RS_DBI_WARNING:
        PROBLEM "%s driver warning: (%s)", driver, msg WARN;
        break;
    case RS_DBI_ERROR:
        PROBLEM "%s driver: (%s)", driver, msg ERROR;
        break;
    case RS_DBI_TERMINATE:
        PROBLEM "%s driver fatal: (%s)", driver, msg ERROR;     /* was TERMINATE */
        break;
    }
    return;
}

/* wrapper to strcpy */
char *
RS_DBI_copyString(const char *str)
{
    char *buffer;

    buffer = (char *) malloc((size_t) strlen(str) + 1);
    if (!buffer) {
        RS_DBI_errorMessage("internal error in RS_DBI_copyString: could not alloc string space", RS_DBI_ERROR);
    }
    return strcpy(buffer, str);
}

/* wrapper to strncpy, plus (optionally) deleting trailing spaces */
char *
RS_DBI_nCopyString(const char *str, size_t len, int del_blanks)
{
    char *str_buffer, *end;

    str_buffer = (char *) malloc(len + 1);
    if (!str_buffer) {
        char errMsg[128];
        (void) sprintf(errMsg, "could not malloc %ld bytes in RS_DBI_nCopyString", (long) len + 1);
        RS_DBI_errorMessage(errMsg, RS_DBI_ERROR);
    }
    if (len == 0) {
        *str_buffer = '\0';
        return str_buffer;
    }

    (void) strncpy(str_buffer, str, len);

    /* null terminate string whether we delete trailing blanks or not */
    if (del_blanks) {
        for (end = str_buffer + len - 1; end >= str_buffer; end--) {
            if (*end != ' ') {
                end++;
                break;
            }
        }
        *end = '\0';
    }
    else {
        end = str_buffer + len;
        *end = '\0';
    }
    return str_buffer;
}

s_object *
RS_DBI_copyfields(RS_DBI_fields * flds)
{
    S_EVALUATOR s_object *S_fields;
    Sint n = (Sint) 8;
    char *desc[] = { "name", "Sclass", "type", "len", "precision",
        "scale", "isVarLength", "nullOK"
    };
    Stype types[] = { CHARACTER_TYPE, INTEGER_TYPE, INTEGER_TYPE,
        INTEGER_TYPE, INTEGER_TYPE, INTEGER_TYPE,
        LOGICAL_TYPE, LOGICAL_TYPE
    };
    Sint lengths[8];
    int i, j, num_fields;

    num_fields = flds->num_fields;
    for (j = 0; j < n; j++) {
        lengths[j] = (Sint) num_fields;
    }
    PROTECT(S_fields = RS_DBI_createNamedList(desc, types, lengths, n));
    /* copy contentes from flds into an R/S list */
    for (i = 0; i < num_fields; i++) {
        SET_LST_CHR_EL(S_fields, 0, i, C_S_CPY(flds->name[i]));
        LST_INT_EL(S_fields, 1, i) = (Sint) flds->Sclass[i];
        LST_INT_EL(S_fields, 2, i) = (Sint) flds->type[i];
        LST_INT_EL(S_fields, 3, i) = (Sint) flds->length[i];
        LST_INT_EL(S_fields, 4, i) = (Sint) flds->precision[i];
        LST_INT_EL(S_fields, 5, i) = (Sint) flds->scale[i];
        LST_INT_EL(S_fields, 6, i) = (Sint) flds->isVarLength[i];
        LST_INT_EL(S_fields, 7, i) = (Sint) flds->nullOk[i];
    }
    UNPROTECT(1);
    return S_fields;
}

s_object *
RS_DBI_createNamedList(char **names, Stype * types, Sint * lengths, Sint n)
{
    S_EVALUATOR s_object *output, *output_names, *obj = S_NULL_ENTRY;
    Sint num_elem;
    int j;

    MEM_PROTECT(output = NEW_LIST(n));
    MEM_PROTECT(output_names = NEW_CHARACTER(n));
    for (j = 0; j < n; j++) {
        num_elem = lengths[j];
        switch ((int) types[j]) {
        case LOGICAL_TYPE:
            MEM_PROTECT(obj = NEW_LOGICAL(num_elem));
            break;
        case INTEGER_TYPE:
            MEM_PROTECT(obj = NEW_INTEGER(num_elem));
            break;
        case NUMERIC_TYPE:
            MEM_PROTECT(obj = NEW_NUMERIC(num_elem));
            break;
        case CHARACTER_TYPE:
            MEM_PROTECT(obj = NEW_CHARACTER(num_elem));
            break;
        case LIST_TYPE:
            MEM_PROTECT(obj = NEW_LIST(num_elem));
            break;
#ifndef USING_R
        case RAW_TYPE:
            MEM_PROTECT(obj = NEW_RAW(num_elem));
            break;
#endif
        default:
            {
                char msg[256];
                sprintf(msg,"unsupported data type in createNamedList: %i in list %i (%s)", types[j], j, names[j]);
                RS_DBI_errorMessage(msg, RS_DBI_ERROR);
            }
        }
        SET_ELEMENT(output, (Sint) j, obj);
        SET_CHR_EL(output_names, j, C_S_CPY(names[j]));
    }
    SET_NAMES(output, output_names);
    MEM_UNPROTECT(n + 2);
    return (output);
}

s_object *
RS_DBI_SclassNames(s_object * type)
{
    s_object *typeNames;
    Sint *typeCodes;
    Sint n;
    int i;
    const char *s;

    PROTECT(type = AS_INTEGER(type));
    n = LENGTH(type);
    typeCodes = INTEGER_DATA(type);
    PROTECT(typeNames = NEW_CHARACTER(n));
    for (i = 0; i < n; i++) {
        s = RS_DBI_getTypeName(typeCodes[i], RS_dataTypeTable);
        if (!s) {
            RS_DBI_errorMessage("internal error RS_DBI_SclassNames: unrecognized S type", RS_DBI_ERROR);
        }
        SET_CHR_EL(typeNames, i, C_S_CPY(s));
    }
    UNPROTECT(2);
    return typeNames;
}

/* The following functions roughly implement a simple object
 * database.
 */

Mgr_Handle *
RS_DBI_asMgrHandle(Sint mgrId)
{
    Mgr_Handle *mgrHandle;

    MEM_PROTECT(mgrHandle = NEW_INTEGER((Sint) 1));
    MGR_ID(mgrHandle) = mgrId;
    MEM_UNPROTECT(1);
    return mgrHandle;
}

Con_Handle *
RS_DBI_asConHandle(Sint mgrId, Sint conId)
{
    Con_Handle *conHandle;

    MEM_PROTECT(conHandle = NEW_INTEGER((Sint) 2));
    MGR_ID(conHandle) = mgrId;
    CON_ID(conHandle) = conId;
    MEM_UNPROTECT(1);
    return conHandle;
}

Res_Handle *
RS_DBI_asResHandle(Sint mgrId, Sint conId, Sint resId)
{
    Res_Handle *resHandle;

    MEM_PROTECT(resHandle = NEW_INTEGER((Sint) 3));
    MGR_ID(resHandle) = mgrId;
    CON_ID(resHandle) = conId;
    RES_ID(resHandle) = resId;
    MEM_UNPROTECT(1);
    return resHandle;
}

RS_DBI_manager *
RS_DBI_getManager(Mgr_Handle * handle)
{
    RS_DBI_manager *mgr;

    if (!is_validHandle(handle, MGR_HANDLE_TYPE)) {
        RS_DBI_errorMessage("invalid dbManager handle", RS_DBI_ERROR);
    }
    mgr = dbManager;
    if (!mgr) {
        RS_DBI_errorMessage("internal error in RS_DBI_getManager: corrupt dbManager handle", RS_DBI_ERROR);
    }
    return mgr;
}

RS_DBI_connection *
RS_DBI_getConnection(Con_Handle * conHandle)
{
    RS_DBI_manager *mgr;
    Sint indx;

    mgr = RS_DBI_getManager(conHandle);
    indx = RS_DBI_lookup(mgr->connectionIds, mgr->length, CON_ID(conHandle));
    if (indx < 0) {
        RS_DBI_errorMessage("internal error in RS_DBI_getConnection: corrupt connection handle", RS_DBI_ERROR);
    }
    if (!mgr->connections[indx]) {
        RS_DBI_errorMessage("internal error in RS_DBI_getConnection: corrupt connection object", RS_DBI_ERROR);
    }
    return mgr->connections[indx];
}

RS_DBI_resultSet *
RS_DBI_getResultSet(Res_Handle * rsHandle)
{
    RS_DBI_connection *con;
    Sint indx;

    con = RS_DBI_getConnection(rsHandle);
    indx = RS_DBI_lookup(con->resultSetIds, con->length, RES_ID(rsHandle));
    if (indx < 0) {
        RS_DBI_errorMessage
            ("internal error in RS_DBI_getResultSet: could not find resultSet in connection", RS_DBI_ERROR);
    }
    if (!con->resultSets[indx]) {
        RS_DBI_errorMessage("internal error in RS_DBI_getResultSet: missing resultSet", RS_DBI_ERROR);
    }
    return con->resultSets[indx];
}

/* Very simple objectId (mapping) table. newEntry() returns an index
 * to an empty cell in table, and lookup() returns the position in the
 * table of obj_id.  Notice that we decided not to touch the entries
 * themselves to give total control to the invoking functions (this
 * simplify error management in the invoking routines.)
 */
Sint
RS_DBI_newEntry(Sint * table, Sint length)
{
    Sint i, indx, empty_val;

    indx = empty_val = (Sint) - 1;
    for (i = 0; i < length; i++) {
        if (table[i] == empty_val) {
            indx = i;
            break;
        }
    }
    return indx;
}

Sint
RS_DBI_lookup(Sint * table, Sint length, Sint obj_id)
{
    Sint i, indx;

    indx = (Sint) - 1;
    for (i = 0; i < length; ++i) {
        if (table[i] == obj_id) {
            indx = i;
            break;
        }
    }
    return indx;
}

/* return a list of entries pointed by *entries (we allocate the space,
 * but the caller should free() it).  The function returns the number
 * of entries.
 */
Sint
RS_DBI_listEntries(Sint * table, Sint length, Sint * entries)
{
    int i, n;

    for (i = n = 0; i < length; i++) {
        if (table[i] < 0) {
            continue;
        }
        entries[n++] = table[i];
    }
    return n;
}

void
RS_DBI_freeEntry(Sint * table, Sint indx)
{                               /* no error checking!!! */
    Sint empty_val = (Sint) - 1;
    table[indx] = empty_val;
    return;
}

int
is_validHandle(Db_Handle * handle, HANDLE_TYPE handleType)
{
    Sint mgr_id, len, indx;
    RS_DBI_manager *mgr;
    RS_DBI_connection *con;

    if (IS_INTEGER(handle)) {
        handle = AS_INTEGER(handle);
    }
    else {
        return 0;               /* non handle object */
    }

    len = (int) GET_LENGTH(handle);
    if (len < handleType || handleType < 1 || handleType > 3) {
        return 0;
    }
    mgr_id = MGR_ID(handle);
    if (((Sint) getpid()) != mgr_id) {
        return 0;
    }

    /* at least we have a potential valid dbManager */
    mgr = dbManager;
    if (!mgr || !mgr->connections) {
        return 0;               /* expired manager */
    }
    if (handleType == MGR_HANDLE_TYPE) {
        return 1;               /* valid manager id */
    }

    /* ... on to connections */
    indx = RS_DBI_lookup(mgr->connectionIds, mgr->length, CON_ID(handle));
    if (indx < 0) {
        return 0;
    }
    con = mgr->connections[indx];
    if (!con) {
        return 0;
    }
    if (!con->resultSets) {
        return 0;               /* un-initialized (invalid) */
    }
    if (handleType == CON_HANDLE_TYPE) {
        return 1;               /* valid connection id */
    }

    /* .. on to resultSets */
    indx = RS_DBI_lookup(con->resultSetIds, con->length, RES_ID(handle));
    if (indx < 0) {
        return 0;
    }
    if (!con->resultSets[indx]) {
        return 0;
    }

    return 1;
}

/* The following 3 routines provide metadata for the 3 main objects
 * dbManager, dbConnection and dbResultSet.  These functions
 * an object Id and return a list with all the meta-data. In R/S we
 * simply invoke one of these and extract the metadata piece we need,
 * which can be NULL if non-existent or un-implemented.
 *
 * Actually, each driver should modify these functions to add the
 * driver-specific info, such as server version, client version, etc.
 * That's how the various RS_MySQL_managerInfo, etc., were implemented.
 */

s_object *                      /* named list */
RS_DBI_managerInfo(Mgr_Handle * mgrHandle)
{
    S_EVALUATOR RS_DBI_manager *mgr;
    s_object *output;
    Sint i, num_con;
    Sint n = (Sint) 7;
    char *mgrDesc[] = { "connectionIds", "fetch_default_rec", "managerId",
        "length", "num_con", "counter", "clientVersion"
    };
    Stype mgrType[] = { INTEGER_TYPE, INTEGER_TYPE, INTEGER_TYPE,
        INTEGER_TYPE, INTEGER_TYPE, INTEGER_TYPE,
        CHARACTER_TYPE
    };
    Sint mgrLen[] = { 1, 1, 1, 1, 1, 1, 1 };

    mgr = RS_DBI_getManager(mgrHandle);
    num_con = (Sint) mgr->num_con;
    mgrLen[0] = num_con;

    PROTECT(output = RS_DBI_createNamedList(mgrDesc, mgrType, mgrLen, n));
    for (i = 0; i < num_con; i++) {
        LST_INT_EL(output, 0, i) = (Sint) mgr->connectionIds[i];
    }
    LST_INT_EL(output, 1, 0) = (Sint) mgr->fetch_default_rec;
    LST_INT_EL(output, 2, 0) = (Sint) mgr->managerId;
    LST_INT_EL(output, 3, 0) = (Sint) mgr->length;
    LST_INT_EL(output, 4, 0) = (Sint) mgr->num_con;
    LST_INT_EL(output, 5, 0) = (Sint) mgr->counter;
    SET_LST_CHR_EL(output, 6, 0, C_S_CPY("NA"));        /* client versions? */
    UNPROTECT(1);
    return output;
}

/* The following should be considered templetes to be
 * implemented by individual drivers.
 */

s_object *                      /* return a named list */
RS_DBI_connectionInfo(Con_Handle * conHandle)
{
    S_EVALUATOR RS_DBI_connection *con;
    s_object *output;
    Sint i;
    Sint n = (Sint) 8;
    char *conDesc[] = { "host", "port", "user", "dbname" 
        "serverVersion", "protocolVersion",
        "threadId", "rsHandle"
    };
    Stype conType[] = { CHARACTER_TYPE, CHARACTER_TYPE, CHARACTER_TYPE,
        CHARACTER_TYPE,  CHARACTER_TYPE, INTEGER_TYPE,
        INTEGER_TYPE, INTEGER_TYPE
    };
    Sint conLen[] = { 1, 1, 1, 1, 1, 1, 1, -1 };

    con = RS_DBI_getConnection(conHandle);
    conLen[7] = con->num_res; 

    PROTECT(output = RS_DBI_createNamedList(conDesc, conType, conLen, n));
    /* dummy */
    SET_LST_CHR_EL(output, 0, 0, C_S_CPY("NA"));        /* host */
    SET_LST_CHR_EL(output, 1, 0, C_S_CPY("NA"));        /* port */
    SET_LST_CHR_EL(output, 2, 0, C_S_CPY("NA"));        /* dbname */
    SET_LST_CHR_EL(output, 3, 0, C_S_CPY("NA"));        /* user */
    SET_LST_CHR_EL(output, 4, 0, C_S_CPY("NA"));        /* serverVersion */
    LST_INT_EL(output, 5, 0) = (Sint) - 1;      /* protocolVersion */
    LST_INT_EL(output, 6, 0) = (Sint) - 1;      /* threadId */

    for (i = 0; i < con->num_res; i++) {
        LST_INT_EL(output, 7, (Sint) i) = con->resultSetIds[i];
    }
    UNPROTECT(1);
    return output;
}

s_object *                      /* return a named list */
RS_DBI_resultSetInfo(Res_Handle * rsHandle)
{
    S_EVALUATOR RS_DBI_resultSet *result;
    s_object *output, *flds;
    Sint n = (Sint) 6;
    char *rsDesc[] = { "statement", "isSelect", "rowsAffected",
        "rowCount", "completed", "fields"
    };
    Stype rsType[] = { CHARACTER_TYPE, INTEGER_TYPE, INTEGER_TYPE,
        INTEGER_TYPE, INTEGER_TYPE, LIST_TYPE
    };
    Sint rsLen[] = { 1, 1, 1, 1, 1, 1 };

    result = RS_DBI_getResultSet(rsHandle);
    if (result->fields) {
        PROTECT(flds = RS_DBI_copyfields(result->fields));
    }
    else {
        PROTECT(flds = S_NULL_ENTRY);
    }

    PROTECT(output = RS_DBI_createNamedList(rsDesc, rsType, rsLen, n));
    SET_LST_CHR_EL(output, 0, 0, C_S_CPY(result->statement));
    LST_INT_EL(output, 1, 0) = result->isSelect;
    LST_INT_EL(output, 2, 0) = result->rowsAffected;
    LST_INT_EL(output, 3, 0) = result->rowCount;
    LST_INT_EL(output, 4, 0) = result->completed;
    SET_ELEMENT(LST_EL(output, 5), (Sint) 0, flds);
    UNPROTECT(2);
    return output;
}

s_object *                      /* named list */
RS_DBI_getFieldDescriptions(RS_DBI_fields * flds)
{
    S_EVALUATOR s_object *S_fields;
    Sint n = (Sint) 7;
    Sint lengths[7];
    char *desc[] = { "name", "Sclass", "type", "len", "precision",
        "scale", "nullOK"
    };
    Stype types[] = { CHARACTER_TYPE, INTEGER_TYPE, INTEGER_TYPE,
        INTEGER_TYPE, INTEGER_TYPE, INTEGER_TYPE,
        LOGICAL_TYPE
    };
    Sint i, j;
    int num_fields;

    num_fields = flds->num_fields;
    for (j = 0; j < n; j++) {
        lengths[j] = (Sint) num_fields;
    }
    PROTECT(S_fields = RS_DBI_createNamedList(desc, types, lengths, n));
    /* copy contentes from flds into an R/S list */
    for (i = 0; i < (Sint) num_fields; i++) {
        SET_LST_CHR_EL(S_fields, 0, i, C_S_CPY(flds->name[i]));
        LST_INT_EL(S_fields, 1, i) = (Sint) flds->Sclass[i];
        LST_INT_EL(S_fields, 2, i) = (Sint) flds->type[i];
        LST_INT_EL(S_fields, 3, i) = (Sint) flds->length[i];
        LST_INT_EL(S_fields, 4, i) = (Sint) flds->precision[i];
        LST_INT_EL(S_fields, 5, i) = (Sint) flds->scale[i];
        LST_INT_EL(S_fields, 6, i) = (Sint) flds->nullOk[i];
    }
    UNPROTECT(1);
    return (S_fields);
}

/* given a type id return its human-readable name.
 * We define an RS_DBI_dataTypeTable */
const char *
RS_DBI_getTypeName(Sint t, const struct data_types table[])
{
    int i;
    char buf[128];

    for (i = 0; table[i].typeName != (char *) 0; i++) {
        if (table[i].typeId == t) {
            return table[i].typeName;
        }
    }
    sprintf(buf, "unknown (%ld)", (long) t);
    RS_DBI_errorMessage(buf, RS_DBI_WARNING);
    return "UNKNOWN";
}

/* Translate R/S identifiers (and only R/S names!!!) into
 * valid SQL identifiers;  overwrite input vector. Currently,
 *   (1) translate "." into "_".
 *   (2) first character should be a letter (traslate to "X" if not),
 *       but a double quote signals a "delimited identifier"
 *   (3) check that length <= 18, but only warn, since most (all?)
 *       dbms allow much longer identifiers.
 *   (4) SQL reserved keywords are handled in the R/S calling
 *       function make.SQL.names(), not here.
 * BUG: Compound SQL identifiers are not handled properly.
 *      Note the the dot "." is a valid SQL delimiter, used for specifying
 *      user/table in a compound identifier.  Thus, it's possible that
 *      such compound name is mapped into a legal R/S identifier (preserving
 *      the "."), and then we incorrectly map such delimiting "dot" into "_"
 *      thus loosing the original SQL compound identifier.
 */
#define RS_DBI_MAX_IDENTIFIER_LENGTH 18 /* as per SQL92 */
s_object *
RS_DBI_makeSQLNames(s_object * snames)
{
    S_EVALUATOR long nstrings;
    char *name, c;
    char errMsg[128];
    size_t len;
    Sint i;

    nstrings = (Sint) GET_LENGTH(snames);
    for (i = 0; i < nstrings; i++) {
        name = (char *) CHR_EL(snames, i);      /* NOTE: Sameer.... casted RHS using (char*) */
        if (strlen(name) > RS_DBI_MAX_IDENTIFIER_LENGTH) {
            (void) sprintf(errMsg, "SQL identifier %s longer than %d chars", name, RS_DBI_MAX_IDENTIFIER_LENGTH);
            RS_DBI_errorMessage(errMsg, RS_DBI_WARNING);
        }
        /* check for delimited-identifiers (those in double-quotes);
         * if missing closing double-quote, warn and treat as non-delim
         */
        c = *name;
        len = strlen(name);
        if (c == '"' && name[len - 1] == '"') {
            continue;
        }
        if (!isalpha(c) && c != '"') {
            *name = 'X';
        }
        name++;
        while ((c = *name)) {
            /* TODO: recognize SQL delim "." instances that may have
             * originated in SQL and R/S make.names() left alone */
            if (c == '.') {
                *name = '_';
            }
            name++;
        }
    }

    return snames;
}

#ifdef USING_R
/*  These 2 R-specific functions are used by the C macros IS_NA(p,t)
 *  and NA_SET(p,t) (in this way one simply use macros to test and set
 *  NA's regardless whether we're using R or S.
 */
void
RS_na_set(void *ptr, Stype type)
{
    double *d;
    Sint *i;
    switch (type) {
    case INTEGER_TYPE:
        i = (Sint *) ptr;
        *i = NA_INTEGER;
        break;
    case LOGICAL_TYPE:
        i = (Sint *) ptr;
        *i = NA_LOGICAL;
        break;
    case NUMERIC_TYPE:
        d = (double *) ptr;
        *d = NA_REAL;
        break;
    }
}
int
RS_is_na(void *ptr, Stype type)
{
    int *i, out = -2;
    char *c;
    double *d;

    switch (type) {
    case INTEGER_TYPE:
    case LOGICAL_TYPE:
        i = (int *) ptr;
        out = (int) ((*i) == NA_INTEGER);
        break;
    case NUMERIC_TYPE:
        d = (double *) ptr;
        out = ISNA(*d);
        break;
    case STRING_TYPE:
        c = (char *) ptr;
        out = (int) (strcmp(c, CHR_EL(NA_STRING, 0)) == 0);
        break;
    }
    return out;
}
#endif

/* the codes come from from R/src/main/util.c */
const struct data_types RS_dataTypeTable[] = {
#ifdef USING_R
    {"NULL", NILSXP},           /* real types */
    {"symbol", SYMSXP},
    {"pairlist", LISTSXP},
    {"closure", CLOSXP},
    {"environment", ENVSXP},
    {"promise", PROMSXP},
    {"language", LANGSXP},
    {"special", SPECIALSXP},
    {"builtin", BUILTINSXP},
    {"char", CHARSXP},
    {"logical", LGLSXP},
    {"integer", INTSXP},
    {"double", REALSXP},              /*-  "real", for R <= 0.61.x */
    {"complex", CPLXSXP},
    {"character", STRSXP},
    {"...", DOTSXP},
    {"any", ANYSXP},
    {"expression", EXPRSXP},
    {"list", VECSXP},
    /* aliases : */
    {"numeric", REALSXP},
    {"name", SYMSXP},
    {(char *) 0, -1}
#else
    {"logical", LGL},
    {"integer", INT},
    {"single", REAL},
    {"numeric", DOUBLE},
    {"character", CHAR},
    {"list", LIST},
    {"complex", COMPLEX},
    {"raw", RAW},
    {"any", ANY},
    {"structure", STRUCTURE},
    {(char *) 0, -1}
#endif
};
