% $Id: dbDriver-methods.Rd,v 1.1 2003/12/02 14:53:01 dj Exp dj $
\name{dbDriver-methods}
\docType{methods}
\alias{dbDriver-methods}
\alias{dbUnloadDriver-methods}
\alias{dbDriver,character-method}
\alias{dbUnloadDriver,PostgreSQLDriver-method}
\title{
  PostgreSQL implementation of the Database Interface (DBI) classes
  and drivers 
}
\description{
  PostgreSQL driver initialization and closing
}
\section{Methods}{\describe{
\item{drvName}{
  character name of the driver to instantiate.
  }
\item{drv}{
  an object that inherits from \code{PostgreSQLDriver} as created by
  \code{dbDriver}.
}
\item{max.con}{optional integer requesting the maximum number of
simultanous connections (may be up to 100)}.
\item{fetch.default.rec}{default number of records to retrieve per fetch.
Default is 500.  This may be overridden in calls to \code{\link[DBI]{fetch}}
with the \code{n=} argument.}
\item{force.reload}{optional logical used to force re-loading or recomputing
the size of the connection table. Default is \code{FALSE}.}
\item{...}{currently unused.}
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
  \code{\link[DBI]{dbListTables}},
  \code{\link[DBI]{dbReadTable}}.
}
\examples{\dontrun{
# create an PostgreSQL instance and set 10000 of rows per fetch.
m <- dbDriver("PostgreSQL", fetch.default.records=10000)

con <- dbConnect(m, username="usr", password = "pwd",
           dbname = "iptraffic")
rs <- dbSubmitQuery(con, 
         "select * from HTTP_ACCESS where IP_ADDRESS = '127.0.0.1'")
df <- fetch(rs, n = 50)
df2 <- fetch(rs, n = -1)
dbClearResult(rs)

pcon <- dbConnect(p, group = "wireless")
dbListTables(pcon)
}
}
\keyword{methods}
\keyword{interface}
\keyword{database}
% vim: syntax=tex
