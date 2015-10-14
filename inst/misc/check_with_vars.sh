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

echo " "
echo "-------------- write version info ----------------------"

if [ -x /usr/bin/sw_vers ]
then
	sw_vers
elif [ -x /usr/bin/lsb_release ]
then
	lsb_release -a
fi
echo ""
svn_version=$(svnversion -n)
echo "RPostgreSQL svn version: $svn_version"
echo ""
psql --version | head -n 1
echo ""
Rscript -e 'sessionInfo(); capabilities()' | sed -n -e '1,2p'
echo ""
Rscript -e 'packageDescription("RPostgreSQL", fields = c("Package", "Version", "Packaged", "Built"))' | sed -n -e '1,4p'


#R CMD check RPostgreSQL

for f in exttests/*.R
do
    echo ""
    echo "==== Running $f"
R -d "valgrind --tool=memcheck --leak-check=full" --slave < $f
done
for f in RPostgreSQL/tests/*.R
do
    echo ""
    echo "==== Running $f"
if $skip_valgrind 
then R --slave < $f
else R -d "valgrind --tool=memcheck --leak-check=full" --slave < $f
fi
done

echo "Done"
exit 0
