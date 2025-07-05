#include <stdio.h>
#include <string.h>
int main(){char l[256];setbuf(stdin,NULL);setbuf(stdout,NULL);while(fgets(l,256,stdin)){if(strncmp(l,"uci",3)==0){printf("id name Agent4k\nid author Aider\nuciok\n");}else if(strncmp(l,"isready",7)==0){printf("readyok\n");}else if(strncmp(l,"go",2)==0){printf("bestmove a1a2\n");}else if(strncmp(l,"quit",4)==0){break;}}return 0;}
