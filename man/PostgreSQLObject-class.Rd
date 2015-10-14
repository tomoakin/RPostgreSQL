% $Id: PostgreSQLObject-class.Rd,v 0.1 2008/07/22 20:56:01 psk Exp $
\name{PostgreSQLObject-class}
\docType{class}
\alias{PostgreSQLObject-class}
\title{Class PostgreSQLObject}
\description{
   Base class for all PostgreSQL-specific DBI classes
}
\section{Objects from the Class}{
   A virtual Class: No objects may be created from it.
}
\section{Extends}{
Class \code{"DBIObject"}, directly.
Class \code{"dbObjectId"}, directly.
}
\section{Methods}{
  \describe{
    \item{\link{coerce}}{\code{signature(from = "PostgreSQLObject", to = "PostgreSQLriver")}: ... }
    \item{\link[DBI]{dbDataType}}{\code{signature(dbObj = "PostgreSQLObject")}: ... }
    \item{\link[DBI]{isSQLKeyword}}{\code{signature(dbObj = "PostgreSQLObject", name = "character")}: ... }
    \item{\link[DBI]{make.db.names}}{\code{signature(dbObj = "PostgreSQLObject", snames = "character")}: ... }
    \item{\link[DBI]{SQLKeywords}}{\code{signature(dbObj = "PostgreSQLObject")}: ... }
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
con <- dbConnect(drv, dbname = "template1")
}
}
\keyword{database}
\keyword{interface}
\keyword{classes}
% vim: syntax=tex
