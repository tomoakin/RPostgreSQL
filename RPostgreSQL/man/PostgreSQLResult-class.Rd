% $Id: PostgreSQLResult-class.Rd,v 1.1 2003/12/02 14:53:01 dj Exp dj $
\name{PostgreSQLResult-class}
\docType{class}
\alias{PostgreSQLResult-class}
\title{Class PostgreSQLResult}
\description{
   PostgreSQL's query results class.  This classes encapsulates the
   result of an SQL statement (either \code{select} or not).
}
\section{Generators}{
   The main generator is \code{\link[DBI]{dbSendQuery}}.
}
\section{Extends}{
Class \code{"DBIResult"}, directly.
Class \code{"PostgreSQLObject"}, directly.
Class \code{"DBIObject"}, by class "DBIResult".
Class \code{"dbObjectId"}, by class "PostgreSQLObject".
}
\section{Methods}{
  \describe{
    \item{\link{coerce}}{\code{signature(from = "PostgreSQLConnection", to = "PostgreSQLResult")}: ... }
    \item{\link[DBI]{dbClearResult}}{\code{signature(res = "PostgreSQLResult")}: ... }
    \item{\link[DBI]{dbColumnInfo}}{\code{signature(res = "PostgreSQLResult")}: ... }
    \item{\link[DBI]{dbGetException}}{\code{signature(conn = "PostgreSQLResult")}: ... }
    \item{\link[DBI]{dbGetInfo}}{\code{signature(dbObj = "PostgreSQLResult")}: ... }
    \item{\link[DBI]{dbGetRowCount}}{\code{signature(res = "PostgreSQLResult")}: ... }
    \item{\link[DBI]{dbGetRowsAffected}}{\code{signature(res = "PostgreSQLResult")}: ... }
    \item{\link[DBI]{dbGetStatement}}{\code{signature(res = "PostgreSQLResult")}: ... }
    \item{\link[DBI]{dbHasCompleted}}{\code{signature(res = "PostgreSQLResult")}: ... }
    \item{\link[DBI]{dbListFields}}{\code{signature(conn = "PostgreSQLResult", name = "missing")}: ... }
    \item{\link[DBI]{fetch}}{\code{signature(res = "PostgreSQLResult", n = "numeric")}: ... }
    \item{\link[DBI]{fetch}}{\code{signature(res = "PostgreSQLResult", n = "missing")}: ... }
    \item{\link{summary}}{\code{signature(object = "PostgreSQLResult")}: ... }
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
con <- dbConnect(drv, dbname = "rsdbi.db")
}
}
\keyword{database}
\keyword{interface}
\keyword{classes}
% vim: syntax=tex
