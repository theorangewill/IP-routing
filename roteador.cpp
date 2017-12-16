#include <iostream>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <netdb.h>
#include <bitset>

using namespace std;

#define UDP 17
#define SERVER "127.0.0.1"

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

struct table {
  unsigned int destiny;
  unsigned int mask;
  unsigned int gateway;
  unsigned int interface;
};

string source, destiny; 
unsigned int port;

unsigned int CIDRtoIP(string temp)
{
  int CIDR = atoi(temp.c_str());
  int i;
  unsigned int number = 0;

  for(i=0; i < CIDR; i++){
    number = number + 1 << 1;
  }
  
  number = number << 31 - i;

  return number;
}

void readInput(int argc, char* *argv, table route[]) 
{
  port = atoi(argv[1]);
  string line, temp;
  int i, bar;

  struct in_addr ipv4;

  for(i=0; i < argc-2; i++){
    line = argv[i+2];

    temp = line.substr(0,line.find('/'));
    inet_aton(temp.c_str(),&ipv4);
    route[i].destiny = htonl(ipv4.s_addr);
    bar = line.find('/');

    temp = line.substr(bar+1,line.find('/',bar+1)-bar-1);
    if(temp.find('.') == string::npos){
      route[i].mask = CIDRtoIP(temp);
    }
    else{
      inet_aton(temp.c_str(),&ipv4);
      route[i].mask = htonl(ipv4.s_addr);
    }
    bar = line.find('/',bar+1);

    temp = line.substr(bar+1,line.find('/',bar+1)-bar-1);
    inet_aton(temp.c_str(),&ipv4);
    route[i].gateway = htonl(ipv4.s_addr);
    bar = line.find('/',bar+1);

    temp = line.substr(bar+1,line.find('/',bar+1)-bar-1);
    route[i].interface = atoi(temp.c_str());

    //cout << bitset<32>(route[i].destiny) << ' ' << bitset<32>(route[i].mask) << ' ' << bitset<32>(route[i].gateway) << ' ' << route[i].interface << ' ' << endl;
  }
}

void readMessage(string *message, ip_header *header, unsigned char tempMessage[])
{
  unsigned char byte;

  byte = tempMessage[0];
  (*header).version = byte >> 4;
  (*header).IHL = byte;

  byte = tempMessage[1];
  (*header).precedence_DSCP = byte >> 5;
  (*header).dDSCP = byte >> 4;
  (*header).tDSCP = byte >> 3;
  (*header).rDSCP = byte >> 2;
  (*header).ECN = byte;

 
  unsigned short int *size;

  size = (unsigned short int*) &(tempMessage)[2];
  (*header).totalLength = ntohs(*size);

  size = (unsigned short int*) &(tempMessage)[4];
  (*header).identification = ntohs(*size);

  byte = tempMessage[6];
  (*header).reservedFlag = byte >> 7;
  (*header).dontFragmentFlag = byte >> 6;
  (*header).moreFragments = byte >> 5;
  (*header).fragmentOffset1 = byte;

  byte = tempMessage[7];
  (*header).fragmentOffset2 = byte;


  byte = tempMessage[8];
  (*header).timeToLive = byte;

  byte = tempMessage[9];
  (*header).protocol = byte;


  size = (unsigned short int*) &(tempMessage)[10];
  (*header).headerChecksum = ntohs(*size); //TODO 

  unsigned int *ip;

  ip = (unsigned int*) &(tempMessage)[12];
  (*header).sourceIPAddress = *ip;

  ip = (unsigned int*) &(tempMessage)[16];
  (*header).destinationIPAddress = *ip;

  int i = sizeof((*header));
  stringstream ss;
  ss << &(tempMessage)[i];
  ss >> *message;
}

void receiveMessage(string *message, ip_header *header)
{
  int i, destinySockSize;
  int sock;
  struct sockaddr_in destinySock;
  unsigned char tempMessage[1048576];

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
  if(!inet_aton(SERVER,&(destinySock.sin_addr))){
    cout << "INVALID SERVER " << SERVER << endl;
    exit(1);
  }
  memset(destinySock.sin_zero,0x00,8);
  destinySockSize = sizeof(destinySock);


  if(bind(sock, (struct sockaddr*) &destinySock, destinySockSize)<0){
    perror("ERROR bind ");
    exit(1);
  }
  /*
  if(sendto(sock,allMessage.c_str(),allMessage.size(),0,(struct sockaddr*)&destinySock, destinySockSize)<0){
    perror("ERROR sendto ");
    exit(1);
  }*/
  
  if(recvfrom(sock,(char*)(tempMessage),sizeof(tempMessage),0,(struct sockaddr*)&destinySock,(socklen_t*)&destinySockSize)<0){
    perror("ERROR recvfrom ");
    exit(1);
  }
  close(sock);

  readMessage(message,header,tempMessage);
}

int toRoute(table route[], int size, unsigned int ipAddress)
{
  int i, h = 0, index = -1, defaultRoute = -1;
  for(i = 0; i< size; i++){
    if((route[i].destiny & route[i].mask) == (ipAddress & route[i].mask)){
      if(h < route[i].mask){
        index = i;
        h = route[i].mask;
      }
    }
    if(route[i].destiny == 0 && route[i].mask == 0){
      defaultRoute = i;
    }
  }
  return (index != -1)? index : defaultRoute;
}

void sendMessage(string message, ip_header header, table route)
{
  int i, destinySockSize;
  int sock;
  struct sockaddr_in destinySock;
  //unsigned char *allMessage;
  string allMessage;
  header.timeToLive = header.timeToLive - 1;
  allMessage.assign((const char*) &header, sizeof(header));
  allMessage.append(message);
  
  //Criacao do socket
  sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
  if(sock == -1){
    perror("ERROR socket ");
    exit(1);
  }

  //Inicializacao
  destinySock.sin_family = AF_INET;
  destinySock.sin_port = htons(route.interface);
  if(!inet_aton(SERVER,&(destinySock.sin_addr))){
    cout << "INVALID SERVER " << SERVER << endl;
    exit(1);
  }
  memset(destinySock.sin_zero,0x00,8);
  destinySockSize = sizeof(destinySock);

  if(sendto(sock,allMessage.c_str(),allMessage.size(),0,(struct sockaddr*)&destinySock, destinySockSize)<0){
    perror("ERROR sendto ");
    exit(1);
  }
  close(sock);
}

int main(int argc, char* argv[])
{
  if(argc < 3){
    cout << "input: <port> <table>" << endl;
    exit(1);
  }
  table route[argc-2];
  string message;
  int index;
  ip_header header;
  struct in_addr ipDestiny, ipSource;

  readInput(argc,argv,route);
  while(1){
    receiveMessage(&message,&header);
    ipDestiny.s_addr = htonl(header.destinationIPAddress);
    index = toRoute(route,argc-2,header.destinationIPAddress);
    if(index < 0){
      cout << "destination " << inet_ntoa(ipDestiny) << " not found in routing table, dropping packet " << endl;
    }
    else if(route[index].gateway == 0){
      //inet_aton((const char*)&(header.sourceIPAddress),&ipSource);
      ipSource.s_addr = htonl(header.sourceIPAddress);
      cout << "destination reached. From " << inet_ntoa(ipSource) <<" to ";
      cout << inet_ntoa(ipDestiny) <<" : " << message << endl;
    }
    else if(header.timeToLive == 0){
      cout << "dropping packet, time to live has expired " << endl;
    }
    else{
      //inet_aton((const char*)&(route[index].gateway),&ipSource);
      ipSource.s_addr = htonl(route[index].gateway);
      cout << "forwarding packet for " << inet_ntoa(ipDestiny) << " to next hop ";
      cout << inet_ntoa(ipSource) << " over interface " << route[index].interface << endl;
      sendMessage(message,header,route[index]);
    }
  }
  return 0;
}