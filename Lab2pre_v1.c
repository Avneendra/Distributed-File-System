#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>

#define	QLEN 32	/* maximum connection queue length	*/

#define Max_clients 100
extern int	errno;
int		passivesock(const char *portnum, int qlen);
int		errexit(const char *format, ...);

/*------------------------------------------------------------------------
 * main - Concurrent TCP server for ECHO service
 *------------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
	char	*portnum,*server;	/* Standard server port number	*/
	struct sockaddr_in fsin;	/* the from address of a client	*/
	int	msock1,msock2,msock3,msock4,ssock1,ssock2,ssock3,ssock4;			/* master server socket		*/
    unsigned int	alen;		/* from-address length		*/
	
	
	switch (argc) 
	{
		case	1:
		break;
		case	3:
		server = argv[1];
		portnum = argv[2];
		break;
		default:
		errexit("usage: %s [port]\n",argv[0]);
	}
    //having 4 different file descriptors for 4 different servers
	if(!strcmp(server,"/DFS1"))
	{
		msock1 = passivesock(portnum, QLEN);
		printf("msock 1 %d\n",msock1);	
	}	
	
    if(!strcmp(server,"/DFS2"))
    {
    	msock2 = passivesock(portnum, QLEN);
    	printf("msock 2 %d\n",msock2);	
    }
	
    if(!strcmp(server,"/DFS3"))
    {
      msock3 = passivesock(portnum, QLEN);
      printf("%d\n",msock3);	
    }
	
	if(!strcmp(server,"/DFS4"))
	{
	  msock4 = passivesock(portnum, QLEN);
	  printf("%d\n",msock4);	
	}
   
    	
	if((ssock1 = accept(msock1, (struct sockaddr *)&fsin, &alen))>0)
	{
		while(1)
		{
			char	buf[50];
			int	cc;
			cc = read(ssock1, buf, sizeof buf);
			if (cc && write(ssock1, buf, cc) >= 0)
		    printf("Echo from server 1: %s \n",buf);
		    else
		    {
		    	close(ssock1);
		    	printf("Cant connect\n");
		    	break;
		    }
		  }  
	}	
	 
	if((ssock2 = accept(msock2, (struct sockaddr *)&fsin, &alen))>0)
	{
		char	buf[50];
			int	cc;
		while(1)
		{
			
			cc = read(ssock2, buf, sizeof buf);
			if (cc && write(ssock2, buf, cc) >= 0)
		    printf("Echo from server2: %s \n",buf);
		    else
		    {
		    	close(ssock2);
		    	printf("Cant connect to sock2\n");
		    	break;
		    }
        }
	}		

	
	 
}
int
passivesock(const char *portnum, int qlen)
/*
 * Arguments:
 *      portnum   - port number of the server
 *      qlen      - maximum server request queue length
 */
{
        struct sockaddr_in sin; /* an Internet endpoint address  */
        int     s;              /* socket descriptor             */

        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = INADDR_ANY;

    /* Map port number (char string) to port number (int) */
        if ((sin.sin_port=htons((unsigned short)atoi(portnum))) == 0)
              errexit("can't get \"%s\" port number\n", portnum);

    /* Allocate a socket */
        s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s < 0)
            errexit("can't create socket: %s\n", strerror(errno));

    /* Bind the socket */
        if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
           
                 errexit("can't bind socket: %s\n", strerror(errno));
            }
        

        if (listen(s, qlen) < 0)
            errexit("can't listen on %s port: %s\n", portnum, strerror(errno));
        return s;
}
/*------------------------------------------------------------------------
 * errexit - print an error message and exit
 *------------------------------------------------------------------------
 */
int
errexit(const char *format, ...)
{
        va_list args;

        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        exit(1);
}