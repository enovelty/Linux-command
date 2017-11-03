//添加注释测试
//在linux下环境实测能够运行 

#include <stdio.h>
#include <unistd.h>		//
#include <stdlib.h>		//exit
#include <dirent.h>		//opendir, readdir, closedir
#include <sys/stat.h>	//lstat
#include <sys/types.h>	//uid_t, time_t等等_t 
#include <pwd.h>		//getpwuid  
#include <grp.h>		//getgrgid
#include <time.h>		//tm

int print_type(mode_t st_mode);
void print_perm(mode_t st_mode);
void print_link(nlink_t st_nlink);
void print_usrname(uid_t st_uid);
void print_grname(gid_t st_gid);
void print_time(time_t time);
void print_filename(struct dirent *currentdp);


struct stat currentstat;

int main(int argc, char*argv[])
{
	struct dirent *currentdp;
	const char *buf;
	DIR *currentdir;

	if(argc!=2)  
		exit(1);

	buf = argv[1];
	
	if ((currentdir = opendir(buf)) == NULL){
		printf("open directory fail\n");
		return 0;
	}
	
	while ((currentdp = readdir(currentdir)) != NULL){
		if (currentdp->d_name[0] != '.'){
			
			if (lstat(currentdp->d_name, &currentstat) == -1){
				printf("get stat error\n");
				continue;
			}
			
			print_type(currentstat.st_mode);	//文件类型 
			print_perm(currentstat.st_mode);	//文件权限 
			print_link(currentstat.st_nlink);	//硬连接数 
			print_usrname(currentstat.st_uid);	//所有者名 
			print_grname(currentstat.st_gid);	//所在组名 
			printf("%6ld ", currentstat.st_size);	//文件大小  
			print_time(currentstat.st_mtime);	//修改时间 
			print_filename(currentdp);		//文件名 
			printf("\n");
		}
	}//while

	closedir(currentdir);
	
	return 0;
}

//文件类型
int print_type(mode_t st_mode)	//这段照搬的PPT，此处是带参数的宏
{   
	if		(S_ISREG(st_mode))
		printf("-");
	else if (S_ISDIR(st_mode))
		printf("d");
	else if (S_ISCHR(st_mode))
		printf("c");
	else if (S_ISBLK(st_mode))
		printf("b");
	else if (S_ISFIFO(st_mode))
		printf("p");
	else if (S_ISLNK(st_mode))
		printf("l");
	else if (S_ISSOCK(st_mode))
		printf("s");	
	
	return 0;   
}

//文件权限 
void print_perm(mode_t st_mode){	//这段代码判断条件需要注意
	if ((S_IRUSR &st_mode) == S_IRUSR){		
        printf("r");
		}
	else printf("-");
	if ((S_IWUSR &st_mode) == S_IWUSR){
        printf("w");
		}
	else printf("-"); 
	if ((S_IXUSR &st_mode) == S_IXUSR){
        printf("x");
		}
	else printf("-");
	if ((S_IRGRP &st_mode) == S_IRGRP){
        printf("r");
		}
	else printf("-");
	if ((S_IWGRP &st_mode) == S_IWGRP){
        printf("w");
		}
	else printf("-");
	if ((S_IXGRP &st_mode) == S_IXGRP){
        printf("x");
		}
	else printf("-");
	if ((S_IROTH &st_mode) == S_IROTH){
        printf("r");
		}
	else printf("-");
	if ((S_IWOTH &st_mode) == S_IWOTH){
        printf("w");
		}
	else printf("-");
	if ((S_IXOTH &st_mode) == S_IXOTH){
        printf("x");
		}
	else printf("-");
	
	printf(" "); 
}

//硬链接数 
void print_link(nlink_t st_nlink){
	printf("%ld ", currentstat.st_nlink);
}

//文件所有者名字 
void print_usrname(uid_t st_uid){
	struct passwd *p = getpwuid(st_uid);	//passd结构体
	printf("%s ", p->pw_name);
}

//所在组名字 
void print_grname(gid_t st_gid){
	struct group *p = getgrgid(st_gid);	//group结构体
	printf("%s ", p->gr_name);
}

//修改时间
void print_time(time_t time){
	struct tm *localtime(const time_t *clock);
	struct tm *t = localtime(&currentstat.st_mtime);	//注意这里是&
	printf("%d-%.2d-%.2d %2d:%2.2d "	//这段是从网上看到
			,t->tm_year + 1900
			,t->tm_mon + 1
			,t->tm_mday
			,t->tm_hour
			,t->tm_min);
}

//文件名
void print_filename(struct dirent *currentdp){
	printf("%s", currentdp->d_name);
}
