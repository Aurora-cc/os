#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <conio.h>
#include "simdisk.h"
using namespace std;

//����
void load()
{
	system("cls");
	virDisk.open("virtual_fs.bin", ios::in | ios::binary);
	if (!virDisk.is_open())
	{
		char chSelect = '\0';
		cout << "----------δ��ʼ��----------" << endl;
		cout << "��δ��ʼ��ģ�� Linux �ļ�ϵͳ���Ƿ���У�(Y/N)" << endl;
		cout << "��ѡ��";
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
			virDisk.clear();
			cmd_init();
			return;
		}
		else
		{
			error(FATAL_READ);
		}
	}
	unsigned int i;
	for (i = 0; i < BLOCK_GROUPS_NUM; i++)
		virDisk.read(reinterpret_cast<char *>(&dataBlockGroups[i]), sizeof(blockGroup));
	for (i = 0; i < INODES_NUM; i++)
		virDisk.read(reinterpret_cast<char *>(&bsInodeBmp[i]), sizeof(bitmapStatus));
	for (i = 0; i < INODES_NUM; i++)
		virDisk.read(reinterpret_cast<char *>(&inodeTable[i]), sizeof(i_node));
	for (int i = 0; i < BLOCKS_NUM; i++)
		virDisk.read(reinterpret_cast<char *>(&bsBlockBmp[i]), sizeof(bitmapStatus));
	virDisk.read(reinterpret_cast<char *>(&dirCurPath), sizeof(dir));
	virDisk.close();
	//���Ƶ�ǰ·��
	strcpy(curPath, inodeTable[dirCurPath.inodeNum].strName);
}

//�˳�����
void cmd_exit()
{
	system("cls");
	exit(0);
}

//��������ʱ��
void date_time::set_date_time(tm t)
{
	this->year = t.tm_year;
	this->mon = t.tm_mon;
	this->day = t.tm_mday;
	this->hour = t.tm_hour;
	this->min = t.tm_min;
	this->sec = t.tm_sec;
}

//��ȡ����ʱ��
tm date_time::get_date_time()
{
	tm t;
	t.tm_year = this->year;
	t.tm_mon = this->mon;
	t.tm_mday = this->day;
	t.tm_hour = this->hour;
	t.tm_min = this->min;
	t.tm_sec = this->sec;
	return t;
}