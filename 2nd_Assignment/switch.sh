#!/bin/bash

command_switch () {
	echo $1 > /dev/ttyS0
}

config_port () {
	command_switch "interface fastethernet 0/$1"
	command_switch "switchport mode access"
	command_switch "switchport access vlan $2"
}

if [ "$#" -lt 4 ] || [ "$#" -gt 5 ]
then
	echo "Invalid number of arguments."
	exit
fi

hostname=$(hostname | tr -d 'tux')
stand=$(echo $hostname | head -c 1)

#command_switch "enable"
#sleep 1
#command_switch "8nortel"

command_switch "configure terminal"

command_switch "vlan ${stand}0"
command_switch "exit"

command_switch "vlan ${stand}1"
command_switch "exit"

config_port "1" "${stand}0"
config_port "4" "${stand}0"
config_port "3" "${stand}1"
config_port "2" "${stand}1"
config_port "5" "${stand}1"

command_switch "end"
command_switch "show vlan brief"
