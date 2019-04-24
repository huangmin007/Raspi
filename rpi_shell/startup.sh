#!/bin/bash
#filename RaspiTest

# waiting update.sh exited
sleep 1s  

#update update.sh bash
ping github.com -c 1 > /dev/null 2 > &1
if [ $? -eq 0 ]; then
    echo "Network Connection OK!"
    git checkout update.sh
    git pull origin master
else
    echo "Network Connection Error!"
fi

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
