#include "parser.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>


FILE *logFile;
int history_max_size;
char *history[20];
int historyIndex ;




void openFile( char* argv[] ,FILE *file){
		printf("sa3edny ya rab");
		file = fopen (argv[1], "rt");


}
void printFileErrors(int m)
{	
	if ( m==-1)
	{ 
		perror("File Not Found");
		exit(EXIT_FAILURE);
	}
}
void closeFile( FILE *file)
{
	fclose ( file );
}
void printPrompt()
{
	printf("Shell > ");
}
int readCommadLine( char *line ) 
{
	fgets(line, 520, stdin);
	if ( strlen(line) > 512 ) 
		return -1;
	return 0;
}
int isBuiltInCommand(char *argv[])
{
	if (strstr(argv[0], "cd") != NULL || strstr(argv[0], "history") != NULL 
		|| strstr(argv[0], "=") != NULL 
			||( strstr(argv[0], "echo") != NULL && strstr(argv[1], "$")!=NULL ) )
		return 1;
	return 0;  
        
}

void printHistory()
{
	int i;
	if ( (historyIndex - history_max_size) > history_max_size){
		i = (historyIndex - history_max_size)% history_max_size;
	}else {
		i=0;
	}
	int c=0;
	while ( history[i] != NULL && c < history_max_size ){

		printf("%s",history[i]);
		i ++;
		i = i%history_max_size;
		c++;
	}
}

void addToHistroy(char* line)
{

	int i =historyIndex%history_max_size;
	if(history[i] == NULL)
		history[i] = (char *)malloc(sizeof(char) * strlen(line) + 1);
	else {
		bzero(history[i], strlen(history[i]));
	}
	strncpy(history[i], line, strlen(line));
	historyIndex ++;

}
void clearHistroy()
{

}

void executeBuiltInCommand(char *argv[] ){

	if ( strstr(argv[0], "cd") != NULL )
	{
		if ( strstr(argv[1], "..") != NULL ){
			char buff[512];
			char* p = getcwd(buff, 512 );
			char* x1=p;
			p = p + strlen(p);
			while(*p != '/'){
				p--;
			}
			int x2 =p - x1;
			char*k = (char *)malloc(sizeof(char) * (x2+1));
			strncpy(k, x1, x2);
			chdir(k);
		}
		else
		{
			char buff[512];
			if (chdir (attachPath(argv[1] ,getcwd(buff, 512 ))) < 0) printf("errno is %d\n", errno);
		}
		
		
	}
	else if ( strstr(argv[0], "history") != NULL )
	{
		printHistory();

	}
	else if ( strstr(argv[0], "echo") != NULL && strstr(argv[1], "$")!=NULL ){ /*  very weak support */
		char*p1 = (char *)malloc(sizeof(char) * 2);
		strncpy(p1, argv[1]+1,1);
		strncat(p1, "\0", 1);
		printf("%s\n",getenv(p1));
	}
	else{
		char*p1 = (char *)malloc(sizeof(char) * 2);
		strncpy(p1, argv[0],1);
		strncat(p1, "\0", 1);
		char*p2 = (char *)malloc(sizeof(char) * 2);
		strncpy(p2, argv[0]+2,1);
		strncat(p2, "\0", 1);
		setenv(p1,p2,1);
	}
}
void freePointerArray(char* array[])
{
    int i;

    for ( i = 0; array[i]; i++ )
        free( array[i] );

    free( array );
}
int fileFound(char *filename){

	return (access (filename, F_OK));

}
void signalHandler(int signal)
{
	fprintf(logFile, "Cought signal %d!\n",signal);
	if (signal==SIGCHLD) {
		fprintf(logFile, "Child ended\n");
		wait(NULL);
	}
}
void reset(char** myPointer) {
     if (myPointer) {
         free(*myPointer);
         *myPointer = NULL;
     }
}
int  execute(char *line , char *argv[] , int *background)
{
	if ( argv[0] == NULL)
		return 1 ;
	if (strstr(argv[0], "exit") != NULL)  /* is it an "exit"?     */
		return -1;               /*   exit if it is                */
	
	if ( isBuiltInCommand(argv) )
	{
		executeBuiltInCommand(argv);
	}
	else 
	{ 
	     	pid_t  pid;
	     	int    status;
		pid =  fork();
	     	if (pid ==-1) {     /* fork a child process           */
			printf("*** ERROR: forking child process failed\n");
		 	exit(1);
	     	}
	     	else if (pid == 0) {          /* for the child process:         */

			char *address;
			int i;
			if (*line == '/' ){
				i = execv(argv[0], argv);
			} 
			else{

				static char *env[100];
				char *res1;
				parseEnvVAr(env);
				int index = 0;

				while ( env[index] !=  NULL ) {
					char *res=attachPath(argv[0],  env[index] );
					if (fileFound(res) != -1 ){
						res1= res;
						break;
					}
					index ++ ;
				}

				i =execv(res1, argv);
			}
		  	if ( i < 0) {     /* execute the command  */
				printf("errno is %d\n", errno);
				printf("%s: %s\n", line, "command not found");
		    		exit(EXIT_SUCCESS);
		 	}
	     	}else {  /* for the parent:      */
			if (*background){
				printf("background\n");
			} else {
				while (wait(&status) != pid);     /* wait for completion  */
			}	                                 
	     	}
	}
	return 1;
}

int main (int argc, char *argfe[]) {

	signal(SIGCHLD, signalHandler);
	static char  line[520];             /* the input line                 */
        static char  *argv[512];              /* the command line argument      */
	int background;
	int fileState = 0; //  0 interactive mode // 1 file mode // -1 error file mod
	history_max_size=historyIndex=20;
	logFile= fopen("logfile.txt", "wt");
	if (logFile == NULL)
	{
		printf("Error opening LogFile !\n");
		exit(1);
	}

   	if ( argc > 1 ){ //batch mode
		FILE *file;
		file = fopen (argfe[1], "rt");  /* open the file for reading */
		int fileExited = 0;

		if (file != NULL) {
    			while (fgets (line, 520, file)) 
			{
				background= 0;
				if ( strlen(line) > 512 ) {
					printf("Error too long command !\n");
					continue;
				}
				parse(line, argv, &background ); 
				if ( argv[0] != NULL )	
					printf("%s",line);
				if ( execute(line,argv,&background)  < 0 ){          /* otherwise, execute the command */
					fileExited =1;
					break;
				}
				addToHistroy(line);
				reset(argv);
   			}
			if (!fileExited)	
				printf("Error file didn't exit correctly !\n");
			closeFile(file); 
		}
		else	
		{
			printf("Error finding file !\n");
		}			
	}
	else 
	{ // interavtive mode
		while (1) {                   /* repeat until done ....         */
			background= 0;
			printPrompt();     /*   display a prompt             */
			if (readCommadLine(line) < 0 ){        /*   read in the command line     */	
				printf("Error too long command !\n");
				continue;
			}
			parse(line, argv, &background );       /*   parse the line               */

			if ( execute(line,argv,&background)  < 0 )          /* execute the command */
				break;                             	/* -1 if exit */
			addToHistroy(line);
			reset(argv);
		}
	}
	closeFile(logFile);
	return 0;
}


/*

     int 
     main (int argc, char **argv)
     {
	while (1){
		int childPid;
		char * cmdLine;

	        printPrompt();

	        cmdLine= readCommandLine(); //or GNU readline("");
		
		cmd = parseCommand(cmdLine);

		record command in history list (GNU readline history ?)
 
		if ( isBuiltInCommand(cmd)){
		    executeBuiltInCommand(cmd);
		} else {		
		     childPid = fork();
		     if (childPid == 0){
			executeCommand(cmd); //calls execvp  
			
		     } else {
			if (isBackgroundJob(cmd)){
			        record in list of background jobs
			} else {
				waitpid (childPid);

			}		
		    }
	        }
     }

*/





