##Data Connection Protocol

We used a script "port.sh" that can virtual emulate a connection through a serial port, you can execute that script by running:

```
sh port.sh
```

It's recomended to install ```socat``` to use this script.


Open three terminals.

On the first run the script: sh port.sh
On the second run the ```make``` command to compile the project and then ```sudo ./app 0```
On the third run ```sudo ./app 1```
