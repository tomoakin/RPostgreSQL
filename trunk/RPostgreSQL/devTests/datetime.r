#!/usr/bin/env r

## assign a basic time type
now <- Sys.time()

## print invokes a conversion to char for 'display' even though the type is really POSIXct
print(now)
print(class(now))

## we can also convert to char() implicitly
print(format(now))
print(class(format(now)))

## but what is important is that 'now' is still a time type
## that we can 'compute'
print(now)
print(now + 60)  ## one minute later
twomin <- now + 120
print(as.numeric(now))

## and the time is even stored at millisecond granularity!!
options("digits.secs"=7)		## need to tell R we want sub-second display up to 7 digits
print(now)				## and now we do
print(as.numeric(now), digits=16)	## same milli/microseconds here


## you can also go the other way and parse a datetime object from a char vector
then <- strptime("2008-07-01 14:15:16", "%Y-%m-%d %H:%M:%S")
print(then)
print(class(then))
## and we can convert this from its default 'POSIXlt' ('long type) representation to 'POSIXct' ('compact type')
then <- as.POSIXct(then)
print(then)
print(class(then))


