#include<stdio.h>                          
#include<stdlib.h>
#include<string>
#include<iostream>
#include<windows.h>
#include "head.h"
using namespace std;

//�����ļ�ϵͳ
void create_file_system()                                     
{
	fp=fopen(file_system_name,"wb+");              //�Զ�д��ʽ�򿪣�������ڣ��򸲸�ԭ����Ϣ��
	if(fp==NULL)                                   //���򴴽����ļ�    
	{
		printf("Create file system error!\n");
		exit(1);
	}
                                                   //����ռ�
	int total=sizeof(int)*(DIR_NUM+FILE_NUM+BLOCK_NUM+3)+sizeof(struct dir_node)*DIR_NUM+sizeof(struct file_node)*FILE_NUM+BLOCK_SIZE*BLOCK_NUM;
	for(long len=0;len<total;len++)
		fputc(0,fp);

	fseek(fp,0,SEEK_SET);                          //д��������Ϣ
	used_dir=1;
	fwrite(&used_dir,sizeof(int),1,fp);	
	used_file=0;
	used_block=0;
	fwrite(&used_file,sizeof(int),2,fp);
	dir_flag[0]=1;
	fwrite(&dir_flag[0],sizeof(int),1,fp);         //��־��ӦĿ¼�ڵ㡢�ļ��ڵ��ʹ�����
	fwrite(&dir_flag[1],sizeof(int),DIR_NUM+FILE_NUM+BLOCK_NUM-1,fp);

	strcpy(dir[0].dir_name,"C:");                  //д��Ŀ¼��Ϣ
	dir[0].dir_count=0;
	dir[0].file_count=0;
	dir[0].parent=-1;
	GetLocalTime(&dir[0].ctime);                   //��õ�ǰʱ��
	fwrite(&dir[0],sizeof(struct dir_node),1,fp);
                                                   
	for(int i=0;i<DIR_NUM;i++)                     //�����б������
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
//����Ŀ¼���ļ����ɹ��򷵻ظ��ļ���Ŀ¼������ʧ���򷵻�-1��
//type��������Ҫ���������ļ�����Ŀ¼0��ʾĿ¼��1��ʾ�ļ���
//index����ָ���ҵ����ļ���Ŀ¼�ڸ�Ŀ¼�µ�λ��
int search(int parent,char *name,int type,int &index) 
{                                                      
	struct dir_node *p=&dir[parent];               
	int i,temp;                                    
	if(type==0)    //��ʾĿ¼                                                
		for(i=0;i<p->dir_count;i++)
		{
			temp=p->child_dir[i];
			if(strcmp(name,dir[temp].dir_name)==0)// �ַ������ʱ 
			{
				index=i;
				return temp;
			}
		}
	else
		for(i=0;i<p->file_count;i++)//��ʾ���ļ� 
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

//����������ַ�������ø�Ŀ¼���������ļ���Ŀ¼�������ò���p����ָ��Ŀ¼���ļ�������ʼλ��
int get_parent(char *name,int &p)         
{                                          
	char buf[32],*path,*s;                         
	int pos,start=0,index;
	path=new char[128];
	strcpy(path,name);

	s=strrchr(path,'/');                   //���û��ָ��·�����򽫵�ǰĿ¼ָ��Ϊ��Ŀ¼
	if(s==NULL)                              
		return curr;

	pos=(int)(s-path);
	p=pos+1;
	if(p==(int)strlen(path))
		return -1;
	path[p]='\0';
	
	s=strchr(path,'/');
	pos=(int)(s-path);
	if(strncmp(path,"C:",pos)!=0)                  //ǰ��û�и���Ŀ¼������ӵ�ǰĿ¼��������
		start=curr;
	else	
		path+=3;
	while(start!=-1 && (int)strlen(path)>0)        //����������֪��·������������ֲ�ƥ��
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

//Ϊ�ļ�������̿飬һ�ε�������һ�飬�ɹ��򷵻ش��̿�������ʧ�ܷ���-1 
int get_block(int pos)                                                
{                                                  
	for(int i=0;i<BLOCK_NUM;i++)
		if(block_flag[i]==0)
		{                                          //�޸Ĵ��̿�ʹ��������ļ��Ŀ�����Ϣ
			block_flag[i]=1;
			al_bflag[i]++;               //Ŀ¼�ڵ�ռ�ñ�־
			used_block++;                //���õĴ��̿���
			int top=file[pos].block_count;
			file[pos].block[top]=i;
			file[pos].block_count++;
			return i;
		}
	return -1;
}

//�ͷ��ļ�ռ�õĴ��̿飬����ɾ���ļ�ʱ��
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

int get_dir(int parent,char *dir_name)             //����Ŀ¼�ڵ㣬�ɹ��򷵻�Ŀ¼��������ʧ���򷵻�-1
{
	int index;
	if(search(parent,dir_name,0,index)!=-1)        //�����ڸ�Ŀ¼���Ƿ���ͬ��Ŀ¼�����򴴽�ʧ��        
	{
		printf("Directory name repeated!\n");
		return -1;
	}
	for(int i=1;i<DIR_NUM;i++)                     //�������е�Ŀ¼�ڵ�           
		if(dir_flag[i]==0)
		{                                          //�Ǽǲ���Ŀ¼�ڵ��ʼ��
			dir_flag[i]=1; 
			al_dflag[i]++;
			al_dir[i]++;
			used_dir++;
			strcpy(dir[i].dir_name,dir_name);
			dir[i].dir_count=0;
			dir[i].file_count=0;
			dir[i].parent=parent;
			GetLocalTime(&dir[i].ctime);           //��õ�ǰʱ��
			return i;
		}
	return -1;
}
//�����ļ��ڵ㣬�ɹ��򷵻��ļ��������ţ�ʧ�ܷ���-1
int get_file(int parent,char *file_name)          
{
	int index;
	if(search(parent,file_name,1,index)!=-1)       //�����ڸ�Ŀ¼���Ƿ���ͬ���ļ����ڣ����򴴽�ʧ��
	{
		printf("File name repeated!\n");
		return -1;
	}
	for(int i=0;i<FILE_NUM;i++)                    //�������е��ļ��ڵ�             
		if(file_flag[i]==0)
		{
			strcpy(file[i].file_name,file_name);
			file[i].block_count=0;
			if(get_block(i)==-1)                   //���´������ļ�������̿飬���ʧ�ܣ������ļ�           
			{                                      //��ʧ��
				printf("Disk volumn error!\n");
				return -1;
			}
			file_flag[i]=1;
			al_fflag[i]++;                         //�Ǽǲ����ļ��ڵ��ʼ��
			al_file[i]++;
			used_file++;
			file[i].file_length=0;
			file[i].parent=parent;
			GetLocalTime(&file[i].ctime);          //��õ�ǰʱ��
			return i; 
		}
	return -1;
}

//���ļ����뻺���������ļ�ʱ���ã�ʧ��ʱ����-1
int get_buffer(int pos)
{                            
	for(int i=0;i<BUF_NUM;i++)
		if(buffer[i].flag==0)                      //buffer[] ����������    
		{
			buffer[i].flag=1;                      //�޸�ʹ�ñ�־����ʼ��
			buffer[i].file_id=pos;
			buffer[i].length=0;
			buffer[i].offset=0;
			return i;
		}
	return -1;
}
 // �õ������ļ��Ļ����������ţ�ʧ���򷵻�-1
int get_buffer_id(int pos)                        
{
	for(int i=0;i<BUF_NUM;i++)
		if(buffer[i].flag==1 && buffer[i].file_id==pos)
			return i;
	return -1;
}

 // ��ָ����Ŀ¼�´����ļ�����������ɹ��򷵻��ļ��������ţ����򷵻�-1
//	�����Ŀ¼�������򴴽�ʧ��
int create_file(int parent,char *file_name)         
{                                                  
	if(dir[parent].file_count==16)                 
	{
		printf("Parent directory is full!\n");
		return -1;
	}					
	int pos=get_file(parent,file_name);            //��ʼ�����ļ�
	if(pos==-1)
	{
		printf("Create file error!\n");
		return -1;
	}
                                           
	struct dir_node *p=&dir[parent];               //�޸ĸ�Ŀ¼�Ŀ�����Ϣ
	int top=p->file_count;              
	p->child_file[top]=pos;
	p->file_count++;
	al_dir[parent]++;

	return pos;
}

 //��ָ����Ŀ¼�´���Ŀ¼������ɹ��򷵻�Ŀ¼�������ţ����򷵻�-1 
//�����Ŀ¼�������򴴽�ʧ��
int create_dir(int parent,char *dir_name)                
{                                                  
	if(dir[parent].dir_count==DIR_NUM-1)     //�����Ŀ¼�������򴴽�ʧ��               
	{
		printf("Parent directory is full!\n");
		return -1;
	}
	int pos=get_dir(parent,dir_name);              //��ʼ����Ŀ¼
	if(pos==-1)
	{
		printf("Create directory error!\n");
		return -1;
	}

	struct dir_node *p=&dir[parent];               //�޸ĸ�Ŀ¼�Ŀ�����Ϣ
	int top=p->dir_count;         
	p->child_dir[top]=pos;
	p->dir_count++;
	al_dir[parent]++;

	return pos;
}

// ����Ŀ¼���������������Ĳ���ֻ���û�����
//��������ɹ����򷵻�Ŀ¼�������ţ����򷵻�-1
int md(char *name)                                
{                                                  
	int parent,p=0;                                
	parent=get_parent(name,p);
	if(parent==-1)                                 //��Ŀ¼�Ҳ������������󣬴���ʧ��
	{
		printf("Path name error!\n");
		return -1;
	}
	return create_dir(parent,name+p);              //��ʼ����Ŀ¼
}

 // �����ļ�������������ֱ�ӽ����û�����ɹ������ļ��������ţ����򷵻�-1
int create(char *name)                             
{                                               
	int parent,p=0;
	parent=get_parent(name,p);
	if(parent==-1)                                 //�Ҳ�����Ŀ¼��������󣬴���ʧ��
	{
		printf("Path name error!\n");
		return -1;
	}
	return create_file(parent,name+p);             //��ʼ�����ļ�
}

// ɾ���ļ������øú�����ǰ�����Ѿ�ȡ��Ҫɾ���ļ���������
void del_file(int pos)                                              
{                                                  
	return_block(pos);                             //�ͷŴ��̿�
	file_flag[pos]=0; 
	used_file--;
	al_fflag[pos]++;
	al_file[pos]=0;

	if(open_files[pos]>0)                          //����Ѿ���û��������������ͷ�
	{
		int buf_id=get_buffer_id(pos);
		buffer[buf_id].flag=0;
		open_files[pos]=0;
	}
}


//��ָ����Ŀ¼��ɾ���ļ���ɾ���ɹ��򷵻��ļ��������ţ����򷵻�-1   
int del_file(int parent,char *file_name)                       
{                                                  
	int del_pos,index;
	if((del_pos=search(parent,file_name,1,index))==-1)//�������ļ��Ƿ���ڣ���������ɾ��ʧ��
	{
		printf("The file to delete not exist!\n");
		return -1;
	}
	del_file(del_pos);                             //��ʼɾ���ļ�

	struct dir_node *p=&dir[parent];               //�޸ĸ�Ŀ¼�Ŀ�����Ϣ
	if(p->file_count>=2)
	{
		int top=p->file_count-1;         	
		p->child_file[index]=p->child_file[top];
	}
	p->file_count--;
	al_dir[parent]++;

	return del_pos;
}
//ɾ���ļ�����������  
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

//��ָ����Ŀ¼��ɾ���ļ���ɾ���ɹ��򷵻��ļ��������ţ����򷵻�-1   
int open_file(int parent,char *file_name)                       
{                                                  
	int open_pos,index;
	if((open_pos=search(parent,file_name,1,index))==-1)//�������ļ��Ƿ���ڣ���������ɾ��ʧ��
	{
		printf("The file to delete not exist!\n");
		return -1;
	}
	open_file(open_pos);                             //��ʼ���ļ�
	return open_pos;
}



//���ļ�����������  
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

//ɾ��ָ����Ŀ¼�ڵ㣬��Ŀ¼�Ѿ�Ϊ��
void del_dir(int pos)                                                
{
	dir_flag[pos]=0;  
	used_dir--;
	al_dflag[pos]++;
	al_dir[pos]=0;
}

//ɾ��һ��ָ��Ŀ¼��������������ļ�������Ŀ¼   
void del(int pos)                                                           
{
	for(int i=0;i<dir[pos].file_count;i++)         //ɾ����ǰĿ¼�µ������ļ�
		del_file(dir[pos].child_file[i]);
	for(int i=0;i<dir[pos].dir_count;i++)              //ɾ����Ŀ¼
		del(dir[pos].child_dir[i]);
	del_dir(pos);                                  //ɾ����ǰĿ¼
}

//��һ��ָ����Ŀ¼��ɾ��һ��Ŀ¼��������������ļ�������Ŀ¼      
void del_dir(int parent,int del_pos,int index)                    
{                                                  
	del(del_pos);                                  //��ʼɾ��Ŀ¼
	if(dir[parent].dir_count>=2)                   //�޸ĸ�Ŀ¼�Ŀ�����Ϣ
	{
		int top=dir[parent].dir_count-1;
		dir[parent].child_dir[index]=dir[parent].child_dir[top];
	}
	dir[parent].dir_count--;
	al_dir[parent]++;
}

 //�������õ�ǰ·��
void paste(int dir_id)                           
{
	if(dir_id==0)
		return;
	paste(dir[dir_id].parent);
	strcat(curr_path,dir[dir_id].dir_name);
	strcat(curr_path,"/");
}

//�ı乤��Ŀ¼���ɹ��򷵻ظ�Ŀ¼�������ţ����򷵻�-1
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
	if(pos==-1)                                    //�����Ŀ¼�����ڣ���ʧ��
	{
		printf("The dictory not exist!\n");
		return -1;
	}
	curr_path[3]='\0';
	paste(pos);
	curr=pos;                                      //�ı䵱ǰĿ¼��·��
	curr_dir=&dir[curr];                                                 
	return curr;                               
}

//�ú������ڴ�ӡ|�ã����Խ�Ŀ¼�ṹ��������ʾ
int total[DIR_NUM];                                //������ʾĿ¼��ʱ��

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

//�ú�����ÿ��ӡһ��Ŀ¼���ļ���֮ǰ����
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

//��ӡ�ļ���
void print_file(int file_id)                       
{
	print_pre(file_id,1);
	printf("|");
	for(int i=0;i<6;i++)
		printf("_");
	printf("%s\n",file[file_id].file_name);
}

 //��ӡĿ¼��
void print_dir(int dir_id)                         
{
	print_pre(dir_id,0);
	printf("|");
	for(int i=0;i<6;i++)
		printf("_");
	printf("%s\057\n",dir[dir_id].dir_name);
	total[dir[dir_id].parent]++;
}

 //��ӡ���νṹ�ĺ��ĺ���
void print(int dir_id)                             
{
	print_dir(dir_id);
	for(int i=0;i<dir[dir_id].file_count;i++)
		print_file(dir[dir_id].child_file[i]);
	for(int i=0;i<dir[dir_id].dir_count;i++)
		print(dir[dir_id].child_dir[i]);
}

//��ӡ���νṹ����������
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

//���¼�������������ʾ��ǰĿ¼�����Ŀ¼���ļ��������治ͬ���ǣ���������ʾĿ¼���ļ�֮��Ĺ�ϵ��
//	����������ʾһЩϸ�ڣ��紴��ʱ�䣬�ļ�����
// ��ʾ��ǰĿ¼�µ��ļ�����Ŀ¼��Ϣ
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

//��ʾһ���ļ�
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
	if(open_files[file_id]==2)                     //������ļ��Ѿ���д�ķ�ʽ���򿪣�����ı�
	{
		int buf_id=get_buffer_id(file_id);
		length=buffer[buf_id].length;
	}
	printf("len: %d bytes\n",length);
}

//��ʾĿ¼���ļ��ĺ��ĺ��������˵ݹ�˼��
void show(int dir_id)                                                     
{
	for(int i=0;i<dir[dir_id].file_count;i++)
		show_file(dir[dir_id].child_file[i]);
	for(int i=0;i<dir[dir_id].dir_count;i++)
		show_dir(dir[dir_id].child_dir[i]);
}

    //��ʾ�ļ���Ŀ¼����������
void list()                                      
{ 
	show(curr);                                    //�ú����Ĳ�����ʾ��Ŀ¼������
	printf("%d files.",curr_dir->file_count);  //ͳ���ļ���
	printf("\n%d dirs.",curr_dir->dir_count);    //ͳ��Ŀ¼��
}

//д��Ŀ¼�ڵ㡢�ļ��ڵ����̿��ռ�ñ�־
//typeΪ0ʱ��дĿ¼��
//typeΪ1ʱ��д�ļ���
//typeΪ2ʱ��д���̿�
//�ú���ֻ���ڴ�����ɾ��ʱ�ŵ���
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

//д��Ŀ¼���ļ��ڵ�Ŀ�����Ϣ 
//typeΪ0ʱ��дĿ¼
//typeΪ1ʱ��д�ļ�
//��Ŀ¼���ļ��Ŀ�����Ϣ���޸ģ��ú����ͱ�����
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

//��ʽ�����ļ��ĺ��ĺ���   
void savefile(int file_id)                         
{
	if(al_fflag[file_id]%2!=0)                     //���������ɾ������Ҫд�ش���	
	{					
		write_bit(file_id,1);		
		al_fflag[file_id]=0;	
	}
	if(al_file[file_id]>0)                         //���������Ϣ���޸ģ���Ҫд�ش���
	{	
		write_inode(file_id,1);	
		al_file[file_id]=0;
		for(int i=0;i<file[file_id].block_count;i++)		
		{			
			int temp=file[file_id].block[i];			
			if(al_bflag[temp]%2!=0)                //������̿鱻�ͷŻ�ռ�ã���Ҫд�ش���			
			{				
				write_bit(temp,2);				
				al_bflag[temp]=0;
			}
		}
	}
}

void save_dir(int dir_id)                          //�����Ŀ¼����
{
	if(al_dflag[dir_id]%2!=0)                      //���ɾ���򴴽�����Ҫд�ش���	
	{		
		write_bit(dir_id,0);			
		al_dflag[dir_id]=0;	
	}
	if(al_dir[dir_id]>0)                           //���������Ϣ���޸ģ���Ҫд�ش���		
	{
		write_inode(dir_id,0);		
		al_dir[dir_id]=0;
	}
}
void save(int dir_id)                              //����һ��Ŀ¼�������������Ŀ¼���ļ�
{                                                  //���˵ݹ�
	for(int i=0;i<dir[dir_id].dir_count;i++)
		save(dir[dir_id].child_dir[i]);
	for(int i=0;i<dir[dir_id].file_count;i++)
		savefile(dir[dir_id].child_file[i]);
	save_dir(dir_id);
}
//��ʽ��
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
	curr_dir=&dir[curr];                           //���õ�ǰĿ¼Ϊ��Ŀ¼	
	strcpy(curr_path,curr_dir->dir_name);	
	strcat(curr_path,"/");
}
//���ļ�ϵͳ   
void open_file_system()                                                 
{
	int flag=0;
	fp=fopen(file_system_name,"rb");               //�Զ���ʽ�򿪣���������ڣ��򴴽��ļ�ϵͳ
	if(fp==NULL)                            
	{
		create_file_system();
		flag=1;
	}
	fp=fopen(file_system_name,"rb+");              //�Զ�д��ʽ��
	if(fp==NULL)
	{
		printf("Open file system error!\n");
		exit(1);
	}
	if(flag==0)                                    //������ļ�ϵͳ��ʹ�����������Ϣ��������                          
	{
		fseek(fp,0,SEEK_SET);                      //����������Ϣ
		fread(&used_dir,sizeof(int),1,fp);
		fread(&used_file,sizeof(int),1,fp);
		fread(&used_block,sizeof(int),1,fp);

		for(int i=0;i<DIR_NUM;i++)
			fread(&dir_flag[i],sizeof(int),1,fp);
		for(int i=0;i<FILE_NUM;i++)
			fread(&file_flag[i],sizeof(int),1,fp);
		for(int i=0;i<BLOCK_NUM;i++)
			fread(&block_flag[i],sizeof(int),1,fp);

		for(int i=0;i<DIR_NUM;i++)                     //��Ŀ¼���ļ��Ŀ�����Ϣ
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
	curr_dir=&dir[curr];                           //���õ�ǰĿ¼Ϊ��Ŀ¼
	strcpy(curr_path,curr_dir->dir_name);
	strcat(curr_path,"/");
}

//�ر��ļ�ϵͳ����������Ӧ��Ϣ������ϵͳ��ʽ�����ļ���Ŀ¼�ķ���
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

//��ӡ�������
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

//������ܼ�������ת����Ӧ��������   
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
		for(int i=0;i<=10;i++)                     //����ƥ��
			if(strncmp(buf1,command[i],(int)strlen(command[i]))==0)
			{
				key=i;
				break;
			}
		if(key!=-1)                                //�����ֽ�
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

