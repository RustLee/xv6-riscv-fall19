#include "kernel/types.h"
#include "user/user.h"

void func(int *input, int num){
	if(num == 1){
		printf("prime %d\n", *input);
		return;
	}
	int p[2],i;
	int prime = *input;
	int temp;
	printf("prime %d\n", prime);
	//递归fork进程并使用管道链接，形成一条pipeline来对素数进行过滤
	pipe(p);
    if(fork() == 0){
        for(i = 0; i < num; i++){
            temp = *(input + i);
			write(p[1], (char *)(&temp), 4);
		}
        exit();
    }
	close(p[1]);
	//每个进程收到的第一个数p一定是素数，后续的数如果能被p整除则之间丢弃，如果不能则输出到下一个进程
	if(fork() == 0){
		int counter = 0;
		char buffer[4];
		while(read(p[0], buffer, 4) != 0){
			temp = *((int *)buffer);
			if(temp % prime != 0){
				*input = temp;
				input += 1;
				counter++;
			}
		}
		func(input - counter, counter);
		exit();
    }
	wait();
	wait();
}

int main(){
    int input[34];
	int i = 0;
	for(; i < 34; i++){
		input[i] = i+2;
	}
	func(input, 34);
    exit();
}
