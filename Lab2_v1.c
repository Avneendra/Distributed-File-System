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

#define	QLEN		  32	/* maximum connection queue length	*/
#define	BUFSIZ		100
#define Max_clients 100
extern int	errno;
int		errexit(const char *format, ...);
int		passivesock(const char *portnum, int qlen);
int     dfs(int fd,int servernum);


/*------------------------------------------------------------------------
 * main - Concurrent TCP server for ECHO service
 *------------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
	char	*portnum,*server;	/* Standard server port number	*/
	struct sockaddr_in fsin;	/* the from address of a client	*/
	int	msock1,msock2,msock3,msock4;			/* master server socket		*/
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
	msock1 = passivesock(portnum, QLEN);
    if(!strcmp(server,"/DFS2"))
	msock2 = passivesock(portnum, QLEN);
    if(!strcmp(server,"/DFS3"))
	msock3 = passivesock(portnum, QLEN);
	if(!strcmp(server,"/DFS4"))
	msock4 = passivesock(portnum, QLEN);

	if((ssock1 = accept(msock1, (struct sockaddr *)&fsin, &alen))>0)
	{
		if (dfs(ssock1,1) < 0) 
	      (void) close(ssock1);
	}	
	
	if((ssock2 = accept(msock2, (struct sockaddr *)&fsin, &alen))>0)
	{
		if (dfs(ssock2,2) < 0) 
	      (void) close(ssock2);
	}	
	
	if((ssock3 = accept(msock3, (struct sockaddr *)&fsin, &alen))>0)
	{
		if (dfs(ssock3,3) < 0) 
	      (void) close(ssock3);
	}	
	
	if((ssock4 = accept(msock4, (struct sockaddr *)&fsin, &alen))>0)
	{
		if (dfs(ssock4,4) < 0) 
	      (void) close(ssock4);
	}	
	
	
}


int
dfs(int fd,int servernum)
{

	int nofbyte;
	char recfile[BUFSIZ],buf[BUFSIZ],line[Max_clients];
	char *userpass[2],*storeduserpass[2],*request[2] ,*cp,*cq,*cr;
	char root[100]="/home/avneendra/Desktop/netsys/Lab2/";
	// For Authentication..here buf will have the username password of the client..if it matches, then next requests will be processed 
	nofbyte=recv(fd, buf, sizeof buf,0);
	if (nofbyte<0)
	 exit(1);
	char *servername;
	if(servernum==1)
		servername="DFS1/";
	if(servernum==2)
		servername="DFS2/";
	if(servernum==3)
		servername="DFS3/";
	if(servernum==4)
		servername="DFS4/";
	
	cp = strdup (buf);  
	//userpass[0] will have username userpass[1] will have password
	userpass[0]= strtok(cp," ");
	userpass[1]= strtok(NULL," ");
	FILE *fp= fopen(" ");  // Path name of dfs.confg file
	if(fp==NULL)
	{
	 printf("Cant open dfs confg file \n");	
	 exit(1);
	}
	while(fgets(line,Max_clients,fp)!=NULL)
	{
		cq = strdup (line); 
		storeduserpass[0]= strtok(cq," ");
		storeduserpass[1]= strtok(NULL," ");
		if(!strcmp(storeduserpass[0],userpass[0]))
		{
			printf(" USERNAME MATCH \n");
			if(!strcmp(storeduserpass[1],userpass[1]))
			{
				printf("password MATCH Authentication successful\n");
				nofbyte=recv(fd, buf,sizeof buf,0); //For subsequent requests from clients
				cq = strdup (buf); 
				//request from client separated by " " 
				request[0]= strtok(cp," ");
				request[1]= strtok(NULL," ");
				if(!strcmp(request[0],"LIST"))
				{
					// LIST the files stored under the particular folder

				}
				if(!strcmp(request[0],"GET"))
				{
					//downloading all the files from all the servers

				}
				if(!strcmp(request[0],"PUT"))
				{
					//UPLOAD FILE TO SERVER..file is already divided
					char directory[200];
					printf("Server sends ok msg to client saying you can send the file\n");
					
					strcpy(directory,root);
					strcat(directory,servername);
					strcat(directory,request[1]);
					FILE fc=fopen(directory,"w");
					nofbyte=recv(fd,recfile, sizeof recfile,0);
					fprintf(fc,"%s",recfile);
				}





			}
			else
				printf(" password doesnot match \n");

		} 
		else
			printf("USERNAME DOESN'T MATCh \n");

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