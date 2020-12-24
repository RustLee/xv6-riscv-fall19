#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char const *argv[])
{
	int parent_fd[2],child_fd[2];
	char buffer[] = {'X'};
	long length = sizeof(buffer);
	//两次调用pipe()创建两个管道，分别对应两个方向。
	pipe(parent_fd);
	//使用fork()创建子进程，在子进程中先从管道1 read()再向管道2 write()，父进程中则与之相反
	pipe(child_fd);
	if(fork() == 0){
		close(parent_fd[1]);
		close(child_fd[0]);
		if (read(parent_fd[0],buffer,length) != length)
		{
			printf("a--->b read error!\n");
			exit();
		}
		printf("%d: received ping\n",getpid());
		if (write(child_fd[1],buffer,length) != length)
		{
			printf("a<---b write error!\n");
			exit();
		}
		exit();
	}
	close(parent_fd[0]);
	close(child_fd[1]);
	if(write(parent_fd[1],buffer,length) != length){
		printf("a--->b write error!\n");
		exit();
	}
	if(read(child_fd[0],buffer,length) != length){
		printf("a<---b read error!\n");
		exit();
	}
	printf("%d: received pong\n",getpid());
	wait();
	exit();
}