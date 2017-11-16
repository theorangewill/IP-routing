#include <iostream>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <netdb.h>

#define UDP 17;
using namespace std;
string server, port, source, destiny; 

struct ip_header {
  uint8_t version : 4;
  uint8_t IHL : 4;
  //uint8_t DSCP : 6;

  uint8_t precedence_DSCP : 3;
  uint8_t dDSCP : 1;

  uint8_t tDSCP : 1;
  uint8_t rDSCP : 1;
  uint8_t ECN : 2;
  unsigned short int totalLength;
  unsigned short int identification;
  uint8_t reservedFlag : 1;
  uint8_t dontFragmentFlag : 1;
  uint8_t moreFragments : 1;
  unsigned short int fragmentOffset : 13;
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
  port = argv[2];
  source = argv[3];
  destiny = argv[4];
  *message = argv[5];
}

void creatMessage(unsigned char *allMessage, string message)
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
  header.totalLength = sizeof(header) + message.size();
  header.identification = 1;
  header.reservedFlag = 0;
  header.dontFragmentFlag = 0;
  header.moreFragments = 0;
  header.fragmentOffset = 0;
  header.timeToLive = 3;
  header.protocol = UDP;
  header.headerChecksum = 0;
  header.sourceIPAddress = 111;
  header.destinationIPAddress = 1111;

  unsigned char *allMessage1;
  allMessage1 = (unsigned char *) malloc((sizeof(header) + message.size() + 1)*sizeof(unsigned char));
  allMessage1 = strcpy(allMessage1,"ooooo");
  //copy(allMessage, (unsigned char *) &header + message + '\0');
  //allMessage = (unsigned char *) &header;
  //allMessage[sizeof(header)] = (unsigned char) message.c_str();
  //allMessage[sizeof(header) + message.size()] = '\0';
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
  unsigned char *allMessage;

  creatMessage(allMessage, message);

  cout << allMessage <<endl;
  /*
  //Criacao do socket
  sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
  if(sock == -1){
    perror("ERROR socket ");
    exit(1);
  }

  //Inicializacao
  destinySock.sin_family = AF_INET;
  destinySock.sin_port = htons(port);
  if(!inet_aton(server,&(destinySock.sin_addr))){
    printf("INVALID SERVER %s\n", server);
    exit(1);
  }
  memset(destinySock.sin_zero,0x00,8);
  destinySockSize = sizeof(destinySock);

  if(bind(sock, (struct sockaddr*) &destinySock, destinySockSize) == -1){
    perror("ERROR bind ");
    exit(i);
  }
  
  if(sendto(s,message,sizeof(message),0,(struct sockaddr*)&destinySock, destinySockSize)==-1){
    perror("ERROR sendto ");
    exit(1);
  }
  */
}

int main(int argc, char* argv[])
{
  string message;
  readInput(argv,&message);
  //sendMessage(message);
  cout << server << ' ' << port << ' ' << source <<' ' << destiny << ' '<< message << endl;

  //struct sockaddr_in destinySock;
  //cout << inet_aton(server.c_str(),&(destinySock.sin_addr))<< endl;
  //unsigned int address = destinySock.sin_addr.s_addr;
  //cout << address <<endl;
  return 0;
}