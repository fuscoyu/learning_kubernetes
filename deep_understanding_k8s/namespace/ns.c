#define _GNU_SOURCE
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>


#define STACK_SIZE (1024 * 1024)

static char container_stack[STACK_SIZE];


char* const container_args[] = {
  "/bin/bash",
  NULL
};

int container_main(void* arg)
{
  printf("Container[%5d] - inside the container!\n", getpid());
  sethostname("container",10);
  system("mount -t proc proc /proc");
  execv(container_args[0], container_args);
  printf("Something's wrong!\n");
  return 1;
}

int main()
{
  printf("Parent[%5d] - start a container!\n", getpid());
  // UTS 命名空间来说，它隔离的是主机名和域名
  // IPC 用于隔离进程间通信的资源，包括进程间通信的信号量、消息队列和共享内存等。 
  // PID
  // Mount
  int container_pid = clone(container_main, container_stack+STACK_SIZE, CLONE_NEWUTS | CLONE_NEWIPC | CLONE_NEWPID | CLONE_NEWNS | SIGCHLD , NULL);
  waitpid(container_pid, NULL, 0);
  printf("Parent - container stopped!\n");
  return 0;

}
