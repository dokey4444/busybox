#!/bin/bash

./debug --arg_int32=99	# ok
./debug --arg_int32=100	# error

./debug --flagfile=example.conf # load from conf

