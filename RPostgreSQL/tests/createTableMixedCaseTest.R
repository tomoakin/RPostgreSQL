
## createTableCase test
##
##
## Assumes that
##  a) PostgreSQL is running, and
##  b) the current user can connect
## both of which are not viable for release but suitable while we test
##
## Neil Tiffin, 30 Oct 2009

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
                     dbname=Sys.getenv("POSTGRES_DATABASE"))



    res <- dbSendQuery(con, "create table Foo1 (f1 int)")
    res <- dbSendQuery(con, "create table \"Foo2\" (f1 int)")

    print("Test should create foo1 and Foo2 tables\n")
    ## res <- dbSendQuery(con, "SELECT * FROM information_schema.tables WHERE table_schema = 'public'")
    ## print res

    if (dbExistsTable(con, "Foo1")) {
        print("OK - Foo1 Table exists.\n")
    }
    else {
        print("Wrong - Foo1 Table does not exist.\n")
    }

    if (dbExistsTable(con, "foo1")) {
        print("OK - foo1 Table exists.\n")
    }
    else {
        print("Wrong - foo1 Table does not exist.\n")
    }

    if (dbExistsTable(con, "Foo2")) {
        print("Wrong - Foo2 Table exists.\n")
    }
    else {
        print("OK - Foo2 Table does not exist.\n")
    }

    if (dbExistsTable(con, "foo2")) {
        print("Wrong - foo2 Table exists.\n")
    }
    else {
        print("OK foo2 - Table does not exist.\n")
    }

    if (dbExistsTable(con, "\"Foo2\"")) {
        print("OK Foo2 - Table exists.\n")
    }
    else {
        print("Wrong - Foo2 Table does not exist.\n")
    }

    if (dbExistsTable(con, "\"foo2\"")) {
        print("foo2 Wrong - Table exists.\n")
    }
    else {
        print("foo2 Ok -Table does not exist.\n")
    }

    ## and disconnect
    dbDisconnect(con)
}
