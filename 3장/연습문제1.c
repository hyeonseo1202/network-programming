#include <stdio.h>
#include <stdlib.h>

extern char **environ;

int init_global_var = 3;   // 초기화된 전역변수 -> 데이터 영역
int uninit_global_var;     // 초기화되지 않은 전역변수 -> BSS 영역

int main(int argc, char **argv) {
    int auto_var;              // 자동 변수 -> 스택
    static int static_var;     // 초기화 안 했으므로 BSS 영역
    register int reg_var;      // 레지스터 변수
    char *auto_ptr;            // 포인터 변수 자체는 스택
    auto_ptr = (char *)malloc(10); // malloc으로 할당된 10바이트 -> 힙

    printf("=== 메모리 주소 출력 ===\n");

    /* 코드 영역 확인용: 함수 주소 출력 */
    printf("main 함수 주소(코드 영역)         : %p\n", (void *)main);

    /* 데이터 영역 */
    printf("init_global_var 주소             : %p\n", (void *)&init_global_var);
    printf("uninit_global_var 주소           : %p\n", (void *)&uninit_global_var);
    printf("static_var 주소                  : %p\n", (void *)&static_var);

    /* 힙 영역 */
    printf("malloc으로 할당한 주소(auto_ptr) : %p\n", (void *)auto_ptr);

    /* 스택 영역 */
    printf("argc 주소                        : %p\n", (void *)&argc);
    printf("argv 주소                        : %p\n", (void *)&argv);
    printf("auto_var 주소                    : %p\n", (void *)&auto_var);
    printf("auto_ptr 변수 주소               : %p\n", (void *)&auto_ptr);

    /*
      register 변수는 메모리 주소를 구할 수 없음.
      &reg_var 는 컴파일 오류 발생 가능.
    */
    reg_var = 100;
    printf("reg_var 값                       : %d (register 변수라 주소 출력 불가)\n", reg_var);

    /* 환경변수 영역 */
    printf("environ 주소                     : %p\n", (void *)environ);
    if (environ[0] != NULL) {
        printf("environ[0]가 가리키는 문자열 주소 : %p\n", (void *)environ[0]);
        printf("첫 번째 환경변수 내용            : %s\n", environ[0]);
    }

    free(auto_ptr);
    return 0;
}
