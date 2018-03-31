#!/usr/bin/env bash
#tempfi=`mktemp`
#cloc src/*.cpp src/*.hpp > $tempfi
#cat $tempfi | tail -n 7 | head -n 1
#lastlin=cat $tempfi | tail -n 2 | head -n 1
set -o nounset
fullq="$(cloc src/*.cpp src/*.hpp)"
echo "$fullq" | tail -n 7 | head -n 1
lastlin="`(echo "$fullq") | tail -n 2 | head -n 1`"
echo "$lastlin"
comment=`(echo $lastlin) | cut -d' ' -f4`
code=$((echo $lastlin) | cut -d' ' -f5)
echo -en "\t\t\t\t\t\t\t\t\t   "
expr \( $comment + $code \)
