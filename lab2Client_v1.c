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


/*------------------------------------------------------------------------
 * main - TCP client for ECHO service
 *------------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
	char line[100];
	char *cp,*cq,*reading[3],*dfs1addr,*dfs2addr,*dfs3addr,*dfs4addr,*username,*password;
	FILE *fp=fopen("/home/avneendra/Desktop/netsys/Lab2/DFC/dfc.confg","rb");
	while(fgets(line,100,fp)!=NULL)
	{
		cp = strdup (line); 
		reading[0]= strtok(cp," ");
		reading[1]= strtok(NULL," ");
		//Reading[0] will have first word of each line while Reading[1] will have second 
		if(!strcmp(reading[0],"Server"))
		{
			if(!strcmp(reading[1],"DFS1"))
			   strcpy(dfs1addr,reading[2]);
				
			if(!strcmp(reading[1],"DFS2"))
			   strcpy(dfs2addr,reading[2]);
			
			if(!strcmp(reading[1],"DFS3"))
			   strcpy(dfs3addr,reading[2]);

			if(!strcmp(reading[1],"DFS4"))
			   strcpy(dfs4addr,reading[2]);

		}
		if(!strcmp(reading[0],"Username"))
			strcpy(username,reading[1]);

		if(!strcmp(reading[0],"Password"))
			strcpy(password,reading[1]);
	}
	fclose(fp);
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

	



	exit(0);
}
