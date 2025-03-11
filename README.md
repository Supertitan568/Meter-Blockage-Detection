# Senior Project Microwave Sensor Code 

This code is written for the Raspberry Pi. It uses the CQRobot Microwave Sensor to detect movement. It is written in C so it should be very easy to portto the Arduino for those who have that. The only difference is that you would have to switch out libevent for WebServer.h. They do use a similar callback function structure though so it should be fairly easy.

## Dependencies
The only dependencies are make, gcc, libevent, and WiringPi
Just run the commands in the next section

## Building on the Raspberry Pi

First Run these commands
```
$ sudo apt install build-essential libevent-dev wiringPi
$ make
```

You next need to find out the ip address of the Raspberry Pi. If you want to access the ui it would be localhost. Otherwise go to the command line andtype `$ ip a`. You should see its ip under the connected network interface 

Note: Make sure they are in the same lan

Then go into a browser and type in "http://<insert Raspberry Pi IP addr here>:8080/ui.html"

Congrats!!! It is now sensing.

