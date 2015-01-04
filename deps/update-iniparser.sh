#! /usr/bin/env sh
VER=$1
URL="http://ndevilla.free.fr/iniparser/iniparser-${VER}.tar.gz"
echo "Downloading $URL"
curl "$URL" > /tmp/iniparser.tar.gz
rm -rf iniparser
tar xvfz /tmp/iniparser.tar.gz
