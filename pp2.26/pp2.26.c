#include <stdio.h>
#include <stdlib.h>

int main(){
  char * in, *out, c;
  FILE *fp1, *fp2;
  if((in = malloc(sizeof(char)*4096)) == NULL) return 1;
  if((out = malloc(sizeof(char)*4096)) == NULL) return 1;
  printf("Input file name:");
  scanf("%s",in);
  if((fp1 = fopen(in,"r")) == NULL){
    printf("Cannot open file \"%s\" for reading. Program terminated.\n",in);
    return 1;
  }
  printf("Output file name:");
  scanf("%s",out);
  if((fp2 = fopen(out,"w")) == NULL){
    printf("Failed to open/create file \"%s\" for writing. Program terminated.\n",out);
    fclose(fp1);
    return 1;
  }
  printf("%s => %s\nMoving in progress...\n",in,out);
  while((c=fgetc(fp1)) != EOF) fputc(c,fp2);
  fclose(fp1);
  fclose(fp2);
  printf("Done.\n");
  free(in);
  free(out);
  return 0;
}
