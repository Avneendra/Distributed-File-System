
#include <sys/types.h>
#include <sys/socket.h>

#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <openssl/md5.h>
int connectsock(const char *host, const char *portnum);
int errexit(const char *format, ...);
#define LINELEN     128

/*------------------------------------------------------------------------
 * main - TCP client for ECHO service
 *------------------------------------------------------------------------
 */
char *text;

int
main(int argc, char *argv[])
{
 
    char    buf[LINELEN+1];     /* buffer for one line of text  */
    int s, n;           /* socket descriptor, read count*/
    int outchars, inchars;  /* characters sent and received */
    char  *host = "localhost";    /* host to use if none supplied */
    char    *portnum = "5004";  /* default server port number   */
     
     s = connectsock(host, portnum);
     
     while (fgets(buf, sizeof(buf), stdin)) {
        buf[LINELEN] = '\0';    /* insure line null-terminated  */
        outchars = strlen(buf);
        (void) write(s, buf, outchars);

        /* read it back */
       /* for (inchars = 0; inchars < outchars; inchars+=n ) {
            n = read(s, &buf[inchars], outchars - inchars);
            if (n < 0)
                errexit("socket read failed: %s\n",
                    strerror(errno)); */
        
        fputs(buf, stdout);
    }

}



int
connectsock(const char *host, const char *portnum)
/*
 * Arguments:
 *      host      - name of host to which connection is desired
 *      portnum   - server port number
 */
{
        struct hostent  *phe;   /* pointer to host information entry    */
        struct sockaddr_in sin; /* an Internet endpoint address         */
        int     s;              /* socket descriptor                    */


        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;

    /* Map port number (char string) to port number (int)*/
        if ((sin.sin_port=htons((unsigned short)atoi(portnum))) == 0)
                errexit("can't get \"%s\" port number\n", portnum);

    /* Map host name to IP address, allowing for dotted decimal */
        if ( phe = gethostbyname(host) )
                memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
        else if ( (sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE )
                errexit("can't get \"%s\" host entry\n", host);

    /* Allocate a socket */
        s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s < 0)
                errexit("can't create socket: %s\n", strerror(errno));

    /* Connect the socket */
        if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
                errexit("can't connect to %s.%s: %s\n", host, portnum,
                        strerror(errno));
        return s;
}

int
errexit(const char *format, ...)
{
        va_list args;

        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        exit(1);
}
