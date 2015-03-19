Since the introduction of Yosemite, Apple changed the default path that is used in GUI programs like R.app.  As a result of this change, the CRAN source install on Yosemite might fail to find pg\_config even if pg\_config works from the command line.

You can verify this by using the following R command:
```
Sys.getenv("PATH")
```

The default Yosemite install should show a path result like:
```
"/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin"
```

Depending on where you installed PostgreSQL this may not be enough.  The best way to fix this is to go to the Terminal and type the following:
```
pg_config
```
You should get something like the following:
```
MBP:~$ pg_config       
BINDIR = /opt/local/lib/postgresql94/bin
DOCDIR = /opt/local/share/doc/postgresql
HTMLDIR = /opt/local/share/doc/postgresql
....
```

The line that starts with BINDIR is the important line, but will probably be different than this example. Use the path after the equal sign (in this example "/opt/local/lib/postgresql94/bin") to replace the path in the following example R command:
```
Sys.setenv("PATH" = paste("/opt/local/lib/postgresql94/bin",Sys.getenv("PATH"),sep=":"))
```

After executing the above R command you should be able to use the GUI R Package Installer to install RPostGreSQL from CRAN source.