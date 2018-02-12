#!/bin/bash

go run -gcflags "-N -l" src/main.go -log_dir=./log -v=3

