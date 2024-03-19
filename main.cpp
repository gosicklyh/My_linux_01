#include <cstddef>
#include <iostream>
#include <pthread.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define MAX 50

int number = 0;
// 线程处理函数
void *funcA_num(void *arg) {
  for (int i = 0; i < MAX; i++) {
    int cur = number;
    cur++;
    usleep(3);
    number = cur;
    std::cout << "funcA,id = " << pthread_self() << " number = " << number
              << std::endl;
  }
  return NULL;
}

void *funcB_num(void *arg) {
  for (int i = 0; i < MAX; i++) {
    int cur = number;
    cur++;
    number = cur;
    std::cout << "funcB,id = " << pthread_self() << " number = " << number
              << std::endl;
  }
  return NULL;
}

int main(int argc, const char *argv[]) {
  pthread_t p1, p2;
  pthread_create(&p1, NULL, funcA_num, NULL);
  pthread_create(&p2, NULL, funcB_num, NULL);
  pthread_join(p1, NULL);
  pthread_join(p2, NULL);
}