
#include "utilities.h"

int getInfo(char * str,url * url_info){

  char * token = malloc(strlen(str));
  strcpy(token, str);

  url_info->type = calloc(strlen(token), sizeof(char));
  url_info->user = calloc(strlen(token), sizeof(char));
  url_info->password = calloc(strlen(token), sizeof(char));
  url_info->host = calloc(strlen(token), sizeof(char));
  url_info->url_path = calloc(strlen(token), sizeof(char));


  strcpy(url_info->type, token);
  token = strtok(token+6, ":");
  strcpy(url_info->user, token);
  token = strtok(NULL, "@");
  strcpy(url_info->password, token);
  token = strtok(NULL, "/");
  strcpy(url_info->host, token);
  token = strtok(NULL, "");
  strcpy(url_info->url_path, token);

  if(0){
    printf("%s\n",url_info->type);
    printf("%s\n",url_info->user );
    printf("%s\n",url_info->password);
    printf("%s\n",url_info->host);
    printf("%s\n",url_info->url_path);
  }
  return 1;
}

int get_filename(char * path, char * filename){

  char * token = malloc(strlen(path));
  memcpy(token, path, strlen(path));
  token = strtok(path, "/");

  while(token != NULL){
	  memset(filename,0,strlen(filename));
    memcpy(filename, token, strlen(token));
    token = strtok(NULL, "/");
  }

  return 1;
}
