#include <stdio.h>
#include <string.h>

char reverseCapitalize(char input) {
    if('a' <= input && input <= 'z') {
        return input - 'a' + 'A';
    } else if('A' <= input && input <= 'Z') {
        return input - 'A' + 'a';
    }
    
    return ' ';
}

void printResult(char* str) {
    for(int i = strlen(str) - 1; i >= 0; i--) {
        printf("%c", reverseCapitalize(str[i]));
    }
    
    printf("\n");
}

int main() {

    char input[1000];
    scanf("%[^\n]", input); getchar();
    
    printResult(input);

    return 0;
}