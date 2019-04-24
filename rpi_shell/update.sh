#!/bin/bash
#filename update

#update bash
git checkout startup.sh
git pull origin master

echo "RS"
echo "RE"

sudo chmod 777 startup.sh

echo "ready startup ..."
sleep 2s

#startup
sudo ./startup.sh

#exit
#exit 0
