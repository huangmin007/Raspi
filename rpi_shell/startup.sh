#!/bin/bash
#filename RaspiTest

sleep 1s

#update update.sh bash
git checkout update.sh
git pull origin master

sudo chmode 777 update.sh

#test echo
echo "Hello"

#test echo
echo "World"

#test
_IP=$(hostname -I) || true
echo $_IP

echo "test"
