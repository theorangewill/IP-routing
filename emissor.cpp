#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/in.h>

using namespace std;
string server, port, source, destiny, message; 


void readInput(char* *argv)
{
  server = argv[1];
  port = argv[2];
  source = argv[3];
  destiny = argv[4];
  message = argv[5];
}

void sendMessage()
{
  int i, destinySize;
  int sock;
  struct sockaddr_in destinySock;
  long *p;
  struct timeval tv;


  //Inicializacao
  destinySock.sin_family = AF_INET;
  destinySock.sin_port = htons(port);
  if(!inet_aton(server,&(destinySock.sin_addr))){
    printf("INVALID SERVER %s\n", server);
    exit(1);
  }
  memset(destinySock.sin_zero,0x00,8);
  destinySize = sizeof(destinySock);


  //Criacao do socket
  sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
  if(sock == -1){
    perror("ERROR socket ");
    return;
  }
  tv.tv_sec = 5;
  tv.tv_usec= 0;
  setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(struct timeval*)&tv,sizeof(struct timeval));

  
}

int main(int argc, char* argv[])
{
  readInput(argv);

  cout << server << ' ' << port << ' ' << source <<' ' << destiny << ' '<< message << endl;

  return 0;
}