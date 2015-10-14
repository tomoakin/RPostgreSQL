% $Id: PostgreSQLDriver-class.Rd,v 0.1 2008/07/22 20:56:01 psk Exp $
\name{PostgreSQLDriver-class}
\docType{class}
\alias{PostgreSQLDriver-class}
\title{Class PostgreSQLDriver}
\description{
   An PostgreSQL driver implementing the R/S-Plus database (DBI) API.
}
\section{Generators}{
   The main generators are \code{\link[DBI]{dbDriver}} and 
   \code{\link{PostgreSQL}}.
}
\section{Extends}{
Class \code{"DBIDriver"}, directly.
Class \code{"PostgreSQLObject"}, directly.
Class \code{"DBIObject"}, by class "DBIDriver".
Class \code{"dbObjectId"}, by class "PostgreSQLObject".
}
\section{Methods}{
  \describe{
    \item{\link{coerce}}{\code{signature(from = "PostgreSQLObject", to = "PostgreSQLDriver")}: ... }
    \item{\link[DBI]{dbConnect}}{\code{signature(drv = "PostgreSQLDriver")}: ... }
    \item{\link[DBI]{dbGetInfo}}{\code{signature(dbObj = "PostgreSQLDriver")}: ... }
    \item{\link[DBI]{dbListConnections}}{\code{signature(drv = "PostgreSQLDriver")}: ... }
    \item{\link[DBI]{dbUnloadDriver}}{\code{signature(drv = "PostgreSQLDriver")}: ... }
    \item{\link{summary}}{\code{signature(object = "PostgreSQLDriver")}: ... }
  }
}
\references{
  See the Database Interface definition document
  \code{DBI.pdf} in the base directory of this package
  or \url{http://developer.r-project.org/db}.
}

\seealso{
 DBI base classes:

 \code{\link[DBI]{DBIObject-class}}
 \code{\link[DBI]{DBIDriver-class}}
 \code{\link[DBI]{DBIConnection-class}}
 \code{\link[DBI]{DBIResult-class}}

 PostgreSQL classes:

 \code{\link{PostgreSQLObject-class}}
 \code{\link{PostgreSQLDriver-class}}
 \code{\link{PostgreSQLConnection-class}}
 \code{\link{PostgreSQLResult-class}}
}

\examples{\dontrun{
drv <- dbDriver("PostgreSQL")
con <- dbConnect(drv, dbname="template1")
}
}
\keyword{database}
\keyword{interface}
\keyword{classes}
% vim: syntax=tex
