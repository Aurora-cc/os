#include <iostream>
#include <ctime>
#include <conio.h>
#include "simdisk.h"
using namespace std;

//dir��ĳ�Ա��������

//���ļ�����������̶�ȡ���ݵ�strBuffer��
long dir::open_file(unsigned int nInode, char *strBuffer)
{
	//��ȡָ����ַ
	virDisk.open("virtual_fs.bin", ios::in | ios::binary);
	if (!virDisk.is_open()) error(FATAL_READ);
	virDisk.seekg(inodeTable[nInode].lBlockAddr, ios::beg);
	virDisk.read(reinterpret_cast<char *>(strBuffer), inodeTable[nInode].lSize);
	virDisk.close();
	strBuffer[inodeTable[nInode].lSize - 1] = 0;
	return inodeTable[nInode].lSize;
}

//�����ļ�����strBuffer����д���������
void dir::save_file(const char *strFileName, char *strBuffer, unsigned long lFileLen, fileAttribute privilege)
{
	long lAddr = -1;
	int nInode = -1;
	unsigned int nIndex;
	unsigned int nBlockNum;
	//��ż����
	if ((lFileLen + 1) % BLOCK_SIZE == 0)
		nBlockNum = (lFileLen + 1) / BLOCK_SIZE;
	else
		nBlockNum = (lFileLen + 1) / BLOCK_SIZE + 1;
	//�������ݿ��i-���
	lAddr = alloc_block(nBlockNum, nIndex);
	if (lAddr < 0)
	{
		error(SPACE_NOT_ENOUGH);
		return;
	}
	nInode = alloc_inode();
	if (nInode < 0)
	{
		error(INODE_ALLOC_FAILED);
		return;
	}
	//��ʼ�����ļ������������Ӧ��Ϣ
	inodeTable[nInode].ftType = TYPE_FILE;
	inodeTable[nInode].privilege = privilege;
	inodeTable[nInode].lSize = lFileLen + 1;
	inodeTable[nInode].lBlockAddr = lAddr;
	inodeTable[nInode].nBlocks = nBlockNum;
	strcpy(inodeTable[nInode].strName, strFileName);
	time_t lTime;
	time(&lTime);
	tm tmCreatedTime = *localtime(&lTime);
	inodeTable[nInode].createdTime.set_date_time(tmCreatedTime);
	//�ڸ�Ŀ¼�������Ϣ
	this->nSubInode[this->nSize] = (unsigned int)nInode;
	this->nSize++;
	if (this->inodeNum == dirCurPath.inodeNum) dirCurPath = *this;
	//����
	unsigned int i;
	virDisk.open("virtual_fs.bin", ios::out | ios::binary | ios::_Nocreate);
	if (!virDisk.is_open()) error(FATAL_WRITE);
	for (i = 0; i < BLOCK_GROUPS_NUM; i++)
		virDisk.write(reinterpret_cast<char *>(&dataBlockGroups[i]), sizeof(blockGroup));
	virDisk.seekp((sizeof(blockGroup)*BLOCK_GROUPS_NUM + sizeof(bitmapStatus)*nInode), ios::beg);
	virDisk.write(reinterpret_cast<char *>(&bsInodeBmp[nInode]), sizeof(bitmapStatus));
	virDisk.seekp((sizeof(blockGroup)*BLOCK_GROUPS_NUM + sizeof(bitmapStatus)*INODES_NUM
		+ sizeof(i_node)*nInode), ios::beg);
	virDisk.write(reinterpret_cast<char *>(&inodeTable[nInode]), sizeof(i_node));
	virDisk.seekp((sizeof(blockGroup)*BLOCK_GROUPS_NUM + sizeof(bitmapStatus)*INODES_NUM
		+ sizeof(i_node)*INODES_NUM + sizeof(bitmapStatus)*nIndex), ios::beg);
	for (i = 0; i < nBlockNum; i++)
		virDisk.write(reinterpret_cast<char *>(&bsBlockBmp[nIndex]), sizeof(bitmapStatus));
	virDisk.seekp(lAddr, ios::beg);
	virDisk.write(reinterpret_cast<char *>(strBuffer), lFileLen);
	virDisk.seekp(inodeTable[this->inodeNum].lBlockAddr, ios::beg);
	virDisk.write(reinterpret_cast<char *>(this), sizeof(dir));
	virDisk.close();
}

//ɾ���ļ���������Ϣ
void dir::delete_file(const char *strFileName)
{
	unsigned int i;
	unsigned int nInode;
	unsigned int nPos;
	//�����ļ�
	for (i = 2; i < this->nSize; i++)
	{
		if (strcmp(inodeTable[nSubInode[i]].strName, strFileName) == 0 &&
			inodeTable[nSubInode[i]].ftType == TYPE_FILE)
		{
			nInode = this->nSubInode[i];
			nPos = i;
			break;
		}
	}
	//�Ҳ���
	if (i == this->nSize)
	{
		error(FILE_NOT_EXIST, inodeTable[this->inodeNum].strName, strFileName);
		return;
	}
	if (inodeTable[nInode].privilege == READ_ONLY)
	{
		error(FILE_READONLY, strFileName);
		return;
	}
	//ɾ�����ݿ顢i-����Ŀ¼��Ϣ
	free_block(inodeTable[nInode].nBlocks, ((inodeTable[nInode].lBlockAddr - DATA_AREA_ADDR) / BLOCK_SIZE));
	free_inode(nInode);
	for (i = nPos; i < nSize; i++)
		this->nSubInode[i] = this->nSubInode[i + 1];
	this->nSize--;
	if (this->inodeNum == dirCurPath.inodeNum)
		dirCurPath = *this;
	//����
	virDisk.open("virtual_fs.bin", ios::out | ios::binary | ios::_Nocreate);
	if (!virDisk.is_open()) error(FATAL_WRITE);
	for (i = 0; i < BLOCK_GROUPS_NUM; i++)
		virDisk.write(reinterpret_cast<char *>(&dataBlockGroups[i]), sizeof(blockGroup));
	virDisk.seekp((sizeof(blockGroup)*BLOCK_GROUPS_NUM + sizeof(bitmapStatus)*nInode), ios::beg);
	virDisk.write(reinterpret_cast<char *>(bsInodeBmp), sizeof(bitmapStatus));
	virDisk.seekp((sizeof(blockGroup)*BLOCK_GROUPS_NUM + (sizeof(bitmapStatus) + sizeof(i_node))*INODES_NUM
		+ (inodeTable[nInode].lBlockAddr - DATA_AREA_ADDR) / BLOCK_SIZE), ios::beg);
	for (i = 0; i < (int)inodeTable[nInode].nBlocks; i++)
		virDisk.write(reinterpret_cast<char *>(bsBlockBmp), sizeof(bitmapStatus));
	virDisk.seekp(inodeTable[this->inodeNum].lBlockAddr, ios::beg);
	virDisk.write(reinterpret_cast<char *>(this), sizeof(dir));
	virDisk.close();
}

// ɾ��Ŀ¼���ݹ�ɾ�������������ļ�����Ŀ¼
void dir::remove_dir(dir dirRemove, unsigned int nIndex)
{
	unsigned int i;		//ѭ�����Ʊ���
	for (i = 2; i < dirRemove.nSize; i++)
	{
		if (inodeTable[dirRemove.nSubInode[i]].ftType == TYPE_DIR)	//Ŀ¼����������ļ�����Ŀ¼
		{
			dir dirSub;
			virDisk.open("virtual_fs.bin", ios::in | ios::binary);
			if (!virDisk.is_open()) error(FATAL_READ);
			virDisk.seekg(inodeTable[dirRemove.nSubInode[i]].lBlockAddr, ios::beg);
			virDisk.read(reinterpret_cast<char *>(&dirSub), sizeof(dir));
			virDisk.close();
			//ɾ�����ļ�����Ŀ¼
			dirRemove.remove_dir(dirSub, i);
		}
		else	//�ļ���ֱ��ɾ��
		{
			dirRemove.delete_file(inodeTable[dirRemove.nSubInode[i]].strName);
		}
	}
	//ɾ�����ݿ顢i-����Ŀ¼��Ϣ
	free_block(inodeTable[dirRemove.inodeNum].nBlocks,
		((inodeTable[dirRemove.inodeNum].lBlockAddr - DATA_AREA_ADDR) / BLOCK_SIZE));
	free_inode(dirRemove.inodeNum);
	for (i = nIndex; i < this->nSize; i++)
		this->nSubInode[i] = this->nSubInode[i + 1];
	this->nSize--;
	if (this->inodeNum == dirCurPath.inodeNum)
		dirCurPath = *this;
	//����
	virDisk.open("virtual_fs.bin", ios::out | ios::binary | ios::_Nocreate);
	if (!virDisk.is_open()) error(FATAL_WRITE);
	for (i = 0; i < BLOCK_GROUPS_NUM; i++)
		virDisk.write(reinterpret_cast<char *>(&dataBlockGroups[i]), sizeof(blockGroup));
	virDisk.seekp((sizeof(blockGroup)*BLOCK_GROUPS_NUM + dirRemove.inodeNum), ios::beg);
	virDisk.write(reinterpret_cast<char *>(bsInodeBmp), sizeof(bitmapStatus));
	virDisk.seekp((sizeof(blockGroup)*BLOCK_GROUPS_NUM + (sizeof(bitmapStatus) + sizeof(i_node))*INODES_NUM
		+ (inodeTable[dirRemove.inodeNum].lBlockAddr - DATA_AREA_ADDR) / BLOCK_SIZE), ios::beg);
	for (i = 0; i < (int)inodeTable[dirRemove.inodeNum].nBlocks; i++)
		virDisk.write(reinterpret_cast<char *>(bsBlockBmp), sizeof(bitmapStatus));
	virDisk.seekp(inodeTable[this->inodeNum].lBlockAddr, ios::beg);
	virDisk.write(reinterpret_cast<char *>(this), sizeof(dir));
	virDisk.close();
}

//�ж��ļ������Ƿ����ͬ���ļ�Ŀ¼
bool dir::have_child(const char *strDirName)
{
	for (unsigned int i = 2; i < nSize; i++)
	{
		if (strcmp(inodeTable[this->nSubInode[i]].strName, strDirName) == 0)
			return true;
	}
	return false;
}

//�ȴ� dTime ��ĺ��������ڵȴ��û��鿴��Ϣ
void wait(double dTime)
{
	clock_t start;
	clock_t end;
	start = clock(); //��ʼ
	while (1)
	{
		end = clock();
		if (double(end - start) / 1000 >= dTime / 2)
			//�������dTime��
			break;
	}
}

//�ж��ַ��Ƿ����ʮ�����ƹ淶
bool is_hex_num(char chNum)
{
	if ((chNum >= '0' && chNum <= '9') || (chNum >= 'a' && chNum <= 'f') || (chNum >= 'A' && chNum <= 'F'))
		return true;
	else
		return false;
}

void error(error_num errNum, const char *strArg1, const char *strArg2)
{
	switch (errNum)
	{
	case UNKNOW_ERROR:
		cout << "����δ֪error" << endl;
		break;
	case FATAL_READ:
		cerr << "��ȡģ�� Linux �ļ�ϵͳʧ��" << endl;
		wait(2);
		exit(0);
		break;
	case FATAL_WRITE:
		cerr << "д��ģ�� Linux �ļ�ϵͳʧ��" << endl;
		wait(2);
		exit(0);
		break;
	case DIR_READONLY:
		cerr << "error��Ŀ¼ " << strArg1 << " ֻ�����޷��޸�" << endl;
		break;
	case FILE_READONLY:
		cerr << "error���ļ� " << strArg1 << " ֻ�����޷�ɾ��" << endl;
		break;
	case DIR_WRITEONLY:
		cerr << "error��Ŀ¼ " << strArg1 << " ֻд���޷���ȡ" << endl;
		break;
	case FILE_WRITEONLY:
		cerr << "error���ļ� " << strArg1 << " ֻд���޷���ȡ" << endl;
		break;
	case CD_FAILED:
		cerr << "�л�Ŀ¼ʧ��" << endl;
		break;
	case DIR_FAILED:
		cerr << "Ŀ¼��ʾʧ��" << endl;
		break;
	case MD_FAILED:
		cerr << "Ŀ¼����ʧ��" << endl;
		break;
	case RD_FAILED:
		cerr << "Ŀ¼ɾ��ʧ��" << endl;
		break;
	case NEW_FILE_FAILED:
		cerr << "�ļ�����ʧ��" << endl;
		break;
	case CAT_FAILED:
		cerr << "�ļ���ʧ��" << endl;
		break;
	case DEL_FAILED:
		cerr << "�ļ�ɾ��ʧ��" << endl;
		break;
	case COPY_FAILED:
		cerr << "�ļ�����ʧ��" << endl;
		break;
	case CHMOD_FAILED:
		cerr << "�����ļ���Ŀ¼����ʧ��" << endl;
		break;
	case CHOWN_FAILED:
		cerr << "�����ļ���Ŀ¼������ʧ��" << endl;
		break;
	case CANCEL_INIT:
		cerr << "\nȡ����ʼ������" << endl;
		break;
	case FILE_EXIST:
		cerr << "error��Ŀ¼ " << strArg1 << " ���Ѵ�����Ϊ " << strArg2 << " ���ļ���Ŀ¼" << endl;
		break;
	case NOT_BOTH_HOST:
		cerr << "error��������Դ��Ŀ�겻�ܾ�Ϊ��������" << endl;
		break;
	case HOST_FILE_NOT_EXIST:
		cerr << "error���������������ļ� " << strArg1 << "���ļ�����ʧ��" << endl;
		break;
	case HOST_FILE_WRITE_FAILED:
		cerr << "error��������д���ļ� " << strArg1 << " �����ļ�����ʧ��" << endl;
		break;
	case FILE_NOT_EXIST:
		cerr << "error��Ŀ¼ " << strArg1 << " �²�������Ϊ " << strArg2 << " ���ļ�" << endl;
		break;
	case DIR_NOT_EXIST:
		cerr << "error��Ŀ¼ " << strArg1 << " ������" << endl;
		break;
	case PATH_NOT_FOUND:
		cerr << "error���Ҳ���ָ��·��" << endl;
		break;
	case NO_DEL_CUR:
		cerr << "error������ɾ����ǰĿ¼" << endl;
		break;
	case ILLEGAL_FILENAME:
		cerr << "error���Ƿ��ļ�����\n�ļ����в��ܺ����ַ���/�������Ȳ��ܳ��� " << MAX_NAME_LENGTH << " ���ַ�" << endl;
		break;
	case SPACE_NOT_ENOUGH:
		cerr << "error�����̿ռ䲻�㣡" << endl;
		break;
	case INODE_ALLOC_FAILED:
		cerr << "error��i�ڵ����ʧ�ܣ�" << endl;
		break;
	case INVILID_CMD:
		cerr << "error����Ч������ " << strArg1 << endl;
		break;
	case TOO_MANY_ARGS:
		cerr << "error��������������" << endl;
		break;
	case WRONG_ARGS:
		cerr << "error����������" << endl;
		break;
	case WRONG_COMMANDLINE:
		cerr << "�����в�������" << endl;
		break;
	}
}