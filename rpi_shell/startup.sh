#!/bin/bash
#filename RaspiTest

#update update.sh bash
git checkout update.sh
git pull origin master

#test echo
echo "Hello"

#test echo
echo "World"

#test
_IP=$(hostname -I) || true
echo $_IP

echo "test"
