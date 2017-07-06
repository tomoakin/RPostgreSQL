## Test of special value 'infinity' in date/time columns
##
## Michael Kaminsky,  8 May 2015

## only run this if this env.var is set correctly
if ((Sys.getenv("POSTGRES_USER") != "") &
    (Sys.getenv("POSTGRES_HOST") != "") &
    (Sys.getenv("POSTGRES_DATABASE") != "")) {
   
   ## try to load our module and abort if this fails
   stopifnot(require(RPostgreSQL))
   
   ## Force a timezone to make the tests comparable at different locations
   Sys.setenv("PGDATESTYLE"="German")
   Sys.setenv("TZ"="UTC")
   
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
   
   dbGetQuery(con, "SET TIMEZONE TO 'UTC'")
   
   dbTypeTests <- function(con, dateclass="timestamp without time zone") {
      cat("\n\n**** Trying with ", dateclass, "\n")
      
      if (dbExistsTable(con, "tempostgrestable"))
         dbRemoveTable(con, "tempostgrestable")
      
      dbGetQuery(con, paste("create table tempostgrestable (tt ", dateclass, ", zz integer);", sep=""))
      dbGetQuery(con, paste("insert into tempostgrestable values('", "infinity", "', 1);", sep=""))
      dbGetQuery(con, paste("insert into tempostgrestable values('", "-infinity", "', 2);", sep=""))
      res <- dbReadTable(con, "tempostgrestable")
      print(res)
      
      res <- dbSendQuery(con, "select tt from tempostgrestable;")
      data <- fetch(res, n=-1)
      print(dbColumnInfo(res))
      
      posinfinity <- data[1,1]
      neginfinity <- data[2,1]
      
      if(is.infinite(posinfinity) & is.infinite(neginfinity)){
         if (posinfinity > Sys.Date() & neginfinity < Sys.Date())
            cat('PASS: Infinity returned properly\n')
      }else{
         cat('FAIL: Infinity not returned properly\n')
      }
      
      dbRemoveTable(con, "tempostgrestable")
      invisible(NULL)
   }
   
   dbTypeTests(con, "timestamp")
   dbTypeTests(con, "timestamp with time zone")
   dbTypeTests(con, "date")
   
   dbDisconnect(con)
}else{
   cat("Skip.\n")
}
