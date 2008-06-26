## Class: dbObjectId		Last Modified:

## This package was developed as a part of Summer of Code program organized by Google.
## Thanks to David A. James & Saikat DebRoy, the authors of RMySQL package.
## Code from RMySQL package was reused with the permission from the authors.
## Also Thanks to my GSoC mentor Dirk Eddelbuettel for helping me in the development.


##
## This mixin helper class is NOT part of the database interface definition,
## but it is extended by the Oracle, MySQL,PostgreSQL and SQLite implementations
## to allow us to conviniently (and portably)
## implement all database foreign objects methods (i.e., methods for show(), 
## print() format() the dbManger, dbConnection, dbResultSet, etc.) 
## A dbObjectId is an  identifier into an actual remote database objects.  
## This class and its derived classes <driver-manager>Object need to 
## be VIRTUAL to avoid coercion (green book, p.293) during method dispatching.

setClass("dbObjectId", representation(Id = "integer", "VIRTUAL"))

## coercion methods 
setAs("dbObjectId", "integer", 
   def = function(from) as(slot(from,"Id"), "integer")
)
setAs("dbObjectId", "numeric",
   def = function(from) as(slot(from, "Id"), "integer")
)
setAs("dbObjectId", "character",
   def = function(from) as(slot(from, "Id"), "character")
)   

## formating, showing, printing,...
setMethod("format", "dbObjectId", 
   def = function(x, ...) {
      paste("(", paste(as(x, "integer"), collapse=","), ")", sep="")
   },
   valueClass = "character"
)

setMethod("show", "dbObjectId", def = function(object) print(object))

setMethod("print", "dbObjectId",
   def = function(x, ...){
      expired <- if(isIdCurrent(x)) "" else "Expired "
      str <- paste("<", expired, class(x), ":", format(x), ">", sep="")
      cat(str, "\n")
      invisible(NULL)
   }
)

"isIdCurrent" <- 
function(obj)
## verify that obj refers to a currently open/loaded database
{ 
   obj <- as(obj, "integer")
   .Call("RS_DBI_validHandle", obj, PACKAGE = .PostgreSQLPkgName)
}

 
