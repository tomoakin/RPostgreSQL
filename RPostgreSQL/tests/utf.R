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

    if (dbExistsTable(con, "texttable"))
        dbRemoveTable(con, "texttable")

        x_df <- data.frame(x = c("a", "ö", "漢字"), stringsAsFactors = FALSE)

        dbGetQuery(con, "SHOW server_encoding")
        dbGetQuery(con, "SET client_encoding = 'UTF8'")
        dbGetQuery(con, "SHOW client_encoding")
# write with client_encoding = 'UTF8' and read back what was wrote
        dbWriteTable(con, "texttable", x_df, overwrite = TRUE)
        (x_db <- dbReadTable(con, "texttable"))
        (e <- Encoding(x_db$x))
        stopifnot( e[1] == "unknown" )
        stopifnot( e[2] == "UTF-8" )
        stopifnot( e[3] == "UTF-8" )
        cat("GOOD -- all encoding are as expected\n")

        x_df <- data.frame(x = c("a", "ö"), stringsAsFactors = FALSE)
        dbWriteTable(con, "texttable", x_df, overwrite = TRUE)
        dbGetQuery(con, "SET client_encoding = 'LATIN1'")
        dbGetQuery(con, "SHOW client_encoding")
# read with client_encoding = 'LATIN1' what was already in the database
        (x_db <- dbReadTable(con, "texttable"))
        (e <- Encoding(x_db$x))
        stopifnot( e[1] == "unknown" )
        stopifnot( e[2] == "latin1" )
        cat("GOOD -- all encoding are as expected\n")

# write with client_encoding = 'LATIN1' and read back what was wrote
        dbWriteTable(con, "texttable", x_df, overwrite = TRUE)
        (x_db <- dbReadTable(con, "texttable"))
        (e <- Encoding(x_db$x))
        stopifnot( e[1] == "unknown" )
        stopifnot( e[2] == "latin1" )
        cat("GOOD -- all encoding are as expected\n")

    dbRemoveTable(con, "texttable")
    dbDisconnect(con)
    dbUnloadDriver(drv)
    cat("DONE\n")
}else{
    cat("Skip because envirinmental variables are not set to tell the connection params.\n")
}
