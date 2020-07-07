/*
*   This header contains all the sockets API functions wrapped. In this manner we avoid having to handle 
*   Errors from the main application making the code cleaner and easy to read.
*   
*
*/
#include <iostream>
#include	<sys/types.h>	/* basic system data types */
#include	<sys/socket.h>	/* basic socket definitions */
#include	<sys/time.h>	/* timeval{} for select() */
#include	<time.h>		/* timespec{} for pselect() */
#include	<netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include	<arpa/inet.h>	/* inet(3) functions */
#include	<errno.h>
#include	<fcntl.h>		/* for nonblocking */
#include	<netdb.h>
#include	<signal.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<sys/stat.h>	/* for S_xxx file mode constants */
#include	<sys/uio.h>	/* for iovec{} and readv/writev */
#include	<unistd.h>
#include	<sys/wait.h>
#include  <cstdarg>
#include	<sys/un.h>	/* for Unix domain sockets */
namespace thor
{
     /* Miscellaneous constants */
     #define MAXLINE	  4096 /* max text line length */
     #define MAXSOCKADDR  128 /* max socket address structure size */
     #define BUFFSIZE	 8192 /* buffer size for reads and writes */
     #define SERVER_PORT 55555

     constexpr int std_input = 0;
     constexpr int std_output = 1;
     constexpr int std_error = 2;
     typedef void sig_handler (int);


     sig_handler* setSignalHandler(int signo, sig_handler* handler)
     {
          struct sigaction action, oldAction;
          
          // set new handler
          action.sa_handler = handler;
          
          /* Clear all signals from SET.  */
          sigemptyset(&action.sa_mask);
          action.sa_flags = 0;
          
          if (signo == SIGALRM)
          {
     #ifdef SA_INTERRUPT
               action.sa_flags |= SA_INTERRUPT; /* SunOs 4.x*/
     #endif
          }
          else
          {
     #ifdef SA_RESTART
               action.sa_flags |= SA_RESTART; /*SVR4,4.4BSD*/
     #endif   
          }

          /* Get old action and Set the action for signal SIG.  */
          if ( sigaction(SIGINT, &action, &oldAction) < 0 ) 
               return (SIG_ERR);
          
          return oldAction.sa_handler;
     }


     /* this is the handle  to handle child zombies it is important to use waitpid to 
     handle properly this signal because it could be more than one child licked
     because a signal could be generated before the signal handler is executed*/

     void sig_child(int signo)
     {
          pid_t pid; int status;
          // buffer for debuging
          //buffer for the message
          char buffer[MAXLINE]{};
          // the parameter -1, means match any process
          // the WNHANG flag tells to not block if the are running children that have not yet terminated 
          while( (pid = waitpid(-1,&status,WNOHANG )) > 0)
          {
               snprintf(buffer, sizeof(buffer),"child %d terminaed\n",pid);
               write(std_output,buffer,sizeof(buffer));
          }
     } 

     void err_quit(const char *fmt, ...)
     {
          va_list args;

          va_start(args, fmt);
          vfprintf(stderr, fmt, args);
          va_end(args);

          exit(EXIT_FAILURE);
     }

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
          std::cout << "listen error code:" << error_code <<std::endl;
          if( error_code < 0)
               err_sys("Listening socket error");
     }

     /* The accept is called by a TCP server to return the next completed connection from the fron
     of the completed connection queue. If the completed connection queue is empty, the process
     is put to sleep ( assuming the default of a blocking socket).(page 109- Unix network programing)*/
     int Accept( int sockfd, struct sockaddr* client_address, socklen_t* addrlen)
     {
          int socket_fd = accept(sockfd, client_address,addrlen );

          // if there is an error and the error is not an interrupt we fail
          // the interrup erro can be caused by a handler so we must chatch interrupted
          // system calls when we catch a signal
          if ( socket_fd < 0 && socket_fd != EINTR)
               err_sys("Accept function error");
          
          return socket_fd;
     }

     /* Close the socket */
     void Close(int socket_fd)
     {
          int err_code = close(socket_fd);
          
          if ( err_code < 0)
               err_sys("Close socket error");
     }

     /* Wrapper to return only the address family IPv4 or IPv6*/
     int get_socketfd_family(int socket_file_descriptor)
     {
          // this is the address structure with the larger size since we do not know in advance
          // which kind of sockaddr structure we are dealing with, we use sockaddr_storage
          struct sockaddr_storage ss;
          socklen_t len = sizeof(ss);
          
          //get the socket address family and return the address family if the file descriptor is open if not -1
          if( getsockname(socket_file_descriptor,(struct sockaddr*)&ss, &len)< 0 )
               return -1;

          return ss.ss_family;
     }


     /*Read n bytes from a descriptor*/
     ssize_t ReadNumBytes( int fd, void* voidPtrBuffer,size_t numberOfBytesToRead)
     {
          size_t bytesLeftToRead = numberOfBytesToRead;
          ssize_t numberOfBytesReaded = {};
          char* bufferPtr = (char*)voidPtrBuffer;

          while ( bytesLeftToRead > 0 )
          {
               numberOfBytesReaded = read(fd, bufferPtr, bytesLeftToRead);

               if( numberOfBytesReaded < 0 )
               {
                    if ( errno == EINTR )// chek if the read system call was interrupted
                         numberOfBytesReaded = 0;// then we start again
                    else //we have an error
                      return -1;
               }
               else if ( numberOfBytesReaded == 0 )
                    break; /*EOF*/
               
               bytesLeftToRead -= numberOfBytesReaded; // substract the bytes already readed
               bufferPtr += numberOfBytesReaded; //advance the pointer many bytes
          }
          return ( numberOfBytesToRead - bytesLeftToRead );
     }

}// end thor_serv namespace

using namespace thor;