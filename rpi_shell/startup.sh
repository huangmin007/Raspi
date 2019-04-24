#!/bin/bash
#filename RaspiTest

# waiting update.sh exited
sleep 1s  

#update update.sh bash
_R=$(ping github.com -c 1)
_K1="ttl="
_K2="time="
if [[ $_R == *$_K1* ]]      #if [[ $_R == *$_K1* -a $_R == *$_K2* ]]
then
    echo "Network Connection OK!"
	echo "Ready Update update.sh file"
    git checkout startup.sh
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
