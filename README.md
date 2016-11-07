[![Build Status](https://travis-ci.com/pedro-c/RCOM-FEUP.svg?token=EybUXufTwGCDe4ebsfmh&branch=llopen())](https://travis-ci.com/pedro-c/RCOM-FEUP)

##Data Connection Protocol

We used a script "port.sh" that can virtual emulate a connection through a serial port, you can execute that script by running:

```
sh port.sh
```

It's recomended to install ```socat``` to use this script.

##Compile and Run
Open three terminals.

1.On the first run the script:``` sh port.sh ```

2.On the second run the ``` make ``` command to compile the project and then ``` sudo ./app 0 ``` for the sender.

3.On the third run ``` sudo ./app 1 ``` for the receiver.

##Screenshot
![image](https://raw.githubusercontent.com/pedro-c/RCOM-FEUP/master/screenshot.png)
