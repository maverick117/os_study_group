/* Sudoku Solution Analysis with Multithreading Program */
/* Author: Andrew Jianzhong Liu */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define uchar unsigned char

uchar * grid;
int flag;

typedef struct coordinates{
  int row;
  int column;
}Coords;

void * verifyRow(void * params){
  int rowNum = *(int*)params;
  int flags[10];
  int i;
  memset(flags,0,sizeof(int)*10); 
  for(i = 0; i < 9; i++){
    flags[grid[rowNum*9+i]-1] += 1;
  }
  for(i = 0; i < 9; i++){
    if(flags[i] == 0)
      flag = 0;
  }
  pthread_exit(0);
}

void * verifyColumn(void * params){
  int colNum = *(int*)params;
  int flags[10];
  int i;
  memset(flags,0,sizeof(int)*10);
  for(i = 0; i < 9; i++){
    flags[grid[i*9+colNum]-1] += 1;
  }
  for(i = 0;i<9;i++){
    if(flags[i] == 0)
      flag = 0;
  }
  pthread_exit(0);
}

void * verifyCell(void* params){
  Coords current = *(Coords*)params;
  int flags[10];
  int i,j,row=current.row,col=current.column;
  memset(flags,0,sizeof(int)*10);
  for(i = -1; i <= 1; i++){
    for(j = -1; j <= 1; j++){
      flags[grid[(row+i)*9+col+j] - 1] += 1;
    }
  }
  for(i = 0;i<9;i++){
    if(flags[i] == 0){
      flag = 0;
    }
  }
  pthread_exit(0);
}

int main(){
  int i,j;
  pthread_t rv[9];
  pthread_t cv[9];
  pthread_t cellv[9];
  int nums[9] = {0,1,2,3,4,5,6,7,8};
  Coords * coords = (Coords*)malloc(sizeof(Coords)*9);
  Coords * current = NULL;
  flag = 1;
  grid = (uchar *) malloc(sizeof(uchar)*81);
  for(i = 1; i <= 7; i+=3){
    for(j = 1;j <= 7; j+=3){
      current = &coords[i-1+(j-1)/3];
      current->row = i;
      current->column = j;
    }
  }
  for(i = 0; i < 81; i++){
    scanf("%d",&j);
    grid[i] = (uchar)j;
  }
  for(i = 0; i < 9; i++){
    pthread_create(&rv[i],NULL,verifyRow,(void*)&nums[i]);
    pthread_create(&cv[i],NULL,verifyColumn,(void*)&nums[i]);
  }
  for(i = 0; i < 9; i++){
    pthread_create(&cellv[i],NULL,verifyCell,(void*)&coords[i]);
  }
  for(i = 0; i < 9; i++){
    pthread_join(rv[i],NULL);
    pthread_join(cv[i],NULL);
    pthread_join(cellv[i],NULL);
  }

  if(!flag) printf("The solution is not correct.\n");
  else printf("The solution is correct.\n");


  free(coords); 
  free(grid);
  return 0;

}
