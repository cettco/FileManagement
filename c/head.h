#define BLOCK_SIZE 512                             //���̿��С                  
#define DIR_NUM 64                                 //���Ŀ¼�ڵ���                      
#define FILE_NUM 512                               //����ļ��ڵ���
#define BLOCK_NUM 1024                             //���̿���Ŀ
#define BUF_NUM 32                               //���滺������Ŀ

const char file_system_name[]="FileSys.bat";            //�����ļ��� 

struct dir_node                                    //Ŀ¼�ڵ�                     
{
	char space;
	SYSTEMTIME ctime;                              //����ʱ��
	char dir_name[32];                             //Ŀ¼��                   
	int child_dir[8];                              //��Ŀ¼����                 
	int dir_count;                                 //��ǰ��Ŀ¼��                     
	int child_file[16];                            //���ļ�����              
	int file_count;                                //��ǰ���ļ���
	int parent;                                    //��Ŀ¼����
};

struct file_node                                   //�ļ��ڵ�
{
	char space;
	SYSTEMTIME ctime;                              //����ʱ��
	char file_name[32];                            //�ļ���                
	int block[4];                                  //���ļ�ռ�еĴ��̿�����
	int block_count;                               //���ļ���ǰռ�еĴ��̿���
	int file_length;                               //�ļ�����                  
	int parent;                                    //��Ŀ¼����
	char contest[10000];
};

struct buffer_node                                 //���滺�����ڵ�                      
{
	int flag;                                      //ռ�ñ�־                              
	int file_id;                                   //ռ�øû������ڵ���ļ�����                    
	char buf[BLOCK_SIZE*4];                        //����������Ŷ�д�ļ�������          
	int length;                                    //��������С                                          
	int offset;                                    //��ϵͳ���һ��д���ļ��Ŀ�ʼλ��
};

struct dir_node dir[DIR_NUM+1];                    //Ŀ¼�ڵ��              
struct file_node file[FILE_NUM];                   //�ļ��ڵ��       
struct buffer_node buffer[BUF_NUM];                //����������    

int dir_flag[DIR_NUM];                             //��Ŀ¼�ڵ�ռ�ñ�־��0��ʾ���У�1��ʾ��ռ��
int used_dir;                                      //���õ�Ŀ¼�ڵ���
int file_flag[FILE_NUM];                           //���ļ��ڵ��ռ�ñ�־
int used_file;                                     //���õ��ļ��ڵ���
int block_flag[BLOCK_NUM];                         //���̿��ռ�ñ�־           
int used_block;                                    //���õĴ��̿���

int al_dir[DIR_NUM];                               //Ŀ¼�ڵ��޸ĵǼǱ�                  
int al_file[FILE_NUM];                             //�ļ��ڵ��޸ĵǼǱ�                   

int al_dflag[DIR_NUM];                             //Ŀ¼�ڵ�ռ�ñ�־�ǼǱ�
int al_fflag[FILE_NUM];                            //�ļ��ڵ�ռ�ñ�־�ǼǱ�
int al_bflag[BLOCK_NUM];                           //���̿�ռ�ñ�־�ǼǱ�            

int open_files[FILE_NUM];                          //�ļ���д�ǼǱ�                 

int curr;                                          //��ǰĿ¼����                                 
struct dir_node *curr_dir;                         //��ǰĿ¼�ڵ�ָ��     
char curr_path[512];                               //��ǰ·��              

FILE *fp;                                          //�ļ�ָ��                                 
