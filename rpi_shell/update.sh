#!/bin/bash
#filename update

#update startup.sh bash
ping github.com -c 1 > /dev/null 2 > &1
if [ $? -eq 0 ]; then
    echo "Network Connection OK!"
    git checkout startup.sh
    git pull origin master
else
    echo "Network Connection Error!"
fi

echo "ready startup ..."
sudo chmod 777 startup.sh
sleep 1s

echo "startup ..."
#startup
sudo ./startup.sh

#exit
#exit 0
