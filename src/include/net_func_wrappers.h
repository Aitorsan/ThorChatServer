/*
*   This header contains all the sockets API functions wrapped. In this manner we avoid having to handle 
*   Errors from the main application making the code cleaner and easy to read.
*   
*
*/

#include <sys/socket.h>

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
    int error_code = socket(family, type, protocol);

   if( error_code < 0)
   	err_sys("socket error");

   return error_code;
}