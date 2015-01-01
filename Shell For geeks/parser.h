#ifndef PARSER_H_   /* Include guard */
#define PARSER_H_

void parse(char *tmp_argv,char *my_argv[] , int *background );  /* An example function declaration */
char* attachPath(char* line ,  char *path );
void parseEnvVAr(char *env[]);

#endif
