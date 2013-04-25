#include<stdio.h>                          
#include<stdlib.h>
#include<string>
#include<iostream>
#include<windows.h>
#include "head.h"
using namespace std;

//创建文件系统
void create_file_system()                                     
{
	fp=fopen(file_system_name,"wb+");              //以读写方式打开，如果存在，则覆盖原来信息，
	if(fp==NULL)                                   //否则创建该文件    
	{
		printf("Create file system error!\n");
		exit(1);
	}
                                                   //申请空间
	int total=sizeof(int)*(DIR_NUM+FILE_NUM+BLOCK_NUM+3)+sizeof(struct dir_node)*DIR_NUM+sizeof(struct file_node)*FILE_NUM+BLOCK_SIZE*BLOCK_NUM;
	for(long len=0;len<total;len++)
		fputc(0,fp);

	fseek(fp,0,SEEK_SET);                          //写超级块信息
	used_dir=1;
	fwrite(&used_dir,sizeof(int),1,fp);	
	used_file=0;
	used_block=0;
	fwrite(&used_file,sizeof(int),2,fp);
	dir_flag[0]=1;
	fwrite(&dir_flag[0],sizeof(int),1,fp);         //标志对应目录节点、文件节点的使用情况
	fwrite(&dir_flag[1],sizeof(int),DIR_NUM+FILE_NUM+BLOCK_NUM-1,fp);

	strcpy(dir[0].dir_name,"C:");                  //写根目录信息
	dir[0].dir_count=0;
	dir[0].file_count=0;
	dir[0].parent=-1;
	GetLocalTime(&dir[0].ctime);                   //获得当前时间
	fwrite(&dir[0],sizeof(struct dir_node),1,fp);
                                                   
	for(int i=0;i<DIR_NUM;i++)                     //将所有标记清零
	{
		al_dir[i]=0;
		al_dflag[i]=0;
		dir[i].dir_count=0;
		dir[i].file_count=0;
	}
	for(int i=0;i<FILE_NUM;i++)                
	{
		al_file[i]=0;
		al_fflag[i]=0;
		open_files[i]=0;
	}
	for(int i=0;i<BLOCK_NUM;i++)                
		al_bflag[i]=0;

	for(int i=0;i<BUF_NUM;i++)
		buffer[i].flag=0;

	fflush(fp);
	fclose(fp);
}
//搜索目录或文件，成功则返回该文件或目录的索引失败则返回-1，
//type用来区分要搜索的是文件还是目录0表示目录，1表示文件，
//index用来指定找到的文件或目录在父目录下的位置
int search(int parent,char *name,int type,int &index) 
{                                                      
	struct dir_node *p=&dir[parent];               
	int i,temp;                                    
	if(type==0)    //表示目录                                                
		for(i=0;i<p->dir_count;i++)
		{
			temp=p->child_dir[i];
			if(strcmp(name,dir[temp].dir_name)==0)// 字符串相等时 
			{
				index=i;
				return temp;
			}
		}
	else
		for(i=0;i<p->file_count;i++)//表示找文件 
		{
			temp=p->child_file[i];
			if(strcmp(name,file[temp].file_name)==0)
			{
				index=i;
				return temp;
			}
		}

	return -1;
}

//根据输入的字符串，获得父目录及真正的文件或目录名，引用参数p用来指定目录或文件名的起始位置
int get_parent(char *name,int &p)         
{                                          
	char buf[32],*path,*s;                         
	int pos,start=0,index;
	path=new char[128];
	strcpy(path,name);

	s=strrchr(path,'/');                   //如果没有指定路径，则将当前目录指定为父目录
	if(s==NULL)                              
		return curr;

	pos=(int)(s-path);
	p=pos+1;
	if(p==(int)strlen(path))
		return -1;
	path[p]='\0';
	
	s=strchr(path,'/');
	pos=(int)(s-path);
	if(strncmp(path,"C:",pos)!=0)                  //前面没有跟根目录名，则从当前目录往下搜索
		start=curr;
	else	
		path+=3;
	while(start!=-1 && (int)strlen(path)>0)        //进行搜索，知道路径名结束或出现不匹配
	{
		s=strchr(path,'/');
		pos=(int)(s-path);
		strncpy(buf,path,pos);
		buf[pos]='\0';
		start=search(start,buf,0,index);
		path+=pos+1;
	}
	return start;                                 
}                                                  

//为文件申请磁盘块，一次调用申请一块，成功则返回磁盘块索引，失败返回-1 
int get_block(int pos)                                                
{                                                  
	for(int i=0;i<BLOCK_NUM;i++)
		if(block_flag[i]==0)
		{                                          //修改磁盘块使用情况及文件的控制信息
			block_flag[i]=1;
			al_bflag[i]++;               //目录节点占用标志
			used_block++;                //已用的磁盘块数
			int top=file[pos].block_count;
			file[pos].block[top]=i;
			file[pos].block_count++;
			return i;
		}
	return -1;
}

//释放文件占用的磁盘块，用于删除文件时用
void return_block(int pos)                        
{
	for(int i=0;i<file[pos].block_count;i++)
	{
		int temp=file[pos].block[i];
		block_flag[temp]=0;
		al_bflag[temp]++;
	}
	used_block-=file[pos].block_count;
}

int get_dir(int parent,char *dir_name)             //创建目录节点，成功则返回目录的索引，失败则返回-1
{
	int index;
	if(search(parent,dir_name,0,index)!=-1)        //搜索在父目录下是否有同名目录，有则创建失败        
	{
		printf("Directory name repeated!\n");
		return -1;
	}
	for(int i=1;i<DIR_NUM;i++)                     //搜索空闲的目录节点           
		if(dir_flag[i]==0)
		{                                          //登记并给目录节点初始化
			dir_flag[i]=1; 
			al_dflag[i]++;
			al_dir[i]++;
			used_dir++;
			strcpy(dir[i].dir_name,dir_name);
			dir[i].dir_count=0;
			dir[i].file_count=0;
			dir[i].parent=parent;
			GetLocalTime(&dir[i].ctime);           //获得当前时间
			return i;
		}
	return -1;
}
//创建文件节点，成功则返回文件的索引号，失败返回-1
int get_file(int parent,char *file_name)          
{
	int index;
	if(search(parent,file_name,1,index)!=-1)       //搜索在父目录下是否有同名文件存在，有则创建失败
	{
		printf("File name repeated!\n");
		return -1;
	}
	for(int i=0;i<FILE_NUM;i++)                    //搜索空闲的文件节点             
		if(file_flag[i]==0)
		{
			strcpy(file[i].file_name,file_name);
			file[i].block_count=0;
			if(get_block(i)==-1)                   //给新创建的文件申请磁盘块，如果失败，创建文件           
			{                                      //将失败
				printf("Disk volumn error!\n");
				return -1;
			}
			file_flag[i]=1;
			al_fflag[i]++;                         //登记并给文件节点初始化
			al_file[i]++;
			used_file++;
			file[i].file_length=0;
			file[i].parent=parent;
			GetLocalTime(&file[i].ctime);          //获得当前时间
			return i; 
		}
	return -1;
}

//给文件申请缓冲区，打开文件时调用，失败时返回-1
int get_buffer(int pos)
{                            
	for(int i=0;i<BUF_NUM;i++)
		if(buffer[i].flag==0)                      //buffer[] 缓冲区数组    
		{
			buffer[i].flag=1;                      //修改使用标志并初始化
			buffer[i].file_id=pos;
			buffer[i].length=0;
			buffer[i].offset=0;
			return i;
		}
	return -1;
}
 // 得到被打开文件的缓冲区索引号，失败则返回-1
int get_buffer_id(int pos)                        
{
	for(int i=0;i<BUF_NUM;i++)
		if(buffer[i].flag==1 && buffer[i].file_id==pos)
			return i;
	return -1;
}

 // 在指定的目录下创建文件，如果创建成功则返回文件的索引号，否则返回-1
//	如果父目录已满，则创建失败
int create_file(int parent,char *file_name)         
{                                                  
	if(dir[parent].file_count==16)                 
	{
		printf("Parent directory is full!\n");
		return -1;
	}					
	int pos=get_file(parent,file_name);            //开始创建文件
	if(pos==-1)
	{
		printf("Create file error!\n");
		return -1;
	}
                                           
	struct dir_node *p=&dir[parent];               //修改父目录的控制信息
	int top=p->file_count;              
	p->child_file[top]=pos;
	p->file_count++;
	al_dir[parent]++;

	return pos;
}

 //在指定的目录下创建目录，如果成功则返回目录的索引号，否则返回-1 
//如果父目录已满，则创建失败
int create_dir(int parent,char *dir_name)                
{                                                  
	if(dir[parent].dir_count==DIR_NUM-1)     //如果父目录已满，则创建失败               
	{
		printf("Parent directory is full!\n");
		return -1;
	}
	int pos=get_dir(parent,dir_name);              //开始创建目录
	if(pos==-1)
	{
		printf("Create directory error!\n");
		return -1;
	}

	struct dir_node *p=&dir[parent];               //修改父目录的控制信息
	int top=p->dir_count;         
	p->child_dir[top]=pos;
	p->dir_count++;
	al_dir[parent]++;

	return pos;
}

// 创建目录的主调函数，它的参数只有用户输入
//如果创建成功，则返回目录的索引号，否则返回-1
int md(char *name)                                
{                                                  
	int parent,p=0;                                
	parent=get_parent(name,p);
	if(parent==-1)                                 //父目录找不到，输入有误，创建失败
	{
		printf("Path name error!\n");
		return -1;
	}
	return create_dir(parent,name+p);              //开始创建目录
}

 // 创建文件的主调函数，直接接受用户输入成功返回文件的索引号，否则返回-1
int create(char *name)                             
{                                               
	int parent,p=0;
	parent=get_parent(name,p);
	if(parent==-1)                                 //找不到父目录，输入错误，创建失败
	{
		printf("Path name error!\n");
		return -1;
	}
	return create_file(parent,name+p);             //开始创建文件
}

// 删除文件，调用该函数的前提是已经取得要删除文件的索引号
void del_file(int pos)                                              
{                                                  
	return_block(pos);                             //释放磁盘块
	file_flag[pos]=0; 
	used_file--;
	al_fflag[pos]++;
	al_file[pos]=0;

	if(open_files[pos]>0)                          //如果已经获得缓冲区，则立即释放
	{
		int buf_id=get_buffer_id(pos);
		buffer[buf_id].flag=0;
		open_files[pos]=0;
	}
}


//在指定的目录下删除文件，删除成功则返回文件的索引号，否则返回-1   
int del_file(int parent,char *file_name)                       
{                                                  
	int del_pos,index;
	if((del_pos=search(parent,file_name,1,index))==-1)//搜索该文件是否存在，不存在则删除失败
	{
		printf("The file to delete not exist!\n");
		return -1;
	}
	del_file(del_pos);                             //开始删除文件

	struct dir_node *p=&dir[parent];               //修改父目录的控制信息
	if(p->file_count>=2)
	{
		int top=p->file_count-1;         	
		p->child_file[index]=p->child_file[top];
	}
	p->file_count--;
	al_dir[parent]++;

	return del_pos;
}
//删除文件的主调函数  
int delfile(char *name)                           
{
	int parent,p=0;
	parent=get_parent(name,p);
	if(parent==-1)
	{
		printf("Path name error!\n");
		return -1;
	}
	return del_file(parent,name+p);
}

void open_file(int pos)                                              
{                                                  
	
}

//在指定的目录下删除文件，删除成功则返回文件的索引号，否则返回-1   
int open_file(int parent,char *file_name)                       
{                                                  
	int open_pos,index;
	if((open_pos=search(parent,file_name,1,index))==-1)//搜索该文件是否存在，不存在则删除失败
	{
		printf("The file to delete not exist!\n");
		return -1;
	}
	open_file(open_pos);                             //开始打开文件
	return open_pos;
}



//打开文件的主调函数  
int openfile(char *name)                           
{
	int parent,p=0;
	parent=get_parent(name,p);
	if(parent==-1)
	{
		printf("Path name error!\n");
		return -1;
	}
	return open_file(parent,name+p);
}

//删除指定的目录节点，该目录已经为空
void del_dir(int pos)                                                
{
	dir_flag[pos]=0;  
	used_dir--;
	al_dflag[pos]++;
	al_dir[pos]=0;
}

//删除一个指定目录及它下面的所有文件及所有目录   
void del(int pos)                                                           
{
	for(int i=0;i<dir[pos].file_count;i++)         //删除当前目录下的所有文件
		del_file(dir[pos].child_file[i]);
	for(int i=0;i<dir[pos].dir_count;i++)              //删除子目录
		del(dir[pos].child_dir[i]);
	del_dir(pos);                                  //删除当前目录
}

//在一个指定的目录下删除一个目录及它下面的所有文件及所有目录      
void del_dir(int parent,int del_pos,int index)                    
{                                                  
	del(del_pos);                                  //开始删除目录
	if(dir[parent].dir_count>=2)                   //修改父目录的控制信息
	{
		int top=dir[parent].dir_count-1;
		dir[parent].child_dir[index]=dir[parent].child_dir[top];
	}
	dir[parent].dir_count--;
	al_dir[parent]++;
}

 //用于设置当前路径
void paste(int dir_id)                           
{
	if(dir_id==0)
		return;
	paste(dir[dir_id].parent);
	strcat(curr_path,dir[dir_id].dir_name);
	strcat(curr_path,"/");
}

//改变工作目录，成功则返回该目录的索引号，否则返回-1
int change_dir(char *name)                         
{                                                  
	int parent,p=0,pos,index;
	if(strcmp(name,"/")==0)
		pos=0;
	else if(strcmp(name,"..")==0)
		pos=curr_dir->parent;
	else
	{	
		parent=get_parent(name,p);	
		if(parent==-1)	
		{		
			printf("Path name error!\n");		
			return -1;	
		}
		pos=search(parent,name+p,0,index);
	}
	if(pos==-1)                                    //如果该目录不存在，则失败
	{
		printf("The dictory not exist!\n");
		return -1;
	}
	curr_path[3]='\0';
	paste(pos);
	curr=pos;                                      //改变当前目录及路径
	curr_dir=&dir[curr];                                                 
	return curr;                               
}

//该函数用于打印|用，可以将目录结构以树形显示
int total[DIR_NUM];                                //用于显示目录树时用

void print_sp(int dir_id)                         
{                                                  
	if(dir_id<0)
		return;
	print_sp(dir[dir_id].parent);
	if(dir[dir_id].dir_count>total[dir_id])
		printf("|");
	else
		printf(" ");
	for(int i=0;i<6;i++)
		printf(" ");
}

//该函数在每打印一个目录或文件名之前调用
void print_pre(int id,int type)                    
{
	int parent,pre;
	if(type==0)
		parent=dir[id].parent;
	else
		parent=file[id].parent;
	pre=dir[parent].parent;
	print_sp(pre);
}

//打印文件名
void print_file(int file_id)                       
{
	print_pre(file_id,1);
	printf("|");
	for(int i=0;i<6;i++)
		printf("_");
	printf("%s\n",file[file_id].file_name);
}

 //打印目录名
void print_dir(int dir_id)                         
{
	print_pre(dir_id,0);
	printf("|");
	for(int i=0;i<6;i++)
		printf("_");
	printf("%s\057\n",dir[dir_id].dir_name);
	total[dir[dir_id].parent]++;
}

 //打印树形结构的核心函数
void print(int dir_id)                             
{
	print_dir(dir_id);
	for(int i=0;i<dir[dir_id].file_count;i++)
		print_file(dir[dir_id].child_file[i]);
	for(int i=0;i<dir[dir_id].dir_count;i++)
		print(dir[dir_id].child_dir[i]);
}

//打印树形结构的主调函数
void show_tree()                                  
{
	if(used_file==0 && used_dir==1)
	{
		printf("The file system is empty!\n");
		return;
	}
	printf("%s\057\n",dir[0].dir_name);
	for(int i=0;i<DIR_NUM;i++)
		total[i]=0;
	for(int i=0;i<dir[0].file_count;i++)
		print_file(dir[0].child_file[i]);
	for(int i=0;i<dir[0].dir_count;i++)
		print(dir[0].child_dir[i]);
}

//以下几个函数用于显示当前目录下面的目录和文件，跟上面不同的是，它不能显示目录及文件之间的关系，
//	但它可以显示一些细节，如创建时间，文件长度
// 显示当前目录下的文件和子目录信息
void show_dir(int dir_id)                         
{                                                  
	printf("%s",dir[dir_id].dir_name);             
	for(int i=strlen(dir[dir_id].dir_name);i<20;i++)
		printf(" ");
	printf("<DIR>");
	for(int i=25;i<30;i++)
		printf(" ");
	printf("%d-%.2d-%.2d %.2d:%.2d:%.2d\n",dir[dir_id].ctime.wYear,dir[dir_id].ctime.wMonth,dir[dir_id].ctime.wDay,dir[dir_id].ctime.wHour,dir[dir_id].ctime.wMinute,dir[dir_id].ctime.wSecond);
}

//显示一个文件
void show_file(int file_id)                        
{
	printf("%s",file[file_id].file_name);
	for(int i=strlen(file[file_id].file_name);i<20;i++)
		printf(" ");
	printf("<FILE>");
	for(int i=26;i<30;i++)
		printf(" ");
	printf("%d-%.2d-%.2d %.2d:%.2d:%.2d",file[file_id].ctime.wYear,file[file_id].ctime.wMonth,file[file_id].ctime.wDay,file[file_id].ctime.wHour,file[file_id].ctime.wMinute,file[file_id].ctime.wSecond);
	for(int i=0;i<8;i++)
		printf(" ");
	int length=file[file_id].file_length;
	if(open_files[file_id]==2)                     //如果该文件已经以写的方式被打开，则需改变
	{
		int buf_id=get_buffer_id(file_id);
		length=buffer[buf_id].length;
	}
	printf("len: %d bytes\n",length);
}

//显示目录及文件的核心函数，用了递归思想
void show(int dir_id)                                                     
{
	for(int i=0;i<dir[dir_id].file_count;i++)
		show_file(dir[dir_id].child_file[i]);
	for(int i=0;i<dir[dir_id].dir_count;i++)
		show_dir(dir[dir_id].child_dir[i]);
}

    //显示文件及目录的主调函数
void list()                                      
{ 
	show(curr);                                    //该函数的参数表示根目录的索引
	printf("%d files.",curr_dir->file_count);  //统计文件数
	printf("\n%d dirs.",curr_dir->dir_count);    //统计目录数
}

//写回目录节点、文件节点或磁盘块的占用标志
//type为0时，写目录；
//type为1时，写文件；
//type为2时，写磁盘块
//该函数只有在创建和删除时才调用
void write_bit(int pos,int type)                   
{                                                  
	if(type==0)                                   
	{                                             
		fseek(fp,sizeof(int)*(pos+3),SEEK_SET);    
		fwrite(&dir_flag[pos],sizeof(int),1,fp);
	}
	else if(type==1)
	{
		fseek(fp,sizeof(int)*(DIR_NUM+pos+3),SEEK_SET);
		fwrite(&file_flag[pos],sizeof(int),1,fp);
	}
	else
	{		
		fseek(fp,sizeof(int)*(DIR_NUM+FILE_NUM+pos+3),SEEK_SET);
		fwrite(&block_flag[pos],sizeof(int),1,fp);
	}
}

//写回目录或文件节点的控制信息 
//type为0时，写目录
//type为1时，写文件
//有目录或文件的控制信息被修改，该函数就被调用
void write_inode(int pos,int type)                        
{                                                  
	if(type==0)                                   
	{                                              
		fseek(fp,sizeof(int)*(DIR_NUM+FILE_NUM+BLOCK_NUM+3)+sizeof(struct dir_node)*pos,SEEK_SET);
		fwrite(&dir[pos],sizeof(struct dir_node),1,fp);
	}
	else
	{
		fseek(fp,sizeof(int)*(DIR_NUM+FILE_NUM+BLOCK_NUM+3)+sizeof(struct dir_node)*DIR_NUM+sizeof(struct file_node)*pos,SEEK_SET);
		fwrite(&file[pos],sizeof(struct file_node),1,fp);
	}
}

//显式保存文件的核心函数   
void savefile(int file_id)                         
{
	if(al_fflag[file_id]%2!=0)                     //如果创建或删除，需要写回磁盘	
	{					
		write_bit(file_id,1);		
		al_fflag[file_id]=0;	
	}
	if(al_file[file_id]>0)                         //如果控制信息被修改，需要写回磁盘
	{	
		write_inode(file_id,1);	
		al_file[file_id]=0;
		for(int i=0;i<file[file_id].block_count;i++)		
		{			
			int temp=file[file_id].block[i];			
			if(al_bflag[temp]%2!=0)                //如果磁盘块被释放或被占用，需要写回磁盘			
			{				
				write_bit(temp,2);				
				al_bflag[temp]=0;
			}
		}
	}
}

void save_dir(int dir_id)                          //保存空目录函数
{
	if(al_dflag[dir_id]%2!=0)                      //如果删除或创建，需要写回磁盘	
	{		
		write_bit(dir_id,0);			
		al_dflag[dir_id]=0;	
	}
	if(al_dir[dir_id]>0)                           //如果控制信息被修改，需要写回磁盘		
	{
		write_inode(dir_id,0);		
		al_dir[dir_id]=0;
	}
}
void save(int dir_id)                              //保存一个目录及它下面的所有目录及文件
{                                                  //用了递归
	for(int i=0;i<dir[dir_id].dir_count;i++)
		save(dir[dir_id].child_dir[i]);
	for(int i=0;i<dir[dir_id].file_count;i++)
		savefile(dir[dir_id].child_file[i]);
	save_dir(dir_id);
}
//格式化
void format()                                      
{
	create_file_system();
	fp=fopen(file_system_name,"rb+");
	if(fp==NULL)
	{
		printf("Open file system error!\n");
		exit(1);
	}

	curr=0;	
	curr_dir=&dir[curr];                           //设置当前目录为根目录	
	strcpy(curr_path,curr_dir->dir_name);	
	strcat(curr_path,"/");
}
//打开文件系统   
void open_file_system()                                                 
{
	int flag=0;
	fp=fopen(file_system_name,"rb");               //以读方式打开，如果不存在，则创建文件系统
	if(fp==NULL)                            
	{
		create_file_system();
		flag=1;
	}
	fp=fopen(file_system_name,"rb+");              //以读写方式打开
	if(fp==NULL)
	{
		printf("Open file system error!\n");
		exit(1);
	}
	if(flag==0)                                    //如果该文件系统早就创建，则将其信息读入主存                          
	{
		fseek(fp,0,SEEK_SET);                      //读超级块信息
		fread(&used_dir,sizeof(int),1,fp);
		fread(&used_file,sizeof(int),1,fp);
		fread(&used_block,sizeof(int),1,fp);

		for(int i=0;i<DIR_NUM;i++)
			fread(&dir_flag[i],sizeof(int),1,fp);
		for(int i=0;i<FILE_NUM;i++)
			fread(&file_flag[i],sizeof(int),1,fp);
		for(int i=0;i<BLOCK_NUM;i++)
			fread(&block_flag[i],sizeof(int),1,fp);

		for(int i=0;i<DIR_NUM;i++)                     //读目录及文件的控制信息
			if(dir_flag[i]==1)
			{
				fseek(fp,sizeof(int)*(DIR_NUM+FILE_NUM+BLOCK_NUM+3)+sizeof(struct dir_node)*i,SEEK_SET);			
				fread(&dir[i],sizeof(struct dir_node),1,fp);
			}
		for(int i=0;i<FILE_NUM;i++)
			if(file_flag[i]==1)
			{
				fseek(fp,sizeof(int)*(DIR_NUM+FILE_NUM+BLOCK_NUM+3)+sizeof(struct dir_node)*DIR_NUM+sizeof(struct file_node)*i,SEEK_SET);					
				fread(&file[i],sizeof(struct file_node),1,fp);
			}
	}
 	fflush(fp);

	curr=0;
	curr_dir=&dir[curr];                           //设置当前目录为根目录
	strcpy(curr_path,curr_dir->dir_name);
	strcat(curr_path,"/");
}

//关闭文件系统，并保存相应信息，这是系统隐式保存文件及目录的方法
void close_file_system()                           
{                                               
	save(0);
	for(int i=0;i<DIR_NUM;i++)
		if(al_dflag[i]%2!=0)
			write_bit(i,0);
	for(int i=0;i<FILE_NUM;i++)
		if(al_fflag[i]%2!=0)
			write_bit(i,1);
	for(int i=0;i<BLOCK_NUM;i++)
		if(al_bflag[i]%2!=0)
			write_bit(i,2);

	fseek(fp,0,SEEK_SET);
	fwrite(&used_dir,sizeof(int),1,fp);
	fwrite(&used_file,sizeof(int),1,fp);
	fwrite(&used_block,sizeof(int),1,fp);

	fclose(fp);
}

//打印命令及解释
void help()                               
{
	printf("Commands:                 Explanation:\n");
	printf("    0  exit:                 Exit!\n");
	printf("    1  format:                 Format the disk!\n");  
	printf("    2  mkf:                 Create a new file!\n");  
	printf("    3  mkd:                 Create a new directory!\n");  
	printf("    4  delf:                 Delete a file!\n");  
	printf("    5  deld:                 Delete a directory!\n");  
	printf("    6  cd:                 Change the current directory!\n");
	printf("    7  dir:                 Show the current files and directories!\n");
    printf("    8  tree:                 Show directory tree!\n");
	printf("    9  help:                 Print commands!\n");
	printf("    10 open file:           Open file!\n");
	
}

//命令接受及解析，转入相应函数运行   
void run()                               
{
	char *command[11]={"exit","dir","mkf","mkd","delf","deld","cd","format","help","tree","open"};
	char buf1[256];   /* 0    , 1  ,  2  ,  3  ,  4   ,  5   ,  6  ,  7   ,  8   ,     9  	*/					
	char buf2[2048];
	char buf3[128];
	char buf4[128];
	int key,num,index,del_pos,parent,p;
	char *s,flag='\0';
	printf("==============================================================================\n");
	help();
	printf("==============================================================================\n");
	printf("Input help to print commands!\n");
	while(1)
	{
		strcpy(buf1,"");
		strcpy(buf2,"");
		strcpy(buf3,"");
		strcpy(buf4,"");
		p=0;
		printf("\n%s ",curr_path);
		cin.getline(buf1,256,'\n');
		key=-1;
		for(int i=0;i<=10;i++)                     //命令匹配
			if(strncmp(buf1,command[i],(int)strlen(command[i]))==0)
			{
				key=i;
				break;
			}
		if(key!=-1)                                //参数分解
		{
			s=strchr(buf1,' ');
			if(s!=NULL)
			{
				index=(int)(s-buf1);
				while(buf1[index]==' ')
					index++;
				strcpy(buf3,buf1+index);
				s=strchr(buf3,' ');
				if(s!=NULL)
				{
					num=(int)(s-buf3);		
					buf3[num]='\0';
				}
				s=strrchr(buf1+index,' ');
				if(s!=NULL)
				{
					index=(int)(s-buf1);
					strcpy(buf4,buf1+index+1);
				}
			}
		}
		switch(key)
		{
		case -1:
			printf("The command you input not exist!\n");
			break;
		case 0:  //exit
			return;
		case 1:  //dir
			list();
			break;
		case 2:  //mkf
			if((int)strlen(buf3)==0)
			{			
				printf("please input the name of the file:\n");			
				cin.getline(buf3,128,'\n');			
			}
			if(create(buf3)!=-1)				
				printf("Succeed!");
			break;
		case 3:  //mkd
			if((int)strlen(buf3)==0)			
			{			
				printf("please input the name of the directory:\n");	
				cin.getline(buf3,128,'\n');			
			}
			if(md(buf3)!=-1)				
				printf("Succeed!");
			break;
		case 4: //delf
			if((int)strlen(buf3)==0)
			{			
				printf("please input the name of the file:\n");			
				cin.getline(buf3,128,'\n');			
			}
			if(delfile(buf3)!=-1)				
				printf("Succeed!");
			break;
		case 5: //deld
			if((int)strlen(buf3)==0)
			{
				printf("please input the name of the directory:\n");			
				cin.getline(buf3,128,'\n');
			}			
			parent=get_parent(buf3,p);
			if(parent==-1)
			{		
				printf("Path name error!");		
				break;	
			}
			if((del_pos=search(parent,buf3+p,0,index))==-1)
			{	
				printf("The directory to delete not exist in current directory!\n");
				break;
			}
			if(dir[del_pos].dir_count>0 || dir[del_pos].file_count>0)
			{
				printf("The directory is not empty!\nAre you sure to delete it?('Y' or 'N')");			
				cin.getline(&flag,32,'\n');			
				if(flag!='Y' && flag!='y')			
					break;
			}
			del_dir(parent,del_pos,index);
			printf("Succeed!");
			break;
		case 6:  //cd
			if((int)strlen(buf3)==0)
			{
				printf("please input the name of the diectory:\n");	
				cin.getline(buf3,128,'\n');
			}
			if(change_dir(buf3)!=-1)
				printf("Succeed!");	
			break;
		case 7:
			format();
			printf("Succeed!");
			break;
		case 8:
			help();
			break;
		case 9:
			show_tree();
			break;
		case 10:if(openfile(buf3)!=-1) printf("Succeed!");
		default:printf("no such a order,please input again\n");
		}
	}
}

void main()
{
	open_file_system();
	run();
	close_file_system();
}

