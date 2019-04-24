#!/bin/bash
#filename update

#update startup.sh bash
git checkout startup.sh
git pull origin master

echo "ready startup ..."
sudo chmod 777 startup.sh
sleep 1s

echo "startup ..."
#startup
sudo ./startup.sh

#exit
#exit 0
