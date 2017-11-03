#include <stdio.h>
#include <pthread.h>	//pthread
#include <unistd.h>		//fork,execl,getcwd
#include <stdlib.h>		//exit
#include <dirent.h>		//opendir, readdir, closedir
#include <sys/stat.h>	//lstat,mkdir
#include <sys/types.h>	//uid_t, time_t等等_t 
#include <pwd.h>		//getpwuid  
#include <grp.h>		//getgrgid
#include <time.h>		//tm
#include <sys/wait.h>	//wait
#include <string.h>		//strmp

void ls(void);
void cp(void);
//cp -r
int is_dir(char* path);
void copy_file(char* source_path,char *destination_path);
void copy_folder(char *source_path,char *destination_path);
//ls -l
int print_type(mode_t st_mode);
void print_perm(mode_t st_mode);
void print_link(nlink_t st_nlink);
void print_usrname(uid_t st_uid);
void print_grname(gid_t st_gid);
void print_time(time_t time);
void print_filename(struct dirent *currentdp);

char file_source_path[1024] = {0}; 
char file_destination_path[1024] = {0}; 
struct stat currentstat;

int main(){
	pthread_t ls_t;		//*************** 注意线程号不能与函数名重名 ***************************
	
	if ((pthread_create(&ls_t, NULL, (void *)ls, NULL)) != 0){
		printf("线程创建失败\n");
		exit(0);
	}
	pthread_join(ls_t, NULL);

	return 0;
}

void ls(void)
{
	struct dirent *currentdp;
	DIR *currentdir;
	pthread_t cp_t;
	
	if ((currentdir = opendir(".")) == NULL){
		printf("open directory fail\n");
		exit(0);
	}
	
	mkdir("/home/zero/liang/", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);		//*********
	
	while ((currentdp = readdir(currentdir)) != NULL){
		if (currentdp->d_name[0] != '.'){
			
			if (lstat(currentdp->d_name, &currentstat) == -1){
				printf("get stat error\n");
				continue;
			}
				 
        	snprintf(file_source_path, sizeof(file_source_path), "%s/%s", ".", currentdp->d_name); 
        	snprintf(file_destination_path, sizeof(file_destination_path), "%s/%s", "/home/zero/liang", currentdp->d_name);
        	
        	if((pthread_create(&cp_t, NULL, (void *)cp, NULL)) != 0){
        		printf("线程创建失败\n");
				exit(0);
        	}
        	pthread_join(cp_t, NULL);
			
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

}

void cp(void){
	if (is_dir(file_source_path)){
		copy_folder(file_source_path, file_destination_path);
	}
	else {
		copy_file(file_source_path, file_destination_path);
	}

}

//文件类型
int print_type(mode_t st_mode)
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
void print_perm(mode_t st_mode){
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
	struct passwd *p = getpwuid(st_uid);
	printf("%s ", p->pw_name);
}

//所在组名字 
void print_grname(gid_t st_gid){
	struct group *p = getgrgid(st_gid);
	printf("%s ", p->gr_name);
}


//修改时间
void print_time(time_t time){
	struct tm *localtime(const time_t *clock);
	struct tm *t = localtime(&currentstat.st_mtime);
	printf("%d-%.2d-%.2d %2d:%2.2d "
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

int is_dir(char* path){
	struct stat st;
    stat(path,&st);  
    if(S_ISDIR(st.st_mode)){  
        return 1;  
    }  
    else{  
        return 0;  
    }  
}

void copy_file(char *source_path, char *destination_path){//复制文件  
    char buffer[1024];  
    FILE *in,*out;//定义两个文件流，分别用于文件的读取和写入
    if((in=fopen(source_path,"r"))==NULL){//打开源文件的文件流  
        printf("源文件打开失败！\n");  
        exit(1);  
    }  
    if((out=fopen(destination_path,"w"))==NULL){//打开目标文件的文件流  
        printf("目标文件创建失败！\n");  
        exit(1);  
    }  
    int len;//len为fread读到的字节长  
    while((len=fread(buffer,1,1024,in))>0){//从源文件中读取数据并放到缓冲区中，第二个参数1也可以写成sizeof(char)  
        fwrite(buffer,1,len,out);//将缓冲区的数据写到目标文件中  
    }  
    fclose(out);  
    fclose(in);  
}

void copy_folder(char *source_path,char *destination_path){//复制文件夹 
    DIR* des_dir =  opendir(destination_path);
    if(!des_dir){  
        if (mkdir(destination_path, 0777))//如果不存在就用mkdir函数来创建  
        {  
            printf("创建文件夹失败！");  
        }  
    }  
    struct dirent* filename;
    DIR* dp=opendir(source_path);//用DIR指针指向这个文件夹  
    while((filename = readdir(dp)) != NULL){//遍历DIR指针指向的文件夹，也就是文件数组。   
        char file_source_path[1024] = {0};  
        snprintf(file_source_path,sizeof(file_source_path),"%s/%s",source_path,filename->d_name);
        
        char file_destination_path[1024] = {0};  
        snprintf(file_destination_path,sizeof(file_destination_path),"%s/%s",destination_path,filename->d_name);
		if(is_dir(file_source_path)){//如果是目录  
            if((strcmp(filename->d_name,".") != 0 )&& (strcmp(filename->d_name,"..") != 0 )){//同时并不以.结尾，因为Linux在所有文件夹都有一个.文件夹用于连接上一级目录，必须剔除，否则进行递归的话，后果无法相像  
                copy_folder(file_source_path,file_destination_path);//进行递归调用，相当于进入这个文件夹进行复制～  
            }         
        }  
        else{  
            copy_file(file_source_path,file_destination_path);//否则按照单一文件的复制方法进行复制。  
            printf("复制%s到%s成功！\n",file_source_path,file_destination_path);  
        }  
    }//while

    if (!des_dir) {
        closedir(des_dir);
    }
    if (!dp) {
        closedir(dp); 
    }  
}


