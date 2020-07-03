/*
*   This header contains all the sockets API functions wrapped. In this manner we avoid having to handle 
*   Errors from the main application making the code cleaner and easy to read.
*   
*
*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>

void err_sys(const char* x) 
{ 
    perror(x); 
    exit(1); 
}

/*  
     AF_ : stands for "Address Family"
     PF_ : stands for "Protocol Family"

    -----------------------------------
    | familiy   |  description
    -----------------------------------
    | AF_INET   | IPv4 protocols
    | AF_INET6  | IPv6 protocols
    | AF_LOCAL  | Unix domain protocols
    | AF_ROUTE  | Routing sockets
    | AF_KEY    | Key socket
    -----------------------------------

    -------------------------------------------
    | type            |  description
    -------------------------------------------
    | SOCK_STREAM    | stream socket
    | SOCK_DGRAM     | datagram socket
    | SOCK_SEQPACKET | sequenced packet socket 
    | SOCK_RAW       | raw socket
    -------------------------------------------

    Note: Protocol of sockets for AF_INET or AF_INET6 
    ------------------------------------------
    | Protocol      |  description
    ------------------------------------------
    | IPPROTO_TCP   | TCP transport protocol
    | IPPROTO_UDP   | UDP transport protocol
    | IPPROTO_SCTP  | SCTP transport protocol 
    ------------------------------------------

     Combinations of familiy and types for the socket function:
     ----------------------------------------------------------

                    -----------------------------------------------------------------------
                    | AF_INET      | AF_INET6    | AF_LOCAL (AF_UNIX) | AF_ROUTE | AF_KEY
     --------------------------------------------------------------------------------------                
     SOCK_STREAM    |  TCP or SCTP | TCP or SCTP | Yes                |          |
     --------------------------------------------------------------------------------------
     SOCK_DGRAM     |  UDP         | UDP         | Yes                |          |
     --------------------------------------------------------------------------------------
     SOCK_SEQPACKET |  SCTP        | SCTP        | Yes                |          |
     --------------------------------------------------------------------------------------
     SOCK_RAW       |  IPv4        | IPv6        |                    | Yes      | Yes
     --------------------------------------------------------------------------------------
**/
int Socket( int family, int type, int protocol)
{
     int socket_fd = socket(family, type, protocol);
     
     if ( socket_fd < 0 )
          err_sys("socket error");

   return socket_fd;
}

/* The connect function is used by a TCP client to stablish a connection with a TCP server*/
void Connect( int socket_file_descriptor, const struct sockaddr* server_address, socklen_t address_length)
{
     int error_code = connect(socket_file_descriptor,server_address, address_length );

     if( error_code < 0 )
          err_sys("connection error");
}

/* 
The bind function assigns a local protocol address to a socket. with Internet protocols,
the protocol address is a combination of an IPv4/IPv6 address and a 16-bit TCP/UDP port number

Result when specifiying IP address and/or port number to bind:

   ---------------------------------------------------------------------------------
   Process specifies           | 
   ----------------------------|
   IP address        | port    |   Result
   ---------------------------------------------------------------------------------
   Wildcard          |  0      | Kernel chooses IP address and port
   ---------------------------------------------------------------------------------
   Wildcard          | nonzero | Kernel chooses IP address, process specifies port
   ---------------------------------------------------------------------------------
   Local IP address  | 0       | Process specifies IP address, kernel chooses port
   ---------------------------------------------------------------------------------
   Local Ip address  | nonzero | Process specifies IP address and port
   ---------------------------------------------------------------------------------

   Note: The wildcard address is specified by the constant INADDR_ANY
*/
void Bind(int sockfd, const struct sockaddr* myaddr, socklen_t addrlen)
{
     int error_code =  bind(sockfd, myaddr, addrlen);

     if( error_code < 0)
          err_sys("Bind socket error");
}

/* listen function is called only by a TCP server and it performs two actions.

   1. Converts an unconnected socket into a passive socket, indicating that the kernel
      should accept incoming connection requests directed to this socket. In tcp listen
      moves the socket from CLOSED state to LISTEN state.

   2. The second argument specifies the maximum number of connections the kernel should
      queue for this socket.

     This function is normally called afer both the socket and bind functions and must be called
     before calling the accpet function.

    To understand the backlog argument, we must realized that for a given listening socket, the 
    kernel mantains two queues:

    1. An incomplete connection queue, which contains an entry for each SYN that has arrived
       from a client for which the server is awaiting completion of the TCP three-way handshake.
       (SYN_RCVD state).

    2. A completed connection queue, which contains an entry for each client with  whom the TCP
       three-way handshake has completed. (ESTABLISHED state).

     the sum of the two queues cannot exceed the backlog parameter. Never put 0 as a backlog
     as different implementations interpret this differently.If you don't want clients connecting
     close the socket.( see page 107- Unix network programming - volume 1 W.Richard Stevens)
*/
void Listen(int sockfd, int backlog)
{
     const char* ptr = getenv("LISTENQ");

     if( ptr != nullptr)
         backlog = atoi(ptr);

     int error_code = listen(sockfd, backlog);
     
     if( error_code > 0)
          err_sys("Listening socket error");
}

/* The accept is called by a TCP server to return the next completed connection from the fron
   of the completed connection queue. If the completed connection queue is empty, the process
   is put to sleep ( assuming the default of a blocking socket).(page 109- Unix network programing)*/
int Accept( int sockfd, struct sockaddr* client_address, socklen_t* addrlen)
{
     int socket_fd = accept(sockfd, client_address,addrlen );

     if ( socket_fd < 0)
          err_sys("Accept funciton error");
     
     return socket_fd;
}