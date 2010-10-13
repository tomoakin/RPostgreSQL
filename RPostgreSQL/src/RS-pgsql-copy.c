/*
 *    RS-pgsql-copy.c
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

/*    fprintf(stderr, "about to open: %s\n", dyn_filename);*/
    filehandle=fopen(dyn_filename, "r");
    if(filehandle == NULL){
        char errmsg[1024];
        snprintf(errmsg, 1024, "could not open file: %s", dyn_filename);
        RS_DBI_errorMessage(dyn_filename, RS_DBI_ERROR);
        return S_NULL_ENTRY;
    }

    while((len = fread(copybuf,1,COPY_IN_BUFSIZE, filehandle))){
        pqretcode = PQputCopyData(my_connection, copybuf, len);
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
        /* some error handling should be added */
/*        fprintf(stderr, "PQ putCopydata retval: %i\n", pqretcode);*/
        
    }
    PQputCopyEnd(my_connection, NULL);
    fclose(filehandle);

    free(dyn_filename);
    return S_NULL_ENTRY;
}

