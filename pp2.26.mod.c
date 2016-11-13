#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
  char * in, *out, c;
  FILE *fp1, *fp2;
  if (argc != 3){
    printf("Usage: %s [input_file] [output_file]\n",argv[0]);
    return 1;
  }
  in = argv[1];
  out = argv[2];
  if((fp1 = fopen(in,"r")) == NULL){
    printf("Cannot open file \"%s\" for reading. Program terminated.\n",in);
    return 1;
  }
  fp2 = fopen(out,"w");
  while((c=fgetc(fp1)) != EOF) fputc(c,fp2);
  fclose(fp1);
  fclose(fp2);
  return 0;
}
