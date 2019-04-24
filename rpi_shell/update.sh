#!/bin/bash
#filename update

#update bash
_R=$(git pull origin master)

echo "RS"
echo $_R
echo "RE"

sleep 2s

#startup
sudo ./startup.sh

#exit
#exit 0
