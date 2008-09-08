% $Id: dbConnect-methods.Rd,v 1.1 2003/12/02 14:53:01 dj Exp dj $
\name{dbConnect-methods}
\docType{methods}
\alias{dbDisconnect-methods}
\alias{dbConnect-methods}
\alias{dbDisconnect,PostgreSQLConnection-method}
\alias{dbConnect,PostgreSQLDriver-method}
\alias{dbConnect,PostgreSQLConnection-method}
\alias{dbConnect,character-method}
\title{
  Create a connection object to an PostgreSQL DBMS
}
\description{
   These methods are straight-forward implementations of the corresponding
   generic functions.
}
\section{Methods}{\describe{
\item{drv}{
  an object of class \code{PostgreSQLDriver}, or
  the character string "PostgreSQL" or an \code{PostgreSQLConnection}.
  }
\item{conn}{
  an \code{PostgreSQLConnection} object as produced by \code{dbConnect}.
  }
\item{host}{Name of host to connect to. If this begins with a slash, it specifies Unix-domain communication rather than TCP/IP communication; the value is the name of the directory in which the socket file is stored. The default behavior when host is not specified is to connect to a Unix-domain socket in /tmp (or whatever socket directory was specified when PostgreSQL was built). On machines without Unix-domain sockets, the default is to connect to localhost.}
\item{dbname}{The database name. Defaults to be the same as the user name.}
\item{user}{PostgreSQL user name to connect as. Defaults to be the same as the operating system name of the user running the application.}
\item{password}{Password to be used if the server demands password authentication.}
\item{hostaddr}{Numeric IP address of host to connect to. This should be in the standard IPv4 address format, e.g., 172.28.40.9. If your machine supports IPv6, you can also use those addresses. TCP/IP communication is always used when a nonempty string is specified for this parameter.}
\item{port}{Port number to connect to at the server host, or socket file name extension for Unix-domain connections.}
\item{tty}{Ignored (formerly, this specified where to send server debug output).}
\item{options}{Command-line options to be sent to the server}
}
}
\section{Side Effects}{
  A connection between R/S-Plus and an PostgreSQL  server is
  established.   The current implementation supports up to
  100 simultaneous connections.
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
\examples{\dontrun{
# create an PostgreSQL instance and create one connection.
drv <- dbDriver("PostgreSQL")

# open the connection using user, passsword, etc., as
con <- dbConnect(drv, user = "user", password="password")

# Run an SQL statement by creating first a resultSet object
rs <- dbSendQuery(con, statement = paste(
                      "SELECT w.laser_id, w.wavelength, p.cut_off",
                      "FROM WL w, PURGE P", 
                      "WHERE w.laser_id = p.laser_id", 
                      "SORT BY w.laser_id")
# we now fetch records from the resultSet into a data.frame
data <- fetch(rs, n = -1)   # extract all rows
dim(data)
}
}
\keyword{methods}
\keyword{interface}
\keyword{database}
% vim: syntax=tex
