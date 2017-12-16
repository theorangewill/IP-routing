#include <iostream>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <netdb.h>

#define UDP 17
using namespace std;
string server, source, destiny; 
unsigned int port;

struct ip_header {
  uint8_t IHL : 4;
  uint8_t version : 4;

  uint8_t ECN : 2;
  uint8_t rDSCP : 1;
  uint8_t tDSCP : 1;
  uint8_t dDSCP : 1;
  uint8_t precedence_DSCP : 3;

  unsigned short int totalLength;

  unsigned short int identification;

  uint8_t fragmentOffset1 : 5;
  uint8_t moreFragments : 1;
  uint8_t dontFragmentFlag : 1;
  uint8_t reservedFlag : 1;
  uint8_t fragmentOffset2;

  uint8_t timeToLive;

  uint8_t protocol;

  unsigned short int headerChecksum;

  unsigned int sourceIPAddress;

  unsigned int destinationIPAddress;

  //unsigned int options[4];
};

void readInput(char* *argv, string *message) 
{
  server = argv[1];
  port = atoi(argv[2]);
  source = argv[3];
  destiny = argv[4];
  *message = argv[5];
}

void creatMessage(string *allMessage, string message)
{
  ip_header header;
  if(message.size() >= 65535 - sizeof(header)){
    perror("ERROR large message "); 
    exit(1);
  }

  header.version = 4;
  header.IHL = 5;
  header.precedence_DSCP = 0;
  header.dDSCP = 0;
  header.tDSCP = 0;
  header.rDSCP = 0;
  header.ECN = 0;
  header.totalLength = htons(sizeof(header) + message.size());
  header.identification = htons(1);
  header.reservedFlag = 0;
  header.dontFragmentFlag = 1;
  header.moreFragments = 0;
  unsigned short int fragmentOffset = 0; 
  header.fragmentOffset1 = fragmentOffset >> 8;
  header.fragmentOffset2 = fragmentOffset;

  header.timeToLive = 3;

  header.protocol = UDP;

  header.headerChecksum = htons(200); //TODO 

  struct in_addr ipv4;
  inet_aton(source.c_str(),&ipv4);
  //cout << ipv4.s_addr << endl; 
  header.sourceIPAddress = htonl(ipv4.s_addr);

  inet_aton(destiny.c_str(),&ipv4);
  //cout << ipv4.s_addr << endl;
  header.destinationIPAddress = htonl(ipv4.s_addr);

 
  (*allMessage).assign((const char*) &header, sizeof(header));
  (*allMessage).append(message);

  //header.options[0] = 0;
  //header.options[1] = 0;
  //header.options[2] = 0;
  //header.options[3] = 0;
}

void sendMessage(string message)
{
  int i, destinySockSize;
  int sock;
  struct sockaddr_in destinySock;
  //unsigned char *allMessage;
  string allMessage;
  creatMessage(&allMessage, message);

  //cout << "000000    '" << allMessage <<"'  >" << allMessage.size() <<endl;
  
  //Criacao do socket
  sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
  if(sock == -1){
    perror("ERROR socket ");
    exit(1);
  }

  //Inicializacao
  destinySock.sin_family = AF_INET;
  destinySock.sin_port = htons(port);
  if(!inet_aton(server.c_str(),&(destinySock.sin_addr))){
    cout << "INVALID SERVER " << server << endl;
    exit(1);
  }
  memset(destinySock.sin_zero,0x00,8);
  destinySockSize = sizeof(destinySock);
/*
  if(bind(sock, (struct sockaddr*) &destinySock, destinySockSize)<0){
    perror("ERROR bind ");
    exit(1);
  }
  */
  if(sendto(sock,allMessage.c_str(),allMessage.size(),0,(struct sockaddr*)&destinySock, destinySockSize)<0){
    perror("ERROR sendto ");
    exit(1);
  }
  close(sock);
}

int main(int argc, char* argv[])
{
  string message;
  if(argc < 6){
    cout << "input: <net> <port> <source> <destiny> <message>" << endl;
    exit(1);
  }
  readInput(argv,&message);
  //while(1){
    sendMessage(message);
    //cout << "oi" << endl;
    //sleep(2);
  //}
  //cout << server << ' ' << port << ' ' << source <<' ' << destiny << ' '<< message << endl;
  return 0;
}