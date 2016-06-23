#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
  char argument[] = "";
  char text[] = "";
  
  for (int i=0; i < argc; i++) {
    printf("argv[%d] = %s\n", i, argv[i]);
    if (strcmp(argv[i], "-text") == 0) {
      strcat(text, strcat(argv[i+1], "\n"));
    } else if (strcmp(argv[i], "-toggle") == 0) {
      strcat(argument, "-toggle");
    }
  }
  
  
  printf("Hello World\n");
  printf(text);

  if (!strcmp(argument, "-toggle"))
    printf("EEHHHH!!!\n");
  
  
  return 0;
}
