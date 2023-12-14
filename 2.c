#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHAR_LIMIT 200
#define DATA_COUNT 8
#define EXTRA_PADDING 5

const char heading[][CHAR_LIMIT] = {
  "Location", "City", "Price", "Rooms", "Bathrooms", "Carparks", "Type", "Furnish"
};

// =========== HELPER ==================
typedef struct Data {
  char location1[CHAR_LIMIT];
  char location2[CHAR_LIMIT];
  long int price;
  int rooms;
  int bathrooms;
  int carParks;
  char type[CHAR_LIMIT];
  char furnish[CHAR_LIMIT];
} Data;

// Return angka yg lebih gede.
int max(int input1, int input2) {
  return input1 > input2 ? input1 : input2;
}

// strlen tpi buat angka
int intlen(int input) {
  if(input == 0) return 1;

  int length = 0;
  while(input != 0) {
    length++;
    input /= 10;
  }

  return length;
}

int abs(int input) {
  return input < 0 ? input * -1 : input;
}


// ========== FILE HANDLING ================

Data* getFileData(FILE** inputFile, int* length) {

  // // Ignore line pertamanya.
  char* dump[CHAR_LIMIT];
  fscanf(*inputFile, "%[^\n]\n", &dump);
  
  /*
    Dynamically allocate memory buat setiap baris dari filenya
    pake realloc, soalnya kalo pake memory dari stack takutnya ga cukup nanti jadi error jadi mau gmau pake heap.
  */
  Data* result = (Data*)malloc(sizeof(Data));

  while(fscanf(*inputFile, "%[^,],%[^,],%ld,%d,%d,%d,%[^,],%s\n", 
    &result[*length - 1].location1, 
    &result[*length - 1].location2, 
    &result[*length - 1].price, 
    &result[*length - 1].rooms, 
    &result[*length - 1].bathrooms, 
    &result[*length - 1].carParks, 
    &result[*length - 1].type, 
    &result[*length - 1].furnish) == DATA_COUNT) {

    (*length)++;

    // Re Allocate memory lagi sama tambahannya biar iterasi berikutnya muat. 
    result = (Data*)realloc(result, sizeof(Data) * (*length));
  }

  fclose(*inputFile);
  return result;
}


// ============== TABLE HANDLING ===================

// gtau kenapa angka gabisa pake "%*d" jadi manual aja pake memset.
void printPaddedNum(int num,int width) {
  int w = (width - intlen(num));
  char padding[w];

  memset(padding, ' ', w);
  padding[w] = '\0';

  printf("%d", num);
  printf("%s", padding);
}

// Print the data on sepecified column
void printDataOnCol(Data** data, int row, int col,int width) {
  switch(col) {
    case 0:
      printf("%-*s", width, (*data)[row].location1);
      break;
    case 1:
      printf("%-*s", width, (*data)[row].location2);
      break;
    case 2: {
      printPaddedNum((*data)[row].price, width);
      break;
    }
    case 3: {
      printPaddedNum((*data)[row].rooms, width);
      break;
    }
    case 4:
      printPaddedNum((*data)[row].bathrooms, width);
      break;
    case 5:
      printPaddedNum((*data)[row].carParks, width);
      break;
    case 6:
      printf("%-*s", width, (*data)[row].type);
      break;
    case 7:
      printf("%-*s", width, (*data)[row].furnish);
      break;
    default:
      fprintf(stderr, "Invalid column specified (%d).\n", col);
      exit(EXIT_FAILURE);
  }  
}

// Print yg kek ========= 
void printBorder(int width, char character) {
  for(int i = 0; i < width; i++) {
    printf("%c", character);
  }
  printf("\n");
}

// Cari huruf paling panjang dalam 1 column.
int getLongestColumnWidth(Data* data, int length, int col) {
  int longest = strlen(heading[col]);
  
  for(int i = 0; i < length; i++) {
    switch(col) {
      case 0:
        longest = max(longest, strlen(data[i].location1));
        break;
      case 1:
        longest = max(longest, strlen(data[i].location2));
        break;
      case 2:
        longest = max(longest, intlen(data[i].price));
        break;
      case 3:
        longest = max(longest, intlen(data[i].rooms));
        break;
      case 4:
        longest = max(longest, intlen(data[i].bathrooms));
        break;
      case 5:
        longest = max(longest, intlen(data[i].carParks));
        break;
      case 6:
        longest = max(longest, strlen(data[i].type));
        break;
      case 7:
        longest = max(longest, strlen(data[i].furnish));
        break;
      default:
        fprintf(stderr, "Invalid colum specified (%d).\n", col);
        exit(EXIT_FAILURE);
    }
  }
  
  return longest;
}

// kalo oop ini yg public methodnya buat print table
void printTable(Data* data, int length) {
  if(data == NULL) {
    printf("No Data Found!\n");
    return;
  }

  int columnWidth[DATA_COUNT];
  int totalWidthAndPadding = 0;

  // Calculate berapa width setiap column berdasarakan yang paling panjang cellnya
  for(int i = 0; i < DATA_COUNT; i++) {
    columnWidth[i] = getLongestColumnWidth(data, length, i);
    totalWidthAndPadding += columnWidth[i] + EXTRA_PADDING;
  }

  // Print judul + bordernya
  printBorder(totalWidthAndPadding, '=');

  for(int i = 0; i < DATA_COUNT; i++) {
    printf("%-*s", columnWidth[i] + EXTRA_PADDING, heading[i]);
  }
  printf("\n");

  printBorder(totalWidthAndPadding, '=');

  // Print Cell itemnya.
  for(int i = 0; i < length; i++) {
    // Gtau kenapa kalo di sort, ada data yang isinya "" "" 0 0 0, jadi mesti fix pake ini. di sekip aja kalo ketemu
    if(strcmp(data[i].location1, "") == 0) continue;
    
    for(int j = 0; j < DATA_COUNT; j++) {
      printDataOnCol(&data, i, j, columnWidth[j] + EXTRA_PADDING);
    }
    printf("\n");
  }

  printBorder(totalWidthAndPadding, '=');
};

// ========== Search Handling ================

// Convert dari integer ke string pake sprintf, sprintf itu jadi kaya scanf tpi instead of outputting ke stdout / console, dia simpen di buffer variable. 
char* intToString(int input) {
  char* res = (char*)malloc(sizeof(char) * CHAR_LIMIT);
  sprintf(res, "%d", input);

  return res;
}

// Check apakah di dalem str ada substring query, kalo ada return 1 kalo ga, return 0.
int includesSubstring(char* str, char* query) {
  return strstr(str, query) != NULL;
}

// Mirip filter di javascript, jadi semua data value yang mengandung string query dimasukin ke block memory di heap baru
Data* filterTable(Data* data, int length, char* query, int* resultCount) {
  Data* result = NULL;
  
  // Result index iterator
  int newLength = 1;
  for(int i = 0; i < length; i++) {
    int isValid = 0;
    
    for(int col = 0; col < DATA_COUNT; col++) {
      if(isValid) break;

      switch(col) {
        case 0: {
          isValid = includesSubstring(data[i].location1, query);
          break;
        }
        case 1: {
          isValid = includesSubstring(data[i].location2, query);
          break;
        }
        case 2: {
          isValid = includesSubstring(intToString(data[i].price), query);
          break;
        }
        case 3: {
          isValid = includesSubstring(intToString(data[i].rooms), query);
          break;
        }
        case 4: {
          isValid = includesSubstring(intToString(data[i].bathrooms), query);
          break;
        }
        case 5: {
          isValid = includesSubstring(intToString(data[i].carParks), query);
          break;
        }
        case 6: {
          isValid = includesSubstring(data[i].type, query);
          break;
        }
        case 7: {
          isValid = includesSubstring(data[i].furnish, query);
          break;
        }

      }
    }

    if(isValid) {
      //0. Tambahin jumlah resultnya.
      (*resultCount)++;
      
      // 1. allocate more memory
      result = (Data*)realloc(result, sizeof(Data) * (newLength));
      
      // 2. insert new data
      result[newLength - 1] = data[i];

      // 3. increment
      newLength++;
    }
  } 

  return result;
}


// ================ SORT HANDLING ====================

// void mergeMove(char* type, char* sortDirection) {
//   switch(type) {
//     case "Location":

//   }
// }

void merge(Data* data, int left, int mid, int right, char* type, char* sortDirection) {
  int leftLen = mid - left + 1;
  int rightLen = right - mid;

  Data* leftData = (Data*)malloc(sizeof(Data) * leftLen);
  Data* rightData = (Data*)malloc(sizeof(Data) * rightLen);

  for(int i = 0; i < leftLen; i++) leftData[i] = data[left + i];
  for(int i = 0; i < rightLen; i++) rightData[i] = data[mid + 1 + i];

  int i = 0, j = 0, k = left;

  /**
   * Ini comparing string value gabisa pake switch statement, jadi mau gmau mesti gini wkwkwkwkwk
   * pasti ada cara lebih cepet, tapi dari pada ngebug gini aja uda aman. (kalo javascript bisa cmn bbrp line smh)
  */
  while(i < leftLen && j < rightLen) {
    if(strcmp(type, "Rooms") == 0) {
      if(strcmp(sortDirection, "ascending") == 0) {
        data[k++] = leftData[i].rooms < rightData[j].rooms ? leftData[i++] : rightData[j++];
      } else {
        data[k++] = leftData[i].rooms > rightData[j].rooms ? leftData[i++] : rightData[j++];        
      }      
    }
    if(strcmp(type, "Price") == 0) {
      if(strcmp(sortDirection, "ascending") == 0) {
        data[k++] = leftData[i].price < rightData[j].price ? leftData[i++] : rightData[j++];
      } else {
        data[k++] = leftData[i].price > rightData[j].price ? leftData[i++] : rightData[j++];        
      }      
    }
    if(strcmp(type, "Bathroom") == 0) {
      if(strcmp(sortDirection, "ascending") == 0) {
        data[k++] = leftData[i].bathrooms < rightData[j].bathrooms ? leftData[i++] : rightData[j++];
      } else {
        data[k++] = leftData[i].bathrooms > rightData[j].bathrooms ? leftData[i++] : rightData[j++];        
      }      
    }
    if(strcmp(type, "Carparks") == 0) {
      if(strcmp(sortDirection, "ascending") == 0) {
        data[k++] = leftData[i].carParks < rightData[j].carParks ? leftData[i++] : rightData[j++];
      } else {
        data[k++] = leftData[i].carParks > rightData[j].carParks ? leftData[i++] : rightData[j++];        
      }      
    }
    if(strcmp(type, "Location") == 0) {
      if(strcmp(sortDirection, "ascending") == 0) {
        data[k++] = strcmp(leftData[i].location1, rightData[j].location1) < 0 ? leftData[i++] : rightData[j++];
      } else {
        data[k++] = strcmp(leftData[i].location1, rightData[j].location1) > 0 ? leftData[i++] : rightData[j++];        
      }      
    }
    if(strcmp(type, "City") == 0) {
      if(strcmp(sortDirection, "ascending") == 0) {
        data[k++] = strcmp(leftData[i].location2, rightData[j].location2) < 0 ? leftData[i++] : rightData[j++];
      } else {
        data[k++] = strcmp(leftData[i].location2, rightData[j].location2) > 0 ? leftData[i++] : rightData[j++];        
      }      
    }    
    if(strcmp(type, "Type") == 0) {
      if(strcmp(sortDirection, "ascending") == 0) {
        data[k++] = strcmp(leftData[i].type, rightData[j].type) < 0 ? leftData[i++] : rightData[j++];
      } else {
        data[k++] = strcmp(leftData[i].type, rightData[j].type) > 0 ? leftData[i++] : rightData[j++];        
      }      
    } 
    if(strcmp(type, "Furnish") == 0) {
      if(strcmp(sortDirection, "ascending") == 0) {
        data[k++] = strcmp(leftData[i].furnish, rightData[j].furnish) < 0 ? leftData[i++] : rightData[j++];
      } else {
        data[k++] = strcmp(leftData[i].furnish, rightData[j].furnish) > 0 ? leftData[i++] : rightData[j++];        
      }      
    }                 
  }

  while(i < leftLen) data[k++] = leftData[i++];
  while(j < rightLen) data[k++] = rightData[j++];
} 

void mergeSort(Data* data, int left, int right, char* type, char* sortDirection) {
  if(left < right) {
    int mid = (right + left) / 2;
    mergeSort(data, left, mid, type, sortDirection);
    mergeSort(data, mid + 1, right, type, sortDirection);
    merge(data, left, mid, right, type, sortDirection);
  }
}

// check apakah input string itu bagian dari title heading, kalo bukan return 0 kalo ya, 1 
int columnsExist(char* str) {
  for(int i = 0; i < DATA_COUNT; i++) {
    if(strcmp(heading[i], str) == 0) return 1;
  }

  return 0;
}

Data* sortTable(Data* data, int length, char* basedOn) {
  Data* result = NULL;

}

// ================== MAIN FUNCTION ====================

int main() {
  // Open the file
  FILE* inputFile = fopen("file.csv", "r");
  
  // Value buat nampung panjang datanya. Di pass jadi ref aja.
  int length = 0;

  // Ambil data filenya
  Data* result = getFileData(&inputFile, &length);


  while(1) {
    int selected;
    printBorder(24, '=');
    printf("What do you want to do?\n");
    printBorder(24, '=');
    printf("1. Display Data\n");
    printf("2. Search Data\n");
    printf("3. Sort Data\n");
    printf("4. Export Data\n");
    printf("5. Exit\n");
    printf("Your choice : ");
    scanf("%d",&selected); getchar();

    switch(selected){
      case 1: {
        int rows;
        printf("Number of rows to be displayed: ");
        scanf("%d",&rows); getchar();

        printTable(result, rows);
        break;
      }
      case 2: {
        char* query = (char*)malloc(sizeof(char) * CHAR_LIMIT);
        printf("What do you want to find? ");
        scanf("%[^\n]",query); getchar();

        int dataFoundCount = 0;
        Data* searchResult = filterTable(result, length, query, &dataFoundCount);

        printTable(searchResult, dataFoundCount);
        if(dataFoundCount != 0) printf("%d Data Found\n", dataFoundCount);
        break;
      }
      case 3: {
        char* columns = (char*)malloc(sizeof(char) * CHAR_LIMIT);
        printf("Choose columns (Case Sensitive): ");
        scanf("%s",columns); getchar();

        if(!columnsExist(columns)) {
          printf("\n\n\nColumn Not Found\n(Input is Case Sentisive)\n\n\n\n");
          continue;
        }

        int sortType;
        printf("Select sort type, ascending (1) or descending (2) : ");
        scanf("%d",&sortType); getchar();

        if(sortType != 1 && sortType != 2) {
          printf("\n\n\nInvalid Sorting Type\n\n\n\n");
          continue;
        }

        int rows;
        printf("Number of rows to be displayed: ");
        scanf("%d",&rows); getchar();

        // Mesti bikin deep clone, soalnya mergesort yg dibikin itu ga pure, jadi dia mutate struct array awalnya. Harus di buat deep clone dlu, kalo cuman deepClone = result nanti jadinya shallow clone dan abis di sort tetep ke sort terus.
        FILE* inputFile = fopen("file.csv", "r");
        int length = 0;
        Data* deepClone = getFileData(&inputFile, &length);

        mergeSort(deepClone, 0, length, columns, sortType == 1 ? "ascending" : "descending");

        printTable(deepClone, rows);
        
        // Mesti di free memorynya kalo ga nanti numpuk.
        free(deepClone);
        break;
      }
      case 4: {
        char* fileName = (char*)malloc(sizeof(char) * CHAR_LIMIT);
        printf("File name: ");
        scanf("%s", fileName);

        // Append .csv ke file name pake sprintf
        sprintf(fileName, "%s.csv", fileName);
        
        FILE* outputFile = fopen(fileName, "w");

        // Input file paling atas
        fprintf(outputFile, "Location 1,Location 2,Price,Rooms,Bathrooms,CarParks,Type,Furnish\n");

        for(int i = 0; i < length; i++) {
          fprintf(outputFile, "%s,%s,%ld,%d,%d,%d,%s,%s\n",
            result[i].location1,
            result[i].location2,
            result[i].price,
            result[i].rooms,
            result[i].bathrooms,
            result[i].carParks,
            result[i].type,
            result[i].furnish
          );
        }
        fclose(outputFile);
        printf("Data successfully written to file %s!\n", fileName);
        break;
      }
      case 5:
        return 0;
      default:
        printBorder(30, '=');
        printf("\n\n\nInvalid Input\n\n\n\n");
        printBorder(30, '=');
        continue;
    }

    char confirmRestart;
    printf("Continue? (y/n) ");
    scanf("%c",&confirmRestart); getchar();

    if(confirmRestart == 'n') {
      break;
    } else {
      printf("\n\n\n\n");
    }
  }


  // Cleanup functions.
  free(result);
 
  return 0;
}