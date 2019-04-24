#!/bin/bash
#filename RaspiTest

# waiting update.sh exited
sleep 1s  

#update update.sh bash
git checkout update.sh
git pull origin master

sudo chmod 777 update.sh

#Start Running bash
#...

#test echo
echo "Hello"

#test echo
echo "World"

#test
_IP=$(hostname -I) || true
echo $_IP

echo "test"
