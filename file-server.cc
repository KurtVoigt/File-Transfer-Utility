/* This code is an updated version of the sample code from "Computer Networks: A Systems
 * Approach," 5th Edition by Larry L. Peterson and Bruce S. Davis. Some code comes from
 * man pages, mostly getaddrinfo(3). */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<fstream>
#include<iostream>

#define MAX_LINE 256
#define MAX_PENDING 5
//some of the bits that aren't used for flags in the send command include 8, 16, and 32. We'll use those.
#define MSG_BADFILE 8 // the received filename doesn't exist



/*
 * Create, bind and passive open a socket on a local interface for the provided service.
 * Argument matches the second argument to getaddrinfo(3).
 *
 * Returns a passively opened socket or -1 on error. Caller is responsible for calling
 * accept and closing the socket.
 */
int bind_and_listen( const char *service );

int main( int argc, char *argv[] ) {
  char buf[MAX_LINE];
  int s, new_s, n;
  std::ifstream reader;
  /* Bind socket to local interface and passive open */
  if ( ( s = bind_and_listen( argv[1] ) ) < 0 ) {
    exit( 1 );
  }

  /* Wait for connection, then receive and print text */
  // 1. Wait for a client connection
  if ( ( new_s = accept( s, NULL, NULL ) ) < 0 ) {
    perror( "stream-talk-server: accept" );
    close( s );
    exit( 1 );}

  //else the conncetion was successful

  //get the file name from the client

  bzero(buf, MAX_LINE);
  n = recv(new_s, buf, MAX_LINE, 0); //recieves file name;


  if(n<0) {std::cerr <<"Error on receive 1 \n";}
  //create filename and attempt to open file 
  std::string filetry = "";
  for(int i = 0; buf[i] != '\0'; ++i){
    filetry = filetry+buf[i];
  }

  //attempt to open file and send error message if not
  bzero(buf, MAX_LINE);
  reader.open(filetry, std::ios::in | std::ios::binary); //change to filename
  if(!reader.is_open()){
    buf[0] = 'L'; buf[1] = 'a'; buf[2] = 'u'; buf[3] = 'g'; buf[4] = 'e'; buf[5] = 'w'; buf[6] = 'e'; buf[7] = 'c'; buf[8] = 'k'; buf[9] = 'l'; buf[10] = 'e';
    // n = send(new_s, buf, sizeof(buf), 0);
  }
  else{buf[0] = 'O'; buf[1] = 'K';} //send
  //else the file is open and ready to be sent
  n = send(new_s, buf, sizeof(buf), 0);
  
  while ( reader.peek() != EOF ) {
    reader.read(buf, MAX_LINE);
    n = send(new_s, buf, sizeof(buf) , 0);
    if(n<0){ std::cerr << "Error writing to socket\n";}
    memset(buf, '\0', MAX_LINE);
  }
  close( new_s ); // client bound socket
  close( s ); // file descriptor of bound socket
  return 0;

}
int bind_and_listen( const char *service ) {
  struct addrinfo hints;
  struct addrinfo *rp, *result;
  int s;

  /* Build address data structure */
  memset( &hints, 0, sizeof( struct addrinfo ) );
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_protocol = 0;

  /* Get local address info */
  if ( ( s = getaddrinfo( NULL, service, &hints, &result ) ) != 0 ) {
    fprintf( stderr, "stream-talk-server: getaddrinfo: %s\n", gai_strerror( s ) );
    return -1;
  }

  /* Iterate through the address list and try to perform passive open */
  for ( rp = result; rp != NULL; rp = rp->ai_next ) {
    if ( ( s = socket( rp->ai_family, rp->ai_socktype, rp->ai_protocol ) ) == -1 ) {
      continue;
    }

    if ( !bind( s, rp->ai_addr, rp->ai_addrlen ) ) {
      break;
    }

    close( s );
  }
  if ( rp == NULL ) {
    perror( "stream-talk-server: bind" );
    return -1;
  }
  if ( listen( s, MAX_PENDING ) == -1 ) {
    perror( "stream-talk-server: listen" );
    close( s );
    return -1;
  }
  freeaddrinfo( result );

  return s;
}
