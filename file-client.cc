
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

#define MAX_LINE 256
/*
 * Lookup a host IP address and connect to it using service. Arguments match the first two
 * arguments to getaddrinfo(3).
 *
 * Returns a connected socket descriptor or -1 on error. Caller is responsible for closing
:q
 * the returned socket.
 */
int lookup_and_connect( const char *host, const char *service );

int main(int argc, char *argv[]) {
   int s, n; //s is socket, n is write/read functions
   const char *host;
   const char *port = argv[2];
   std::string fileTarget = argv[3];
   std::ofstream giver;
   giver.open(fileTarget, std::ios::out | std::ios::binary);
   if(!giver.is_open()) {std::cout << "didnt open";}
   int rBytes;
   if(argc == 4) host = argv[1];
   else{
      std::cerr << "not enough arguments\n"; exit(1);
   }
   /* Lookup IP and connect to server */
   if ( ( s = lookup_and_connect( host, port ) ) < 0 ) {
      std::cerr << "Client Error: Unable to find host " + fileTarget << '\n';
      exit( 1 );
   }
   
   // request
   std::string req = fileTarget;
   //char* request = (char*)"GET /~kkredo/reset_instructions.pdf HTTP/1.0\r\n\r\n";
   char request[req.size()];
   for(int i = 0; i < req.size(); ++i){
      request[i] = req.at(i);
   }
   //std::cout << request << std::endl;


   //buffer used to write file
   char receive[MAX_LINE];

   //sends request to server
    n = send(s, request, sizeof(request), 0);
    //recieves whether the file was found, errormess will be sent in case of FileNotFound
    bzero(receive, MAX_LINE);
    n = recv(s, receive, sizeof(receive), 0);
    if(receive[0] == 'O' && receive[1] == 'K'){
   /* for(int i = 0; i < 11; ++i){
        std::cout << receive[i] << '\n';
        std::cout << errorMess[i] << '\n';
        if(receive[i] != errorMess[i]) errorFree = false;
    }*/
    bzero(receive, MAX_LINE);}
    else{std::cerr << "Server Error: Unable to access file " + fileTarget <<'\n'; exit (1);}


   //read in file data sent by server and write it to file
   while(0 < (rBytes = recv(s, receive, sizeof(receive), 0))) // || byteTally < 87581)
   {  
      
     
      
      giver.write((char*)receive, MAX_LINE);
       
      //cout << receive << endl;
      bzero(receive, MAX_LINE);
   }



   close( s );
   giver.close();

   return 0;
}//main

int lookup_and_connect( const char *host, const char *service ) {
  /*struct addrinfo:
  preps the socket address structures for use. Used to 
  host name and service name lookups. First thing you call 
when making a connection. After you specify some fields, call
getaddrinfo(). 
  */
    struct addrinfo hints;
   struct addrinfo *rp, *result;
   int s;

   /* Translate host name into peer's IP address */
   /*ai_family = IPv4/6,AF_INET = 4
ai_socktype = Stream or datagram
ai_protocol, 0 for any


*/
   memset( &hints, 0, sizeof( hints ) );
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = 0;
   hints.ai_protocol = 0;

   if ( ( s = getaddrinfo( host, service, &hints, &result ) ) != 0 ) {
      fprintf( stderr, "stream-talk-client: getaddrinfo: %s\n", gai_strerror( s ) );
      return -1;
   }

/* Iterate through the address list and try to connect */
   for ( rp = result; rp != NULL; rp = rp->ai_next ) {
      if ( ( s = socket( rp->ai_family, rp->ai_socktype, rp->ai_protocol ) ) == -1 ) {
      continue;
      }

      if ( connect( s, rp->ai_addr, rp->ai_addrlen ) != -1 ) {
      break;
      }

      close( s );
   }
   if ( rp == NULL ) {
      perror( "stream-talk-client: connect" );
      return -1;
   }
   freeaddrinfo( result );

   return s;
}
