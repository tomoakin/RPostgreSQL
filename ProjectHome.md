## Synopis ##

RPostgreSQL provides a [DBI](http://stat.bell-labs.com/RS-DBI/index.html)-compliant database connection from [GNU R](http://www.r-project.org) to [PostgreSQL](http://www.postgresql.org).

Development of RPostgreSQL was supported via the Google Summer of Code 2008 program.

The package is now available on the [CRAN](http://cran.r-project.org) mirror network and
can be installed via `install.packages()` from within [R](http://www.r-project.org).

A mailing list `rpostgresql-dev@googlegroups.com` is available at [Google Groups as rpostgresql-dev](https://groups.google.com/forum/?pli=1#!forum/rpostgresql-dev); it is also mirrored at [Gmane.org](http://news.gmane.org/gmane.comp.lang.r.rpostgresql).

## Summary of basic usage ##

1. dbDriver(drv, ...) instantiates the driver object. Eg.
```
drv <- dbDriver("PostgreSQL")
```

2.dbConnect(drv,...) creates and opens a connection to the database implemented by the driver drv. Connection string should be specified with parameters like user, password, dbname, host, port, tty and options. For more details refer to the documentation.
Eg.
```
con <- dbConnect(drv, dbname="tempdb")
```

3.dbListConnection(drv, ...) provides List of connections handled by the driver
Eg.
```
dbListConnections(drv)
```

4.dbGetInfo(dbObject, ...) and summary(dbObject) returns information about the dbObject (driver, connection or resultSet).
Eg.
```
dbGetInfo(drv)
summary(con)
```

5.dbSendQuery(con, statement, ...) submits one statement to the database.
Eg.
```
rs <- dbSendQuery(con,"select * from TableName")
```

6.fetch(rs,n, ...) fetches the next n elements from the result set.
Eg.
```
fetch(rs,n=-1) ## return all elements
fetch(rs,n=2) ##returns last 2 elements in record set.
```

7. dbGetQuery(con,statement, ...) submits, execute, and extract output in one operation. Eg.
```
dbGetQuery(con,"select * from TableName")
```

8. dbGetException(con, ...) returns the status of the last DBMS statement sent over the connection.
Eg.
```
dbGetException(con)
```

9. dbListResults(con, ...) returns the resultsets active on the given connection. Please note that the current RPostgreSQL package can handle only one resultset per connection (which may change in the future).
Eg.
```
dbListResults(con)
```

10. dbListTables(con, ...) returns the list of tables available on the connection.
Eg.
```
dbListTables(con)
```

11. dbExistsTable(con, TableName, ...) checks whether a particular table exists on the given connection. Returns a logical.
Eg.
```
dbExistsTable(con,"TableName")
```

12. dbRemoveTable(con, TableName, ...) removes the specified table on the connection. Returns a logical indicating operation succeeded or not.
Eg.
```
dbRemoveTable(con,"TableName")
```

13. dbListFields(con, TableName, ...) returns the list of column names (fields) in the table.
Eg.
```
dbListFields(con,"TableName")
```

14. dbColumnInfo(res, ...) produces a query that describes the output of the query.
Eg.
```
dbColumnInfo(rs)
```

15. dbReadTable(conn, name, ...) imports the data stored remotely in the table name on connection conn. Use the field row.names as the row.names attribute of the output data.frame. Returns a data.frame.
Eg.
```
dframe <-dbReadTable(con,"TableName"). 
```

16. dbWriteTable(conn, name, value, ...) writes the contents of the dataframe value into the table name specified. Returns a logical indicating whether operation succeeded or not.
Eg.
```
dbWriteTable(con,"newTable",dframe)
```

17. dbGetStatement(res, ...) returns the DBMS statement associated with the result.
Eg.
```
dbGetStatement(rs)
```

18. dbGetRowsAffected(res, ...) returns the rows affected the executed statement. If no rows are affected, "-1" is returned.
Eg.
```
dbGetRowsAffected(rs)
```

19. dbHasCompleted(res, ...) returns a logical to indicate whether an operation is completed or not.
Eg.
```
dbHasCompleted(rs)
```

20.dbGetRowCount(res, ...) returns number of rows fetched so far.
Eg.
```
dbGetRowCount(rs)
```

21.dbBeginTransaction begins the PostgreSQL transaction. dbCommit commits the transaction while dbRollback rolls back the transaction. Returns a logical indicating whether the operation succeeded or not.
Eg.
```
dbBeginTransaction(con)
dbRemoveTable(con,"newTable")
dbExistsTable(con,"newTable")
dbRollback(con)
dbExistsTable(con,"newTable")

dbBeginTransaction(con)
dbRemoveTable(con,"newTable")
dbExistsTable(con,"newTable")
dbCommit(con)
dbExistsTable(con,"newTable")
```

22. dbClearResult(rs, ...) flushes any pending data and frees the resources used by resultset.
Eg.
```
dbClearResult(rs)
```

23. dbDisconnect(con, ...) closes the connection.
Eg.
```
dbDisconnect(con)
```

24. dbUnloadDriver(drv,...) frees all the resources used by the driver.
Eg.
```
dbUnloadDriver(drv)
```


## Example ##

```
library(RPostgreSQL)

## loads the PostgreSQL driver
drv <- dbDriver("PostgreSQL")

## Open a connection
con <- dbConnect(drv, dbname="R_Project")

## Submits a statement
rs <- dbSendQuery(con, "select * from R_Users")

## fetch all elements from the result set
fetch(rs,n=-1)

## Submit and execute the query
dbGetQuery(con, "select * from R_packages")

## Closes the connection
dbDisconnect(con)

## Frees all the resources on the driver
dbUnloadDriver(drv)
```

For any queries, suggestions or bugs, please contact the mailing list stated above.