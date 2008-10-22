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
                     dbname=Sys.getenv("POSTGRES_DATABSE"))

    if (dbExistsTable(con, "tempostgrestable"))
        dbRemoveTable(con, "tempostgrestable")

    ## Test the numeric mapping
    dbSendQuery(con, "create table tempostgrestable (intcolumn integer, floatcolumn float);")

    i <- as.integer(10)
    j <- as.numeric(56.6)

    sql <- paste("insert into tempostgrestable ",
                 "values (",i, "," ,j ,") ", sep="")
    res <- dbSendQuery(con, sql)


    dat <- dbReadTable(con, "tempostgrestable")
    cat("Read Numeric values\n")

    ## now test the types of the colums we got
    stopifnot( class(dat[,1]) == "integer" )
    stopifnot( class(dat[,2]) == "numeric" )
    cat("GOOD -- all numeric types are as expected\n")

    ## and test the values
    stopifnot( identical( dat[1,1], i))
    stopifnot( identical( dat[1,2], j))
    cat("GOOD -- all numeric values are as expected\n")

    ## Test the logical mapping
    dbSendQuery(con,"create table testlogical (col1 boolean, col2 boolean)")

    i <- as.logical(TRUE)
    j <- as.logical(FALSE)

    sql <- paste("insert into testlogical ",
                 "values (",i, "," ,j ,") ", sep="")
    res <- dbSendQuery(con, sql);

    dat <- dbReadTable(con, "testlogical")
    cat("Read Logical values\n")

    ## now test the types of the colums we got
    stopifnot( class(dat[,1]) == "logical" )
    stopifnot( class(dat[,2]) == "logical" )
    cat("GOOD -- all logical types are as expected\n")

    ## and test the values
    stopifnot( identical( dat[1,1], i))
    stopifnot( identical( dat[1,2], j))
    cat("GOOD -- all logical values are as expected\n")

    ## Test the character mapping
    dbSendQuery(con,"create table testchar (code char(3),city varchar(20),country text);")

    i <- as.character("IN")
    j <- as.character("Hyderabad")
    k <- as.character("India")

    sql <- paste("insert into testchar ",
                 "values ('",i,"' , '",j ,"' , '",k,"') ", sep="")
    res <- dbSendQuery(con, sql);

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

    dbRemoveTable(con, "tempostgrestable")

    dbDisconnect(con)
    dbUnloadDriver(drv)

    cat("DONE\n")
}
