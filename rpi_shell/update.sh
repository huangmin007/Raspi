#!/bin/bash
#filename update


#update startup.sh bash
_R=$(ping github.com -c 1)
_K1="ttl="
_K2="time="
#if [[ $_R == *$_K1* -a $_R == *$_K2* ]]
if [[ $_R == *$_K1* ]]
then
    echo "Network Connection OK!"
	echo "Ready Update startup.sh file"
    git checkout startup.sh
    git pull origin master
else
    echo "Network Connection Error!"
fi

echo "ready startup ..."

#_SH=`echo $SHELL`
_SH=$(echo $SHELL)
echo $_SH
if [[ $_SH == "/bin/hash" ]]
then
	echo "sudo ..."
	sudo chmod 777 startup.sh
else
	echo "no ..."
	chmod 777 startup.sh
fi

sleep 1s

echo "startup ..."
#startup
sudo ./startup.sh

#exit
#exit 0
