/*
 *  Tiny RADIUS Client Library
 *  Copyright (C) 2022 David M. Syzdek <david@syzdek.net>.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of David M. Syzdek nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL DAVID M. SYZDEK BE LIABLE FOR
 *  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 */
#define _SRC_SOCKET_EXAMPLE_C 1


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <poll.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions

#undef PROGRAM_NAME
#define PROGRAM_NAME "socket-example"
#ifndef PACKAGE_BUGREPORT
#   define PACKAGE_BUGREPORT "unknown"
#endif
#ifndef PACKAGE_COPYRIGHT
#   define PACKAGE_COPYRIGHT "unknown"
#endif
#ifndef PACKAGE_NAME
#   define PACKAGE_NAME "Tiny RADIUS Client Library"
#endif
#ifndef PACKAGE_VERSION
#   define PACKAGE_VERSION "unknown"
#endif

#define MY_VERBOSE      0x0001U
#define MY_QUIET        0x0002U
#define MY_SERVER       0x0004U
#define MY_TCP          0x0008U
#define MY_UDP          0x0000U

#define MY_TIMEOUT      30 // in seconds


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#pragma mark - Variables

static int should_exit = 0;


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

int main( int argc, char * argv[] );

int my_client( int s, unsigned opts, struct sockaddr_storage * sap, socklen_t sa_len );
void my_connlog( unsigned opts, const struct sockaddr_storage * sa, const char * fmt, ...);
int my_error( const char * fmt, ...);
char * my_ntop(const struct sockaddr_storage * sa, char * buff, size_t len);
void my_print( unsigned opts, const char * fmt, ...);
int my_server( int s, unsigned opts );
int my_server_tcp( int s, unsigned opts );
int my_server_udp( int s, unsigned opts );
void my_signal( int sig );
void my_usage( void );
void my_verbose( unsigned opts, const char * fmt, ...);


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

int main(int argc, char * argv[])
{
   int                           c;
   int                           opt_index;
   int                           rc;
   int                           s;
   unsigned                      opt;
   unsigned                      opts;
   const char *                  laddr;
   const char *                  port;
   struct addrinfo               rinfo;
   struct addrinfo               hints;
   struct addrinfo *             res;
   struct sockaddr_storage       lsa;
   struct sockaddr_storage       sa;
   socklen_t                     lsa_len;
   socklen_t                     sa_len;
   char                          host[INET6_ADDRSTRLEN+INET_ADDRSTRLEN];

   // getopt options
   static char          short_opt[] = "46hl:p:qtVv";
   static struct option long_opt[] =
   {
      {"help",             no_argument,       NULL, 'h' },
      {"quiet",            no_argument,       NULL, 'q' },
      {"silent",           no_argument,       NULL, 'q' },
      {"tcp",              no_argument,       NULL, 't' },
      {"version",          no_argument,       NULL, 'V' },
      {"verbose",          no_argument,       NULL, 'v' },
      { NULL, 0, NULL, 0 }
   };

   bzero(&hints, sizeof(struct addrinfo));
   rinfo.ai_family      = PF_UNSPEC;
   rinfo.ai_protocol    = IPPROTO_UDP;
   rinfo.ai_socktype    = SOCK_DGRAM;
   rinfo.ai_flags       = AI_NUMERICSERV | AI_ADDRCONFIG;
   opts                 = 0;
   laddr                = NULL;
   port                 = "1980";

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:       /* no more arguments */
         case 0:        /* long options toggles */
         break;

         case '4':
         rinfo.ai_family = PF_INET;
         break;

         case '6':
         rinfo.ai_family = PF_INET6;
         break;

         case 'h':
         my_usage();
         return(0);

         case 'l':
         laddr = optarg;
         break;

         case 'p':
         port = optarg;
         break;

         case 'q':
         opts |= MY_QUIET;
         opts &= ~MY_VERBOSE;
         break;

         case 't':
         rinfo.ai_protocol  = IPPROTO_TCP;
         rinfo.ai_socktype  = SOCK_STREAM;
         opts              |= MY_TCP;
         break;

         case 'V':
         printf("%s (%s) %s\n", PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION);
         return(0);

         case 'v':
         opts |= MY_VERBOSE;
         opts &= ~MY_QUIET;
         break;

         case '?':
         fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
         return(1);

         default:
         fprintf(stderr, "%s: unrecognized option `--%c'\n", PROGRAM_NAME, c);
         fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
         return(1);
      };
   };

   if ((optind + 1) < argc)
   {
      fprintf(stderr, "%s: unknown argument `%s'\n", PROGRAM_NAME, argv[optind+1]);
      fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };

   if (optind == argc)
      opts |= MY_SERVER;

   // initializes signals
   my_verbose(opts, "configuring signals");
   signal(SIGINT,    my_signal);
   signal(SIGQUIT,   my_signal);
   signal(SIGTERM,   my_signal);
   signal(SIGHUP,    my_signal);
   signal(SIGVTALRM, SIG_IGN);
   signal(SIGPIPE,   SIG_IGN);
   signal(SIGALRM,   SIG_IGN);
   signal(SIGUSR1,   SIG_IGN);
   signal(SIGUSR2,   SIG_IGN);

   // resolve remote host
   bzero(&sa, sizeof(sa));
   sa_len = 0;
   if ((optind + 1) == argc)
   {
      my_verbose(opts, "resolving remote host \"%s\" port \"%s\" ...", argv[optind], port);
      res = NULL;
      memcpy(&hints, &rinfo, sizeof(struct addrinfo));
      if ((rc = getaddrinfo(argv[optind], port, &hints, &res)) != 0)
      {
         my_error("getaddrinfo(\"%s\", \"%s\"): %s", argv[optind], port, gai_strerror(rc));
         return(1);
      };
      port            = NULL;
      rinfo.ai_family = res->ai_family;
      bzero(&sa, sizeof(sa));
      memcpy(&sa, res->ai_addr, res->ai_addrlen);
      sa_len = res->ai_addrlen;
      freeaddrinfo(res);
      if (!(laddr))
         laddr = (sa.ss_family == AF_INET) ? "0.0.0.0" : "::";
   };

   // resolve local address
   my_verbose(opts, "resolving local address \"%s\" port \"%s\"...", (((laddr)) ? laddr : "any"), (((port)) ? port : "any"));
   memcpy(&hints, &rinfo, sizeof(struct addrinfo));
   res = NULL;
   hints.ai_flags |= AI_NUMERICHOST | AI_PASSIVE | AI_V4MAPPED | AI_ADDRCONFIG;
   if ((rc = getaddrinfo(laddr, port, &hints, &res)) != 0)
   {
      my_error("getaddrinfo(\"%s\", \"%s\", ...): %s\n", (((laddr)) ? laddr : "NULL"), (((port)) ? port : "NULL"), gai_strerror(rc));
      return(1);
   };
   bzero(&lsa, sizeof(lsa));
   memcpy(&lsa, res->ai_addr, res->ai_addrlen);
   lsa_len = res->ai_addrlen;
   freeaddrinfo(res);

   // create socket
   my_verbose(opts, "opening %s socket ...", (hints.ai_socktype == SOCK_STREAM) ? "TCP" : ((hints.ai_socktype == SOCK_DGRAM) ? "UDP" : "unknown"));
   if ((s = socket(lsa.ss_family, hints.ai_socktype, 0)) == -1)
   {
      my_error("socket(): %s\n", strerror(errno));
      return(1);
   };

   // configures socket
   my_verbose(opts, "configuring socket ...");
#ifdef SO_NOSIGPIPE
   opt = 1; setsockopt(s, SOL_SOCKET, SO_NOSIGPIPE, (void *)&opt, sizeof(int));
#endif
   opt = 1; setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(int));
   opt = 1; setsockopt(s, SOL_SOCKET, SO_REUSEPORT, (void *)&opt, sizeof(int));
   opt = 1;
   if (hints.ai_protocol == IPPROTO_TCP)
      setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (void *)&opt, sizeof(int));

   // bind to local address
   my_verbose(opts, "binding to \"%s\" ...", my_ntop(&lsa, host, sizeof(host)));
   if (bind(s, (struct sockaddr *)&lsa, lsa_len) == -1)
   {
      my_error("bind(): %s\n", strerror(errno));
      close(s);
      return(1);
   };

   return(((opts & MY_SERVER)) ? my_server(s, opts) : my_client(s, opts, &sa, sa_len));
}


int my_client( int s, unsigned opts, struct sockaddr_storage * sap, socklen_t sa_len )
{
   struct pollfd                 fds[2];
   struct sockaddr_storage       sa;
   char                          buff[4096];
   ssize_t                       len;

   fds[0].fd      = s;
   fds[0].events  = POLLIN | POLLPRI | POLLRDBAND;
   fds[0].revents = 0;
   fds[1].fd      = STDIN_FILENO;
   fds[1].events  = POLLIN | POLLPRI | POLLRDBAND;
   fds[1].revents = 0;

   // connects to remote host
   my_verbose(opts, "connecting to \"%s\" ...", my_ntop(sap, buff, sizeof(buff)));
   if (connect(s, (struct sockaddr *)sap, sa_len) == -1)
   {
      my_error("connect(): %s\n", strerror(errno));
      close(s);
      return(1);
   };

   if ((opts & MY_VERBOSE))
   {
      sa_len = sizeof(sa);
      getpeername(s, (struct sockaddr *)&sa, &sa_len);
      my_verbose(opts, "remote socket: %s", my_ntop(&sa, buff, sizeof(buff)));
      sa_len = sizeof(sa);
      getsockname(s, (struct sockaddr *)&sa, &sa_len);
      my_verbose(opts, "local socket:  %s", my_ntop(&sa, buff, sizeof(buff)));
   };

   my_verbose(opts, "starting client loop");
   while (!(should_exit))
   {
      // check for network data
      if (poll(fds, 2, 1000) == -1)
      {
         if (errno == EINTR)
            continue;
         my_error("poll(): %s", strerror(errno));
         my_verbose(opts, "closing socket ...");
         close(s);
         return(1);
      };

      // receives data
      if ((fds[0].revents & POLLIN))
      {
         fds[0].revents = 0;
         sa_len = sizeof(sa);
         if ((len = recvfrom(s, buff, sizeof(buff), 0, (struct sockaddr *)&sa, &sa_len)) == -1)
         {
            my_error("recvfrom(): %s", strerror(errno));
            close(s);
            return(1);
         };
         if (len == 0)
         {
            my_error("recvfrom(): zero bytes received");
            close(s);
            return(1);
         };
         buff[len] = '\0';
         my_connlog(opts, &sa, "received %i bytes", len);
         printf(">>> %s\n", buff);
      };

      if ((fds[0].revents & (POLLHUP | POLLERR | POLLNVAL)))
      {
         my_verbose(opts, "Connection closed by foreign host.");
         close(s);
         return(0);
      };

      // read STDIN
      if ((fds[1].revents & POLLIN))
      {
         fds[1].revents = 0;
         if ((len = read(STDIN_FILENO, buff, sizeof(buff))) == -1)
         {
            my_error("read(): %s", strerror(errno));
            close(s);
            return(1);
         };
         buff[len] = '\0';
         my_verbose(opts, "STDIN: read %i bytes", len);
         if (send(s, buff, strlen(buff), 0) == -1)
         {
            my_error("send(): %s", strerror(errno));
            close(s);
            return(1);
         };
      };
   };

   my_verbose(opts, "closing socket ...");
   close(s);

   return(0);
}


void my_connlog( unsigned opts, const struct sockaddr_storage * sa, const char * fmt, ...)
{
   va_list     args;
   char        host[INET6_ADDRSTRLEN+32];

   if ((opts & MY_QUIET))
      return;

   printf("%s: %s: ", PROGRAM_NAME, my_ntop(sa, host, sizeof(host)));
   va_start(args, fmt);
   vprintf(fmt, args);
   va_end(args);
   printf("\n");

   return;
}


int my_error( const char * fmt, ...)
{
   va_list args;
   fprintf(stderr, "%s: ", PROGRAM_NAME);
   va_start(args, fmt);
   vfprintf(stderr, fmt, args);
   va_end(args);
   fprintf(stderr, "\n");
   return(1);
}


char * my_ntop(const struct sockaddr_storage * sa, char * buff, size_t len)
{
   char  addrstr[INET6_ADDRSTRLEN];
   int   port;
   switch(sa->ss_family)
   {
      case AF_INET:
      inet_ntop(AF_INET, &((const struct sockaddr_in *)sa)->sin_addr, addrstr, sizeof(addrstr));
      port = ntohs(((const struct sockaddr_in *)sa)->sin_port);
      snprintf(buff, len, "%s:%i", addrstr, port);
      break;

      case AF_INET6:
      inet_ntop(AF_INET6, &((const struct sockaddr_in6 *)sa)->sin6_addr, addrstr, sizeof(addrstr));
      port = ntohs(((const struct sockaddr_in6 *)sa)->sin6_port);
      snprintf(buff, len, "[%s]:%i", addrstr, port);
      break;

      default:
      return(NULL);
   };
   buff[len-1] = '\0';
   return(buff);
}


void my_print( unsigned opts, const char * fmt, ...)
{
   va_list args;
   if ((opts & MY_QUIET))
      return;
   printf("%s: ", PROGRAM_NAME);
   va_start(args, fmt);
   vprintf(fmt, args);
   va_end(args);
   printf("\n");
   return;
}


int my_server( int s, unsigned opts )
{
   struct sockaddr_storage    sa;
   socklen_t                  sa_len;
   char                       buff[INET6_ADDRSTRLEN+INET6_ADDRSTRLEN];

   fcntl(s, F_SETFL, O_NONBLOCK);

   sa_len = sizeof(sa);
   getsockname(s, (struct sockaddr *)&sa, &sa_len);
   my_print(opts, "starting %s server on \"%s\"", (((opts & MY_TCP)) ? "TCP" : "UDP"), my_ntop(&sa, buff, sizeof(buff)));

   if ((opts & MY_TCP))
      return(my_server_tcp(s, opts));
   return(my_server_udp(s, opts));
}


int my_server_tcp( int s, unsigned opts )
{
   struct pollfd                 fds[10];
   nfds_t                        nfds;
   nfds_t                        pos;
   nfds_t                        x;
   int                           rc;
   struct sockaddr_storage       sa;
   struct sockaddr_storage       sa_list[10];
   socklen_t                     sa_len;
   int                           opt;
   socklen_t                     opt_len;
   char                          buff[4096];
   ssize_t                       len;

   bzero(sa_list, sizeof(sa_list));
   fds[0].fd      = s;
   fds[0].events  = POLLIN | POLLPRI | POLLRDBAND;
   fds[0].revents = 0;
   nfds           = 1;

   my_verbose(opts, "listening on socket");
   if (listen(s, 30) == -1)
   {
      my_error("listen(): %s", strerror(errno));
      close(s);
      return(1);
   };

   my_verbose(opts, "starting server loop");
   while ( (!(should_exit)) && ((nfds)) )
   {
      // check for network data
      if ((rc = poll(fds, nfds, (MY_TIMEOUT*1000))) == -1)
      {
         if (errno == EINTR)
            continue;
         my_error("poll(): %s", strerror(errno));
         my_verbose(opts, "closing sockets ...");
         for(pos = 0; (pos < nfds); pos++)
            close(fds[pos].fd);
         return(1);
      };
      if (!(rc))
         continue;

      // checks for new connections
      if ((fds[0].revents & POLLIN))
      {
         sa_len = sizeof(sa);
         if ((s = accept(fds[0].fd, (struct sockaddr *)&sa, &sa_len)) == -1)
         {
            my_error("accept(): %s", strerror(errno));
            my_verbose(opts, "closing sockets ...");
            for(pos = 0; (pos < nfds); pos++)
               close(fds[pos].fd);
            return(1);
         };
         my_connlog(opts, &sa, "new connection");
         if (nfds < 10)
         {
            memcpy(&sa_list[nfds], &sa, sizeof(struct sockaddr_storage));
            fds[nfds].fd      = s;
            fds[nfds].events  = fds[0].events;
            fds[nfds].revents = 0;
            nfds++;
         } else {
            my_connlog(opts, &sa, "connection limit exceeded, closing");
            close(s);
         };
      };

      // services client connections
      for(pos = 1; (pos < nfds); pos++)
      {
         // closes socket
         if ( ((fds[pos].revents & POLLERR)) || ((fds[pos].revents & POLLHUP))  )
         {
            opt_len = sizeof(opt);
            getsockopt(fds[pos].fd, SOL_SOCKET, SO_ERROR, &opt, &opt_len);
            my_connlog(opts, &sa_list[pos], "closing socket");
            close(fds[pos].fd);
            for(x = pos+1; (x < nfds); x++)
            {
               memcpy(&sa_list[x-1], &sa_list[x], sizeof(struct sockaddr_storage));
               fds[x-1].fd = fds[x].fd;
            };
            nfds--;
         };

         // reads socket
         if ((fds[pos].revents & POLLIN))
         {
            if ((len = recv(fds[pos].fd, buff, sizeof(buff), 0)) > 0)
            {
               my_connlog(opts, &sa_list[pos], "received %i bytes", len);
               snprintf(buff, sizeof(buff), "you sent %i bytes\n", (int)len);
               if ((len = send(fds[pos].fd, buff, strlen(buff), 0)) > 0)
                  my_connlog(opts, &sa_list[pos], "sent %i bytes", len);
            };
         };
      };
   };

   my_verbose(opts, "closing sockets ...");
   for(pos = 0; (pos < nfds); pos++)
      close(fds[pos].fd);

   return(0);
}


int my_server_udp( int s, unsigned opts )
{
   struct pollfd                 fds[1];
   int                           rc;
   struct sockaddr_storage       sa;
   socklen_t                     sa_len;
   char                          buff[4096];
   ssize_t                       len;

   fds[0].fd      = s;
   fds[0].events  = POLLIN | POLLPRI | POLLRDBAND;
   fds[0].revents = 0;

   my_verbose(opts, "starting server loop");
   while (!(should_exit))
   {
      // check for network data
      if ((rc = poll(fds, 1, (MY_TIMEOUT*1000))) == -1)
      {
         if (errno == EINTR)
            continue;
         my_error("poll(): %s", strerror(errno));
         my_verbose(opts, "closing socket ...");
         close(s);
         return(1);
      };
      if (!(rc))
         continue;

      // receives data
      if ((fds[0].revents & POLLIN))
      {
         sa_len = sizeof(sa);
         if ((len = recvfrom(s, buff, sizeof(buff), 0, (struct sockaddr *)&sa, &sa_len)) == -1)
         {
            my_error("recvfrom(): %s", strerror(errno));
            close(s);
            return(1);
         };
         my_connlog(opts, &sa, "received %i bytes", len);
         snprintf(buff, sizeof(buff), "you sent %i bytes\n", (int)len);
         if ((len = sendto(s, buff, strlen(buff), 0, (struct sockaddr *)&sa, sa_len)) == -1)
         {
            my_error("sendto(): %s", strerror(errno));
            close(s);
            return(1);
         };
         my_connlog(opts, &sa, "sent %i bytes", len);
      };
   };

   my_verbose(opts, "closing socket ...");
   close(s);

   return(0);
}


void my_signal( int sig )
{
   signal(sig, my_signal);
   should_exit = 1;
   my_verbose(MY_VERBOSE, "caught signal");
   return;
}


void my_usage(void)
{
   printf("Usage: %s [OPTIONS] url url ... url\n", PROGRAM_NAME);
   printf("Options:\n");
   printf("  -4                                        use IPv4 addresses\n");
   printf("  -6                                        use IPv6 addresses\n");
   printf("  -h, --help                                print this help and exit\n");
   printf("  -l address                                local address\n");
   printf("  -q, --quiet, --silent                     do not print messages\n");
   printf("  --tcp, -t                                 use TCP/IP\n");
   printf("  -V, --version                             print version number and exit\n");
   printf("  -v, --verbose                             print verbose messages\n");
   printf("\n");
   return;
}

void my_verbose( unsigned opts, const char * fmt, ...)
{
   va_list args;
   if (!(opts & MY_VERBOSE))
      return;
   printf("%s: ", PROGRAM_NAME);
   va_start(args, fmt);
   vprintf(fmt, args);
   va_end(args);
   printf("\n");
   return;
}

/* end of source */
