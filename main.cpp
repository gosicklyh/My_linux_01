#include <cstddef>
#include <iostream>
#include <pthread.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define MAX 50

int number = 0;
pthread_mutex_t mutex;
// 线程处理函数
void *funcA_num(void *arg) {
  for (int i = 0; i < MAX; i++) {
    //加锁
    pthread_mutex_lock(&mutex);
    int cur = number;
    cur++;
    usleep(3);
    number = cur;
    std::cout << "funcA,id = " << pthread_self() << " number = " << number
              << std::endl;
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}

void *funcB_num(void *arg) {
  for (int i = 0; i < MAX; i++) {
    pthread_mutex_lock(&mutex);
    int cur = number;
    cur++;
    number = cur;
    std::cout << "funcB,id = " << pthread_self() << " number = " << number
              << std::endl;
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}

int main(int argc, const char *argv[]) {
  pthread_t p1, p2;
  //初始化锁
  pthread_mutex_init(&mutex, NULL);
  //创建两个子线程
  pthread_create(&p1, NULL, funcA_num, NULL);
  pthread_create(&p2, NULL, funcB_num, NULL);
  //阻塞，释放线程
  pthread_join(p1, NULL);
  pthread_join(p2, NULL);
  //释放锁
  pthread_mutex_destroy(&mutex);
}