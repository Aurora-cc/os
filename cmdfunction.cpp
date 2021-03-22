#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <conio.h>
#include "simdisk.h"
using namespace std;

//init�����ʼ��
void cmd_init()
{

	cout << "��ʼ��ģ��Linux�ļ�ϵͳing����" << endl;
	int i;
	for (i = 0; i < BLOCK_GROUPS_NUM; i++)
	{
		dataBlockGroups[i].s_blocks.freeBlocksCount = BLOCKS_NUM - DIR_SIZE;
		dataBlockGroups[i].s_blocks.freeInodesCount = INODES_NUM - 1;
		dataBlockGroups[i].d_g_info.nBlockBmp = i * BLOCKS_EACH;
		dataBlockGroups[i].d_g_info.nInodeBmp = i * INODES_EACH;
		dataBlockGroups[i].d_g_info.nInodeTable = i * INODES_EACH;
		dataBlockGroups[i].d_g_info.lBlockAddr = DATA_AREA_ADDR + i * BLOCKS_EACH * BLOCK_SIZE;
		dataBlockGroups[i].d_g_info.freeBlocksCountNum = BLOCKS_EACH;
		dataBlockGroups[i].d_g_info.freeInodesCountNum = INODES_EACH;
	}
	for (i = 0; i < BLOCKS_NUM; i++)
	{
		bsBlockBmp[i] = NOT_USED;
	}
	for (i = 0; i < INODES_NUM; i++)
	{
		bsInodeBmp[i] = NOT_USED;
	}
	for (i = 0; i < INODES_NUM; i++)
	{
		inodeTable[i].privilege = READ_WRITE;
		inodeTable[i].ftType = TYPE_DIR;
		inodeTable[i].lSize = 0;
		inodeTable[i].nBlocks = 0;
		inodeTable[i].lBlockAddr = -1;
	}
	//Ĭ��·��Ϊ��Ŀ¼�����û�����Ϣ��Ĭ�ϣ�
	dataBlockGroups[0].d_g_info.freeBlocksCountNum = BLOCKS_EACH - DIR_SIZE;
	dataBlockGroups[0].d_g_info.freeInodesCountNum = INODES_EACH - 1;
	for (i = 0; i < DIR_SIZE; i++) bsBlockBmp[i] = USED;
	bsInodeBmp[0] = USED;
	inodeTable[0].privilege = PROTECTED;
	inodeTable[0].ftType = TYPE_DIR;
	inodeTable[0].lSize = sizeof(dir);
	inodeTable[0].nBlocks = DIR_SIZE;
	inodeTable[0].lBlockAddr = DATA_AREA_ADDR;
	strcpy(inodeTable[0].strName, "root");
	time_t lTime;
	time(&lTime);
	tm tmCreatedTime = *localtime(&lTime);
	inodeTable[0].createdTime.set_date_time(tmCreatedTime);
	dirCurPath.inodeNum = 0;
	dirCurPath.nSize = 2;
	dirCurPath.nSubInode[0] = 0;
	dirCurPath.nSubInode[1] = 0;
	strcpy(curPath, "root");
	//���������Ϣ
	virDisk.open("virtual_fs.bin", ios::out | ios::binary);
	if (!virDisk.is_open()) error(FATAL_WRITE);
	for (i = 0; i < BLOCK_GROUPS_NUM; i++)
	{
		virDisk.write(reinterpret_cast<char *>(&dataBlockGroups[i]), sizeof(blockGroup));
	}
	for (i = 0; i < INODES_NUM; i++)
	{
		virDisk.write(reinterpret_cast<char *>(&bsInodeBmp[i]), sizeof(bitmapStatus));
	}
	for (i = 0; i < INODES_NUM; i++)
	{
		virDisk.write(reinterpret_cast<char *>(&inodeTable[i]), sizeof(i_node));
	}
	for (i = 0; i < BLOCKS_NUM; i++)
	{
		virDisk.write(reinterpret_cast<char *>(&bsBlockBmp[i]), sizeof(bitmapStatus));
	}
	virDisk.seekp(inodeTable[dirCurPath.inodeNum].lBlockAddr, ios::beg);
	virDisk.write(reinterpret_cast<char *>(&dirCurPath), sizeof(dir));
	cout << "���ڴ���ģ�� Linux �ļ�ϵͳ�洢�ռ�" << endl;
	//���� 100M �Ŀռ�
	long lFileSize = BLOCK_SIZE * BLOCKS_NUM;
	char *strBuffer = new char[lFileSize];
	virDisk.close();
	//���ɹ̶���С�ļ���ɣ�׼��������Ϣ
	delete(strBuffer);
	//��ʼ����ɣ�����
	cout << "��ʼ����ɣ�" << endl;
	wait(5);
	system("cls");

}

//��ʾϵͳ��Ϣ
void cmd_info()
{
	cout << "ģ��Linux�ļ�ϵͳInfo��" << endl;
	cout.width(18);
	cout << "����������";
	cout.width(10);
	cout << BLOCKS_NUM * BLOCK_SIZE;
	cout.width(3);
	cout << " �ֽ�\t";
	cout.width(10);
	cout << (float)BLOCKS_NUM / 1024;
	cout.width(3);
	cout << " MB" << endl;
	cout.width(18);
	cout << "���ÿռ䣺";
	cout.width(10);
	cout << (BLOCKS_NUM - dataBlockGroups[0].s_blocks.freeBlocksCount) * BLOCK_SIZE;
	cout.width(3);
	cout << " �ֽ�\t";
	cout.width(10);
	cout << (float)(BLOCKS_NUM - dataBlockGroups[0].s_blocks.freeBlocksCount) / 1024;
	cout.width(3);
	cout << " MB" << endl;
	cout.width(18);
	cout << "���ÿռ䣺";
	cout.width(10);
	cout << dataBlockGroups[0].s_blocks.freeBlocksCount * BLOCK_SIZE;
	cout.width(3);
	cout << " �ֽ�\t";
	cout.width(10);
	cout << (float)dataBlockGroups[0].s_blocks.freeBlocksCount / 1024;
	cout.width(3);
	cout << " MB" << endl;
	cout.width(18);
	cout << "���ÿռ������";
	cout.width(10);
	cout << ((float)dataBlockGroups[0].s_blocks.freeBlocksCount / (float)BLOCKS_NUM) * 100 << "%" << endl;
	cout.width(18);
	cout << "�̿��С��";
	cout.width(10);
	cout << BLOCK_SIZE;
	cout.width(3);
	cout << " �ֽ�" << endl;
	cout.width(18);
	cout << "ÿ���̿�����";
	cout.width(10);
	cout << BLOCKS_EACH;
	cout.width(3);
	cout << " ��" << endl;
	cout.width(18);
	cout << "ÿ��i�������";
	cout.width(10);
	cout << INODES_EACH;
	cout.width(3);
	cout << " ��" << endl;
	cout.width(18);
	cout << "�̿�������";
	cout.width(10);
	cout << BLOCKS_NUM;
	cout.width(3);
	cout << " ��" << endl;
	cout.width(18);
	cout << "i���������";
	cout.width(10);
	cout << INODES_NUM;
	cout.width(3);
	cout << " ��" << endl;
	cout.width(18);
	cout << "���п�������";
	cout.width(10);
	cout << dataBlockGroups[0].s_blocks.freeBlocksCount;
	cout.width(3);
	cout << " ��" << endl;
	cout.width(18);
	cout << "����i���������";
	cout.width(10);
	cout << dataBlockGroups[0].s_blocks.freeInodesCount;
	cout.width(3);
	cout << " ��" << endl;
}

// cd����л�Ŀ¼
void cmd_cd(const char *strPath)
{
	dir dirTemp;
	size_t nPathLen = strlen(strPath);
	if (nPathLen == 0)
	{
		cout << "��ǰ����·����\n" << curPath << endl;
		return;
	}
	//����·������Ч·�����л�����Ч�򱨴�
	if (analyse_path(strPath, nPathLen, dirTemp))
	{
		if (inodeTable[dirTemp.inodeNum].privilege == WRITE_ONLY)
		{
			error(DIR_WRITEONLY, strPath);
			return;
		}
		dirCurPath = dirTemp;
		set_cur_path(dirCurPath);
	}
	else
	{
		error(CD_FAILED);
		return;
	}
}

// dir���Ŀ¼�ļ���Ϣ��ʾ
void cmd_dir(const char *strPath)
{
	dir dirTemp;
	size_t nPathLen = strlen(strPath);
	//����·������Ч·������ʾĿ¼���ݣ���Ч�򱨴�
	if (analyse_path(strPath, nPathLen, dirTemp))
	{
		if (inodeTable[dirTemp.inodeNum].privilege == WRITE_ONLY)
		{
			error(DIR_WRITEONLY, inodeTable[dirTemp.inodeNum].strName);
			return;
		}
		//��ʾ��ͷ
		cout << inodeTable[dirTemp.inodeNum].strName << " ��Ŀ¼" << endl;
		cout.width(20);
		cout << left << "����ʱ��";
		cout.width(20);
		cout << left << "Ŀ¼��/�ļ���";
		cout.width(4);
		cout << "����";
		cout.width(18);
		cout << right << "��С���ֽڣ�";
		cout << "��";
		cout.width(4);
		cout << "����" << endl;

		unsigned int nDirCount = 0, nFileCount = 0;		//����Ŀ¼���ļ�����
		//��ʾ����
		for (unsigned int i = 0; i < dirTemp.nSize; i++)
		{
			tm tmCreatedTime = inodeTable[dirTemp.nSubInode[i]].createdTime.get_date_time();
			cout.fill('0');
			cout.width(4);
			cout << right << tmCreatedTime.tm_year + 1900;
			cout << "/";
			cout.width(2);
			cout << tmCreatedTime.tm_mon + 1;
			cout << "/";
			cout.width(2);
			cout << tmCreatedTime.tm_mday;
			cout << " ";
			cout.width(2);
			cout << tmCreatedTime.tm_hour;
			cout << ":";
			cout.width(2);
			cout << tmCreatedTime.tm_min;
			cout << ":";
			cout.width(2);
			cout << tmCreatedTime.tm_sec;
			cout << " ";
			cout.fill('\0');
			cout.width(20);
			cout << left;
			if (i == 0)
				cout << ".";
			else if (i == 1)
				cout << "..";
			else
				cout << inodeTable[dirTemp.nSubInode[i]].strName;
			if (inodeTable[dirTemp.nSubInode[i]].ftType == TYPE_DIR)
			{
				nDirCount++;
				cout.width(4);
				cout << "Ŀ¼";
				cout.width(18);
				cout << "";
			}
			else
			{
				nFileCount++;
				cout.width(4);
				cout << "�ļ�";
				cout.width(18);
				cout << right << inodeTable[dirTemp.nSubInode[i]].lSize;
			}
			switch (inodeTable[dirTemp.nSubInode[i]].privilege)
			{
			case PROTECTED:
				cout << left << "ϵͳ";
				break;
			case READ_WRITE:
				cout << left << "��д";
				break;
			case READ_ONLY:
				cout << left << "ֻ��";
				break;
			case WRITE_ONLY:
				cout << left << "ֻд";
				break;
			case SHARE:
				cout << left << "����";
				break;
			}
			cout << endl;
		}
		cout << endl;
		cout.width(5);
		cout << right << nFileCount;
		cout << " ���ļ� ";
		cout.width(5);
		cout << right << nDirCount;
		cout << " ��Ŀ¼" << endl;
	}
	else
	{
		error(DIR_FAILED);
	}
}

// md���������Ŀ¼
void cmd_md(const char *strPath, fileAttribute privilege)
{
	dir dirTemp;
	size_t nPathLen = strlen(strPath);
	char strDirName[MAX_NAME_LENGTH];
	if (analyse_path(strPath, nPathLen, dirTemp, strDirName))
	{
		if (inodeTable[dirTemp.inodeNum].privilege == READ_ONLY)
		{
			error(DIR_READONLY, inodeTable[dirTemp.inodeNum].strName);
			return;
		}
		long lAddr = -1;
		int inodeNum = -1;
		unsigned int nIndex;
		//�Ƿ��Ѵ��ڸ����ֵ�����
		if (dirTemp.have_child(strDirName))
		{
			error(FILE_EXIST, inodeTable[dirTemp.inodeNum].strName, strDirName);
			return;
		}
		//����Ŀ¼�Ĵ洢�ռ�
		lAddr = alloc_block(DIR_SIZE, nIndex);
		if (lAddr < 0)	//�ռ䲻��
		{
			error(SPACE_NOT_ENOUGH);
		}
		else
		{
			//����i���
			inodeNum = alloc_inode();
			if (inodeNum < 0)
			{
				error(INODE_ALLOC_FAILED);
				return;
			}
			//��ʼ����Ŀ¼�����������Ӧ��Ϣ
			dir dirNew;
			dirNew.inodeNum = (unsigned int)inodeNum;
			dirNew.nSize = 2;
			strcpy(inodeTable[dirNew.inodeNum].strName, strDirName);
			dirNew.nSubInode[0] = (unsigned int)inodeNum;
			dirNew.nSubInode[1] = dirTemp.inodeNum;
			inodeTable[inodeNum].ftType = TYPE_DIR;
			inodeTable[inodeNum].privilege = privilege;
			inodeTable[inodeNum].lSize = sizeof(dir);
			inodeTable[inodeNum].lBlockAddr = lAddr;
			inodeTable[inodeNum].nBlocks = DIR_SIZE;
			time_t lTime;
			time(&lTime);
			tm tmCreatedTime = *localtime(&lTime);
			inodeTable[inodeNum].createdTime.set_date_time(tmCreatedTime);
			//�ڸ�Ŀ¼�������Ϣ
			dirTemp.nSubInode[dirTemp.nSize] = (unsigned int)inodeNum;
			dirTemp.nSize++;
			if (dirTemp.inodeNum == dirCurPath.inodeNum)
			{
				dirCurPath = dirTemp;
			}
			//����
			virDisk.open("virtual_fs.bin", ios::out | ios::binary | ios::_Nocreate);
			if (!virDisk.is_open()) error(FATAL_WRITE);
			int i;
			for (i = 0; i < BLOCK_GROUPS_NUM; i++)
			{
				virDisk.write(reinterpret_cast<char *>(&dataBlockGroups[i]), sizeof(blockGroup));
			}
			virDisk.seekp((sizeof(blockGroup)*BLOCK_GROUPS_NUM + sizeof(bitmapStatus)*inodeNum), ios::beg);
			virDisk.write(reinterpret_cast<char *>(&bsInodeBmp[inodeNum]), sizeof(bitmapStatus));
			virDisk.seekp((sizeof(blockGroup)*BLOCK_GROUPS_NUM + sizeof(bitmapStatus)*INODES_NUM
				+ sizeof(i_node)*inodeNum), ios::beg);
			virDisk.write(reinterpret_cast<char *>(&inodeTable[inodeNum]), sizeof(i_node));
			virDisk.seekp((sizeof(blockGroup)*BLOCK_GROUPS_NUM + sizeof(bitmapStatus)*INODES_NUM
				+ sizeof(i_node)*INODES_NUM + sizeof(bitmapStatus)*nIndex), ios::beg);
			for (i = 0; i < DIR_SIZE; i++)
			{
				virDisk.write(reinterpret_cast<char *>(&bsBlockBmp[nIndex]), sizeof(bitmapStatus));
			}
			virDisk.seekp(lAddr, ios::beg);
			virDisk.write(reinterpret_cast<char *>(&dirNew), sizeof(dir));
			virDisk.seekp(inodeTable[dirTemp.inodeNum].lBlockAddr, ios::beg);
			virDisk.write(reinterpret_cast<char *>(&dirTemp), sizeof(dir));
			virDisk.close();
		}
	}
	else
	{
		error(MD_FAILED);
	}
}

// rd���ɾ��Ŀ¼
void cmd_rd(const char *strPath)
{
	dir dirTemp;
	size_t nPathLen = strlen(strPath);
	char strRmDirName[MAX_NAME_LENGTH];
	if (analyse_path(strPath, nPathLen, dirTemp, strRmDirName))
	{
		if (inodeTable[dirTemp.inodeNum].privilege == READ_ONLY)
		{
			error(DIR_READONLY, inodeTable[dirTemp.inodeNum].strName);
			return;
		}
		unsigned int i;
		unsigned int nPos = 0, inodeNum;
		//����Ŀ¼
		for (i = 2; i < dirTemp.nSize; i++)
		{
			if (strcmp(inodeTable[dirTemp.nSubInode[i]].strName, strRmDirName) == 0 &&
				inodeTable[dirTemp.nSubInode[i]].ftType == TYPE_DIR)
			{
				nPos = i;
				inodeNum = dirTemp.nSubInode[i];
				break;
			}
		}
		if (i == dirTemp.nSize)	//�Ҳ���Ŀ¼
		{
			error(DIR_NOT_EXIST, strPath);
		}
		else	//�ҵ�
		{
			dir dirRemove;
			//��ȡ��Ϣ
			virDisk.open("virtual_fs.bin", ios::in | ios::binary);
			if (!virDisk.is_open()) error(FATAL_READ);
			virDisk.seekg(inodeTable[inodeNum].lBlockAddr, ios::beg);
			virDisk.read(reinterpret_cast<char *>(&dirRemove), sizeof(dir));
			virDisk.close();
			//��ֹɾ����ǰĿ¼
			if (dirRemove.inodeNum == dirCurPath.inodeNum)
			{
				error(NO_DEL_CUR);
				return;
			}

			if (inodeTable[dirRemove.inodeNum].privilege == READ_ONLY)
			{
				error(DIR_READONLY, strPath);
				return;
			}
			if (dirRemove.nSize > 2)	//��������
			{
				char chSelect = '\0';
				cout << "Ŀ¼ " << inodeTable[dirRemove.inodeNum].strName << " ������Ŀ¼���ļ����Ƿ�ǿ��ɾ����(Y/N)\n";
				while (chSelect != 27)	//�û���ESCȡ��
				{
					chSelect = _getch();
					if (chSelect == 'y' || chSelect == 'Y' || chSelect == 'n' || chSelect == 'N')
					{
						cout.put(chSelect);
						break;
					}
					else
					{
						continue;
					}
				}
				if (chSelect == 'y' || chSelect == 'Y')
				{
					//ȫ��ɾ��
					dirTemp.remove_dir(dirRemove, nPos);
					cout << endl;
					return;
				}
				else
				{
					cout << endl;
					error(RD_FAILED);
					return;
				}
				cout << endl;
			}
			else	//Ŀ¼Ϊ�գ�ֱ��ɾ��
			{
				//���������Ϣ
				free_block(inodeTable[inodeNum].nBlocks, ((inodeTable[inodeNum].lBlockAddr - DATA_AREA_ADDR) / BLOCK_SIZE));
				free_inode(inodeNum);
				//��Ŀ¼��Ϣ
				for (i = nPos; i < dirTemp.nSize; i++)
					dirTemp.nSubInode[i] = dirTemp.nSubInode[i + 1];
				dirTemp.nSize--;
				if (dirTemp.inodeNum == dirCurPath.inodeNum) dirCurPath = dirTemp;
				//����
				virDisk.open("virtual_fs.bin", ios::out | ios::binary | ios::_Nocreate);
				if (!virDisk.is_open()) error(FATAL_WRITE);
				for (i = 0; i < BLOCK_GROUPS_NUM; i++)
				{
					virDisk.write(reinterpret_cast<char *>(&dataBlockGroups[i]), sizeof(blockGroup));
				}
				virDisk.seekp((sizeof(blockGroup)*BLOCK_GROUPS_NUM + sizeof(bitmapStatus)*inodeNum), ios::beg);
				virDisk.write(reinterpret_cast<char *>(bsInodeBmp), sizeof(bitmapStatus));
				virDisk.seekp((sizeof(blockGroup)*BLOCK_GROUPS_NUM + (sizeof(bitmapStatus) + sizeof(i_node))*INODES_NUM
					+ (inodeTable[inodeNum].lBlockAddr - DATA_AREA_ADDR) / BLOCK_SIZE), ios::beg);
				for (i = 0; i < (int)inodeTable[inodeNum].nBlocks; i++)
				{
					virDisk.write(reinterpret_cast<char *>(bsBlockBmp), sizeof(bitmapStatus));
				}
				virDisk.seekp(inodeTable[dirTemp.inodeNum].lBlockAddr, ios::beg);
				virDisk.write(reinterpret_cast<char *>(&dirTemp), sizeof(dir));
				virDisk.close();
			}
		}
	}
	else
	{
		error(RD_FAILED);
	}
}

//newfile����������ļ�
void cmd_newfile(const char *strPath, fileAttribute privilege)
{
	dir dirTemp;
	size_t nPathLen = strlen(strPath);
	char strFileName[MAX_NAME_LENGTH];
	if (analyse_path(strPath, nPathLen, dirTemp, strFileName))
	{
		if (inodeTable[dirTemp.inodeNum].privilege == READ_ONLY)
		{
			error(DIR_READONLY, inodeTable[dirTemp.inodeNum].strName);
			return;
		}
		//Ŀ¼���Ѵ��ڸ�����
		if (dirTemp.have_child(strFileName))
		{
			error(FILE_EXIST, inodeTable[dirTemp.inodeNum].strName, strFileName);
			return;
		}
		unsigned long i;
		unsigned long nSize = 5;		//Ĭ���ļ��ߴ�
		char *strBuffer = new char[nSize];		//�ļ�����
		char *strMoreBuffer;					//�����ļ�����
		char chChar;		//�û�������ַ�
		unsigned long nCharNum = 0;	//���ַ���
		//��ʼ���ַ���
		for (i = 0; i < nSize; i++) strBuffer[i] = 0;
		cout << "�������ļ����ݣ��ԡ�$����β��" << endl;
		while ((chChar = cin.get()) != '$')
		{
			strBuffer[nCharNum] = chChar;
			nCharNum++;
			//Ĭ�Ϸ���ռ䲻�㣬����洢�ռ�
			if (nCharNum >= nSize - 1)
			{
				strMoreBuffer = new char[nSize];
				//���潻������
				strcpy(strMoreBuffer, strBuffer);
				delete(strBuffer);
				//����洢�ռ�
				nSize = nSize * 2;
				strBuffer = new char[nSize];
				for (i = 0; i < nSize; i++) strBuffer[i] = 0;
				//ȡ�ؽ�������
				strcpy(strBuffer, strMoreBuffer);
				delete(strMoreBuffer);
			}
		}
		cin.ignore();
		//�����ļ�
		dirTemp.save_file(strFileName, strBuffer, nCharNum, privilege);
		//ɾ���ڴ��е��ļ�����
		delete(strBuffer);
	}
	else
	{
		error(NEW_FILE_FAILED);
	}
}

//cat�����ʾ�ļ�����
void cmd_cat(const char *strPath)
{
	dir dirTemp;
	size_t nPathLen = strlen(strPath);
	char strFileName[MAX_NAME_LENGTH];
	if (analyse_path(strPath, nPathLen, dirTemp, strFileName))
	{
		if (inodeTable[dirTemp.inodeNum].privilege == WRITE_ONLY)
		{
			error(DIR_WRITEONLY, inodeTable[dirTemp.inodeNum].strName);
			return;
		}
		unsigned int i;
		unsigned int nInode;
		//�����ļ�
		for (i = 2; i < dirTemp.nSize; i++)
		{
			if (strcmp(inodeTable[dirTemp.nSubInode[i]].strName, strFileName) == 0 &&
				inodeTable[dirTemp.nSubInode[i]].ftType == TYPE_FILE)
			{
				nInode = dirTemp.nSubInode[i];
				break;
			}
		}
		if (i == dirTemp.nSize)	//�Ҳ����ļ�
		{
			error(FILE_NOT_EXIST, inodeTable[dirTemp.inodeNum].strName, strFileName);
		}
		else	//�ҵ�
		{
			if (inodeTable[nInode].privilege == WRITE_ONLY)
			{
				error(FILE_WRITEONLY, strPath);
				return;
			}
			//���ļ�
			char *strBuffer = new char[inodeTable[nInode].lSize];
			dirTemp.open_file(nInode, strBuffer);
			//��ʾ�ļ�����
			cout << "�ļ� " << strFileName << " ���������£�" << endl;
			cout << strBuffer << endl;
			delete(strBuffer);
		}
	}
	else
	{
		error(CAT_FAILED);
	}
}

//copy���ģ������и����ļ�
void cmd_copy(const char *strSrcPath, const char *strDesPath)
{
	char strFileName[MAX_NAME_LENGTH];
	char *strBuffer;
	dir dirTemp;
	char strDiv;
	long nLen = 0;
	size_t nSrcLen = strlen(strSrcPath);
	size_t nDesLen = strlen(strDesPath);
	//����·���������޸�
	char *strSrcFinalPath = new char[nSrcLen];
	char *strDesFinalPath = new char[nDesLen];
	strcpy(strSrcFinalPath, strSrcPath);
	strcpy(strDesFinalPath, strDesPath);
	
	if (if_host_path(strSrcFinalPath))
	{
		if (if_host_path(strDesFinalPath))
		{
			error(NOT_BOTH_HOST);
			return;
		}
		
		fstream fsHostIn;
		fsHostIn.open(strSrcFinalPath, ios::in | ios::binary);
		if (!fsHostIn.is_open())
		{
			error(HOST_FILE_NOT_EXIST, strSrcFinalPath);
			return;
		}
		//�����ļ�����
		fsHostIn.seekg(0, ios::end);
		nLen = fsHostIn.tellg();
		//����洢�ռ�
		strBuffer = new char[nLen];
		strBuffer[nLen - 1] = 0;
		fsHostIn.seekg(0, ios::beg);
		fsHostIn.read(reinterpret_cast<char *>(strBuffer), nLen - 1);
		fsHostIn.close();
		//��ȡ�ļ���
		strDiv = '\\';
		strcpy(strFileName, strrchr(strSrcFinalPath, strDiv) + 1);
		//����Ŀ��·��
		if (analyse_path(strDesFinalPath, nDesLen, dirTemp))
		{
			if (inodeTable[dirTemp.inodeNum].privilege == READ_ONLY)
			{
				error(DIR_READONLY, strDesFinalPath);
				delete(strBuffer);
				return;
			}
			//�ж�Ŀ¼�Ƿ��Ѵ���ͬ������
			if (dirTemp.have_child(strFileName))
			{
				delete(strBuffer);
				error(FILE_EXIST, inodeTable[dirTemp.inodeNum].strName, strFileName);
				return;
			}
			//���浽����
			dirTemp.save_file(strFileName, strBuffer, nLen, READ_WRITE);
			delete(strBuffer);
			cout << "�ļ�������ɣ�\n";
		}
		else
		{
			error(COPY_FAILED);
		}
	}
	else	//��һ������������ <host>
	{
		if (if_host_path(strDesFinalPath))	//ģ������ļ����Ƶ� host ��
		{
			//�ָ�·�����õ��ļ���
			if (analyse_path(strSrcFinalPath, nSrcLen, dirTemp, strFileName))
			{
				unsigned int nInode;
				unsigned int i;
				for (i = 2; i < dirTemp.nSize; i++)
				{
					if (strcmp(inodeTable[dirTemp.nSubInode[i]].strName, strFileName) == 0 &&
						inodeTable[dirTemp.nSubInode[i]].ftType == TYPE_FILE)
					{
						nInode = dirTemp.nSubInode[i];
						break;
					}
				}
				if (i == dirTemp.nSize)	//�Ҳ����ļ�
				{
					error(FILE_NOT_EXIST, inodeTable[dirTemp.inodeNum].strName, strFileName);
				}
				else
				{
					if (inodeTable[nInode].privilege == WRITE_ONLY)
					{
						error(FILE_WRITEONLY, strSrcFinalPath);
						return;
					}
					//��ȡ�ļ����ڴ�
					strBuffer = new char[inodeTable[nInode].lSize];
					nLen = dirTemp.open_file(nInode, strBuffer);
					//�ϲ�Ϊ������ȫ·��
					char *strFullPath = new char[nSrcLen + nDesLen + 2];
					stringstream ssStream;
					ssStream << strDesFinalPath;
					if (strDesFinalPath[nDesLen - 1] != '\\') ssStream << "\\";
					ssStream << strFileName;
					ssStream >> strFullPath;
					ssStream.clear();
					//д���ļ���������ϵͳ
					fstream fsHostOut;
					fsHostOut.open(strFullPath, ios::out | ios::binary);
					if (!fsHostOut.is_open())
					{
						error(HOST_FILE_WRITE_FAILED, strFullPath);
						delete(strBuffer);
						delete(strFullPath);
						return;
					}
					fsHostOut.write(reinterpret_cast<char *>(strBuffer), nLen);
					fsHostOut.close();
					delete(strFullPath);
					delete(strBuffer);
					cout << "�ļ�������ɣ�\n";
				}
			}
			else
			{
				error(COPY_FAILED);
			}
		}
		else	//ģ��������ļ�����
		{
			//�ָ�·�����õ��ļ���
			if (analyse_path(strSrcFinalPath, nSrcLen, dirTemp, strFileName))
			{
				unsigned int nInode;
				//�����ļ�
				unsigned int i;
				for (i = 2; i < dirTemp.nSize; i++)
				{
					if (strcmp(inodeTable[dirTemp.nSubInode[i]].strName, strFileName) == 0 &&
						inodeTable[dirTemp.nSubInode[i]].ftType == TYPE_FILE)
					{
						nInode = dirTemp.nSubInode[i];
						break;
					}
				}
				if (i == dirTemp.nSize)	//�Ҳ����ļ�
				{
					error(FILE_NOT_EXIST, inodeTable[dirTemp.inodeNum].strName, strFileName);
				}
				else	//�ҵ�
				{
					if (inodeTable[nInode].privilege == FILE_WRITEONLY)
					{
						error(FILE_WRITEONLY, strSrcFinalPath);
						return;
					}
					fileAttribute privilege = inodeTable[nInode].privilege;
					strBuffer = new char[inodeTable[nInode].lSize];
					nLen = dirTemp.open_file(nInode, strBuffer);
					//�ϲ�Ϊģ�����ȫ·��
					char *strFullPath = new char[nSrcLen + nDesLen + 2];
					stringstream ssStream;
					ssStream << strDesFinalPath;
					if (strDesFinalPath[nDesLen - 1] != '/') ssStream << "/";
					ssStream << strFileName;
					ssStream >> strFullPath;
					ssStream.clear();
					//�ָ�Ŀ��·�����õ��ļ���
					if (analyse_path(strFullPath, nSrcLen + nDesLen + 2, dirTemp, strFileName))
					{
						if (inodeTable[dirTemp.inodeNum].privilege == READ_ONLY)
						{
							error(DIR_READONLY, strDesFinalPath);
							delete(strBuffer);
							delete(strFullPath);
							return;
						}
						//�ж�Ŀ¼�Ƿ��Ѵ���ͬ������
						if (dirTemp.have_child(strFileName))
						{
							delete(strBuffer);
							delete(strFullPath);
							error(FILE_EXIST, inodeTable[dirTemp.inodeNum].strName, strFileName);
							return;
						}
						//�����ļ�
						dirTemp.save_file(strFileName, strBuffer, nLen, privilege);
						cout << "�ļ�������ɣ�\n";
					}
					else
					{
						error(COPY_FAILED);
					}
					delete(strBuffer);
					delete(strFullPath);
				}
			}
			else
			{
				error(COPY_FAILED);
			}
		}
	}
}

//del���ɾ���ļ�
void cmd_del(const char *strPath)
{
	dir dirTemp;
	char strFileName[MAX_NAME_LENGTH];
	size_t nPathLen = strlen(strPath);
	//���·������Ч·������ȡ�ļ�����������ɾ����������Ч�򱨴�
	if (analyse_path(strPath, nPathLen, dirTemp, strFileName))
	{
		if (inodeTable[dirTemp.inodeNum].privilege == READ_ONLY)
		{
			error(DIR_READONLY, inodeTable[dirTemp.inodeNum].strName);
			return;
		}
		dirTemp.delete_file(strFileName);
	}
	else
	{
		error(DEL_FAILED);
	}
}

//check�����Ⲣ�ָ��ļ�ϵͳ�����ļ�ϵͳ�е�����һ���Խ��м�⣬���Զ������ļ�ϵͳ�Ľṹ����Ϣ��������������
void cmd_check()
{
	int i, j;
	int nStart;				//��ʼ��ַ
	bool bException = false;	//�Ƿ����쳣�ı�־
	unsigned int nFreeBlockNum, nFreeInodeNum;			//���п��i���
	unsigned int nFreeBlockAll = 0, nFreeInodeAll = 0;	//���п��п��i���
	cout << "����ļ�ϵͳing����" << endl;
	for (i = 0; i < BLOCK_GROUPS_NUM; i++)
	{
		nFreeBlockNum = 0;
		nFreeInodeNum = 0;
		nStart = i * BLOCKS_EACH;
		//������п�Ϳ���i����ܺ�
		for (j = 0; j < BLOCKS_EACH; j++)
		{
			if (bsBlockBmp[nStart + j] == NOT_USED) nFreeBlockNum++;
			if (bsInodeBmp[nStart + j] == NOT_USED) nFreeInodeNum++;
		}
		//�������ʹ��̼�¼��ͬ���������쳣
		if (dataBlockGroups[i].d_g_info.freeBlocksCountNum != nFreeBlockNum)
		{
			bException = true;
			dataBlockGroups[i].d_g_info.freeBlocksCountNum = nFreeBlockNum;
		}
		if (dataBlockGroups[i].d_g_info.freeInodesCountNum != nFreeInodeNum)
		{
			bException = true;
			dataBlockGroups[i].d_g_info.freeInodesCountNum = nFreeInodeNum;
		}
		//�����ܺ�
		nFreeBlockAll += dataBlockGroups[i].d_g_info.freeBlocksCountNum;
		nFreeInodeAll += dataBlockGroups[i].d_g_info.freeInodesCountNum;
	}
	//�������ʹ��̼�¼��ͬ���������쳣
	if (dataBlockGroups[0].s_blocks.freeBlocksCount != nFreeBlockAll)
	{
		bException = true;
		for (i = 0; i < BLOCKS_EACH; i++)
			dataBlockGroups[0].s_blocks.freeBlocksCount = nFreeBlockAll;
	}
	if (dataBlockGroups[0].s_blocks.freeInodesCount != nFreeInodeAll)
	{
		bException = true;
		for (i = 0; i < BLOCKS_EACH; i++)
			dataBlockGroups[0].s_blocks.freeInodesCount = nFreeInodeAll;
	}
	if (!bException)
	{
		cout << "�����ɣ�û�з����ļ�ϵͳ�쳣" << endl;
	}
	else	//����Ķ�
	{
		cout << "��鷢���ļ�ϵͳ�����쳣�������޸��С���" << endl;
		virDisk.open("virtual_fs.bin", ios::out | ios::binary | ios::_Nocreate);
		if (!virDisk.is_open()) error(FATAL_WRITE);
		for (int i = 0; i < BLOCK_GROUPS_NUM; i++)
			virDisk.write(reinterpret_cast<char *>(&dataBlockGroups[i]), sizeof(blockGroup));
		for (int j = 0; j < INODES_NUM; j++)
			virDisk.write(reinterpret_cast<char *>(&bsInodeBmp[j]), sizeof(bitmapStatus));
		for (int k = 0; k < INODES_NUM; k++)
			virDisk.write(reinterpret_cast<char *>(&inodeTable[k]), sizeof(blockGroup));
		for (int l = 0; l < BLOCKS_NUM; l++)
			virDisk.write(reinterpret_cast<char *>(&bsBlockBmp[l]), sizeof(bitmapStatus));
		virDisk.seekp(inodeTable[dirCurPath.inodeNum].lBlockAddr, ios::beg);
		virDisk.write(reinterpret_cast<char *>(&dirCurPath), sizeof(dir));
		virDisk.close();
		cout << "�ļ�ϵͳ�޸����" << endl;
	}
}