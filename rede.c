#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include<stdio.h>
#include "server.h"

/* HTML source for the start of the page we generate.  */

static char* page_start =
  "<html>\n"
  " <body>\n"
  "  <pre>\n";

/* HTML source for the end of the page we generate.  */

static char* page_end =
  "  </pre>\n"
  " </body>\n"
  "</html>\n";

void module_generate (int fd, char* qry)
{
  pid_t child_pid;
  int rval;
	char* pedaco = strtok(qry, "&"); //pedacos da qry string
char* interf;
/*pegando parametros da qry*/
while (pedaco != NULL)
{
    char param[100]; 
    char value[100];
    char * qry_str = strdup(pedaco);
    readParams(qry_str, param, value);//envia uma parte da qry(ex: ano=2021 => param=value)
    if (strcmp(param, "interface") == 0) {
        interf = strdup(value);
    }
   pedaco = strtok (NULL, "&"); //aponta proximo pedaco(param=value)
 }
if(interf==NULL)
interf = "-a"; //ifconfig -a

  /* Write the start of the page.  */
  write (fd, page_start, strlen (page_start));
  /* Fork a child process.  */
  child_pid = fork ();
  if (child_pid == 0) {
    /* This is the child process.  */
    /* Set up an argumnet list for the invocation of cal.  */
     char* argv[] = { "/sbin/ifconfig", interf, NULL};

    /* Duplicate stdout and stderr to send data to the client socket.  */
    rval = dup2 (fd, STDOUT_FILENO);
    if (rval == -1)
      system_error ("dup2");
    rval = dup2 (fd, STDERR_FILENO);
    if (rval == -1)
      system_error ("dup2");
    /* Run df to show the free space on mounted file systems.  */
    execv (argv[0], argv);
    /* A call to execv does not return unless an error occurred.  */
    system_error ("execv");
  }
  else if (child_pid > 0) {
    /* This is the parent process.  Wait for the child process to
       finish.  */
    rval = waitpid (child_pid, NULL, 0);
    if (rval == -1)
      system_error ("waitpid");
  }
  else 
    /* The call to fork failed.  */
    system_error ("fork");
  /* Write the end of the page.  */
  write (fd, page_end, strlen (page_end));
}
