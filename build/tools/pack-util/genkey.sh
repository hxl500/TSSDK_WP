#!/bin/bash
openssl ecparam -genkey -name SM2 -out priv.key
openssl ec -in priv.key -pubout -out pub.key
