% $Id: dbCommit-methods.Rd,v 0.1 2008/08/10 18:04:01 psk Exp $
\name{dbCommit-methods}
\docType{methods}
\alias{dbCommit-methods}
\alias{dbRollback-methods}
\alias{PostgreSQLConnection-method}
\alias{dbCommit,PostgreSQLConnection-method}
\alias{dbRollback,PostgreSQLConnection-method}
\title{
  DBMS Transaction Management
}
\description{
  Commits or roll backs the current transaction 
  in an PostgreSQL connection.
  \code{dbCommit} and \code{dbRollback} commit and rollback the
  transaction, respectively.

}
\section{Methods}{\describe{
\item{conn}{a \code{PostgreSQLConnection} object, as produced by the function
  \code{dbConnect}.}
\item{\dots }{currently unused.}
}
}
\references{
  See the Database Interface definition document
  \code{DBI.pdf} in the base directory of this package
  or
  \url{http://stat.bell-labs.com/RS-DBI}.
}
\seealso{
   \code{\link{PostgreSQL}},
   \code{\link[DBI]{dbConnect}},
   \code{\link[DBI]{dbSendQuery}},
   \code{\link[DBI]{dbGetQuery}},
   \code{\link[DBI]{fetch}},
   \code{\link[DBI]{dbCommit}},
   \code{\link[DBI]{dbGetInfo}},
   \code{\link[DBI]{dbReadTable}}.
}
\examples{\dontrun{
drv <- dbDriver("PostgreSQL")
con <- dbConnect(drv, dbname="postgres")
dbGetQuery(con, "select count(*) from sales")

dbBeginTransaction(con)
rs <- dbSendQuery(con, 
      "Delete * from sales as p where p.cost>10")
if(dbGetInfo(rs, what = "rowsAffected") > 250){
  warning("Rolling back transaction")
  dbRollback(con)
}

dbGetQuery(con, "select count(*) from sales")
dbDisconnect(con)
}
}

\keyword{methods}
\keyword{interface}
\keyword{database}
% vim: syntax=tex
