#!/bin/bash
#filename update

#update bash
_R=$(git pull origin master)

echo $_R

#startup
sudo ./startup.sh

#exit
exit 0
