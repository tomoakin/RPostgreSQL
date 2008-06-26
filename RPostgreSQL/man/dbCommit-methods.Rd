% $Id: dbCommit-methods.Rd,v 1.1 2003/12/02 14:53:01 dj Exp dj $
\name{dbCommit-methods}
\docType{methods}
\alias{dbCommit-methods}
\alias{dbRollback-methods}
\alias{dbCommit,PostgreSQLConnection-method}
\alias{dbRollback,PostgreSQLConnection-method}
\title{
  DBMS Transaction Management
}
\description{
  Commits or roll backs the current transaction 
  in an PostgreSQL connection
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
con <- dbConnect(drv, group = "group")
rs <- dbSendQuery(con, 
      "delete * from PURGE as p where p.wavelength<0.03")
if(dbGetInfo(rs, what = "rowsAffected") > 250){
  warning("dubious deletion -- rolling back transaction")
  dbRollback(con)
}
}
}
\keyword{methods}
\keyword{interface}
\keyword{database}
% vim: syntax=tex
