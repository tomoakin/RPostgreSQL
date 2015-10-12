## dbWriteTable fail test
##
## Assumes that
##  a) PostgreSQL is running, and
##  b) the current user can connect
## both of which are not viable for release but suitable while we test
##
## Dirk Eddelbuettel, 10 Sep 2009

## only run this if this env.var is set correctly
if (Sys.getenv("POSTGRES_USER") != "" & Sys.getenv("POSTGRES_HOST") != "" & Sys.getenv("POSTGRES_DATABASE") != "") {

    ## try to load our module and abort if this fails
    stopifnot(require(RPostgreSQL))
    stopifnot(require(datasets))

    ## load the PostgresSQL driver
    drv <- dbDriver("PostgreSQL")

    ## connect to the default db
    con <- dbConnect(drv,
                     user=Sys.getenv("POSTGRES_USER"),
                     password=Sys.getenv("POSTGRES_PASSWD"),
                     host=Sys.getenv("POSTGRES_HOST"),
                     dbname=Sys.getenv("POSTGRES_DATABASE"),
                     port=ifelse((p<-Sys.getenv("POSTGRES_PORT"))!="", p, 5432))


    if (dbExistsTable(con, "rockdata")) {
        print("Removing rockdata\n")
        dbRemoveTable(con, "rockdata")
    }
    cat("create incompatible table rockdata\n")
    dbGetQuery(con, "CREATE TABLE rockdata (a int, b varchar(5))")

    cat("write table to rockdata with append=TRUE\n")
    try({res <-dbWriteTable(con, 'rockdata', rock, append=TRUE, overwrite=FALSE)
        print(res)
        if(res == FALSE){
           cat("PASS as the return value is false\n")
        }else{
           cat("FAIL as the return value is true\n")
        }
    })

    cat("write table to rockdata\n")
    try({res <- dbWriteTable(con, 'rockdata', rock)
        if(res == FALSE){
        print(res)
           cat("PASS as the return value is false\n")
        }else{
           cat("FAIL as the return value is true\n")
        }
    })
  
    ## run a simple query and show the query result
    res <- dbGetQuery(con, "select * from rockdata limit 10")
    print(res)


    ## cleanup
    if (dbExistsTable(con, "rockdata")) {
        print("Removing rockdata\n")
        dbRemoveTable(con, "rockdata")
    }

    ## and disconnect
    dbDisconnect(con)
}else{
    cat("Skip.\n")
}
