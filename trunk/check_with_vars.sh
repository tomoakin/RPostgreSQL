#!/bin/bash

## either define the variables here (and get a diff to SVN)
##
## export POSTGRES_USER="someuser"
## export POSTGRES_PASSWD="..."
## export POSTGRES_HOST="somehost"
## export POSTGRES_DATABASE="testing124"
## export POSTGRES_PORT="5432"
##
## or write them to a local file that can get sourced here
##
if [ -f ~/.RPostgreSQL_Test_Vars ]
then
    . ~/.RPostgreSQL_Test_Vars
fi

#R CMD check RPostgreSQL

for f in RPostgreSQL/tests/*.R
do
    echo "==== Running $f"
    R --slave < $f
done

echo "Done"
exit 0
