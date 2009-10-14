% $Id: dbSendQuery-methods.Rd,v 0.1 2008/07/23 02:38:31 psk Exp $
\name{dbSendQuery-methods}
\docType{methods}
\alias{dbSendQuery-methods}
\alias{dbGetQuery-methods}
\alias{dbClearResult-methods}
\alias{dbGetException-methods}
\alias{dbSendQuery,PostgreSQLConnection,character-method}
\alias{dbGetQuery,PostgreSQLConnection,character-method}
\alias{dbClearResult,PostgreSQLResult-method}
\alias{dbGetException,PostgreSQLConnection-method}
\alias{dbGetException,PostgreSQLResult-method}
\title{
  Execute a statement on a given database connection
}
\description{
   These methods are straight-forward implementations of the corresponding
   generic functions.
}
\section{Methods}{\describe{
\item{conn}{
  an \code{PostgreSQLConnection} object.
  }
\item{statement}{a character vector of length 1 with the SQL statement.}
\item{res}{an \code{PostgreSQLResult} object.}
\item{\dots }{additional parameters.}
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
  \code{\link[DBI]{fetch}},
  \code{\link[DBI]{dbCommit}},
  \code{\link[DBI]{dbGetInfo}},
  \code{\link[DBI]{dbReadTable}}.
}
\examples{\dontrun{
drv <- dbDriver("PostgreSQL")
con <- dbConnect(drv, "usr", "password", "dbname")
res <- dbSendQuery(con, "SELECT * from sales")
data <- fetch(res, n = -1)
# alternatively, use dbGetQuery
data <- dbGetQuery(con, "SELECT * from sales")
}
}
\keyword{methods}
\keyword{interface}
\keyword{database}
% vim: syntax=tex
