#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ========== CONFIG ============
/*
Definisi data konstan
*/

// Limit character yang dapat diinput user
#define CHAR_LIMIT 200

// Jumlah kolom tabel
#define DATA_COUNT 8

// Padding kanan untuk pabel
#define EXTRA_PADDING 5

#define FILE_NAME "file.csv"

// Array yang berisi nama heading yang akan di display
const char heading[][CHAR_LIMIT] = {
  "Location", "City", "Price", "Rooms", "Bathrooms", "Carparks", "Type", "Furnish"
};

// =========== HELPER ==================
/*
Function yang type definiton yang digunakan untuk keperluan utility
*/
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

/*
Function yang digunakan mencari angka yang lebih besar

@param input1 {integer}
@param input2 {integer}

@return integer
*/
int max(int input1, int input2) {
  return input1 > input2 ? input1 : input2;
}

/*
Function yang mencari panjang karakter dari sebuah integer

@param input {integer}

@return {integer}
*/
int intlen(int input) {
  if(input == 0) return 1;

  int length = 0;
  while(input != 0) {
    length++;
    input /= 10;
  }

  return length;
}

/*
Function untuk melakukan operasi absolut
@param input {integer}

@return {integer}
*/
int abs(int input) {
  return input < 0 ? input * -1 : input;
}

/*
Function yang mengubah seluruh string menjadi karakter huruf kecil

@param input {string / pointer ke char}

@return {string / pointer ke char}
*/
char* toLowerCase(char* str) {
  int strLength = strlen(str);
  char* res = (char*)malloc(sizeof(char) * strLength);
  for(int i = 0; i < strLength; i++) {
    res[i] = 'A' <= str[i] && str[i] <= 'Z' ? (str[i] - 'A') + 'a' : str[i];
  }
  res[strLength] = '\0';
  return res;
}

// ========== FILE HANDLING ================
/*
Function yang berkontribusi dalam penginputan data.
*/

/*
Function yang akan mengambil data pada file. Function menggunakan pointer agar mengurangi pengguaan memory

@param inputFile {pointer to pointer ke tipe data FILE}
@param length {pointer ke variabel di luar scope function}

@return Data* {array struct dari data yang diambil dari file}
*/
Data* getFileData(FILE** inputFile, int* length) {

  // Membuang / ignore baris pertama dari file
  char* dump[CHAR_LIMIT];
  fscanf(*inputFile, "%[^\n]\n", &dump);
  
  /*
    Menggunakan dynamic memory allocation dari heap untuk mengantisipasi input data dalam jumlah yang besar
  */
  Data* result = (Data*)malloc(sizeof(Data));

  // Scan dilakukan selama fscanf masih mereturn 8 (jumlah data yang berhasil di scan)
  while(fscanf(*inputFile, "%[^,],%[^,],%ld,%d,%d,%d,%[^,],%s\n", 
    &result[*length - 1].location1, 
    &result[*length - 1].location2, 
    &result[*length - 1].price, 
    &result[*length - 1].rooms, 
    &result[*length - 1].bathrooms, 
    &result[*length - 1].carParks, 
    &result[*length - 1].type, 
    &result[*length - 1].furnish) == DATA_COUNT) {
    
    // Menambahkan length data. Length menggunakan pointer agar dapat digunakan oleh function yang berada di luar scope.
    (*length)++;

    // Memory yang sudah di allocate di tambahkan / reallocate ukurannya agar dapat dimasukan data untuk iterasi berikutnya
    result = (Data*)realloc(result, sizeof(Data) * (*length));
  }

  fclose(*inputFile);
  return result;
}


// ============== TABLE HANDLING ===================

/*
Function untuk melakukan printing angka dengan padding yang telah ditentukan, contoh
num = 5, width = 3 -> output : "5     "

@param num {integer}
@param width {integer} - jumlah karakter padding

@return {void} - print angka dengan format padding ke stdout.
*/
void printPaddedNum(int num,int width) {
  int w = (width - intlen(num));
  char padding[w];

  memset(padding, ' ', w);
  padding[w] = '\0';

  printf("%d", num);
  printf("%s", padding);
}

/*
Function yang digunakan oleh function printTable() untuk melakukan print pada kolom tertentu dengan padding

@param data {pointer ke array struct data (Data**)}
@param row {integer} posisi row pada tabel
@param col {integer} posisi column pada tabel
@param width {integer} jumlah padding yang ingin di print

@return {void} - printData pada kolom terntu dengan format
*/
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

/*
Function untuk melakukan print [character] sebanyak [width] kali

@param width {integer} jumlah padding
@param character {character} karakter yang akan di print

@return {void} - Print karakter secara [width] kalo, misal "======="

*/
void printBorder(int width, char character) {
  for(int i = 0; i < width; i++) {
    printf("%c", character);
  }
  printf("\n");
}


/*
Function untuk mencari jumlah karakter (string/integer) terpanjang dari sebuah kolom yang ditentukan

@param data {pointer ke array struct}
@param length {panjang dari array struct}
@param col {kolom yang akan di scan}

@return {integer} panjang angka terpanjang
*/
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


/*
Function metode yang digunakan untuk melakukan table print, seluruh function pada table handling berkontribusi untuk function ini.

@param data {array struct data} pointer ke element pertama array struct yang ingin di print
@param length {integer}
*/
void printTable(Data* data, int length) {
  if(data == NULL) {
    printf("No Data Found!\n");
    return;
  }

  // Array yang berisi padding setiap kolom
  int columnWidth[DATA_COUNT];

  // Jumlah panjang seluruh tabel dengan padding, digunakan untuk melakukan print dekorasi "==="
  int totalWidthAndPadding = 0;

  // Melakukan kalkulasi cell paling panjang dalam satu kolom kemudian ditambahkan padding di kanan.
  for(int i = 0; i < DATA_COUNT; i++) {
    columnWidth[i] = getLongestColumnWidth(data, length, i);
    totalWidthAndPadding += columnWidth[i] + EXTRA_PADDING;
  }

  printBorder(totalWidthAndPadding, '=');

  // Print heading seusai lebar kolom yang telah ditentukan
  for(int i = 0; i < DATA_COUNT; i++) {
    printf("%-*s", columnWidth[i] + EXTRA_PADDING, heading[i]);
  }
  printf("\n");

  printBorder(totalWidthAndPadding, '=');

  // Print Cell itemnya.
  for(int i = 0; i < length; i++) {
    // Mencegah print data yang kosong ketika di sort
    if(strcmp(data[i].location1, "") == 0) continue;
    
    for(int j = 0; j < DATA_COUNT; j++) {
      printDataOnCol(&data, i, j, columnWidth[j] + EXTRA_PADDING);
    }
    printf("\n");
  }

  printBorder(totalWidthAndPadding, '=');
};

// ========== Search Handling ================

/*
Function yang mengubah input integer menjadi string, tujuan dibuat untuk digunakan pada function getLongestColumnWidth() 

@param input {integer}

@return {string} 12345 -> "12345"
*/
char* intToString(int input) {
  char* res = (char*)malloc(sizeof(char) * CHAR_LIMIT);
  sprintf(res, "%d", input);

  return res;
}

/*
Function yang mendeteksi apakah sebuah string memiliki sebuah substring tertenu

@param str {string} string yang ingin di scan
@param query (string) substring yang ingin dicari keberadaannya

@return {integer} - 1 jika berhasil, 0 jika tidak.
*/
int includesSubstring(char* str, char* query) {
  return strstr(str, query) != NULL;
}

/*
Function yang yang membuat shallow copy dari array struct yang berisi seluruh data yang valuenya memiliki query.

@param data {array struct}
@param length {panjang array struct}
@param query {string} string yang akan memvalidasi apakah sebuah data masuk ke dalam hasil atau tidak
@param resultCount {pointer ke integer} pointer yang memberikan jumlah data yang berhasil di scan kepada variabel di luar scope

@return {array of struct} array struct berisi konten yang telah di filter sesuai ketentuan query
*/
Data* filterTable(Data* data, int length, char* query, int* resultCount) {
  Data* result = NULL;
  
  // Result index iterator
  int newLength = 1;
  for(int i = 0; i < length; i++) {
    int isValid = 0;
    
    for(int col = 0; col < DATA_COUNT; col++) {
      if(isValid) break;
      // isValid akan dijadikan truthy jika sebuah kolom mengandun query string
      switch(col) {
        case 0: {
          isValid = includesSubstring((data[i].location1), query);
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

    // Menambahkan data baru yang lolos seleksi ke dalam array temporary
    if(isValid) {
      // Mendokumentasi jumlah data yang berhasil dimasukan ke dalam array temporary
      (*resultCount)++;

      //  Melakukan alokasi memory tambahan agar input file dapat masuk
      result = (Data*)realloc(result, sizeof(Data) * (newLength));
      
      // Memasukan input data ke dalam array result (-1 berguna karena newLength dimulai dari 1)
      result[newLength - 1] = data[i];

      newLength++;
    }
  } 

  return result;
}


// ================ SORT HANDLING ====================

/*
Function untuk melakukan merging pada dua array. Function melakukan mutation pada data yang ditunjuk pointer
@param sortDirection {"asceding" | "descending"} menentukan tipe sort
@param type {string} nama dari kolumn yang ditampilkan (tidak sesuai dengan nama struct) 

@return {void}
*/
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

/*
/*
Function untuk melakukan mergSortpada dua array. Function melakukan mutation pada data yang ditunjuk pointer
@param sortDirection {"asceding" | "descending"} menentukan tipe sort
@param type {string} nama dari kolumn yang ditampilkan (tidak sesuai dengan nama struct) 

@return {void}
*/
void mergeSort(Data* data, int left, int right, char* type, char* sortDirection) {
  if(left < right) {
    int mid = (right + left) / 2;
    mergeSort(data, left, mid, type, sortDirection);
    mergeSort(data, mid + 1, right, type, sortDirection);
    merge(data, left, mid, right, type, sortDirection);
  }
}

/*
Function validasi yang melakukan check apakah sebuah value merupakan bagian dari heading (tipe data yang di display)
@param str {string}
@return {number} 
*/
int columnsExist(char* str) {
  for(int i = 0; i < DATA_COUNT; i++) {
    if(strcmp(heading[i], str) == 0) return 1;
  }

  return 0;
}

// ============== DISPLAY HANDLING ===============

/*
Function untuk melakukan display menu utama yang user liat ketika masuk program.
@return {void}
*/
void displayMainMenu() {
    printBorder(24, '=');
    printf("Joseph Christian Yusmita - 2702295695\n");
    printBorder(24, '=');
    printf("What do you want to do?\n");
    printBorder(24, '=');
    printf("1. Display Data\n");
    printf("2. Search Data\n");
    printf("3. Sort Data\n");
    printf("4. Export Data\n");
    printf("5. Exit\n");
    printf("Your choice : ");
}


// ================== MAIN FUNCTION ====================

int main() {
  // Buka file
  FILE* inputFile = fopen(FILE_NAME, "r");
  
  // Value buat nampung panjang datanya. akan di pass sebagai reference sehingga dapat digunakan pada function lain.
  int length = 0;

  // Mengambil data dari file
  Data* result = getFileData(&inputFile, &length);


  while(1) {

    // Menampilkan menu dan mengambil input dar user
    int selected;
    displayMainMenu();
    scanf("%d",&selected); getchar();

    switch(selected){
      // 1 : Print table berdasakarn jumlah row yang diambil
      case 1: {
        int rows;
        printf("Number of rows to be displayed: ");
        scanf("%d",&rows); getchar();

        printTable(result, rows);
        break;
      }
      // 2 : Print table berdasarkan query yang diberikan
      case 2: {
        // Input query user
        char* query = (char*)malloc(sizeof(char) * CHAR_LIMIT);
        printf("What do you want to find? ");
        scanf("%[^\n]",query); getchar();

        // Tracking jumlah data yang ditemukan oleh function filterTable
        int dataFoundCount = 0;
        // Mengambil hasil search
        Data* searchResult = filterTable(result, length, query, &dataFoundCount);

        printTable(searchResult, dataFoundCount);
        if(dataFoundCount != 0) printf("%d Data Found\n", dataFoundCount);
        break;
      }
      // 3 : Sorting data secara pure (tanpa mutasi) sehingga tidak mempengaruhi array asli
      case 3: {
        char* columns = (char*)malloc(sizeof(char) * CHAR_LIMIT);
        printf("Choose columns (Case Sensitive): ");
        scanf("%s",columns); getchar();

        // Jika input kolom user tidak valid
        if(!columnsExist(columns)) {
          printf("\n\n\nColumn Not Found\n(Input is Case Sentisive)\n\n\n\n");
          continue;
        }

        // Sorting type user
        int sortType;
        printf("Select sort type, ascending (1) or descending (2) : ");
        scanf("%d",&sortType); getchar();

        // Jika sorting type user tidak valud
        if(sortType != 1 && sortType != 2) {
          printf("\n\n\nInvalid Sorting Type\n\n\n\n");
          continue;
        }

        int rows;
        printf("Number of rows to be displayed: ");
        scanf("%d",&rows); getchar();

        // Pembuatan deep clone dari inputFile agar tidak terjadi mutasi array original
        FILE* inputFile = fopen(FILE_NAME, "r");
        int length = 0;
        Data* deepClone = getFileData(&inputFile, &length);

        // Melakukan sorting berdasarakan tipe.
        mergeSort(deepClone, 0, length, columns, sortType == 1 ? "ascending" : "descending");

        printTable(deepClone, rows);
        
        // Membebaskan memory yang dialokasikan untuk deep cloned sorted array
        free(deepClone);
        break;
      }
      // 4: Writing file ke dalam file dengan nama yang sudah di spesifikasi user
      case 4: {
        // Input nama file
        char* fileName = (char*)malloc(sizeof(char) * CHAR_LIMIT);
        printf("File name (No Spaces) : ");
        scanf("%[^\n]", fileName); getchar();

        // Input gagal jika user menggunakan spasi
        int isValid = 1;
        for(int i = 0; i < strlen(fileName); i++) {
          if(fileName[i] == ' ') {
            printf("File name cannot contain spaces!\n");
            isValid = 0;
            break;
          }
        }
        if(!isValid) break;

        // Append .csv kepada string fileName menggunakan sprintf
        sprintf(fileName, "%s.csv", fileName);
        
        // Membuat file baru sesuai input user
        FILE* outputFile = fopen(fileName, "w");

        // Menginput file heading
        fprintf(outputFile, "Location 1,Location 2,Price,Rooms,Bathrooms,CarParks,Type,Furnish\n");

        // Melakukan input data perbaris pada file baru (outputFile)
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
      // 5: Aplikasi ditutup
      case 5:
        return 0;
      default:
        printBorder(30, '=');
        printf("\n\n\nInvalid Input\n\n\n\n");
        printBorder(30, '=');
        continue;
    }

    // Menangani reset program
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