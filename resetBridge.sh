#!/bin/bash

ip link set kvmbr0 down
brctl delbr kvmbr0
./bridge_setup.sh

