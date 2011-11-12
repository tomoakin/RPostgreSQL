## the following variables are defined to be available,
## and to prevent abuse we don't expose anything else
files <- Sys.glob(paste("*", SHLIB_EXT, sep=""))
if (length(files)) {
    libarch <- if (nzchar(R_ARCH)) paste("libs", R_ARCH, sep="") else "libs"
    dest <- file.path(R_PACKAGE_DIR, libarch)
    message('installing to ', dest)
    dir.create(dest, recursive = TRUE, showWarnings = FALSE)
    file.copy(files, dest, overwrite = TRUE)
    if(WINDOWS){
        file.copy('libpq/libpq.dll', dest, overwrite = TRUE)
    }
    ## not clear if this is still necessary, but sh version did so
    if (!WINDOWS) Sys.chmod(file.path(dest, files), "755")
    if (nzchar(Sys.getenv("PKG_MAKE_DSYM")) && length(grep("^darwin", R.version$os))) {
        message('generating debug symbols (dSYM)')
        dylib <- Sys.glob(paste(dest, "/*", SHLIB_EXT, sep=""))
        if (length(dylib)) for (file in dylib) system(paste("dsymutil ", file, sep=""))
    }
}
