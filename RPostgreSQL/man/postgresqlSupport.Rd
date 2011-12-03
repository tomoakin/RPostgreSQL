% $Id: postgresqlSupport.Rd,v 0.1 2008/07/23 02:23:01 psk Exp $
\name{postgresqlSupport}
\alias{postgresqlInitDriver}            % driver-related function
\alias{postgresqlDriverInfo}
\alias{postgresqlDescribeDriver}
\alias{postgresqlCloseDriver}           % connection-related
\alias{postgresqlNewConnection}
\alias{postgresqlCloneConnection}
\alias{postgresqlConnectionInfo}
\alias{postgresqlDescribeConnection}
\alias{postgresqlCloseConnection}
\alias{postgresqlExecStatement}         % result-related
\alias{postgresqlFetch}
\alias{postgresqlQuickSQL}
\alias{postgresqlResultInfo}
\alias{postgresqlDescribeResult}
\alias{postgresqlCloseResult}
\alias{postgresqlDescribeFields}
\alias{postgresqlReadTable}
\alias{postgresqlWriteTable}            % convenience functions
\alias{postgresqlpqExec}
\alias{postgresqlCopyIn}
\alias{postgresqlCopyInDataframe}
\alias{postgresqlgetResult}
\alias{postgresqlEscapeStrings}
\alias{postgresqlQuoteId}
\alias{postgresqlTableRef}
\alias{postgresqlImportFile}
\alias{postgresqlDataType}
\alias{postgresqlTransactionStatement}   % transaction management 
\alias{.PostgreSQLPkgName}              % constants
\alias{.PostgreSQLPkgVersion}
\alias{.PostgreSQLPkgRCS}
\alias{.PostgreSQL.NA.string}
\alias{.PostgreSQLSQLKeywords}
\alias{.conflicts.OK}
%\non_function{}
\title{Support Functions}
\description{
   These functions are the workhorse behind the RPostgreSQL package, but
   users need not invoke these directly.  For details see \code{\link{PostgreSQL}}.
}
\usage{
   ## PostgreSQLDriver-related
   postgresqlInitDriver(max.con=16, fetch.default.rec = 500, force.reload=FALSE)
   postgresqlDriverInfo(obj, what, ...)
   postgresqlDescribeDriver(obj, verbose = FALSE, ...)
   postgresqlCloseDriver(drv, ...)

   ## PostgreSQLConnection-related
   postgresqlNewConnection(drv,user,password,host,dbname,port,tty,options)
   postgresqlCloneConnection(con, ...)
   postgresqlConnectionInfo(obj, what, ...)
   postgresqlDescribeConnection(obj, verbose = FALSE, ...)
   postgresqlCloseConnection(con, ...)

   ## PostgreSQLResult-related
   postgresqlExecStatement(con, statement)
   postgresqlFetch(res, n=0, ...)
   postgresqlQuickSQL(con, statement)
   postgresqlResultInfo(obj, what, ...)
   postgresqlDescribeResult(obj, verbose = FALSE, ...)
   postgresqlCloseResult(res, ...)
   postgresqlDescribeFields(res, ...)

   ## data mappings, convenience functions, and extensions
   postgresqlDataType(obj, ...)
   postgresqlReadTable(con, name, row.names = "row.names", check.names = TRUE, ...)
   postgresqlWriteTable(con, name, value, field.types, row.names = TRUE,
      overwrite=FALSE, append=FALSE, ..., allow.keywords = FALSE)
   postgresqlpqExec(con, statement)
   postgresqlCopyIn(con, filename)
   postgresqlgetResult(con)
   postgresqlEscapeStrings(con, preescapedstring)
   postgresqlQuoteId(identifiers)
   postgresqlTableRef(identifiers)
   postgresqlImportFile(con, name, value, field.types, overwrite=FALSE,
      append=FALSE, header, row.names, nrows=50, sep=",", eol="\n", 
      skip = 0, quote="\"", ...)

   ## Transaction Management
   postgresqlTransactionStatement(con, statement)
}
\arguments{
\item{max.con}{
   positive integer specifying maximum number of open connections.
   The current default of 10 is hardcoded in the C code.
}
\item{fetch.default.rec}{
   default number of rows to fetch (move to R/S-Plus).  This default is
   used in \code{postgresqlFetch}.
   The default is 500.
}
\item{force.reload}{
   logical indicating whether to re-initialize the driver.  This may be
   useful if you want to change the defaults (e.g., \code{fetch.default.rec}).
   Note that the driver is a singleton (subsequent inits just returned the
   previously initialized driver, thus this argument).
}
\item{obj}{
   any of the PostgreSQL DBI objects (e.g., \code{PostgreSQLConnection},
   \code{PostgreSQLResult}).
}
\item{what}{
   character vector of metadata to extract, e.g., "version", "statement",
   "isSelect".
}
\item{verbose}{
   logical controlling how much information to display.
   Defaults to \code{FALSE}.
}
\item{drv}{
   an \code{PostgreSQLDriver} object as produced by \code{postgresqlInitDriver}.
}
\item{con}{
   an \code{PostgreSQLConnection} object as produced by \code{postgresqlNewConnection}
   and \code{postgresqlCloneConnection}.
}
\item{res}{
   an \code{PostgreSQLResult} object as produced by by \code{postgresqlExecStatement}
   and \code{postgresqlgetResult}.
}
\item{user}{
   a character string with the PostgreSQL's user name.
}
\item{password}{
   character string with the PostgreSQL's password.
}
\item{dbname}{ character string with the PostgreSQL database name.
}
\item{host}{
   character string with the name (or IP address) of the machine
   hosting the database. Default is \code{""}, which is
   interpreted as \code{localhost} by the PostgreSQL's API.
}
\item{port}{
   (optional) positive integer specifying the TCP port number that
   the PostgreSQL server is listening to.  Consult the PostgreSQL documentation
   for details.
}
\item{tty}{
Ignored (formerly, this specified where to send server debug output)
}
\item{options}{
Command-line options to be sent to the server
}
\item{force}{
   logical indicating whether to close a connection that has open
   result sets.  The default is \code{FALSE}.
}
\item{statement}{
   character string holding one (and only one) SQL statement.
}
\item{n}{
   number of rows to fetch from the given result set. A value of -1
   indicates to retrieve all the rows.  The default of 0 specifies
   to extract whatever the \code{fetch.default.rec} was specified
   during driver initialization \code{postgresqlInit}.
}
\item{name}{
   character vector of names (table names, fields, keywords).
}
\item{value}{
   a data.frame.
}
\item{field.types}{
   a list specifying the mapping from R/S-Plus fields in the data.frame
   \code{value} to SQL data types.  The default is
   \code{sapply(value,SQLDataType)}, see \code{PostgreSQLSQLType}.
}
\item{header}{
  logical, does the input file have a header line?  Default is the
  same heuristic used by \code{read.table}, i.e., TRUE if the first
  line has one fewer column that the second line.
}
\item{row.names}{
   a logical specifying whether to prepend the \code{value} data.frame 
   row names or not.  The default is \code{TRUE}.
}
\item{check.names}{
   a logical specifying whether to convert DBMS field names into
   legal S names. Default is \code{TRUE}.
}
\item{overwrite}{
   logical indicating whether to replace the table \code{name} with
   the contents of the data.frame \code{value}.
   The defauls is \code{FALSE}.
}
\item{append}{
   logical indicating whether to append \code{value} to the existing
   table \code{name}.
}
\item{nrows}{
  number of lines to rows to import using \code{read.table} from the
  input file to create the proper table definition. Default is 50.
}
\item{sep}{field separator character.}
\item{eol}{end-of-line separator.}
\item{skip}{number of lines to skip before reading data in the input file.}
\item{quote}{the quote character used in the input file (defaults to \code{\"}.}
\item{allow.keywords}{
   logical indicating whether column names that happen to be PostgreSQL
   keywords be used as column names in the resulting relation (table)
   being written.  Defaults to \code{FALSE}, forcing \code{postgresqlWriteTable}
   to modify column names to make them legal PostgreSQL identifiers.
   }
\item{preescapedstring}{character string to be escaped}
\item{identifiers}{one or more character strings to be used as identfier in SQL statement}
\item{filename}{character string indicating the file which contains the data to be copied to the PostgreSQL backend}
\item{\dots}{
   placeholder for future use.
}
}
\value{
   \code{postgresqlInitDriver} returns an \code{PostgreSQLDriver} object.

   \code{postgresqlDriverInfo} returns a list of name-value metadata pairs.

   \code{postgresqlDescribeDriver} returns \code{NULL} (displays the object's
   metadata).

   \code{postgresqlCloseDriver} returns a logical indicating whether the
   operation succeeded or not.

   \code{postgresqlNewConnection} returns an \code{PostgreSQLConnection} object.

   \code{postgresqlCloneConnection} returns an \code{PostgreSQLConnection} object.

   \code{postgresqlConnectionInfo}returns a list of name-value metadata pairs.

   \code{postgresqlDescribeConnection} returns \code{NULL} (displays the
   object's metadata).

   \code{postgresqlCloseConnection} returns a logical indicating whether the
   operation succeeded or not.

   \code{postgresqlExecStatement} returns an \code{PostgreSQLResult} object.

   \code{postgresqlFetch} returns a data.frame.

   \code{postgresqlQuickSQL} returns either a data.frame if the \code{statement} is
   a \code{select}-like or NULL otherwise.

   \code{postgresqlDescribeResult} returns \code{NULL} (displays the object's
   metadata).
   
   \code{postgresqlCloseResult} returns a logical indicating whether the
   operation succeeded or not.
   
   \code{postgresqlDescribeFields} returns a data.frame with one row per field
   with columns \code{name}, \code{Sclass}, \code{type}, \code{len}, 
   \code{precision}, \code{scale}, and \code{nullOK} which fully describe
   each field in a result set.  Except for \code{Sclass} (which shows the
   mapping of the field type into an R/S-Plus class) all the information
   pertains to PostgreSQL's data storage attributes.

   \code{postgresqlReadTable}  returns a data.frame with the contents of the
   DBMS table.
   
   \code{postgresqlWriteTable}  returns a logical indicating whether the
   operation succeeded or not.

   \code{postgresqlpqExec}  returns \code{NUL} (executes the statement
   but does not try to get result.  This is called internally
   from \code{postgresqlWriteTable} before \code{postgresqlCopyInDataframe}

   \code{postgresqlCopyIn}  returns \code{NULL} (copies the content 
   of the file through the socket connection to postgresql backend.
   This should be used just after COPY tablename FROM STDIN query. This is not used now.)

   \code{postgresqlCopyInDataframe}  returns \code{NULL} (copies the content 
   of the dataframe through the socket connection to postgresql backend. 
   Strings are encoded as UTF-8 for transfer.  The client_encoding should be set to UTF-8.
   This should be used just after COPY tablename FROM STDIN query.)

   \code{postgresqlgetResult}  returns an \code{PostgreSQLResult} object.  
   This is called after completion of execution of \code{postgresqlpqExec}.

   \code{postgresqlEscapeStrings} returns a character string which is escaped properly
   so that it can be surrounded with a single quote and used as literal in SQL.
   The escape procedure is dependent on the character encoding of the connection.

   \code{postgresqlQuoteId} returns a character string which is quoted as identifier. Returns vector on vector arguemnt.

   \code{postgresqlTableRef} returns a character string which is quoted as identifier. 
    Reterns a charcter string concatenated with "." so that "dbname"."schemaname"."tablename" reference
    is created upon c("dbname", "schemaname", "tablename") arguemnt.
   
   \code{postgresqlDataType} retuns a character string with the closest
   
   \code{postgresqlResultInfo} returns a list of name-value metadata pairs.

   \code{postgresqlTransactionStatement} returns a logical indicating whether the
   operation succeeded or not.
}
\section{Constants}{
\code{.PostgreSQLPkgName} (currently \code{"RPostgreSQL"}),
\code{.PostgreSQLPkgVersion} (the R package version),
\code{.PostgreSQLPkgRCS} (the RCS revision),
\code{.PostgreSQL.NA.string} (character that PostgreSQL uses to
denote \code{NULL} on input),
\code{.PostgreSQLSQLKeywords} (a lot!)
\code{.conflicts.OK}.
%\non_function{}
}
\keyword{datasets}
\keyword{interface}
\keyword{database}
%\keyword{internal}
% vim:syntax=tex
