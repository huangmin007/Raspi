#!/bin/bash
#filename RaspiTest

git pull origin master

echo "Hello"
_IP=$(hostname -I) || true
echo $_IP

