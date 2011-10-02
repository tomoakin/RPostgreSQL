## dbWriteTable test
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

    difficultstrings <- c("normal", "t\tab", "v\vertical tab", "n\newline", "r carriage \retern", "back \\ slash", "f\form feed", "kanji\u6f22\u5b57", "m\u00fc\u00df")
    df <- data.frame(strings=difficultstrings)

    dbWriteTable(con, "rockdata", df)

    ## run a simple query and show the query result
    res <- dbGetQuery(con, "select * from rockdata")
    for(n in 1:9){
      cat(paste(as.character(n), "\t"))
      cat(res[n,2])
      cat("\n")
    }

    ## cleanup
    if (dbExistsTable(con, "rockdata")) {
        print("Removing rockdata\n")
        dbRemoveTable(con, "rockdata")
    }

    if (dbExistsTable(con, "tempostgrestable"))
        dbRemoveTable(con, "tempostgrestable")

    ## Test the numeric mapping
    dbGetQuery(con, "create table tempostgrestable (intcolumn integer, floatcolumn float);")

    i <- as.integer(10)
    j <- as.numeric(56.6)

    sql <- paste("insert into tempostgrestable ",
                 "values (",i, "," ,j ,") ", sep="")
    res <- dbGetQuery(con, sql)

    dat <- dbReadTable(con, "tempostgrestable")
    dbRemoveTable(con, "tempostgrestable")
    res <- dbWriteTable(con, "numerictable", dat)
    dat <- dbReadTable(con, "numerictable")
    dbRemoveTable(con, "numerictable")
    cat("Read Numeric values\n")

    ## now test the types of the colums we got
    if( class(dat[,1]) == "integer" ) {
       cat("PASS -- all integer is as expected\n")
    }else{
       cat(paste("FAIL -- an integer became ", class(dat[,1]), "\n"))
    }
    stopifnot( class(dat[,2]) == "numeric" )

    ## and test the values
    if( identical( dat[1,1], i)){
       cat("PASS integer value is preserved")
    }else{
       cat(paste("FAIL:", i, "changed to", dat[1,1], "\n")) 
    }
    stopifnot( identical( dat[1,2], j))
    cat("GOOD -- all numeric values are as expected\n")

    ## Test the logical mapping
    if (dbExistsTable(con, "testlogical"))
        dbRemoveTable(con, "testlogical")
    dbGetQuery(con,"create table testlogical (col1 boolean, col2 boolean)")

    i <- as.logical(TRUE)
    j <- as.logical(FALSE)

    sql <- paste("insert into testlogical ",
                 "values (",i, "," ,j ,") ", sep="")
    res <- dbGetQuery(con, sql);

    dat <- dbReadTable(con, "testlogical")
    res <- dbWriteTable(con, "logicaltable", dat)
    dbRemoveTable(con, "testlogical")
    dat2 <- dbReadTable(con,"logicaltable")
    dbRemoveTable(con, "logicaltable")
    cat("Read Logical values\n")

    ## now test the types of the colums we got
    stopifnot( class(dat2[,1]) == "logical" )
    stopifnot( class(dat2[,2]) == "logical" )
    cat("GOOD -- all logical types are as expected\n")

    ## and test the values
    stopifnot( identical( dat2[1,1], i))
    stopifnot( identical( dat2[1,2], j))
    cat("GOOD -- all logical values are as expected\n")

    ## Test the character mapping
    if (dbExistsTable(con, "testchar"))
        dbRemoveTable(con, "testchar")
    dbGetQuery(con,"create table testchar (code char(3),city varchar(20),country text);")

    i <- as.character("IN")
    j <- as.character("Hyderabad")
    k <- as.character("India")

    sql <- paste("insert into testchar ",
                 "values ('",i,"' , '",j ,"' , '",k,"') ", sep="")
    res <- dbGetQuery(con, sql);

    dat <- dbReadTable(con, "testchar")
    dbRemoveTable(con, "testchar")
    dbWriteTable(con, "testchar", dat)
    dat <- dbReadTable(con, "testchar")
    cat("Read Character values\n")

    ## now test the types of the colums we got
    stopifnot( class(dat[,1]) == "character" )
    stopifnot( class(dat[,2]) == "character" )
    stopifnot( class(dat[,3]) == "character" )
    cat("GOOD -- all character types are as expected\n")

    ## and test the values
    ##stopifnot( identical( dat[1,1], i))
    stopifnot( identical( dat[1,2], j))
    stopifnot( identical( dat[1,3], k))
    cat("GOOD -- all character values are as expected\n")

    dbRemoveTable(con, "testchar")
    dbRemoveTable(con, "tempostgrestable")

    ## Test the numeric mapping
    dbGetQuery(con, "create table tempostgrestable (intcolumn date, floatcolumn timestamp with time zone);")

    sql <- paste("insert into tempostgrestable ",
                 "values ('2011-03-07', '2011-03-07 16:30:39') ")
    res <- dbGetQuery(con, sql)

    dat <- dbReadTable(con, "tempostgrestable")
    dbRemoveTable(con, "tempostgrestable")
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

