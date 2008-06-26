% $Id: dbListTables-methods.Rd,v 1.1 2003/12/02 14:53:01 dj Exp dj $
\name{dbListTables-methods}
\docType{methods}
\alias{dbListTables-methods}
\alias{dbListFields-methods}
\alias{dbListConnections-methods}
\alias{dbListResults-methods}
\alias{dbListTables,PostgreSQLConnection-method}
\alias{dbListFields,PostgreSQLConnection,character-method}
\alias{dbListFields,PostgreSQLResult,missing-method}
\alias{dbListConnections,PostgreSQLDriver-method}
\alias{dbListResults,PostgreSQLConnection-method}
\title{
  List items from an PostgreSQL DBMS and from objects
}
\description{
   These methods are straight-forward implementations of the corresponding
   generic functions.
}
\section{Methods}{\describe{
  \item{drv}{an \code{PostgreSQLDriver}.}
  \item{conn}{an \code{PostgreSQLConnection}.}
  \item{name}{a character string with the table name.}
  \item{\dots}{currently not used.}
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
  \code{\link[DBI]{dbGetInfo}},
  \code{\link[DBI]{dbColumnInfo}},
  \code{\link[DBI]{dbDriver}},
  \code{\link[DBI]{dbConnect}},
  \code{\link[DBI]{dbSendQuery}}
}

\examples{\dontrun{
drv <- dbDriver("PostgreSQL")
# after working awhile...
for(con in dbListConnections(drv)){
   dbGetStatement(dbListResults(con))
}
}
}
\keyword{methods}
\keyword{interface}
\keyword{database}
% vim: syntax=tex
