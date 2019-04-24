#!/bin/bash
#filename update

#update bash
_R=$(git pull origin master) || true

echo "RS"
echo $_R
echo "RE"

echo "ready startup ..."
sleep 2s

#startup
sudo ./startup.sh

#exit
#exit 0
