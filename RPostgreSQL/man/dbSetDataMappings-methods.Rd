% $Id: dbSetDataMappings-methods.Rd,v 1.1 2003/12/02 14:53:01 dj Exp dj $
\name{dbSetDataMappings-methods}
\docType{methods}
\alias{dbSetDataMappings-methods}
\alias{dbSetDataMappings,PostgreSQLResult,data.frame-method}
\title{
  Set data mappings between PostgreSQL and R/S-Plus
}
\description{
  Not yet implemented
}
\section{Methods}{\describe{
\item{res}{
  a \code{PostgreSQLResult} object as returned by \code{dbSendQuery}.
  }
\item{flds}{
  a data.frame with field descriptions as returned by 
  \code{\link[DBI]{dbColumnInfo}}.
  }
\item{\dots }{
  any additional arguments are passed to the implementing method.
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
  \code{\link[DBI]{dbSendQuery}},
  \code{\link[DBI]{fetch}},
  \code{\link[DBI]{dbColumnInfo}}.
}
\examples{\dontrun{
makeImage <- function(x) {
  .C("make_Image", as.integer(x), length(x))
}

res <- dbSendQuery(con, statement)
flds <- dbColumnInfo(res)
flds[3, "Sclass"] <- makeImage

dbSetDataMappings(rs, flds)

im <- fetch(rs, n = -1)
}
}
\keyword{methods}
\keyword{interface}
\keyword{database}
% vim: syntax=tex
