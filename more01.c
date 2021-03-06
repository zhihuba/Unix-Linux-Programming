#include <stdio.h>
#define PAGELEN 24  		//一页显示多少行
#define LINELEN 512			//一行最多显示多少字符

void do_more(FILE *);
int see_more(FILE *);

int main(int argc, char *argv[])
{
	FILE *fp;
	if(argc == 1)			
	{
		//more的用法1： $command | more
		do_more(stdin);  //stdin标准输入
	}   
	else
	{
		//more的用法2： $more filename
		while(--argc)
		{
			//一个一个文件显示，但如果真有多个文件的话，缺少了打印文件名
			if((fp = fopen(*++argv, "r")) != NULL)
			{
				do_more(fp);
				fclose(fp);
			}
			else
			{
				exit(1);
			}
		}
	}
	return 0;
}

///读取PAGELEN行，然后调用see_more（）等待用户输入，来确定下一步的显示内容
void do_more(FILE *fp)
{
	char line[LINELEN];		//存放读来的一行数据，一行最多读512个字符
	int num_of_lines = 0;
	int reply;
	FILE *fp_tty;		//加入这个，是对于操作$command | more的从键盘获取输入的处理
	fp_tty = fopen("/dev/tty", "r");	
	//键盘和显示器的设备描述文件，向这个文件写相当于显示在用户屏幕上，读相当于从键盘获取用户的输入
	if(fp_tty == NULL)
		exit(1);
	while( fgets(line, LINELEN, fp) )
		//从文件结构体指针fp中读取数据，每次读取一行。读取的数据保存在line指向的字符数组中，
		//每次最多读取LINELEN-1个字符（第LINELEN个字符赋'\0'）
	{
		//成功读取一行
		if(num_of_lines == PAGELEN)
		{
			//读完一屏后，获取用户输入
			reply= see_more(fp_tty);	//	获取键盘输入
			if(reply == 0)
				break;
			num_of_lines -= reply;
		}
		if(fputs(line, stdout) == EOF)	//向指定的文件写入一个字符串,成功的话返回非负整数，失败的话返回EOF（-1）
			exit(1);
		num_of_lines++;
	}
}

///从键盘获得输入，空格：显示一屏，Enter：显示一行，q：退出
int see_more(FILE *ttyfp)
{
	int c;
	printf("\033[7m more? \033[m");   //反白字符表示
	while( (c=getc(ttyfp)) != EOF)
	{
		if(c == 'q')
			return 0;
		if(c == ' ')		
			return PAGELEN;		//使得num_of_lines=0；重新输出一屏
		if(c == '\n')
			return 1;			//使得num_of_lines=23；重新输出一行
	} 
	return 0;
}