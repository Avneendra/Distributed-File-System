#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>

#define	QLEN		  32	/* maximum connection queue length	*/
#define	BUFSIZE       4096
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
	char	*portnum,*server;	       /* Standard server port number	*/
	struct sockaddr_in fsin;	      /* the from address of a client	*/
	int	msock1,msock2,msock3,msock4,ssock1,ssock2,ssock3,ssock4; /* master server socket		*/
    unsigned int	alen;		    /* from-address length		*/
	
	
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
    while(1)
    {	
		if((ssock1 = accept(msock1, (struct sockaddr *)&fsin, &alen))>0)
		{
			  if (dfs(ssock1,1) < 0) 
		      {
		         (void) close(ssock1);
		          break;	
		      }
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
}


int dfs(int fd,int servernum)
{
  while(1)	
  {	
  	char *root="/home/avneendra/Desktop/netsys/Lab2/";

	int nofbytes,nofbyte,matchfound=1;
	char recfile[BUFSIZ],buf[BUFSIZ],line[Max_clients],path[60],directory[200],temporary[150],t1[50],t2[100],t3[100],t4[100];
	int  come[1],cf;
	char *userpass[2]={NULL} ,*storeduserpass[2]={NULL},*request[2]={NULL},*cp=NULL,*cq=NULL,*cr=NULL;
	// For Authentication..here buf will have the username password of the client..if it matches, then next requests will be processed 
	char fullcommand[300],readtext[200],buffer[100],finish[1]="F";
   
   memset(recfile,0,BUFSIZ);
   memset(buf,0,BUFSIZ);
   memset(line,0,Max_clients);
   memset(path,0,60);
   memset(directory,0,200);
   memset(temporary,0,150);
   memset(t1,0,50);
   memset(t2,0,100);
   memset(t3,0,100);
   memset(t4,0,100);
   memset(fullcommand,0,300);
   memset(readtext,0,200);
   memset(buffer,0,100);
 
   nofbyte=recv(fd, buf, sizeof buf,0); //USErname and password
	if (nofbyte<0)
	    exit(1);
	
	char *servername=NULL;

	if(servernum==1)
		servername="DFS1/";
	if(servernum==2)
		servername="DFS2/";
	if(servernum==3)
		servername="DFS3/";
	if(servernum==4)
		servername="DFS4/";
	
	printf("buffer %s\n",buf);
	cp = strdup (buf);  
	//userpass[0] will have username userpass[1] will have password
	userpass[0]= strtok(cp," \t\n");
	userpass[1]= strtok(NULL," \t\n");
	printf("Username stored %s\n",userpass[0]);
	printf("Password stored %s\n",userpass[1]);
	
	strcpy(path,root);
	strcat(path,"dfs.confg");
	//FILE *fp= fopen(path,"rb");  // Path name of dfs.confg file
	FILE *fp= fopen("dfs.confg","rb");  // Path name of dfs.confg file
	if(fp==NULL)
	{
	  printf("Cant open dfs confg file \n");	
	  exit(1);
	}
	//opening dfs.confg file and matching username password of client with the stored one.
	while((fgets(line,Max_clients,fp)!=NULL) &&(matchfound))
	{
		cq = strdup (line); 
		storeduserpass[0]= strtok(cq," ");
		storeduserpass[1]= strtok(NULL," ");
		printf("Username %s\n",storeduserpass[0]);
		printf("Password %s\n",storeduserpass[1]);

		if(!strcmp(storeduserpass[0],userpass[0]))
		{
			printf(" USERNAME MATCH \n");
		

		  if(!strcmp(storeduserpass[1],userpass[1]))
		   {
			  int checks=0;
			  char m[]="O";
			  //nofbytes=send(fd,m,1,0);
			  
			  while(checks<2)
             { 
             	matchfound=0;
             	printf("checks %d\n",checks);
				printf("password MATCH Authentication successful\n");
				printf("enter request \n");
				
				 memset(buf,0,BUFSIZ);
				nofbyte=recv(fd,buf,BUFSIZ,0); //For subsequent requests from clients
				if(nofbyte>0)
					printf("No.of Bytes %d\n",nofbyte);
				else
					exit(1);

				printf("BUFFER %s\n",buf);
				
				cq = strdup (buf); 
				request[0]= strtok(cq," \n");
				if(strcmp("LIST",request[0]))
					request[1]= strtok(NULL," \n");
				
				printf("Request0 %s\n",request[0]);
				if(strcmp("LIST",request[0]))
					printf("Request1 %s\n",request[1]);
/******************************* LIST IMPLEMENTATION ****************************************************************/	
				if(!strcmp(request[0],"LIST"))
				{
				  // LIST the files stored under the particular folder
					checks=2;
					printf("In LIST\n");
					strcpy(directory,root);
					strcat(directory,servername);
					//programmticcally find out and all the files that exist in this folder and just their names to client
					strcat(directory,userpass[0]);
					printf("Directory %s\n",directory);
					
					strcpy(fullcommand,"cd ");
					strcat(fullcommand,directory);
					strcat(fullcommand," && ls > ");
					strcat(fullcommand,directory);
					strcat(fullcommand,"/server.txt");
					system(fullcommand);
				    
				    strcpy(readtext,directory);
				    strcat(readtext,"/server.txt");
				    printf("FULLPATH %s\n",readtext);
                    FILE *txtopen= fopen(readtext,"r");
				 
				    char msging[10];

				   while(fscanf(txtopen,"%s",buffer)!=EOF)
                    {
                    	printf("buffer %s\n",buffer);
                    	nofbyte=send(fd,buffer,100,0);  //write file descriptor of socket here       
                    }
                    fclose(txtopen);
                    nofbyte=send(fd,finish,1,0); 
				}
/*******************************  END OF LIST IMPLEMENTATION ****************************************************************/	
/*******************************  GET IMPLEMENTATION ****************************************************************/				
				else if(!strcmp(request[0],"GET"))
				{
					checks=2;
					char msg[4],buf[2048],a[1], k[1]={'A'},enc[1];
					char *pointtext;
					int bytes_read;
					memset(msg,0,4);
					memset(buf,0,2048);
					
					printf("In GET\n");
					//checking the file parts in the root directory and sending whatever parts are available to client
					strcpy(directory,root);
					strcat(directory,servername);
					strcat(directory,userpass[0]);
					strcat(directory,"/");
					strcat(directory,request[1]);
					strcpy(temporary,directory);
					
					strcat(temporary,".1");
					printf("Full path is %s\n",temporary);
					
					FILE *g1=fopen(temporary,"r");
					if(g1==NULL)
						printf("#1 doesnot exist\n");
					else
					{
						//else means you are able to open to the file which means it exists
						strcpy(t1,request[1]);
						strcat(t1,".1"); 
						

						/**************** SENDING FILENAME ***************/
						if((nofbytes=send(fd,t1,sizeof t1,0))<0) //Sending filename to client
						   printf("Error sending file \n");
						printf("Filename %s\n",t1);

						/***************** RECEIVING ACK FOR FILENAME ***********************/
						  nofbytes=recv(fd,msg,sizeof msg,0);    //receiving ACk from client 
						printf("Msg received for sending filename %s\n",msg);
						 
                        /************** SENDING ACTUAL FILE CONTENTS ********************/
						 	bytes_read=fread(buf,1,2048,g1);
						 	if(bytes_read>0)
						 	{	
							    if((nofbytes=send(fd,buf,2048,0))<0)
								    printf("Error sending file \n");
						    }
                            printf("text sent %s\n",buf);
                  
						/***************** RECEIVING ACK FROM CLIENT ****************/
						//if((nofbytes=recv(fd,msg,sizeof msg,0))<0) //approval from client after getting the file
						//	printf("Error receiving file \n");
						printf("Done\n");
						 
						 fclose(g1);
					}

					printf("temporary %s\n",temporary);
					memset(temporary,0,150);
					strcpy(temporary,directory);					
					strcat(temporary,".2");					
					printf("temporary %s\n",temporary);					

					FILE *g2=fopen(temporary,"r");
					
					if(g2==NULL)
						printf("#2 doesnot exist\n");
					
					else
					{
						strcpy(t2,request[1]);
						strcat(t2,".2"); //appended part number of file

						
					   /**************** SENDING FILENAME ***************/
						if((nofbytes=send(fd,t2,sizeof t2,0))<0)
						   printf("Error sending file \n");
						printf("Filename %s\n",t2);
					
					/***************** RECEIVING ACK FOR FILENAME ***********************/
						if((nofbytes=recv(fd,msg,sizeof msg,0))<0)
							printf("Error receiving file \n");
						printf("Msg received for sending filename %s\n",msg);
			
						
						//sending the actual file here
						 /************** SENDING ACTUAL FILE CONTENTS ********************/
 	 					bytes_read=fread(buf,1,2048,g2);
					 	if(bytes_read>0)
					 	{	
						    if((nofbytes=send(fd,buf,2048,0))<0)
							    printf("Error sending file \n");
					    }
                        printf("text sent %s\n",buf);
                            

		         		/***************** RECEIVING ACK FROM CLIENT ****************/
					    //if((nofbytes=recv(fd,msg,sizeof msg,0))<0) //approval from client after getting the file
						//printf("Error receiving file \n");
						printf("DONE\n");
					
						 
						 fclose(g2);
					}
					

					strcpy(temporary,directory);					
					strcat(temporary,".3");	
					FILE *g3=fopen(temporary,"r");
					
					if(g3==NULL)
						printf("#3 doesnot exist\n");
					else
					{
						strcpy(t3,request[1]);
						strcat(t3,".3"); //appended part number of file


					    /********************** SENDING FILENAME ***************/
						if((nofbytes=send(fd,t3,sizeof t3,0))<0)  //filename to client
						   printf("Error sending file \n");
						printf("Filename %s\n",t3);
						
						/***************** RECEIVING ACK FOR FILENAME ***********************/
						if((nofbytes=recv(fd,msg,sizeof msg,0))<0) //ok from client
							printf("Error receiving file \n");
						printf("Msg received for sending filename %s\n",msg);
						 
						
						/************** SENDING ACTUAL FILE CONTENTS ********************/
						 	 bytes_read=fread(buf,1,2048,g3);
						 	if(bytes_read>0)
						 	{	
							    if((nofbytes=send(fd,buf,2048,0))<0)
								    printf("Error sending file \n");
						    }
                            printf("text sent %s\n",buf);
					        
						/***************** RECEIVING ACK FROM CLIENT ****************/
						 //if((nofbytes=recv(fd,msg,sizeof msg,0))<0) //approval from client after getting the file
							// printf("Error receiving file \n");
						 printf("DONE\n");
				              
				          fclose(g3);

					}
						
				

					printf("In 4\n");
					strcpy(temporary,directory);					
					strcat(temporary,".4");	
					printf("path is %s\n",temporary);
					FILE *g4=fopen(temporary,"r");
					
					if(g4==NULL)
						printf("#4 doesnot exist\n");

					
					else
					{
						printf("File opened\n");
						strcpy(t4,request[1]);
						strcat(t4,".4"); //appended part number of file
					    printf("Filename sending %s\n",t4);

					    /********************** SENDING FILENAME ***************/
						if((nofbytes=send(fd,t4,sizeof t4,0))<0) //sending part number
						   printf("Error sending file \n");
					    printf("Filename %s\n",t4);
                        
					    /***************** RECEIVING ACK FOR FILENAME ***********************/
						if((nofbytes=recv(fd,msg,sizeof msg,0))<0)
							printf("Error receiving file \n");
						printf("Msg received for sending filename %s\n",msg);
						 
						
						/************** SENDING ACTUAL FILE CONTENTS ********************/
						 bytes_read=fread(buf,1,2048,g4);
					 	if(bytes_read>0)
					 	{	
						    if((nofbytes=send(fd,buf,2048,0))<0)
							    printf("Error sending file \n");
					    }
                        printf("text sent %s\n",buf);

						    
					    /***************** RECEIVING ACK FROM CLIENT ****************/
					   //  if((nofbytes=recv(fd,msg,sizeof msg,0))<0) //approval from client after getting the file
						//	printf("Error receiving file \n");
						printf("Done \n");

						 
						 fclose(g4);
					}
				}
/******************************* END OF  GET IMPLEMENTATION ****************************************************************/	
/*******************************  PUT IMPLEMENTATION ****************************************************************/				
				else if(!strcmp(request[0],"PUT"))
				{
						printf("In Put\n");
						checks++;
						//UPLOAD FILE TO SERVER..file is already divided
						char directory[200],msg[]="O";
						char decryp[1];
						memset(directory,0,200);
						
						strcpy(directory,root);
						strcat(directory,servername);
						strcat(directory,userpass[0]);

					/***MKDIR if it doesnt exist ***/

						struct stat st = {0};
						if (stat(directory, &st) == -1) 
						{
	      						mkdir(directory, 0700);
	    				}

						strcat(directory,"/");
						strcat(directory,request[1]);
					    
					    printf("Directory %s\n",directory);	
						FILE *fc=fopen(directory,"w");

						if(fc==NULL)
						printf("error creating a file\n");

						if((nofbytes=send(fd,msg,sizeof msg,0))<0)
						printf("Error sending OK \n");
					    int jj;
				   	
					
					    int count=0,len;
					    int l;
					    recv(fd, &l, sizeof(int), 0);  //receiving length of file
					    len= l;

					    printf("Length %d\n",len);

					    if((nofbytes=send(fd,msg,sizeof msg,0))<0)  //length  received
					    printf("Error sending OK \n");
						printf("incoming chars\n");
					    
					    while((nofbyte=recv(fd,come,1,0))>0)
					    {
					       char k[]={'a'};	
					       decryp[0]=come[0];

					       printf("%c",*come);
					       fwrite(decryp,1,1,fc);
					       count++;
					    
					       if(count==len)
					       {
					       	 //char nj[]={'\0'};
					       	 //fwrite(nj,1,1,fc);
					       	 break;
					       }
					       	
					    }
				
					
						fclose(fc);
						
						if((nofbytes=send(fd,msg,sizeof msg,0))<0)  //file received
						printf("Error sending OK \n");
					
				}
/******************************* END OF  PUT IMPLEMENTATION ****************************************************************/	
				else
				{				 	
				  printf("Bad request\n");
				}
			  }	
			  break;
			}
		
		  	
			else
				printf("password doesnot match \n");
			
		} 
		
		else
			 printf("USERNAME DOESN'T MATCH \n");
		
			
	}
	 fclose(fp);
    
    if(matchfound==1)
      {
      	char notmatch[1]="F";
	    nofbytes=send(fd,notmatch,1,0);
		printf("Incorrect username or password\n");
		  break;	 
      }
     printf("here2\n");
  }  
     return 1;
	
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
        

        if(listen(s,qlen) < 0)
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