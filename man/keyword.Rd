% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/GatingHierarchy_Methods.R
\name{keyword}
\alias{keyword}
\alias{keyword,GatingHierarchy,character-method}
\alias{keyword,GatingHierarchy,missing-method}
\alias{keyword,GatingSet,character-method}
\alias{keyword,GatingSet,missing-method}
\alias{keyword,GatingSetList,character-method}
\alias{keyword,GatingSetList,missing-method}
\title{Retrieve a specific keyword for a specific sample in a \code{GatingHierarchy} or or set of samples in a \code{GatingSet} or \code{GatingSetList}}
\usage{
\S4method{keyword}{GatingHierarchy,character}(object, keyword)

\S4method{keyword}{GatingHierarchy,missing}(object, keyword = "missing", ...)
}
\arguments{
\item{object}{\code{GatingHierarchy} or \code{GatingSet} or \code{GatingSetList}}

\item{keyword}{\code{character} specifying keyword name. When \code{missing}, extract all keywords.}

\item{...}{other arguments passed to \code{\link[flowCore]{keyword-methods}}}
}
\description{
Retrieve a specific keyword for a specific sample in a \code{GatingHierarchy} or or set of samples in a \code{GatingSet} or \code{GatingSetList}
}
\details{
See \code{keyword} in Package `flowCore'
}
\examples{
\dontrun{
    # get all the keywords from all samples
    keyword(G)
    # get all the keywords from one sample
    keyword(G[[1]])
    # filter the instrument setting
    keyword(G[[1]], compact = TRUE)
    # get single keyword from all samples
    keyword(G, "FILENAME")
    # get single keyword from one sample
    keyword(G[[1]], "FILENAME")
}
}
\seealso{
\code{\link[flowCore]{keyword-methods}}
}
