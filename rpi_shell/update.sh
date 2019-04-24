#!/bin/bash
#filename update

#update bash
_R=$(git checkout startup.sh) || true

echo "RS"
echo $_R
echo "RE"

sudo chmod 777 startup.sh

echo "ready startup ..."
sleep 2s

#startup
sudo ./startup.sh

#exit
#exit 0
