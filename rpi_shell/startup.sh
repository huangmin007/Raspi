#!/bin/bash
#filename RaspiTest

#update this bash
git pull origin master

#test echo
echo "Hello"

#test echo
echo "World"

#test
_IP=$(hostname -I) || true
echo $_IP

echo "testa"
