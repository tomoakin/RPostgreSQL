% $Id: dbCallProc-methods.Rd,v 0.1 2008/08/10 18:04:01 psk Exp $
\name{dbCallProc-methods}
\docType{methods}
\alias{dbCallProc-methods}
\alias{dbCallProc,PostgreSQLConnection-method}
\title{
  Call an SQL stored procedure
}
\description{
  Not yet implemented.
}
\section{Methods}{\describe{
\item{conn}{
  a \code{PostgreSQLConnection} object.
  }
\item{\dots }{
  additional arguments are passed to the implementing method.
  }
}
}
\references{
  See the Database Interface definition document
  \code{DBI.pdf} in the base directory of this package
  or \url{http://stat.bell-labs.com/RS-DBI}.
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
\keyword{methods}
\keyword{interface}
\keyword{database}
% vim: syntax=tex
