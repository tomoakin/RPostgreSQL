## Test of bytea conversion with insert and retrieve to the db.
##

if ((Sys.getenv("POSTGRES_USER") != "") &
    (Sys.getenv("POSTGRES_HOST") != "") &
    (Sys.getenv("POSTGRES_DATABASE") != "")) {

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

    if (dbExistsTable(con, "byteatable"))
        dbRemoveTable(con, "byteatable")

    sample.object <- list("one", "two");
    ser <- serialize(sample.object, NULL, ascii=F);

    ## Test the store/recovery of binary data
    dbGetQuery(con, "CREATE TABLE byteatable (name text, val bytea)")
    dbGetQuery(con, "set standard_conforming_strings to 'on'")
    cat("Note the encoded string could differ depending on the server.\n")
    cat("Show encoded string when standard_conforming_strings is on.\n")
    print(postgresqlEscapeBytea(con, ser))
    iq <- sprintf("INSERT INTO byteatable values('%s', '%s');", "name1", postgresqlEscapeBytea(con, ser))
    dbGetQuery(con, iq)
    rows<-dbGetQuery(con, "SELECT * from byteatable")
    ser2<-postgresqlUnescapeBytea(rows[[2]])
    if (identical(ser, ser2)) {
        cat("PASS: Identical data was recovered\n")
    }else{
        cat("FAIL: The recovered data is not identical\n")
        ser
        ser2
    }
    dbGetQuery(con, "set standard_conforming_strings to 'off'")
    dbGetQuery(con, "set escape_string_warning to 'off'")
    cat("Show encoded string when standard_conforming_strings is off.\n")
    print(postgresqlEscapeBytea(con, ser))
    iq <- sprintf("INSERT INTO byteatable values('%s', '%s');", "name2", postgresqlEscapeBytea(con, ser))
    dbGetQuery(con, iq)
    rows<-dbGetQuery(con, "SELECT * from byteatable where name = 'name2'")
    ser3<-postgresqlUnescapeBytea(rows[[2]])
    if (identical(ser, ser3)) {
        cat("PASS: Identical data was recovered\n")
    }else{
        cat("FAIL: The recovered data is not identical\n")
        ser
        ser2
    }
    dbRemoveTable(con, "byteatable")
    dbDisconnect(con)
    dbUnloadDriver(drv)
    cat("DONE\n")
}else{
    cat("Skip because envirinmental variables are not set to tell the connection params.\n")
}
