# IP-routing
This programs simulate an IPv4 routing.

## sender.cpp
The sender.cpp program is responsible for sending a message. Usage:
```
./sender <net> <port> <source> <destiny> <message>
```
Where:
    <net> is the communication server, use 127.0.0.1
    <port> is the communication port to send the message
    <source> is the sender IP
    <destiny> is the receiver IP
    <message> is the message to send, without spaces

## router.cpp
The router program is the simulation of a router. You should execute multiple instances of this program, simulating multiple routers. Usage:
```
./router <port> <table>
```
Where:
    <port> is the communication port the router is listening
    <table> is the routing table, which each line is separated by space
The table follows the syntax: IP/MASK/GATEWAY/INTERFACE

## example
Consider the following topology:

R1 ----- R2 ------ R3
         |
         |
         R4 ------ R5

R1: 1.2.1.0/255.255.255.0
R2: 1.2.2.0/255.255.255.0
R3: 1.2.3.0/255.255.255.0
R4: 1.2.4.0/255.255.255.0
R5: 1.2.5.0/255.255.255.0

For example, the routing table in R2 is:
1.2.1.0/255.255.255.0/1.2.1.0/12341
1.2.2.0/255.255.255.0/0.0.0.0/12342
1.2.3.0/255.255.255.0/1.2.3.0/12343
1.2.4.0/255.255.255.0/1.2.4.0/12344
1.2.5.0/255.255.255.0/1.2.4.0/12345

#### input
```
//R1
./router 12341 1.2.1.0/255.255.255.0/0.0.0.0/12341 0.0.0.0/0.0.0.0/1.2.2.0/12342

//R2
./router 12342 1.2.2.0/255.255.255.0/0.0.0.0/12341 1.2.1.0/255.255.255.0/1.2.1.0/12342 1.2.3.0/255.255.255.0/1.2.3.0/12343 1.2.4.0/255.255.255.0/1.2.4.0/12344 1.2.5.0/255.255.255.0/1.2.4.0/12344

//R3
./router 12343 1.2.3.0/255.255.255.0/1.2.3.0/12343 0.0.0.0/0.0.0.0/1.2.2.0/12342

//R4
./router 12344 1.2.4.0/255.255.255.0/0.0.0.0/12344 1.2.5.0/255.255.255.0/1.2.5.0/12345 0.0.0.0/0.0.0.0/1.2.2.0/12342

//R5
./router 12345 1.2.5.0/255.255.255.0/0.0.0.0/12345 0.0.0.0/0.0.0.0/1.2.4.0/12344

//Send 'hellor,world!' from R1 to R5
./sender 127.0.0.1 12341 1.2.1.0 1.2.5.0 'hello,world!'
```
#### output
```
//R1
forwarding packet for 1.2.5.0 to next hop 1.2.2.0 over interface 12342
//R2
forwarding packet for 1.2.5.0 to next hop 1.2.4.0 over interface 12344
//R4
forwarding packet for 1.2.5.0 to next hop 1.2.5.0 over interface 12345
//R5
destination reached. From 1.2.1.0 to 1.2.5.0 : hello,world!

```
