#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

#include "commands.h"
#include "built_in.h"

static struct built_in_command built_in_commands[] = {
  { "cd", do_cd, validate_cd_argv },
  { "pwd", do_pwd, validate_pwd_argv },
  { "fg", do_fg, validate_fg_argv }
};

static int is_built_in_command(const char* command_name)
{
  static const int n_built_in_commands = sizeof(built_in_commands) / sizeof(built_in_commands[0]);

  for (int i = 0; i < n_built_in_commands; ++i) {
    if (strcmp(command_name, built_in_commands[i].command_name) == 0) {
      return i;
    }
  }

  return -1; // Not found
}

/*
 * Description: Currently this function only handles single built_in commands. You should modify this structure to launch process and offer pipeline functionality.
 */
int evaluate_command(int n_commands, struct single_command (*commands)[512])
{
  if (n_commands > 0) {
    struct single_command* com = (*commands);

    assert(com->argc != 0);

    int built_in_pos = is_built_in_command(com->argv[0]);
    if (built_in_pos != -1) {
      if (built_in_commands[built_in_pos].command_validate(com->argc, com->argv)) {
        if (built_in_commands[built_in_pos].command_do(com->argc, com->argv) != 0) {
          fprintf(stderr, "%s: Error occurs\n", com->argv[0]);
        }
      } else {
        fprintf(stderr, "%s: Invalid arguments\n", com->argv[0]);
        return -1;
      }
    } else if (strcmp(com->argv[0], "") == 0) {
      return 0;
    } else if (strcmp(com->argv[0], "exit") == 0) {
      return 1;
    }
    else{
	    char path_res[5][100]={
		    "/usr/local/bin/","/usr/bin/","/bin/", "/user/sbin/","/sbin/"
	    };

	    char command_pointer[100];
		
	    strcpy(command_pointer, com->argv[0]);

	   int  background=0;

	    pid_t child_p, pid;
	    int child_status;

	    child_p=fork();

	    if(!strcmp(com->argv[com->argc-1],"&")){
		    com->argv[com->argc-1]=NULL;
		    background=1;
	    }

	    if(child_p<0){
		    exit(1);//fork fail
		   
	    }
	    else if(child_p==0){
	

		    pid=getpid();
		   if(execv(com->argv[0],com->argv)==-1){

		    for(int i=0;i<5;i++){
			    if(execv(com->argv[0],com->argv)==-1){
					    strcat(path_res[i],com->argv[0]);
					    com->argv[0]=path_res[i];
					    if(execv(com->argv[0],com->argv)==-1){
						    com->argv[0]=command_pointer;
							    }
		

					    }
		    }

		   
	
		    fprintf(stderr, "%s: command not found\n", com->argv[0]);
		    return 1;
		    }
	    }
	    else{
		    if(background){
			    printf("%d\n",pid);
		    }

		    else{
			    pid= wait(&child_status);
		    }
	    }
	    
	  
	 //   return 0;
    }
  }
  return 0;
}

void free_commands(int n_commands, struct single_command (*commands)[512])
{
  for (int i = 0; i < n_commands; ++i) {
    struct single_command *com = (*commands) + i;
    int argc = com->argc;
    char** argv = com->argv;

    for (int j = 0; j < argc; ++j) {
      free(argv[j]);
    }

    free(argv);
  }

  memset((*commands), 0, sizeof(struct single_command) * n_commands);
}
