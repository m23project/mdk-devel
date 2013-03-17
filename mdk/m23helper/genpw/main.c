/*
genpw.c - Written by : Guy Fraser <guy@incentre.net>

Usage:
genpw -{p|e} password username
-p password# clear text password
-e password# encrypted password

Output:
An encrypted paswork is send to stdout on success.

To Build type:
cc -o /usr/bin/genpw -lcrypt genpw.c

Example:
adduser -p `genpw -p Sm0k1n johndoe` johndoe
*/

#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define UIDMAX 65535
#define UIDMIN 20


char *encrypt_pw(char *clearpw, char *name);

/* crazy conversion for some older OS's. */
/* turns an 8 bit character to 32 bit integer for some poor switch commands. */
/* this is also useful for constructing crypt salts */
typedef union{
  char ch[4];
  int num;
}ctoi;

int main(int argv, char *argc[], char *envp[]){

  /*******************
  *set main variables*
  *******************/

  struct passwd *inpass;

  int i = 0 ;

  char *message[1] = {
"\nUsage:\n\tgenpw -{p|e} password username\n\n\
\t-e password\tencrypted password\n\
\t-p password\tclear text password\n\n"
    },
    *newpass;

  ctoi x;

  x.num = 0 ;

  if(argv != 4){
 
    if(argv < 4){
      fprintf(stderr,"Not enough arguments.\n\n");
    }else{
      fprintf(stderr,"Too many arguments.\n\n");
    }

    for(i = 0;i < argv;i++){

      printf("entry %d = %s\n",i,argc[i]);

    }/*end diag*/

    fprintf(stderr, message[0]);
    exit(-1);

  }/*end if argv*/
  else{

    strcpy(x.ch, argc[1]);/* crazy conversion for some older OS's. */
    x.ch[0] = x.ch[1];/* copy command character over the hyphen. */
    x.ch[1] = '\0';/* clear the copied character. */
    newpass = strdup(argc[2]);
   
    if (argc[1][0] != '-') {
      fprintf(stderr,"No - found.\n\n");
      fprintf(stderr, message[0]);
      exit(1);
    } /* end if */

    switch(x.num){ /* crazy conversion for some older OS's. */

      case 112: newpass = strdup(encrypt_pw(argc[2], argc[3])); /*p*/

      case 101: printf("%s\n",newpass); /*e*/
                break;

      default:  fprintf(stderr,"No -p or -e found.\n\n");
      fprintf(stderr, message[0]);
exit(1);
                /*break;*/

    }/*end switch*/

  }/*end elseif argv*/

  exit(0);

}/*end main*/

char *encrypt_pw(char *clearpw, char *name){

  long now,week,pert1,pert2;
  char salt[3] = "\0\0\0", *newpw,
       saltset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";
  ctoi tx;
  tx.num = 0;

  tx.ch[0] = name[0];
  pert1 = tx.num;
  tx.ch[0] = name[1];
  pert2 = tx.num;

  now = time(NULL);
  week = now / (60*60*24*7);

  salt[0] = saltset[((week + pert1 + pert2) % 64)];
  salt[1] = saltset[(now % 64)];

  newpw = (char *)crypt(clearpw, salt);
  return newpw;
}