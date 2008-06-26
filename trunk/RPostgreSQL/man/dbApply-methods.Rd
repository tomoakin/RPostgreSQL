% $Id: dbApply-methods.Rd,v 1.1 2003/12/02 14:53:01 dj Exp dj $
\name{dbApply-methods}
\docType{methods}
\alias{dbApply-methods}
\alias{dbApply,PostgreSQLResult-method}
\title{Apply R/S-Plus functions to remote groups of DBMS rows (experimental)}
\description{
  Applies R/S-Plus functions to groups of remote DBMS rows without
  bringing an entire result set all at once.  The result set
  is expected to be sorted by the grouping field.
}
\section{Methods}{\describe{
  \item{res}{a PostgreSQL result set (see \code{\link[DBI]{dbSendQuery}}).}
  \item{...}{any additional arguments to be passed to \code{FUN}.}
}
}
\references{
  See the Database Interface definition document
  \code{DBI.pdf} in the base directory of this package
  or
  \url{http://stat.bell-labs.com/RS-DBI}.
}
\seealso{
   \code{\link{PostgreSQL}}
   \code{\link{postgresqlDBApply}}
   \code{\link[DBI]{dbSendQuery}}
   \code{\link[DBI]{fetch}}
}

\examples{\dontrun{
## compute quanitiles for each network agent
con <- dbConnect(PostgreSQL(), user="user", password="passwd")
rs <- dbSendQuery(con, 
             "select Agent, ip_addr, DATA from pseudo_data order by Agent")
out <- dbApply(rs, INDEX = "Agent", 
        FUN = function(x, grp) quantile(x$DATA, names=FALSE))
}
}
\keyword{programming}
\keyword{interface}
\keyword{database}
% vim: syntax=tex
