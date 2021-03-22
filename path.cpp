#include <iostream>
#include <sstream>
#include "simdisk.h"
using namespace std;

//��ʾ��ǰ��·�����ݹ����
void set_cur_path(dir dirCurDir)
{
	dir dirTemp = dirCurDir;
	//�ҵ���Ŀ¼root
	if (dirCurDir.inodeNum != 0)
	{
		//���Ҹ�Ŀ¼
		virDisk.open("virtual_fs.bin", ios::in | ios::binary);
		if (!virDisk.is_open()) error(FATAL_READ);
		virDisk.seekg(inodeTable[dirCurDir.nSubInode[1]].lBlockAddr, ios::beg);
		virDisk.read(reinterpret_cast<char *>(&dirCurDir), sizeof(dir));
		virDisk.close();
		set_cur_path(dirCurDir);
	}
	//���õ�ǰ·���ַ���
	if (dirTemp.inodeNum == 0)
	{
		strcpy(curPath, "root");
	}
	else
	{
		stringstream ssStream;
		ssStream << curPath << "/" << inodeTable[dirTemp.inodeNum].strName;
		ssStream >> curPath;
		ssStream.clear();
	}
}

// �ж�·�����Ƿ���� <host> ����
bool if_host_path(char *strPath)
{
	char *strDiv;
	strDiv = strstr(strPath, "<host>");
	if (strDiv == strPath)
	{
		strcpy(strPath, strDiv + 6);
		return true;
	}
	return false;
}

//��·������ȡ��Ŀ¼��
bool get_dir_name(const char *strPath, size_t nPathLen, size_t nPos, char *strFileName)
{
	char *strTemp = new char[nPathLen];		//��ʱ�ַ���
	char strDirName[MAX_NAME_LENGTH];	//�õ���Ŀ¼��
	char *strPos;		//�����Ӵ�
	//����ʱ�ַ������д���
	strcpy(strTemp, strPath);
	strPos = strtok(strTemp, "/");
	if (strPos == NULL) return false;
	strcpy(strDirName, strPos);
	strPos = strtok(NULL, "/");
	//ѭ������ֱ�����
	for (size_t i = 1; i < nPos; i++)
	{
		if (strPos)
		{
			if (strPos == NULL) return false;
			strcpy(strDirName, strPos);
			strPos = strtok(NULL, "/");
		}
		else
		{
			return false;
		}
	}
	//���Ƶ��ļ���
	strcpy(strFileName, strDirName);
	return true;
}

//��·���ַ���ת��Ϊ��Ӧ�� dir ��
bool path_to_dir(const char *strPath, size_t nPathLen, size_t &nPos, char *strDirName, dir &dirTemp)
{
	//��ȡÿһ��Ŀ¼������
	while (get_dir_name(strPath, nPathLen, nPos, strDirName))
	{
		unsigned int nDirSize = dirTemp.nSize;
		unsigned int i;
		for (i = 2; i < nDirSize; i++)
		{
			//�����Ƿ����ͬ����Ŀ¼
			if (strcmp(strDirName, inodeTable[dirTemp.nSubInode[i]].strName) == 0 &&
				inodeTable[dirTemp.nSubInode[i]].ftType == TYPE_DIR)
			{
				virDisk.seekg(inodeTable[dirTemp.nSubInode[i]].lBlockAddr, ios::beg);
				virDisk.read(reinterpret_cast<char *>(&dirTemp), sizeof(dir));
				break;
			}
		}
		if (i < nDirSize)	//�ҵ�ͬ����Ŀ¼������Լ���Ѱ����һ��
		{
			nPos++;
		}
		else	//�Ҳ���
		{
			error(PATH_NOT_FOUND);
			return false;
		}
	}
	return true;
}

//����·�����õ����յ�dir�����Ŀ¼�����ļ���
bool analyse_path(const char *strPath, size_t nPathLen, dir &dirTemp, char *strFileName)
{
	dirTemp = dirCurPath;//��ʱĿ¼
	const char *strDiv = strrchr(strPath, '/');
	if (strDiv)
	{
		int nDivPos = int(strDiv - strPath);
		int nLen = nDivPos + 1;
		char *strNewPath = new char[nLen];
		int i;
		//����ȡ���Ҷ��ļ�����Ŀ¼��
		for (i = 1; i < (int)nPathLen - nDivPos; i++)
			strFileName[i - 1] = strDiv[i];
		strFileName[i - 1] = 0;
		//����ȡ��ȥ���Ҷ��ļ�����Ŀ¼����ʣ��·��
		if (nDivPos > 0)
		{
			for (i = 0; i < nDivPos; i++)
				strNewPath[i] = strPath[i];
			strNewPath[i] = 0;
		}
		else	//���ֻ��һ�㣬��ֵΪ��Ŀ¼
		{
			delete(strNewPath);
			if (strPath[0] == '/')
			{
				strNewPath = new char[2];
				strNewPath[0] = '/';
				strNewPath[1] = '\0';
			}
			else
			{
				return true;
			}
		}
		//��ʼ���·��
		char strDirName[MAX_NAME_LENGTH];	//����Ŀ¼�����ļ���
		size_t nPos = 1;		//ָ��Ҫ�ֽ�Ϊ�ַ���λ��
		//��������һ��Ŀ¼������ȡ����ת��Ϊ dir
		get_dir_name(strNewPath, nLen, nPos, strDirName);
		virDisk.open("virtual_fs.bin", ios::in | ios::binary);
		if (!virDisk.is_open()) error(FATAL_READ);
		virDisk.seekg(inodeTable[0].lBlockAddr, ios::beg);
		virDisk.read(reinterpret_cast<char *>(&dirTemp), sizeof(dir));
		if (strNewPath[0] == '/' || strcmp(strDirName, "root") == 0)		//��Ŀ¼
		{
			if (strNewPath[0] != '/') nPos++;
			//���ҵ�·����Ӧ��Ŀ¼���� OK�����򷵻�ʧ��
			if (path_to_dir(strNewPath, nLen, nPos, strDirName, dirTemp))
			{
				virDisk.close();
				delete(strNewPath);
				return true;
			}
			else
			{
				virDisk.close();
				delete(strNewPath);
				return false;
			}
		}
		else if (strcmp(strDirName, "..") == 0)	//�Ǹ�Ŀ¼�������ϲ�Ŀ¼����������ȥ
		{
			//����
			virDisk.seekg(inodeTable[dirTemp.inodeNum].lBlockAddr, ios::beg);
			virDisk.read(reinterpret_cast<char *>(&dirTemp), sizeof(dir));
			nPos++;
			//���ҵ�·����Ӧ��Ŀ¼���� OK�����򷵻�ʧ��
			if (path_to_dir(strNewPath, nLen, nPos, strDirName, dirTemp))
			{
				virDisk.close();
				delete(strNewPath);
				return true;
			}
			else
			{
				virDisk.close();
				delete(strNewPath);
				return false;
			}
		}
		else	//�ڵ�ǰĿ¼������Ŀ¼
		{
			dirTemp = dirCurPath;
			if (strcmp(strDirName, ".") == 0) nPos++;
			//���ҵ�·����Ӧ��Ŀ¼���� OK�����򷵻�ʧ��
			if (path_to_dir(strNewPath, nLen, nPos, strDirName, dirTemp))
			{
				virDisk.close();
				delete(strNewPath);
				return true;
			}
			else
			{
				virDisk.close();
				delete(strNewPath);
				return false;
			}
		}
	}
	else	//û�о���·����Ĭ��Ϊ��ǰĿ¼
	{
		strcpy(strFileName, strPath);	//�����ļ���
		return true;
	}
}

//����·�����õ����յ� dir ����󣬲���ȡĿ¼�����ļ���
bool analyse_path(const char *strPath, size_t nPathLen, dir &dirTemp)
{
	dirTemp = dirCurPath;	//��ʱĿ¼
	if (strcmp(strPath, "..") == 0)	//�ϲ�Ŀ¼
	{
		virDisk.open("virtual_fs.bin", ios::in | ios::binary);
		if (!virDisk.is_open()) error(FATAL_READ);
		virDisk.seekg(inodeTable[dirTemp.nSubInode[1]].lBlockAddr, ios::beg);
		virDisk.read(reinterpret_cast<char *>(&dirTemp), sizeof(dir));
		virDisk.close();
		return true;
	}
	else if (strcmp(strPath, ".") == 0)	//��ǰĿ¼
	{
		//����ı�
		return true;
	}
	else
	{
		//��ʼ���·��
		char strDirName[MAX_NAME_LENGTH];	//����Ŀ¼�����ļ���
		size_t nPos = 1;		//ָ��Ҫ�ֽ�Ϊ�ַ���λ��
		//��������һ��Ŀ¼������ȡ����ת��Ϊ dir
		get_dir_name(strPath, nPathLen, nPos, strDirName);
		virDisk.open("virtual_fs.bin", ios::in | ios::binary);
		if (!virDisk.is_open()) error(FATAL_READ);
		virDisk.seekg(inodeTable[0].lBlockAddr, ios::beg);
		virDisk.read(reinterpret_cast<char *>(&dirTemp), sizeof(dir));
		if (strPath[0] == '/' || strcmp(strDirName, "root") == 0)		//��Ŀ¼
		{
			if (strPath[0] != '/') nPos++;
			//���ҵ�·����Ӧ��Ŀ¼���� OK�����򷵻�ʧ��
			if (path_to_dir(strPath, nPathLen, nPos, strDirName, dirTemp))
			{
				virDisk.close();
				return true;
			}
			else
			{
				virDisk.close();
				return false;
			}
		}
		else if (strcmp(strDirName, "..") == 0)	//�Ǹ�Ŀ¼�������ϲ�Ŀ¼����������ȥ
		{
			//����
			virDisk.seekg(inodeTable[dirTemp.inodeNum].lBlockAddr, ios::beg);
			virDisk.read(reinterpret_cast<char *>(&dirTemp), sizeof(dir));
			nPos++;
			//���ҵ�·����Ӧ��Ŀ¼���� OK�����򷵻�ʧ��
			if (path_to_dir(strPath, nPathLen, nPos, strDirName, dirTemp))
			{
				virDisk.close();
				return true;
			}
			else
			{
				virDisk.close();
				return false;
			}
		}
		else	//�ڵ�ǰĿ¼������Ŀ¼
		{
			dirTemp = dirCurPath;
			if (strcmp(strDirName, ".") == 0) nPos++;
			//�ҵ�·����Ӧ��Ŀ¼
			if (path_to_dir(strPath, nPathLen, nPos, strDirName, dirTemp))
			{
				virDisk.close();
				return true;
			}
			else
			{
				virDisk.close();
				return false;
			}
		}
	}
}