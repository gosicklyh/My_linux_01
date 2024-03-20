#include "threadpool.h"
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <pthread.h>
//任务结构体
struct Task {
  void (*function)(void *arg);
  void *arg;
};

//线程池结构体
struct ThreadPool {
  // 任务队列
  Task *taskQ;
  int queueCapacity; //容量
  int queueSize;     //当前任务个数
  int queueFront;    //队头->取数据
  int queueRear;     //队尾->放数据

  pthread_t managerID;  //管理者线程ID
  pthread_t *threadIDs; //工作的ID
  int minNum;           //最小线程数
  int maxNum;           //最大线程数
  int busyNum;          //忙的线程数
  int liveNum;          //存活线程数
  int exitNum;          //销毁线程数

  pthread_mutex_t mutexPool; //锁整个线程池
  pthread_mutex_t mutexBusy; //锁busyNum变量
  pthread_cond_t notFull;    //任务队列是否为满
  pthread_cond_t notEmpty;   //任务队列是否为空

  int shutdown; // 是不是要销毁线程池, 销毁为1, 不销毁为0
};

ThreadPool *threadPkklCreate(int min, int max, int queueSize) {

  ThreadPool *pool = (ThreadPool *)malloc(sizeof(ThreadPool)); //开辟空间
  do {
    if (pool == NULL) {
      std::cout << "创建失败...\n";
      break;
    }

    pool->threadIDs = (pthread_t *)malloc(sizeof(pthread_t) * max);
    if (pool->threadIDs == NULL) {
      std::cout << "创建失败...\n";
      // delete pool;
      break;
    }
    memset(pool->threadIDs, 0, sizeof(pthread_t) * max);
    pool->minNum = min;
    pool->maxNum = max;
    pool->busyNum = 0;
    pool->liveNum = min; // 和最小个数相等
    pool->exitNum = 0;

    if (pthread_mutex_init(&pool->mutexPool, NULL) != 0 ||
        pthread_mutex_init(&pool->mutexBusy, NULL) != 0 ||
        pthread_cond_init(&pool->notEmpty, NULL) != 0 ||
        pthread_cond_init(&pool->notFull, NULL) != 0) {
      printf("mutex or condition init fail...\n");
      break;
    }
    //初始化任务队列
    pool->taskQ = (Task *)malloc(sizeof(Task) * queueSize);
    pool->queueCapacity = queueSize;
    pool->queueCapacity = queueSize;
    pool->queueFront = 0;
    pool->queueSize = 0;
    pool->queueRear = 0;
    pool->shutdown = 0;

    // 创建线程
    pthread_create(&pool->managerID, NULL, manager, NULL);
    for (int i = 0; i < min; i++) {
      pthread_create(&pool->threadIDs[i], NULL, worked, NULL);
    }
    return pool;
  } while (0);
  if (pool->threadIDs && pool)
    free(pool->threadIDs);
  if (pool && pool->taskQ)
    free(pool->taskQ);
  if (pool)
    free(pool);

  return NULL;
}
void *worker(void *arg) {
  ThreadPool *pool = (ThreadPool *)arg;
  while (1) {
    pthread_mutex_lock(&pool->mutexPool);
    //判断当前任务队列是否为空
    while (pool->queueSize == 0 && !pool->shutdown) {
      //阻塞工程线程
      pthread_cond_wait(&pool->notEmpty, &pool->mutexPool);
    }
    //判断线程池是否被关闭
    if (pool->shutdown) {
      pthread_mutex_unlock(&pool->mutexPool);
      pthread_exit(NULL);
    }
    //从任务队列取出一个任务
    Task task;
    task.function = pool->taskQ[pool->queueFront].function;
    task.arg = pool->taskQ[pool->queueFront].arg;
    //移动头节点
    pool->queueFront = (pool->queueCapacity + 1) % pool->queueCapacity;
    pool->queueSize--;
    // 解锁
    pthread_mutex_unlock(&pool->mutexPool);

    pthread_mutex_lock(&pool->mutexBusy);
    pool->busyNum++;
    pthread_mutex_unlock(&pool->mutexBusy);
    task.function(task.arg);
    free(task.arg);
    task.arg = NULL;
    std::cout << "-------";
    pthread_mutex_lock(&pool->mutexBusy);
    pool->busyNum--;
    pthread_mutex_unlock(&pool->mutexBusy);
  }
}
