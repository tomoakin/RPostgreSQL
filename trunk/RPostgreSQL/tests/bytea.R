## Test of data types, based on earlier version in inst/devTests
##
## Dirk Eddelbuettel, 21 Oct 2008

if ((Sys.getenv("POSTGRES_USER") != "") &
    (Sys.getenv("POSTGRES_HOST") != "") &
    (Sys.getenv("POSTGRES_DATABASE") != "")) {

    ## try to load our module and abort if this fails
    stopifnot(require(RPostgreSQL))

    ## load the PostgresSQL driver
    drv <- dbDriver("PostgreSQL")
    ## can't print result as it contains process id which changes  print(summary(drv))

    ## connect to the default db
    con <- dbConnect(drv,
                     user=Sys.getenv("POSTGRES_USER"),
                     password=Sys.getenv("POSTGRES_PASSWD"),
                     host=Sys.getenv("POSTGRES_HOST"),
                     dbname=Sys.getenv("POSTGRES_DATABASE"),
                     port=ifelse((p<-Sys.getenv("POSTGRES_PORT"))!="", p, 5432))

    if (dbExistsTable(con, "byteatable"))
        dbRemoveTable(con, "byteatable")

    ## Test the numeric mapping
    dbGetQuery(con,"CREATE TABLE byteatable (name text NOT NULL, val bytea, PRIMARY KEY (name))")
    sample.object <- list("one","two");
    ser <- serialize(sample.object,NULL,ascii=F);
    postgresqlEscapeBytea(con, ser)
    iq <- sprintf("INSERT INTO byteatable values('%s',E'%s');","name1", postgresqlEscapeBytea(con, ser))
    dbGetQuery(con, iq)
    rows<-dbGetQuery(con, "SELECT * from byteatable")
    ser2<-postgresqlUnescapeBytea(rows[[2]])
    dbRemoveTable(con, "byteatable")
    dbDisconnect(con)
    dbUnloadDriver(drv)
    cat("DONE\n")
}
