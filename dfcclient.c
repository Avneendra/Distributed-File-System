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
void uploading(char *name);
void list();
void downloading(char *name,int s0,int s1, int s2, int s3);
void writetoclient(FILE *fclient,FILE *fpart);
char* protocol(int s,int m);
int errexit(const char *format, ...);
int hextodec(int c);
int connectsock(const char *host, const char *portnum);
void sendtoserver(FILE *fpart,char *part,int server,int s,int size);
int sending(int fd,char *part,int size);
/*------------------------------------------------------------------------
 * main - TCP client for ECHO service
 *------------------------------------------------------------------------
 */
char *text=NULL;
char *root="/home/avneendra/Desktop/netsys/Lab2/DFC/";

int s[4];
int fg=0;
int
main(int argc, char *argv[])
{
	int i,option=0;
	char path[200],line[100],filename[50],command[50],confg[9];
	char *cp=NULL,*cq=NULL,*reading[4]={NULL},*dfs1addr=NULL,*dfs2addr=NULL,*dfs3addr=NULL,*dfs4addr=NULL,username[20],password[20],*words[2]={NULL},*cc=NULL,userpass[60];
  char pn1[5],pn2[5],pn3[5],pn4[5],auth[1];
  char  *host = "localhost";  /* host to use if none supplied */
	/********** INITIALIZING EVERY VARIABLE TO ZERO ******************/
  memset(line,0,100);
  memset(filename,0,50);
  memset(command,0,50);
  memset(username,0,20);
  memset(password,0,20);
  memset(userpass,0,60);
  memset(auth,0,1);
  memset(confg,0,9);
  
  strcpy(confg,"dfc.confg");
  strcpy(path,root);
  strcat(path,confg);
	
  //FILE *fp=fopen(path,"rb");
  FILE *fp=fopen("dfc.confg","rb");
	if(fp==NULL)
	printf("Cant open");
	//Reading DFC.confg file line by line 
	while(fgets(line,100,fp)!=NULL)
	{
    //Reading[0] will have first word of each line while Reading[1] will have second and so on
		cp = strdup (line); 
		reading[0]= strtok(cp," ");
		reading[1]= strtok(NULL," ");
    reading[2]= strtok(NULL," ");
    reading[3]= strtok(NULL," ");

		if(!strcmp(reading[0],"Server"))
		{
      
	     if(!strcmp(reading[1],"DFS1"))
	       strcpy(pn1,reading[3]);
		
	     if(!strcmp(reading[1],"DFS2"))
	       strcpy(pn2,reading[3]);
	
	     if(!strcmp(reading[1],"DFS3"))
	       strcpy(pn3,reading[3]);

	     if(!strcmp(reading[1],"DFS4"))
	      strcpy(pn4,reading[3]);
			 
		}
		
    if(!strcmp(reading[0],"Username"))
			strcpy(username,reading[1]);
   
		if(!strcmp(reading[0],"Password"))
    	strcpy(password,reading[1]);
      
	}
	fclose(fp);
  
  printf("Username %s",username);
  printf("Password %s",password);
  
  strcpy(userpass,username);
  strcat(userpass,password);

  if((s[0] = connectsock(host,pn1))<0)
    printf("Not connected to server 1\n");
  if((s[1] = connectsock(host,pn2))<0)
    printf("Not connected to server 2\n");
  if((s[2] = connectsock(host,pn3))<0)
    printf("Not connected to server 3\n");
  if((s[3] = connectsock(host,pn4))<0)
    printf("Not connected to server 4\n");

  while(1)
  { 
    //Sending username and password to each server to see if it it authorises or not
    for(i=0;i<4;i++)
    {
      int n;
      if(send(s[i],userpass,strlen(userpass), 0)<0)
       printf("sending failed\n");
       n=recv(s[i],auth,1,0);
      
      if(!strcmp(auth,"F"))  //If incorrect password or username given by user
      {
         printf("Incorrect username or password\n");
         exit(0);
      }

    }
    

	
		printf("Enter the command\n");
    fgets (command,50, stdin);
    printf("%s",command);

    cc= strdup(command);
    words[0] = strtok(cc," ");

    if(strcmp("LIST\n",words[0]))
		  words[1] = strtok(NULL,"\n");

    if(!strcmp("LIST\n",words[0]))
      option=1;
    
    if(!strcmp("GET",words[0]))
      option=2;

    if(!strcmp("PUT",words[0]))
      option=3;
    
    
    printf("Option chosen by user is %d\n",option);
    
    if(strcmp("LIST\n",words[0]))
      strcpy(filename,words[1]);
  
    printf("FILENAME %s\n",filename);
    switch(option)
		{
			//Send username password first to authenticate
			case 1: 
						  list();
						  break;
			case 2:
					   downloading(filename,s[0],s[1],s[2],s[3]);
					   break;
			case 3:	
					   uploading(filename);
					   break;
			default:
			printf("Invalid option\n");

		}

	}	
	exit(0);
}


void list()
{
   int nofbytes,i;
	
	 char message[10], buf[100],msg[]="o";
   memset(message,0,10);
   memset(buf,0,100);

	strcpy(message,"LIST");
	 
   for(i=0;i<4;i++)
	{
	    if(send(s[i], message, strlen(message), 0)<0)
		   printf("sending failed\n");
	
	    printf("Receiving  list of files \n");

      while(1)
      {
          nofbytes = recv(s[i],buf,100,0); // check while too 
          if(!strcmp(buf,"F"))
          {
            printf("\n");
            break;
          }
          printf("%s \n",buf);
          memset(buf,0,100);
      }
  }
	

}
void downloading(char *name,int s0,int s1, int s2, int s3)
/*	Server will send the filename with part number of the file
	Send ok to server
	Server will now send the actual file of the part number
	Since, server has 2 parts of the file do another recv to receive hname of the file with partnumber form server
	Send ok to server
	Server will now send the actual file of the part number
	Repeat this process 3 more times for 3 more servers.*/

{

	// //SEND it to each server DFS1,DFS2,DF3,DFS4 GET name  one at a time
	 char message[20],*p[8]={NULL},text1[512],text2[512],text3[512],text4[512],text5[512],text6[512],text7[512],text8[512];
	 int i=0,patch=0,j,m[4]={0,0,0,0};
   memset(message,0,20);
   memset(text1,0,512);
   memset(text2,0,512);
   memset(text3,0,512);
   memset(text4,0,512);
   memset(text5,0,512);
   memset(text6,0,512);
   memset(text7,0,512);
   memset(text8,0,512);


   s[0]=s0;
   s[1]=s1;
   s[2]=s2;
   s[3]=s3;

   strcpy(message,"GET ");
	 strcat(message,name);
   printf("IN GET \n");
   printf("request %s\n",message);
	 
   for(j=0;j<4;j++)
	 { 
      patch=0;
    	if(send(s[j],message,strlen(message),0)<0) //GET request
   		printf("sending failed\n");
      
      if(j==2 || j==3)
        patch=1;
      
      p[i] =  protocol(s[j],patch);
      
      if(p[i]==NULL)
      printf("ERROR\n");
	    
     printf("part  %s\n",p[i]);
     printf("Text received is %s\n",text);
     
     if(!strcmp(p[i],"1"))
        strcpy(text1,text);
     
     if(!strcmp(p[i],"2"))
        strcpy(text2,text);
     
     if(!strcmp(p[i],"3"))
        strcpy(text3,text);
     
     if(!strcmp(p[i],"4"))
        strcpy(text4,text);
     
     i++;
     patch=1;
     p[i]= protocol(s[j],patch);
     
     if(p[i]==NULL)
      printf("ERROR\n");
     printf("part  %s\n",p[i]);
     printf("Text received2 is %s\n",text);
     
     if(!strcmp(p[i],"1"))
        strcpy(text1,text);
     
    if(!strcmp(p[i],"2"))
        strcpy(text2,text);
     
     if(!strcmp(p[i],"3"))
        strcpy(text3,text);
     
     if(!strcmp(p[i],"4"))
        strcpy(text4,text);
     
     i++;
	 }

	  printf("Text1 %s\n",text1);
    printf("Text2 %s\n",text2);
    printf("Text3 %s\n",text3);
    printf("Text4 %s\n",text4);


   for(i=0;i<8;i++)
   {
     printf(".");
     if(p[i]!=NULL)
     {
      printf("*");
       m[i%4]=1;
     }
      
     
   }
    int check=0;

    for(i=0;i<4;i++)
    {
       if(m[i]==1)
        check=1;
       else
       {
        printf("$");
        check=0;
        break;
       }
    }
	  if(check==0)
      printf("Incomplete file\n");
    else
    {
          char path2[200],nj='\0';
          strcpy(path2,root);
          strcat(path2,"new.txt");
          printf("File received completely\n");
         
          FILE *fp= fopen("new.txt","w+");
          fprintf(fp,"%s",text1);
          fprintf(fp,"%s",text2);
          fprintf(fp,"%s",text3);
          fprintf(fp,"%s",text4);
          //fprintf(fp,"%c",nj);
          printf("File copying done\n");
          fclose(fp);
    }


}

char* protocol(int s,int m)
{
		char filename[50],reply2[2048],come[1],msg2[4]="Cack";
		char *part=NULL;
		int nofbytes=0,count=0,len=9;
    printf("Inside protocol function \n");
    memset(filename,0,50);
    memset(reply2,0,2048);
 		printf("Socket number %d\n",s);

    
    /********************** RECEIVING FILENAME FROM SERVER ***********************************/
    if((nofbytes=recv(s,filename,50,0))<0)//Filename has the appended filename and have to do strrchr to identify the part name
      printf("Error \n");
    printf("Filename received from server is  %s\n",filename);
    part=strrchr(filename,'.')+1	; //partnumber should go here
    printf("Part number found to be %s\n",part);
    
    /**************** SENDING ACK **********************************/
    if((nofbytes=send(s,msg2,4,0))==4)  //partnumber received so sending ok to server
        printf("ACk OK \n");
    printf("file name ack  %s\n",msg2);   
	   
       
    /************** RECEIVING FILE CONTENTS ****************************/
    printf("receiving file...\n");   
      nofbytes=recv(s,reply2,2048,0);
      printf("Received\n");

      /************** RECEIVING FILE CONTENTS (PATCH) ****************************/
      if(m==1)
      {
        printf("receiving file...\n");   
        nofbytes=recv(s,reply2,2048,0);
        printf("Received\n"); 

      }


    /******************** SENDING ACK ************************************/
    //if((nofbytes=send(s,msg2,4,0))==4)  //file received so sending ok to server
      //    printf("ACK sent \n");		
   
    
    printf("File contents are %s \n",reply2);   
    text=reply2;	
    printf("part received %s\n",part);
		return part;
}

void uploading(char *name)
{
  int sum=0,i,size,bytes;
  int nofbytes[]={0,0,0,0,0,0,0,0};
  char msg[10];
  unsigned char abc[16];
  char buffer,c,perm[60],temp1[60],temp2[60],temp3[60],temp4[60],part1[30],part2[30],part3[30],part4[30],data[1024],buf[1024];
  
  memset(abc,0,32);
  memset(msg,0,10);
  memset(perm,0,60);
  memset(temp1,0,60);
  memset(temp2,0,60);
  memset(temp3,0,60);
  memset(temp4,0,60);
  memset(part1,0,30);
  memset(part2,0,30);
  memset(part3,0,30);
  memset(part4,0,30);
  memset(data,0,1024);
  memset(buf,0,1024);
 
  strcpy(perm,root);
  strcat(perm,name);
 
  strcpy(part1,name);
  strcat(part1,".1");

  strcpy(part2,name);
  strcat(part2,".2");

  strcpy(part3,name);
  strcat(part3,".3");

  strcpy(part4,name);
  strcat(part4,".4");
  
  printf("Perm %s",perm);
  
  //FILE *fp=fopen(perm,"r");
  FILE *fp=fopen(name,"r");
  if(fp==NULL)
    printf("FILE NOT OPENING\n");
 
  
  MD5_CTX mdContext;
  MD5_Init (&mdContext);
  while ((bytes = fread (data, 1,1024,fp)) != 0)
  MD5_Update (&mdContext, data, bytes);
  MD5_Final (abc,&mdContext);

  int n;
  char *out = (char*)malloc(33);

   for (n = 0; n < 16; ++n) 
     snprintf(&(out[n*2]), 16*2, "%02x", (unsigned int)abc[n]);
   
   int decimal[32];
   for (i=0; i < 32; i++)
   {   
  	 decimal[i] = hextodec(out[i]);
	   sum = (sum*16 + decimal[i])%4;
   }

  int x=sum;
  printf("MD5sum is %d\n",x);
  fseek(fp, 0L, SEEK_SET);
  fseek(fp, 0L, SEEK_END);
  size = ftell(fp);
  fseek(fp, 0L, SEEK_SET);
  
  int divsize=size/4;
  //PART 1
  strcpy(temp1,perm);
  strcat(temp1,".1");
  //FILE *f=fopen(temp1,"w");
  FILE *f=fopen(part1,"w");
  for(i=0;i<size/4;i++)
  {
  	 c=fgetc(fp);
   	 fputc(c,f);	
  }  

  
  // PART 2
  strcpy(temp2,perm);
  strcat(temp2,".2");
  //FILE *f2=fopen(temp2,"w");
  FILE *f2=fopen(part2,"w");
  for(i=0;i<size/4;i++)
  {
  	c=fgetc(fp);
  	fputc(c,f2);
  }
 
  
  //PART 3
  strcpy(temp3,perm);
  strcat(temp3,".3");
  //FILE *f3=fopen(temp3,"w");
  FILE *f3=fopen(part3,"w");
  for(i=0;i<size/4;i++)
  {
  	c=fgetc(fp);
  	fputc(c,f3);
  }

  
  strcpy(temp4,perm);
  strcat(temp4,".4");
  //FILE *f4=fopen(temp4,"w");
  FILE *f4=fopen(part4,"w");
  for(i=0;i<size/4;i++)
  {
  	c=fgetc(fp);
  	fputc(c,f4);
  }
  
  fcloseall(); 

    FILE *fp1=fopen(part1,"r");
    FILE *fp2=fopen(part2,"r");
    FILE *fp3=fopen(part3,"r");
    FILE *fp4=fopen(part4,"r");


  switch(x)
  {
    case 0: // #1-(1,2), #2-(2,3), #3-(3,4), #4-(4,1)
    		
        //DFS1   	
    		sendtoserver(fp1,part1,1,s[0],divsize);  
        if((nofbytes[0]=recv(s[0],msg,sizeof msg,0))<0)
          printf("Error in message \n"); 	
    	  
        //DFS2   		
    		sendtoserver(fp2,part2,2,s[1],divsize);
        if((nofbytes[1]=recv(s[1],msg,sizeof msg,0))<0)
          printf("Error in message \n");      
      	
        //DFS3    	
    		sendtoserver(fp3,part3,3,s[2],divsize);
        if((nofbytes[2]=recv(s[2],msg,sizeof msg,0))<0)
          printf("Error in message \n");  
    		
    		//DFS4   	
        sendtoserver(fp4,part4,4,s[3],divsize); 
        if((nofbytes[7]=recv(s[3],msg,sizeof msg,0))<0)
          printf("Error in message \n");     		
    		
        //DFS1  
        sendtoserver(fp2,part2,1,s[0],divsize);
        if((nofbytes[0]=recv(s[0],msg,sizeof msg,0))<0)
          printf("Error in message \n");
        
        //DFS2
        sendtoserver(fp3,part3,2,s[1],divsize);
        if((nofbytes[1]=recv(s[1],msg,sizeof msg,0))<0)
          printf("Error in message \n");      
        
        //DFS3
        sendtoserver(fp4,part4,3,s[2],divsize);
        if((nofbytes[2]=recv(s[2],msg,sizeof msg,0))<0)
          printf("Error in message \n");  
        
        //DFS4
        sendtoserver(fp1,part1,4,s[3],divsize);
        if((nofbytes[7]=recv(s[3],msg,sizeof msg,0))<0)
          printf("Error in message \n");  
    		break;

    case 1: // #1-(4,1), #2-(1,2), #3-(2,3), #4-(3,4)
        //DFS1
    		sendtoserver(fp4,part4,1,s[0],divsize);	
        if((nofbytes[0]=recv(s[0],msg,sizeof msg,0))<0)
          printf("Error in message \n");  
         
    	   //DFS2
    		sendtoserver(fp1,part1,2,s[1],divsize); 
        if((nofbytes[1]=recv(s[1],msg,sizeof msg,0))<0)
          printf("Error in message \n");       
    		
    		//DFS3    		
    		sendtoserver(fp2,part2,3,s[2],divsize);   	
        if((nofbytes[2]=recv(s[2],msg,sizeof msg,0))<0)
          printf("Error in message \n");  

        //DFS4   
    		sendtoserver(fp3,part3,4,s[3],divsize);   
        if((nofbytes[7]=recv(s[3],msg,sizeof msg,0))<0)
          printf("Error in message \n");  


        //DFS1
        sendtoserver(fp1,part1,1,s[0],divsize);
        if((nofbytes[0]=recv(s[0],msg,sizeof msg,0))<0)
          printf("Error in message \n"); 

        //DFS2    
        sendtoserver(fp2,part2,2,s[1],divsize);
        if((nofbytes[1]=recv(s[1],msg,sizeof msg,0))<0)
          printf("Error in message \n"); 

        //DFS3  
        sendtoserver(fp3,part3,3,s[2],divsize);
        if((nofbytes[2]=recv(s[2],msg,sizeof msg,0))<0)
          printf("Error in message \n");  

        //DFS4   		
    		sendtoserver(fp4,part4,4,s[3],divsize);
        if((nofbytes[7]=recv(s[3],msg,sizeof msg,0))<0)
          printf("Error in message \n");  
    		break;

    case 2: // #2-(4,1), #3-(1,2), #4-(2,3), #1-(3,4)
    		//DFS1
        sendtoserver(fp3,part3,1,s[0],divsize); 
        if((nofbytes[0]=recv(s[0],msg,sizeof msg,0))<0)
          printf("Error in message \n");   		
    		
        //DFS2
    		sendtoserver(fp4,part4,2,s[1],divsize);	
        if((nofbytes[1]=recv(s[1],msg,sizeof msg,0))<0)
          printf("Error in message \n"); 
           		
        //DFS3
    		sendtoserver(fp1,part1,3,s[2],divsize);  
        if((nofbytes[2]=recv(s[2],msg,sizeof msg,0))<0)
          printf("Error in message \n"); 	
    	
        //DFS4
    		sendtoserver(fp2,part2,4,s[3],divsize);  
        if((nofbytes[7]=recv(s[3],msg,sizeof msg,0))<0)
          printf("Error in message \n"); 	
    		

        //DFS1
        sendtoserver(fp4,part4,1,s[0],divsize);
        if((nofbytes[0]=recv(s[0],msg,sizeof msg,0))<0)
          printf("Error in message \n"); 
        
        //DFS2
        sendtoserver(fp1,part1,2,s[1],divsize);
        if((nofbytes[1]=recv(s[1],msg,sizeof msg,0))<0)
          printf("Error in message \n"); 
        
        //DFS3
        sendtoserver(fp2,part2,3,s[2],divsize);
        if((nofbytes[2]=recv(s[2],msg,sizeof msg,0))<0)
          printf("Error in message \n");        
        
        //DFS4
        sendtoserver(fp3,part3,4,s[3],divsize);
        if((nofbytes[7]=recv(s[3],msg,sizeof msg,0))<0)
          printf("Error in message \n");
    		break;

    case 3: // #3-(4,1), #4-(1,2), #1-(2,3), #2-(3,4)
    		//DFS1    	    		
        sendtoserver(fp2,part2,1,s[0],divsize);         
        if((nofbytes[0]=recv(s[0],msg,sizeof msg,0))<0)
          printf("Error in message \n"); 
    		
        //DFS2
    		sendtoserver(fp3,part3,2,s[1],divsize); 
        if((nofbytes[1]=recv(s[1],msg,sizeof msg,0))<0)
          printf("Error in message \n"); 
    		
    		//DFS3
    		sendtoserver(fp4,part4,3,s[2],divsize);	
        if((nofbytes[2]=recv(s[2],msg,sizeof msg,0))<0)
          printf("Error in message \n");
    		
    		//DFS4
			  sendtoserver(fp1,part1,4,s[3],divsize); 
          if((nofbytes[3]=recv(s[3],msg,sizeof msg,0))<0)
          printf("Error in message \n");  	
        
        //DFS1  
        sendtoserver(fp3,part3,1,s[0],divsize);
          if((nofbytes[4]=recv(s[0],msg,sizeof msg,0))<0)
          printf("Error in message \n");

        //DFS2
         sendtoserver(fp4,part4,2,s[1],divsize);
          if((nofbytes[5]=recv(s[1],msg,sizeof msg,0))<0)
          printf("Error in message \n");

        //DFS3
        sendtoserver(fp1,part1,3,s[2],divsize);
          if((nofbytes[6]=recv(s[2],msg,sizeof msg,0))<0)
          printf("Error in message \n");

        //DFS4
        sendtoserver(fp2,part2,4,s[3],divsize);
        if((nofbytes[7]=recv(s[3],msg,sizeof msg,0))<0)
          printf("Error in message \n");
        
        break;

    default:
    		printf("INVALID\n");

  }
 
 	fcloseall();
}


void sendtoserver(FILE *fpart,char *part,int server,int s,int size)
{
	
  int i,nofbytes[4],key=0;
	char a[1],buf[24],b,enc[1];
  char k[]={'a'}; //Encryption key 
  int n[4],bytes;
  memset(a,0,1);
  memset(buf,0,24);

  fseek(fpart, 0L,SEEK_SET);
	switch(server)
  	{
  		case 1:   
                if((n[0]=sending(s,part,size))==0)
  		          	printf("Something wrong\n");  		          
                
                else
  		          {
                    while((a[0]=fgetc(fpart))!=EOF)   		          
 					         {
                     enc[0] = a[0] ^ k[0] ;	
	    				       if((nofbytes[0]=send(s,enc,1,0))<0)
		    			       printf("Error sending file \n");
    				       }               
    			      }
  				      break;
  		
      case 2:   
               if((n[1]=sending(s,part,size))==0)
  		          	printf("Something wrong\n");
  		          else
  		          {
      					   while((a[0]=fgetc(fpart))!=EOF)    
       					  { 
                      enc[0] = a[0] ^ k[0];    
                     if((nofbytes[1]=send(s,enc,1,0))<0)
                     printf("Error sending file \n");
                   }               
    			      }
              	break;
  		case 3: 
  			      if((n[2]=sending(s,part,size))==0)
  		          	printf("Something wrong\n");
  		        else
  		        {
					        while((a[0]=fgetc(fpart))!=EOF) 		          
 					      { 
                     enc[0] = a[0] ^ k[0]; 
                     if((nofbytes[2]=send(s,enc,1,0))<0)
                     printf("Error sending file \n");
                }               
    			    }
  		     	   break;
  		case 4: 
  				    if((n[3]=sending(s,part,size))==0)
  		          	printf("Something wrong\n");
  		        else
  		          {
					         while((a[0]=fgetc(fpart))!=EOF)  		          
 					        {	
                      enc[0] = a[0] ^ k[0];  
    	    				   if((nofbytes[3]=send(s,enc,1,0))<0)
    		    			   printf("Error sending file \n");
        				  }
        			  }
                break;

  		default: printf("Wrong server\n");

  	}    

}

int sending(int fd,char *part,int size)
{
	char command[50],msg[10];
  int nofbytes;
  
  memset(command,0,50);
  memset(msg,0,10);
 
	 strcpy(command,"PUT ");
   strcat(command,part);

   printf("Command %s\n",command);

  if((nofbytes=send(fd,command,sizeof command,0))<0)
		  printf("Error Sending PUT command \n");

    if((nofbytes=recv(fd,msg,sizeof msg,0))<0)
    {
          printf("Error in message \n");
          return 0;
    }
  
  printf("Size of the part of file is %d Sent to server %d\n",size,fd-2);
  if((nofbytes=send(fd,&size,sizeof(int),0))<0)
     printf("error sending size\n");
  
  if((nofbytes=recv(fd,msg,sizeof msg,0))<0)
    {
          printf("Error in message \n");
          return 0;
    }
   

   return 1;
	
}

int connectsock(const char *host, const char *portnum)
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


int hextodec(int c)
{

  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  else
    return -1;  

}
