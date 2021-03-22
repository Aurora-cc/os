#pragma once
#include <fstream>
#include <ctime>
using namespace std;

#define MAX_PATH_LENGTH		256	//·������󳤶�
#define MAX_NAME_LENGTH		128	//�ļ�����Ŀ¼������󳤶�
#define MAX_SUBITEM_NUM		256	//Ŀ¼�����ļ����������
#define MAX_COMMAND_LENGTH	128	//�������󳤶�
#define COMMAND_COUNTS		12	//���������
#define BLOCK_SIZE			1024//�̿��С1KB
#define DIR_SIZE			(sizeof(dir) / BLOCK_SIZE + 1) //Ŀ¼��С

#define BLOCKS_EACH			1024//ÿ�����ݿ�����̿���
#define INODES_EACH			1024//ÿ�����ݿ����i�����
#define BLOCK_GROUPS_NUM	100	//���ݿ�����
#define BLOCKS_NUM			(BLOCKS_EACH * BLOCK_GROUPS_NUM)//�̿�����
#define INODES_NUM			(INODES_EACH * BLOCK_GROUPS_NUM)//i�������
//�������׵�ַ
#define DATA_AREA_ADDR		(sizeof(blockGroup) * BLOCK_GROUPS_NUM + sizeof(bitmapStatus) * (INODES_NUM + BLOCKS_NUM) + sizeof(i_node) * INODES_NUM)

static const char *cmdCommands[COMMAND_COUNTS] =
{
	"init", "info", "cd", "dir", "md", "rd", "newfile","cat", "copy", "del", "check", "exit"
};

//�ļ�����
enum fileAttribute
{
	PROTECTED,//ϵͳ����
	READ_WRITE,//�����д
	READ_ONLY,//ֻ��
	WRITE_ONLY,//ֻд
	SHARE//�ɹ���
};

//�ļ�����
enum fileType
{
	TYPE_FILE = 1,//�ļ�
	TYPE_DIR = 2//Ŀ¼
};

enum error_num
{
	UNKNOW_ERROR,//δ֪����
	FATAL_READ,	//�޷���ȡ�������
	FATAL_WRITE,//�޷�д���������
	DIR_READONLY,//Ŀ¼ֻ��
	FILE_READONLY,//�ļ�ֻ��
	DIR_WRITEONLY,//Ŀ¼ֻд
	FILE_WRITEONLY,//�ļ�ֻд
	CD_FAILED,//�л�Ŀ¼ʧ��
	DIR_FAILED,//��ʾĿ¼ʧ��
	MD_FAILED,//����Ŀ¼ʧ��
	RD_FAILED,//ɾ��Ŀ¼ʧ��
	NEW_FILE_FAILED,//�����ļ�ʧ��
	CAT_FAILED,//��ʾ�ļ�ʧ��
	DEL_FAILED,//ɾ���ļ�ʧ��
	COPY_FAILED,//�����ļ�ʧ��
	CHMOD_FAILED,//��������ʧ��
	CHOWN_FAILED,//����������ʧ��
	CANCEL_INIT,//ȡ����ʼ��
	FILE_EXIST,	//�ļ��Ѵ���
	NOT_BOTH_HOST,//���ܾ�Ϊ�������ļ�
	HOST_FILE_NOT_EXIST,//�������ļ�������
	HOST_FILE_WRITE_FAILED,//�������ļ�д��ʧ��
	FILE_NOT_EXIST,//�ļ�������
	DIR_NOT_EXIST,//Ŀ¼������
	PATH_NOT_FOUND,//�Ҳ���·��
	NO_DEL_CUR,//����ɾ����ǰĿ¼
	ILLEGAL_FILENAME,//�Ƿ��ļ���
	SPACE_NOT_ENOUGH,//���̿ռ䲻��
	INODE_ALLOC_FAILED,	//i������ʧ��
	INVILID_CMD,//��Ч����
	TOO_MANY_ARGS,//��������
	WRONG_ARGS,	//��������
	WRONG_COMMANDLINE//�����в�������
};

// λͼ״̬
enum bitmapStatus	
{
	NOT_USED = 0,//δʹ��
	USED = 1//�ѱ�ʹ��
};

//ʱ��
class date_time
{
private:
	unsigned int sec;
	unsigned int min;
	unsigned int hour;
	unsigned int day;
	unsigned int mon;
	unsigned int year;

public:
	void set_date_time(tm t);//����ʱ��
	tm get_date_time();	//��ȡʱ��
};

//Ŀ¼
class dir
{
public:
	unsigned int inodeNum;	//i����
	unsigned int nSize;		//���ļ�,��Ŀ¼����
	unsigned int nSubInode[MAX_SUBITEM_NUM];	//����Ŀi���

	long open_file(unsigned int inodeNum, char *strBuffer);	//�����ļ�
	void save_file(const char *strFileName, char *strBuffer, unsigned long lFileLen, fileAttribute privilege);//�������ļ�
	void delete_file(const char *strFileName);//ɾ�����ļ�
	void remove_dir(dir dirRemove, unsigned int nIndex);//ɾ����Ŀ¼
	bool have_child(const char *strDirName);//�Ƿ��Ѵ������ļ�����Ŀ¼
};

//i���
class i_node
{
public:
	unsigned long lSize;//��С
	unsigned long lBlockAddr;//���̿���ʼ��ַ
	unsigned int nBlocks;//ռ�ô��̿���
	char strName[MAX_NAME_LENGTH];//Ŀ¼�����ļ���
	date_time createdTime;//����ʱ��
	fileAttribute privilege;//��дȨ��
	fileType ftType;//����
};

//������
class superBlock
{
public:
	unsigned int freeBlocksCount;//���п���
	unsigned int freeInodesCount;//����i�����
};

//���ݿ�����Ϣ
class infoInGroup
{
public:
	unsigned int nBlockBmp;	//���ݿ�λͼ����
	unsigned int nInodeBmp;	//i���λͼ����
	unsigned int nInodeTable;//i��������
	unsigned long lBlockAddr;//������������ַ
	unsigned int freeBlocksCountNum;//���п���
	unsigned int freeInodesCountNum;//����i�����
};

//���ݿ���
class blockGroup
{
public:
	infoInGroup d_g_info;//���ݿ�����Ϣ
	superBlock s_blocks;//������
};

extern char curPath[MAX_PATH_LENGTH];//��ǰĿ¼·���ַ���
extern dir dirCurPath;//��ǰĿ¼
extern fstream virDisk;//�������
extern blockGroup dataBlockGroups[BLOCK_GROUPS_NUM];//���ݿ���
extern i_node inodeTable[INODES_NUM];//i����
extern bitmapStatus bsBlockBmp[BLOCKS_NUM];	//���ݿ�λͼ����
extern bitmapStatus bsInodeBmp[INODES_NUM];	//i���λͼ����

void cmd_init();
void cmd_info();
void cmd_cd(const char *strPath);
void cmd_dir(const char *strPath);
void cmd_md(const char *strPath, fileAttribute privilege);
void cmd_rd(const char *strPath);
void cmd_newfile(const char *strPath, fileAttribute privilege);
void cmd_cat(const char *strPath);
void cmd_copy(const char *strSrcPath, const char *strDesPath);
void cmd_del(const char *strPath);
void cmd_check();
void cmd_exit();


//���غ���
void load();
//ִ�������
void execute(const char *comm, const char *p1, const char *p2);
//�����Լ��ͷ��ڴ�ĺ���
int alloc_inode();
void free_inode(unsigned int nInode);
long alloc_block(unsigned int nLen, unsigned int &nIndex);
void free_block(unsigned int nLen, unsigned int nIndex);
//·��������
void set_cur_path(dir dirTemp);
bool if_host_path(char *strPath);
bool get_dir_name(const char *strPath, size_t nPathLen, size_t nPos, char*strFileName);
bool path_to_dir(const char *strPath, size_t nPathLen, size_t &nPos, char *strDirName, dir &dirTemp);
bool analyse_path(const char *strPath, size_t nPathLen, dir &dirTemp, char *strFileName);
bool analyse_path(const char *strPath, size_t nPathLen, dir &dirTemp);
//������ʾ����
void error(error_num errNum, const char *strArg1 = "", const char *strArg2 = "");
bool is_hex_num(char chNum);
//��ʱ����
void wait(double dTime);

