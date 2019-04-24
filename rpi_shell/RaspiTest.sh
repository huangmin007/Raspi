#!/bin/bash
#filename RaspiTest


echo "Hello"
_IP=$("hostname -I) || true
echo $_IP

