#include <stdio.h>
#include <stdlib.h>

int main(void) {
    char *value = getenv("myenv");

    if (value == NULL) {
        printf("환경변수 myenv가 설정되지 않았습니다.\n");
    } else {
        printf("myenv = %s\n", value);
    }

    return 0;
}



