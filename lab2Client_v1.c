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
void writetoclient(FILE *fclient,FILE *fpart);
/*------------------------------------------------------------------------
 * main - TCP client for ECHO service
 *------------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
	int option;
	char line[100],filename[100];
	char *cp,*cq,*reading[3],*dfs1addr,*dfs2addr,*dfs3addr,*dfs4addr,*username,*password;
		/*switch (argc) {
	case 1:
		host = "localhost";
		break;
	case 3:
		host = argv[2];
		
	case 2:
		portnum = argv[1];
		break;
	default:
		fprintf(stderr, "usage: %s [host [port]]\n", argv[0]);
		exit(1);
	}*/

	FILE *fp=fopen("/home/avneendra/Desktop/netsys/Lab2/DFC/dfc.confg","rb");
	if(fp==NULL)
	printf("Cant open");
	
	while(fgets(line,100,fp)!=NULL)
	{
		cp = strdup (line); 
		reading[0]= strtok(cp," ");
		reading[1]= strtok(NULL," ");
		//Reading[0] will have first word of each line while Reading[1] will have second 
		if(!strcmp(reading[0],"Server"))
		{
			// if(!strcmp(reading[1],"DFS1"))
			//    memcpy(dfs1addr,reading[2],strlen(reading[2]));
				
			// if(!strcmp(reading[1],"DFS2"))
			//    memcpy(dfs2addr,reading[2],strlen(reading[2]));
			
			// if(!strcmp(reading[1],"DFS3"))
			//    memcpy(dfs3addr,reading[2],strlen(reading[2]));

			// if(!strcmp(reading[1],"DFS4"))
			//    memcpy(dfs4addr,reading[2],strlen(reading[2]));
			 //Reading[2] has the address
		}
		if(!strcmp(reading[0],"Username"))
			memcpy(username,reading[1],strlen(reading[1]));

		if(!strcmp(reading[0],"Password"))
			memcpy(password,reading[1],strlen(reading[1]));
	}
	fclose(fp);

	while(1)
	{	
		printf(" MENU OPTIONS: Enter '1' to LIST\n Enter '2' for GET \n Enter '3' to PUT\n");
		scanf("%d",&option);
		switch(option)
		{
			//Send username password first to authenticate
			case 1: // for LIST command--contact all servers and see what files you can get back..and try and reconstruct them

			case 2: // For GET command--enter what file you want to get and call all servers to see if you can reconstruct the file

			case 3:	// For PUT command--enter the filename that you want to send..break it into pieces and according to MDblah blah ..value
					printf("Enter file name \n");
					scanf("%s",filename);
					uploading(filename);
					break;
			default:
			printf("Invalid option\n");

		}

	}	
	exit(0);
}


void uploading(char *name)
{

  int sum=0,i,size,bytes;
  unsigned char abc[MD5_DIGEST_LENGTH];
  char buffer,c,perm[60],temp[60],data[1024];
  char root[]="/home/avneendra/Desktop/netsys/Lab2/DFC/";
  strcpy(perm,root);
  strcat(perm,name);
  strcpy(temp,perm);
  

  
  FILE *fp=fopen(temp,"r");
  MD5_CTX mdContext;
  MD5_Init (&mdContext);
  while ((bytes = fread (data, 1, 1024, fp)) != 0)
  MD5_Update (&mdContext, data, bytes);
  MD5_Final (abc,&mdContext);
  for(i = 0; i < MD5_DIGEST_LENGTH; ++i)
  	sum+=(unsigned int)abc[i];
  
  printf("SUM %d\n",sum);
  int x=sum%4;

  fseek(fp, 0L, SEEK_SET);
  fseek(fp, 0L, SEEK_END);
  size = ftell(fp);
  fseek(fp, 0L, SEEK_SET);

  strcat(temp,".1");
  FILE *f=fopen(temp,"w");
  for(i=0;i<size/4;i++)
  {
  	c=fgetc(fp);
  	fputc(c,f);
  	
  }  


  strcpy(temp,perm);
  strcat(temp,".2");
  FILE *f2=fopen(temp,"w");
  for(i=0;i<size/4;i++)
  {
  	c=fgetc(fp);
  	fputc(c,f2);
  }
 
  
  
  strcpy(temp,perm);
  strcat(temp,".3");
  FILE *f3=fopen(temp,"w");
  for(i=0;i<size/4;i++)
  {
  	c=fgetc(fp);
  	fputc(c,f3);
  }

  
  strcpy(temp,perm);
  strcat(temp,".4");
  FILE *f4=fopen(temp,"w");
  for(i=0;i<size/4;i++)
  {
  	c=fgetc(fp);
  	fputc(c,f4);
  }
  
  fcloseall(); 

  FILE *fnew=fopen("/home/avneendra/Desktop/netsys/Lab2/DFC/client.txt","w+");
  FILE *fp1=fopen("/home/avneendra/Desktop/netsys/Lab2/DFC/text1.txt.1","r");
  FILE *fp2=fopen("/home/avneendra/Desktop/netsys/Lab2/DFC/text1.txt.2","r");
  FILE *fp3=fopen("/home/avneendra/Desktop/netsys/Lab2/DFC/text1.txt.3","r");
  FILE *fp4=fopen("/home/avneendra/Desktop/netsys/Lab2/DFC/text1.txt.4","r");
  printf("%d X",x);
  switch(x)
  {
    case 0: // #1-(1,2), #2-(2,3), #3-(3,4), #4-(4,1)
    		fputs("DFS1\n",fnew);
    		writetoclient(fnew,fp1);
    		writetoclient(fnew,fp2);
    		
    		fputs("DFS2\n",fnew);
    		writetoclient(fnew,fp2);
    		writetoclient(fnew,fp3);
    		
    		fputs("DFS3\n",fnew);
    		writetoclient(fnew,fp3);
    		writetoclient(fnew,fp4);
    		
    		fputs("DFS4\n",fnew);
    		writetoclient(fnew,fp4);
    		writetoclient(fnew,fp1);
    		break;
    case 1: // #1-(4,1), #2-(1,2), #3-(2,3), #4-(3,4)
    		fputs("DFS1\n",fnew);
    		writetoclient(fnew,fp4);
    		writetoclient(fnew,fp1);
    		
    		fputs("DFS2\n",fnew);
    		writetoclient(fnew,fp1);
    		writetoclient(fnew,fp2);
    		
    		fputs("DFS3\n",fnew);
    		writetoclient(fnew,fp2);
    		writetoclient(fnew,fp3);
    		
    		fputs("DFS4\n",fnew);
    		writetoclient(fnew,fp3);
    		writetoclient(fnew,fp4);
    		break;
    case 2: // #2-(4,1), #3-(1,2), #4-(2,3), #1-(3,4)
    		fputs("DFS1\n",fnew);
    		writetoclient(fnew,fp4);
    		writetoclient(fnew,fp1);
    		
    		fputs("DFS2\n",fnew);
    		writetoclient(fnew,fp1);
    		writetoclient(fnew,fp2);
    		
    		fputs("DFS3\n",fnew);
    		writetoclient(fnew,fp2);
    		writetoclient(fnew,fp3);
    		
    		fputs("DFS4\n",fnew);
    		writetoclient(fnew,fp3);
    		writetoclient(fnew,fp4);
    		break;
    case 3: // #3-(4,1), #4-(1,2), #1-(2,3), #2-(3,4)
    		fputs("DFS1\n",fnew);
    		writetoclient(fnew,fp4);
    		writetoclient(fnew,fp1);
    		
    		fputs("DFS2\n",fnew);
    		writetoclient(fnew,fp1);
    		writetoclient(fnew,fp2);
    		
    		fputs("DFS3\n",fnew);
    		writetoclient(fnew,fp2);
    		writetoclient(fnew,fp3);
    		
    		fputs("DFS4\n",fnew);
    		writetoclient(fnew,fp3);
    		writetoclient(fnew,fp4);
    		break;

    default:
    		printf("INVALID\n");

  }
 
 	fcloseall();
}


void writetoclient(FILE *fclient,FILE *fpart)
{
	char a;
    int sz,i;
    fseek(fpart, 0L, SEEK_END);
  	sz = ftell(fpart);
  	fseek(fpart, 0L, SEEK_SET);
    for(i=0;i<sz;i++)
    {
	  	a=fgetc(fpart);
	  	fputc(a,fclient);
    }


}