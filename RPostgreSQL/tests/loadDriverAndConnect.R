

## First rough version of a test script
##
## Assumes that
##  a) PostgreSQL is running, and
##  b) the current user can connect
## both of which are not viable for release bui suitable while we test
##
## Dirk Eddelbuettel, 02 July 2008


## try to load our module and abort if this fails
stopifnot(require(RPostgreSQL))

## load the PostgresSQL driver
drv <- dbDriver("PostgreSQL")
# can't print result as it contains process id which changes  print(summary(drv))

## connect to the default db
con <- dbConnect(drv, dbname="template1")

## run a simple query and show the query result
res <- dbGetQuery(con, "select datname,encoding,datallowconn from pg_database where datname like 'template%' order by datname")
print(res)

## and disconnect
dbDisconnect(con)

