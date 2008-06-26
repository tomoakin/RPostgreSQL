% $Id: summary-methods.Rd,v 1.1 2003/12/02 14:53:01 dj Exp dj $
\name{summary-methods}
\docType{methods}
\alias{coerce-methods}
\alias{summary-methods}
\alias{format-methods}
\alias{show-methods}
\alias{coerce,dbObjectId,integer-method}
\alias{coerce,dbObjectId,numeric-method}
\alias{coerce,dbObjectId,character-method}
\alias{coerce,PostgreSQLObject,PostgreSQLDriver-method}
\alias{coerce,PostgreSQLConnection,PostgreSQLResult-method}
\alias{coerce,PostgreSQLConnection,PostgreSQLDriver-method}
\alias{coerce,PostgreSQLResult,PostgreSQLConnection-method}
\alias{coerce,PostgreSQLResult,PostgreSQLDriver-method}
\alias{format,dbObjectId-method}
\alias{print,dbObjectId-method}
\alias{show,dbObjectId-method}
\alias{summary,PostgreSQLObject-method}
\alias{summary,PostgreSQLDriver-method}
\alias{summary,PostgreSQLConnection-method}
\alias{summary,PostgreSQLResult-method}
\title{Summarize an PostgreSQL object}
\description{
   These methods are straight-forward implementations of the corresponding
   generic functions.
}
\section{Methods}{\describe{

\item{object = "DBIObject"}{
   Provides relevant metadata information on \code{object}, 
   for instance, the PostgreSQL server file, the
   SQL statement associated with a result set, etc.  
   }
\item{from}{object to be coerced}
\item{to}{coercion class}
\item{x}{object to \code{format} or \code{print} or \code{show}}
}
}
\keyword{methods}
\keyword{database}
\keyword{interface}
% vim: syntax=tex
