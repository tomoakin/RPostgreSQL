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

    if (dbExistsTable(con, "tempostgrestable"))
        dbRemoveTable(con, "tempostgrestable")

    ## Test the numeric mapping
    dbGetQuery(con, "create table tempostgrestable (intcolumn date, floatcolumn timestamp with time zone);")

    sql <- paste("insert into tempostgrestable ",
                 "values ('2011-03-07', '2011-03-07 16:30:39') ")
    res <- dbGetQuery(con, sql)

    dat <- dbReadTable(con, "tempostgrestable")
    dbRemoveTable(con, "tempostgrestable")
    cat("Read Date and TIMESTAMP values\n")

    ## now test the types of the colums we got
    if( class(dat[1,1]) == "Date" ){
        cat("PASS -- Date type is as expected\n")
    }else{
        cat("FAIL -- Date type is other than Date: ")
        cat(class(dat[1,1]))
        cat("\n")
    }
    if( class(dat[1,2])[1] == "POSIXct" ){
        cat("PASS -- TIMESTAMP is received as POSIXct\n")
    }else{
        cat("FAIL -- TIMESTAMP is other than POSIXct: ")
        cat(class(dat[1,2]))
        cat("\n")
    }

    dbWriteTable(con, "tempostgrestable2", dat)
    dat2 <- dbReadTable(con, "tempostgrestable2")
    dbRemoveTable(con, "tempostgrestable2")
    cat("Check that read after write gets the same data types\n")

    ## now test the types of the colums we got
    if( class(dat2[1,1]) == "Date" ){
        cat("PASS -- Date type is as expected\n")
    }else{
        cat("FAIL -- Date type is other than Date: ")
        cat(class(dat2[1,1]))
        cat("\n")
    }
    if( class(dat2[1,2])[1] == "POSIXct" ){
        cat("PASS -- TIMESTAMP is received as POSIXct\n")
    }else{
        cat("FAIL -- TIMESTAMP is other than POSIXct: ")
        cat(class(dat2[1,2]))
        cat("\n")
    }


    dbDisconnect(con)
    dbUnloadDriver(drv)

    cat("DONE\n")
}
