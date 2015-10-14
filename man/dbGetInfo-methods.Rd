% $Id: dbGetInfo-methods.Rd,v 0.1 2008/08/10 18:04:01 psk Exp $
\name{dbGetInfo-methods}
\docType{methods}
\alias{dbGetInfo}
\alias{dbGetDBIVersion-methods}
\alias{dbGetStatement-methods}
\alias{dbGetRowCount-methods}
\alias{dbGetRowsAffected-methods}
\alias{dbColumnInfo-methods}
\alias{dbHasCompleted-methods}
\alias{dbGetInfo,PostgreSQLObject-method}
\alias{dbGetInfo,PostgreSQLDriver-method}        % BUG: this is not needed
\alias{dbGetInfo,PostgreSQLConnection-method}        % BUG: this is not needed
\alias{dbGetInfo,PostgreSQLResult-method}        % BUG: this is not needed
\alias{dbGetStatement,PostgreSQLResult-method}
\alias{dbGetRowCount,PostgreSQLResult-method}
\alias{dbGetRowsAffected,PostgreSQLResult-method}
\alias{dbColumnInfo,PostgreSQLResult-method}
\alias{dbHasCompleted,PostgreSQLResult-method}
\title{
  Database interface meta-data
}
\description{
   These methods are straight-forward implementations of the corresponding
   generic functions.
}
\section{Methods}{\describe{
\item{dbObj}{
  any object that implements some functionality in the R/S-Plus
  interface to databases (a driver, a connection or a result set).
  }
\item{res}{ an \code{PostgreSQLResult}.}
\item{\dots}{currently not being used.}
}
}
\references{
  See the Database Interface definition document
  \code{DBI.pdf} in the base directory of this package
  or \url{http://stat.bell-labs.com/RS-DBI}.
}
\seealso{
  \code{\link{PostgreSQL}},
  \code{\link[DBI]{dbDriver}},
  \code{\link[DBI]{dbConnect}},
  \code{\link[DBI]{dbSendQuery}},
  \code{\link[DBI]{dbGetQuery}},
  \code{\link[DBI]{fetch}},
  \code{\link[DBI]{dbCommit}},
  \code{\link[DBI]{dbGetInfo}},
  \code{\link[DBI]{dbListTables}},
  \code{\link[DBI]{dbReadTable}}.
}
\examples{\dontrun{
drv <- dbDriver("PostgreSQL")
con <- dbConnect(drv, user= "user", password="password", dbname="sample")

dbListTables(con)

rs <- dbSendQuery(con, query.sql)
dbGetStatement(rs)
dbHasCompleted(rs)

info <- dbGetInfo(rs)
names(dbGetInfo(drv))

# DBIConnection info
names(dbGetInfo(con))

# DBIResult info
names(dbGetInfo(rs)) 
}
}
\note{nullOk in \code{dbColumnInfo} was changed. Now it may be TRUE, FALSE, or NA; the column may be totally deleted in future releases;}
\keyword{methods}
\keyword{interface}
\keyword{database}
% vim: syntax=tex
