
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
 FILE *in;
 unsigned int pos;
 
 in=fopen(argv[1],"rb");
 fseek(in,0,SEEK_END); 
 pos=ftell(in); 
 fclose(in); 
 printf("%i\n",pos);

 return EXIT_SUCCESS;
}
