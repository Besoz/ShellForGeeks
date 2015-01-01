#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

void parseEnvVAr(char *env[] ) {
	char *p = (char *)malloc(sizeof(char) * strlen( getenv("PATH") )+1  );
	strncpy(p,getenv("PATH") , strlen(getenv("PATH")));	
	char * pch;
	int index = 0;
	pch = strtok (p,":");
	if(env[index] == NULL)
		env[index] = (char *)malloc(sizeof(char) * strlen(pch) + 1);
	else {
		bzero(env[index], strlen(env[index]));
	}
	strncpy(env[index], pch , strlen(pch));
	strncat(env[index], "\0", 1);
	index++;
        while (pch != NULL)
	{
		if(env[index] == NULL)
			env[index] = (char *)malloc(sizeof(char) * strlen(pch) + 1);
		else {
			bzero(env[index], strlen(env[index]));
		}
		strncpy(env[index], pch , strlen(pch));
		strncat(env[index], "\0", 1);
       		pch = strtok (NULL, ":");
		index++;
	}
	env[index]= NULL;
// access
}

void parse(char *tmp_argv,char *my_argv[] , int *background )
{
	int prevSpace = 1;
	int quoteOpen =0;
	char *foo = tmp_argv;
	int index = 0;
	char ret[100];
	bzero(ret, 100);
	while(*foo != '\0') 
	{
		if ( *foo == '\r' ||*foo == '\n' )
		{
			foo++;
		 	continue;
		}
		if ( *foo == '\t' )
		{
			*foo = ' ';
		}

		if(*foo == '"')
		{
 			quoteOpen = !quoteOpen;
			foo++;	
		}
		if( *foo == ' ' && !prevSpace && !quoteOpen) 
		{
			prevSpace =  1;
			if(my_argv[index] == NULL)
				my_argv[index] = (char *)malloc(sizeof(char) * strlen(ret) + 2);
			else {
				bzero(my_argv[index], strlen(my_argv[index]));
			}
			strncpy(my_argv[index], ret, strlen(ret));
			strncat(my_argv[index], "\0", 1);
			bzero(ret, 100);
			index++;
		} else if (*foo !=' ' || (*foo ==' ' && quoteOpen ) ){
			prevSpace = 0;
			strncat(ret, foo, 1);
		}
		foo++;

	}

	if(ret[0] != '\0') { 

		my_argv[index] = (char *)malloc(sizeof(char) * strlen(ret) + 1);
		strncpy(my_argv[index], ret, strlen(ret));
		strncat(my_argv[index], "\0", 1);
		index++;
	}
	if ( index > 0 )
	{

		char* p = my_argv[index-1];
		if ( p[strlen(p)-1] == '&' ){
			*background = 1 ;
			my_argv[index-1] = (char *)malloc(sizeof(char) * (strlen(p)));
			strncpy(my_argv[index-1], p, strlen(p)-1);
			strncat(my_argv[index], "\0", 1);
			if ( strlen(my_argv[index-1]) == 0){
				my_argv[index-1]= NULL;
			}
				
		}

	}


}
char* attachPath(char* line ,  char *path ){

        char* res;
	if ( *line == '/' ){
		res = (char *)malloc(sizeof(char) * (strlen(line)+ 1));
		strcpy(res, line);
		return res;
	}
	else{

		res = (char *)malloc(sizeof(char) * (strlen(line) + strlen(path) + 2));
		strcpy(res, path);
                strcat(res, "/");
		strcat(res, line);
		return res;
	}
}

