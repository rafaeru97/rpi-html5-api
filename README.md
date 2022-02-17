What is this?
=============

Application written in C ++ with the Qt framework supporting HTML5 and communicating with each other via WebSocket.  
Includes a control system simulator containing a PI controller and an inertial object of the 1st order.  
In addition, a simple user interface allows to observe changes by using charts and graphical animations over time.  
  
![alt text](https://i.imgur.com/3RxJFBQ.png)
  
  
Compiling
=========

You need this JSON library: https://github.com/nlohmann/json  
Also you need Qt libraries:  
```sudo apt-get install qt5-default libqt5webview```  
  
To compile:
```qmake  
make```
