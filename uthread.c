#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"

/*
 * This is where you'll need to implement the user-level functions
 */

uint stack1[64]; // set stack size to max num of processes 

void lock_init(lock_t *lock) {
  lock->locked = 0;
}

void lock_acquire(lock_t *lock) {
  while(xchg(&lock->locked, 1) != 0);
}

void lock_release(lock_t *lock) {
  xchg(&lock->locked, 0);
}

int thread_join(int pid) {
  int t_num;
  if((t_num = join(pid)) != -1){
    free((void *)stack1[t_num]);
  }
  return t_num;
}

int thread_create(void (*start_routine)(void *), void *arg) {
  void *stack2;
  int final_pid;
  stack2 = malloc(2*4096); // 2x pagesize 

  if((uint)stack2 % 4096){
    stack2 = stack2 + (4096 - (uint)stack2 % 4096);
  }
  
  final_pid = clone(start_routine, arg, stack2);

  if((final_pid = clone(start_routine, arg, stack2)) < 0 && final_pid < 64){
    return -1;
  }
  
  stack1[final_pid] = (uint)stack2;
  
  return final_pid;
}


