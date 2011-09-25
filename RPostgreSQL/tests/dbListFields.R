## dbListFields test
##
## Assumes that
##  a) PostgreSQL is running, and
##  b) the current user can connect
## both of which are not viable for release but suitable while we test
##

## only run this if this env.var is set correctly
if (Sys.getenv("POSTGRES_USER") != "" & Sys.getenv("POSTGRES_HOST") != "" & Sys.getenv("POSTGRES_DATABASE") != "") {

    ## try to load our module and abort if this fails
    stopifnot(require(RPostgreSQL))

    ## load the PostgresSQL driver
    drv <- dbDriver("PostgreSQL")

    ## connect to the default db
    con <- dbConnect(drv,
                     user=Sys.getenv("POSTGRES_USER"),
                     password=Sys.getenv("POSTGRES_PASSWD"),
                     host=Sys.getenv("POSTGRES_HOST"),
                     dbname=Sys.getenv("POSTGRES_DATABASE"),
                     port=ifelse((p<-Sys.getenv("POSTGRES_PORT"))!="", p, 5432))


    #  create a table
    res <- dbGetQuery(con, "CREATE SCHEMA testschema")
    res <- dbGetQuery(con, "CREATE TABLE testschema.aa (pid integer, name text)")
    res <- dbGetQuery(con, "CREATE TABLE aa (pk integer, v1 float not null, v2 float)" )

    ## run a simple query and show the query result
    df <- dbListFields(con, "aa")
    print(df)
    if (length(df) == 3){
      cat("PASS: 3 fields returned\n")
    }else{
      cat(paste("FAIL:", length(df), "fields returned\n"))
    } 

    df <- dbListFields(con, c("testschema", "aa"))
    print(df)
    if (length(df) == 2){
      cat("PASS: 2 fields returned\n")
    }else{
      cat(paste("FAIL:", length(df), "fields returned\n"))
    } 


    ## cleanup
    cat("Removing \"AA\"\n")
    dbRemoveTable(con, "aa")
    dbGetQuery(con, "DROP TABLE testschema.aa")
    dbGetQuery(con, "DROP SCHEMA testschema")
    ## and disconnect
    dbDisconnect(con)
}
