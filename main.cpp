#include <iostream>
#include <pthread.h>
#include <unistd.h>

using namespace std;
struct cmp {
  int a;
  int b;
};

void *callback(void *arg) {
  for (int i = 0; i < 5; i++) {
    cout << "子线程:i =" << i << endl;
  }
  cout << "子线程ID: " << pthread_self() << endl;
  cmp *ta = (cmp *)arg;
  ta->a = 1;
  ta->b = 101;
  pthread_exit(ta);
  return NULL;
}

int main() {

  pthread_t tid;
  cmp ss;
  pthread_create(&tid, NULL, callback, &ss);
  for (int i = 1; i <= 5; i++) {
    cout << "主线程i= " << i << endl;
  }
  void *ptr = NULL;
  pthread_join(tid, &ptr);
  // cmp *pp = (cmp *)ptr;
  cout << ss.a << " " << ss.b << endl;
  cout << "主线程ID: " << pthread_self() << endl;
  pthread_detach(tid);
  pthread_exit(NULL);
  return 0;
}