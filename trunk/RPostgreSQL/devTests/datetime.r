#!/usr/bin/env r

## assign a basic time type
now <- Sys.time()

## print invokes a conversion to char for 'display' even though the type is really POSIXct
print(now)
print(class(now))

## we can also convert to char() implicitly
print(format(now))
print(class(format(now)))

## but what is important is that 'now' is still a time type
## that we can 'compute'
print(now)
print(now + 60)  ## one minute later
twomin <- now + 120
print(as.numeric(now))

## and the time is even stored at millisecond granularity!!
options("digits.secs"=7)		## need to tell R we want sub-second display up to 7 digits
print(now)				## and now we do
print(as.numeric(now), digits=16)	## same milli/microseconds here


## you can also go the other way and parse a datetime object from a char vector
then <- strptime("2008-07-01 14:15:16", "%Y-%m-%d %H:%M:%S")
print(then)
print(class(then))
## and we can convert this from its default 'POSIXlt' ('long type) representation to 'POSIXct' ('compact type')
then <- as.POSIXct(then)
print(then)
print(class(then))


tempdb <- "pgdatetime"
system(paste("createdb", tempdb))   # create a temp database

stopifnot(require(RPostgreSQL))
drv <- dbDriver("PostgreSQL")
con <- dbConnect(drv, dbname=tempdb)

dbSendQuery(con, "create table foo (tt timestamp with time zone);")
dbSendQuery(con, "insert into foo values('2008-07-01 14:15:16.123');")

dbSendQuery(con, paste("insert into foo values('", format(now), "');", sep=""))

res <- dbReadTable(con, "foo")  ## fails with 'RS-DBI driver warning: (unrecognized PostgreSQL field type 1184 in column 0)'
print(res)

res <- dbSendQuery(con, "select to_char(tt, 'YYYY-MM-DD HH24:MI:SS.US TZ') as character from foo;")
data <- fetch(res, n=-1)

times <- strptime(data[,1], "%Y-%m-%d %H:%M:%OS")
print(times)
print(class(times))

print(diff(times))	## yes we can compute on date times

dbDisconnect(con)

system(paste("dropdb", tempdb))   # create a temp database
