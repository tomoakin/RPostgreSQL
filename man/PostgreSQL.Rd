% $Id: PostgreSQL.Rd,v 0.1 2008/07/23 03:07:31 psk Exp $
\name{PostgreSQL}
\alias{PostgreSQL}
\title{
  Instantiate a PostgreSQL client from the current R or S-Plus session
}
\description{
This function creates and initializes a PostgreSQL client.
It returns an driver object that allows you to connect
to one or several PostgreSQL servers.
}
\usage{
PostgreSQL(max.con = 16, fetch.default.rec = 500, force.reload = FALSE)
}
\arguments{
\item{max.con }{
  Maximum number of connections that are intended to have open at one time.
  There's no intrinic limit, since strictly speaking this limit applies 
  to PostgreSQL \emph{servers}, but clients can have (at least in theory)
  more than this.  Typically there are at most a handful of open connections,
  thus the internal \code{RPostgreSQL} code uses a very simple linear search
  algorithm to manage its connection table.
  }
\item{fetch.default.rec}{
  number of records to fetch at one time from the database.
  (The \code{\link[DBI]{fetch}} method uses this number as a default.)
  }
\item{force.reload}{
  should the client code be reloaded (reinitialize)?
  Setting this to \code{TRUE} allows you to change
  default settings.  Notice that all connections should be closed
  before re-loading.
  }
}
\value{
An object \code{PostgreSQLDriver} that extends
\code{dbDriver} and 
\code{dbObjectId}.
This object is required to create connections
to one or several PostgreSQL database engines.
}
\section{Side Effects}{
The R/S-Plus client part of the database communication is initialized,
but note that connecting to the database engine needs to be done through
calls to \code{\link[DBI]{dbConnect}}.
}
\details{
This object is a singleton, that is, on subsequent invocations
it returns the same initialized object. 

This implementation allows you to connect
to multiple host servers and run multiple connections on each server
simultaneously.
}
\section{User authentication}{
The passed string can be empty to use all default parameters, or it can
contain one or more parameter settings separated by comma. Each
parameter setting is in the form parameter = "value". Spaces around the
equal sign are optional. 

The most important parameters are \code{user}, \code{password},
\code{host}, \code{dbname}, \code{port}, \code{tty} and \code{options}. 
}
\author{David A. James}
\section{References}{
See \url{stat.bell-labs.com/RS-DBI} 
for more details on the R/S-Plus database interface.

See the documentation at the PostgreSQL Web site
\url{http://www.postgresql.org} for details.
}
\seealso{
On database managers:

\code{\link[DBI]{dbDriver}}
\code{\link[DBI]{dbUnloadDriver}}

On connections, SQL statements and resultSets:

\code{\link[DBI]{dbConnect}}
\code{\link[DBI]{dbDisconnect}}
\code{\link[DBI]{dbSendQuery}}
\code{\link[DBI]{dbGetQuery}}
\code{\link[DBI]{fetch}}
\code{\link[DBI]{dbClearResult}}

On transaction management:

\code{\link[DBI]{dbCommit}}
\code{\link[DBI]{dbRollback}}

On meta-data:

\code{\link{summary}}
\code{\link[DBI]{dbGetInfo}}
\code{\link[DBI]{dbGetDBIVersion}}
\code{\link[DBI]{dbListTables}}
\code{\link[DBI]{dbListConnections}}
\code{\link[DBI]{dbListResults}}
\code{\link[DBI]{dbColumnInfo}}
\code{\link[DBI]{dbGetException}}
\code{\link[DBI]{dbGetStatement}}
\code{\link[DBI]{dbHasCompleted}}
\code{\link[DBI]{dbGetRowCount}}
\code{\link[DBI]{dbGetRowsAffected}}
}
\examples{\dontrun{
# create a PostgreSQL instance and create one connection.
> m <- dbDriver("PostgreSQL")
<PostgreSQLDriver:(4378)>

> con <- dbConnect(m, user="username", password="passwd", dbname="database_name")
> rs <- dbSendQuery(con, "select * sales where price < 10")
> df <- fetch(rs, n = 50)
> dbHasCompleted(rs)
[1] FALSE
> df2 <- fetch(rs, n = -1)
> dbHasCompleted(rs)
[1] TRUE
> dbClearResult(rs)
> dbListTables(con)    
}
}
\keyword{interface}
\keyword{database}
% vim: syntax=tex
