#include <kernel/types.h>
#include <kernel/stat.h>
#include <kernel/fcntl.h>
#include <user/user.h>

//cat（英文全拼：concatenate）命令用于连接文件并打印到标准输出设备上
//grep 命令用于查找文件里符合条件的字符串
//ls（英文全拼：list files）命令用于显示指定工作目录下之内容（列出目前工作目录所含之文件及子目录)
//echo 指令用于字符串的输出


inline int print(int fd, char *str){
	return write(fd, str, strlen(str));
}

//将左边的"|"换为"\0",返回字符串的其余部分或NULL
char *simple_tok(char *p, char d){
	while (*p != '\0' && *p != d)
		p++;
	if (*p == '\0')
	{
		return 0;
	}
	*p = '\0';
	return p + 1;
}

char *trim(char *c){
	char *e = c;
	while (*e)
		e++;
	while (*c == ' ')
		*(c++) = '\0';
	while (*(--e) == ' ');
	*(e + 1) = '\0';
	return c;
}

void redirect(int k, int pd[]){
	close(k);
	//dup用来复制参数oldfd所指的文件描述符
	dup(pd[k]);
	close(pd[0]);
	close(pd[1]);
}

char cmd_buf[1024];
char *a, *n;

void handle(char *cmd){
	char buf[32][32];
	char *pass[32];
	int argc = 0;

	cmd = trim(cmd);
	for (int i = 0; i < 32; i++)
	{
		pass[i] = buf[i];
	}
	char *c = buf[argc];
	int input_pos = 0, output_pos = 0;
	for (char *p = cmd; *p; p++)
	{
		if (*p == ' ' || *p == '\n')
		{
			*c = '\0';
			argc++;
			c = buf[argc];
		}
		else {
			if (*p == '<'){
				input_pos = argc + 1;
			}
			if (*p == '>')
			{
				output_pos = argc + 1;
			}
			*c++ = *p;
		}
	}
	*c = '\0';
	argc++;
	pass[argc] = 0;

	if (input_pos)
	{
		close(0);
		open(pass[input_pos], O_RDONLY);
	}
	if (output_pos)
	{
		close(1);
		open(pass[output_pos], O_WRONLY | O_CREATE);
	}

	char *pass2[32];
	int argc2 = 0;
	for (int pos = 0; pos < argc; pos++)
	{
		if (pos == input_pos - 1)
			pos += 2;
		if (pos == output_pos - 1)
			pos += 2;
		pass2[argc2++] = pass[pos];
	}
	pass2[argc2] = 0;

	if (fork())
	{
		wait(0);
	}
	else{
		exec(pass2[0], pass2);
	}
}

void handle_cmd(){
	if (a)
	{
		//创建管道
		int pd[2];
		pipe(pd);
		if (!fork())
		{
			if (n)
				redirect(1, pd);
			handle(a); 
		}
		else if (!fork())
		{
			if (n)
			{
				redirect(0, pd);
				a = n;
				n = simple_tok(a, '|');
				handle_cmd();
			}
		}
		close(pd[0]);
		close(pd[1]);
		wait(0);
		wait(0);
	}
	exit(0);
}

int main(int argc, char const *argv[])
{
	while (1){
		//"@"作为命令行输入提示符，初始化内存块
		print(1, "@ ");
		memset(cmd_buf, 0, 1024);
		gets(cmd_buf, 1024);

		if (cmd_buf[0] == 0)
		{
			exit(0);
		}
		*strchr(cmd_buf, '\n') = '\0';
		if (fork())
		{
			wait(0);
		}
		else{
			a = cmd_buf;
			n = simple_tok(a, '|');
			handle_cmd();
		}
	}
	exit(0);
}