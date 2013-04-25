#define BLOCK_SIZE 512                             //磁盘块大小                  
#define DIR_NUM 64                                 //最多目录节点数                      
#define FILE_NUM 512                               //最多文件节点数
#define BLOCK_NUM 1024                             //磁盘块数目
#define BUF_NUM 32                               //主存缓冲区数目

const char file_system_name[]="FileSys.bat";            //磁盘文件名 

struct dir_node                                    //目录节点                     
{
	char space;
	SYSTEMTIME ctime;                              //创建时间
	char dir_name[32];                             //目录名                   
	int child_dir[8];                              //子目录索引                 
	int dir_count;                                 //当前子目录数                     
	int child_file[16];                            //子文件索引              
	int file_count;                                //当前子文件数
	int parent;                                    //父目录索引
};

struct file_node                                   //文件节点
{
	char space;
	SYSTEMTIME ctime;                              //创建时间
	char file_name[32];                            //文件名                
	int block[4];                                  //该文件占有的磁盘块索引
	int block_count;                               //该文件当前占有的磁盘块数
	int file_length;                               //文件长度                  
	int parent;                                    //父目录索引
	char contest[10000];
};

struct buffer_node                                 //主存缓冲区节点                      
{
	int flag;                                      //占用标志                              
	int file_id;                                   //占用该缓冲区节点的文件索引                    
	char buf[BLOCK_SIZE*4];                        //缓冲区，存放读写文件的数据          
	int length;                                    //缓冲区大小                                          
	int offset;                                    //打开系统后第一次写该文件的开始位置
};

struct dir_node dir[DIR_NUM+1];                    //目录节点表              
struct file_node file[FILE_NUM];                   //文件节点表       
struct buffer_node buffer[BUF_NUM];                //缓冲区数组    

int dir_flag[DIR_NUM];                             //各目录节点占用标志，0表示空闲，1表示被占用
int used_dir;                                      //已用的目录节点数
int file_flag[FILE_NUM];                           //各文件节点的占用标志
int used_file;                                     //已用的文件节点数
int block_flag[BLOCK_NUM];                         //磁盘块的占用标志           
int used_block;                                    //已用的磁盘块数

int al_dir[DIR_NUM];                               //目录节点修改登记表                  
int al_file[FILE_NUM];                             //文件节点修改登记表                   

int al_dflag[DIR_NUM];                             //目录节点占用标志登记表
int al_fflag[FILE_NUM];                            //文件节点占用标志登记表
int al_bflag[BLOCK_NUM];                           //磁盘块占用标志登记表            

int open_files[FILE_NUM];                          //文件读写登记表                 

int curr;                                          //当前目录索引                                 
struct dir_node *curr_dir;                         //当前目录节点指针     
char curr_path[512];                               //当前路径              

FILE *fp;                                          //文件指针                                 
