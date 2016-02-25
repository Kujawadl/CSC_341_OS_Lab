#include "baseconvert.h"

/*Print any base number specified */
void printNum(unsigned short int num, int baseSystem) {

  // I don't actually know any base numbering system higher than 16,
  //  so if we need a higher one, we should look into it.
  if(baseSystem < 2 || baseSystem > 16){
    printf("I am not going to convert to whatever base you entered.\n");
  } else if (baseSystem > 1 && baseSystem < 17){

    int temp;



    while(num != 0){
      temp = (num%baseSystem);
      num = (num/baseSystem);

      if( temp == 10){
        temp = printf("A");
      }else if( temp == 11){
        temp = printf("B");
      }else if( temp == 12){
        temp = printf("C");
      }else if( temp == 13){
        temp = printf("D");
      }else if( temp == 14){
        temp = printf("E");
      }else if( temp == 15){
        temp = printf("F");
      }else{
        printf("%d", temp);
      }
      // Returns for each remainer option
    }
    printf("\n");
  }
}

/* Converts hex to short int  */
char* readHexInput(char* file) {
  return " ";
}
