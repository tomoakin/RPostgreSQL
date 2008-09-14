% $Id: PostgreSQLConnection-class.Rd,v 0.1 2008/07/23 03:14:11 psk Exp $
\name{PostgreSQLConnection-class}
\docType{class}
\alias{PostgreSQLConnection-class}
\title{Class PostgreSQLConnection}
\description{PostgreSQLConnection class.}
\section{Generators}{
    The method \code{\link[DBI]{dbConnect}} is the main generator.
}
\section{Extends}{
Class \code{"DBIConnection"}, directly.
Class \code{"PostgreSQLObject"}, directly.
Class \code{"DBIObject"}, by class "DBIConnection".
Class \code{"dbObjectId"}, by class "PostgreSQLObject".
}
\section{Methods}{
  \describe{
    \item{\link{coerce}}{\code{signature(from = "PostgreSQLConnection", to = "PostgreSQLResult")}: ... }
    \item{\link[DBI]{dbCallProc}}{\code{signature(conn = "PostgreSQLConnection")}: ... }
    \item{\link[DBI]{dbCommit}}{\code{signature(conn = "PostgreSQLConnection")}: ... }
    \item{\link[DBI]{dbConnect}}{\code{signature(drv = "PostgreSQLConnection")}: ... }
    \item{\link[DBI]{dbDisconnect}}{\code{signature(conn = "PostgreSQLConnection")}: ... }
    \item{\link[DBI]{dbExistsTable}}{\code{signature(conn = "PostgreSQLConnection", name = "character")}: ... }
    \item{\link[DBI]{dbGetException}}{\code{signature(conn = "PostgreSQLConnection")}: ... }
    \item{\link[DBI]{dbGetInfo}}{\code{signature(dbObj = "PostgreSQLConnection")}: ... }
    \item{\link[DBI]{dbGetQuery}}{\code{signature(conn = "PostgreSQLConnection", statement = "character")}: ... }
    \item{\link[DBI]{dbListFields}}{\code{signature(conn = "PostgreSQLConnection", name = "character")}: ... }
    \item{\link[DBI]{dbListResults}}{\code{signature(conn = "PostgreSQLConnection")}: ... }
    \item{\link[DBI]{dbListTables}}{\code{signature(conn = "PostgreSQLConnection")}: ... }
    \item{\link[DBI]{dbReadTable}}{\code{signature(conn = "PostgreSQLConnection", name = "character")}: ... }
    \item{\link[DBI]{dbRemoveTable}}{\code{signature(conn = "PostgreSQLConnection", name = "character")}: ... }
    \item{\link[DBI]{dbRollback}}{\code{signature(conn = "PostgreSQLConnection")}: ... }
    \item{\link[DBI]{dbSendQuery}}{\code{signature(conn = "PostgreSQLConnection", statement = "character")}: ... }
    \item{\link[DBI]{dbWriteTable}}{\code{signature(conn = "PostgreSQLConnection", name = "character", value = "data.frame")}: ... }
    \item{summary}{\code{signature(object = "PostgreSQLConnection")}: ... }
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
drv <- dbDriver("PostgreSQL)
con <- dbConnect(drv, dbname = "template1")
}
}
\keyword{database}
\keyword{interface}
\keyword{classes}
% vim: syntax=tex
