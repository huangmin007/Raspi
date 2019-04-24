#!/bin/bash
#filename update

#update bash
_R=$(git pull origin master)

echo "RS"
echo $_R
echo "RE"

#startup
sudo ./startup.sh

#exit
exit 0
