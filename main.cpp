#include <iostream>
#include <sstream>
#include <string>
#include <conio.h>
#include "simdisk.h"
using namespace std;

fstream virDisk;//��������ļ�
char curPath[MAX_PATH_LENGTH];//��ǰĿ¼·��
dir dirCurPath;//��ǰĿ¼
blockGroup dataBlockGroups[BLOCK_GROUPS_NUM];//���ݿ���
i_node inodeTable[INODES_NUM];//i����
bitmapStatus bsBlockBmp[BLOCKS_NUM];//���ݿ�λͼ����
bitmapStatus bsInodeBmp[INODES_NUM];//i���λͼ����

//ִ������
void execute(const char *comm, const char* p1, const char* p2)
{
	//��ȡ������
	unsigned int i = 0;
	for (i = 0; i < COMMAND_COUNTS; i++)
	{
		if (strcmp(cmdCommands[i], comm) == 0)//�õ�������֮��ֹͣ����
		{
			break;
		}
	}
	char input_cmd = '\0';	
	//���ݱ��ִ������
	switch (i)
	{
	case 0:		
		//init
		if (p1[0] != '\0')//��Ϊֻ��һ��������p1����Ϊ�����������
		{
			error(WRONG_ARGS, comm);//��������
			return;
		}
		cout << "warning����ʼ��֮��������̵�������Ϣ����ʧ��" << endl;
		cout << "��ѡ���Ƿ��ֳ�ʼ����(Y/N)��ȡ����ESC" << endl;
		//��ESCȡ����ASCII
		while (input_cmd != 27)	
		{
			input_cmd = _getch();//��ȡ
			if (input_cmd == 'y' || input_cmd == 'Y' || input_cmd == 'n' || input_cmd == 'N')
			{
				cout.put(input_cmd);//����Ļ�����
				break;
			}
			else
			{
				continue;
			}
		}
		if (input_cmd == 'y' || input_cmd == 'Y')
		{
			virDisk.clear();//���
			cmd_init();
			cin.sync();//���������
			cmd_cd("/");
			system("cls");//�����Ļ
		}
		else
		{
			error(CANCEL_INIT);
		}
		break;
	case 1:		
		//info
		if (p1[0] != '\0')
		{
			error(WRONG_ARGS, comm);
			return;
		}
		cmd_info();
		break;
	case 2:		
		//cd
		if (p2[0] != '\0')
		{
			error(WRONG_ARGS, comm);
			return;
		}
		cmd_cd(p1);
		break;
	case 3:		
		//dir
		if (p2[0] != '\0')
		{
			error(WRONG_ARGS, comm);
			return;
		}
		if (p1[0] == '\0')
			cmd_dir(".");
		else
			cmd_dir(p1);
		break;
	case 4:		
		//md
		if (p1[0] == '\0')
		{
			error(WRONG_ARGS, comm);
			return;
		}
		if (p2[0] != '\0')
		{
			if (strcmp("/r", p2) == 0)
			{
				cmd_md(p1, READ_ONLY);
				return;
			}
			if (strcmp("/w", p2) == 0)
			{
				cmd_md(p1, WRITE_ONLY);
				return;
			}
			if (strcmp("/a", p2) == 0)
			{
				cmd_md(p1, READ_WRITE);
				return;
			}
			if (strcmp("/s", p2) == 0)
			{
				cmd_md(p1, SHARE);
				return;
			}
			error(WRONG_ARGS, comm);
			return;
		}
		else
		{
			cmd_md(p1, READ_WRITE);
		}
		break;
	case 5:		
		//rd
		if (p1[0] == '\0' || p2[0] != '\0')
		{
			error(WRONG_ARGS, comm);
			return;
		}
		cmd_rd(p1);
		break;
	case 6:		
		//newfile
		if (p1[0] == '\0')
		{
			error(WRONG_ARGS, comm);
			return;
		}
		if (p2[0] != '\0')
		{
			if (strcmp("/r", p2) == 0)
			{
				cmd_newfile(p1, READ_ONLY);
				return;
			}
			if (strcmp("/w", p2) == 0)
			{
				cmd_newfile(p1, WRITE_ONLY);
				return;
			}
			if (strcmp("/a", p2) == 0)
			{
				cmd_newfile(p1, READ_WRITE);
				return;
			}
			if (strcmp("/s", p2) == 0)
			{
				cmd_newfile(p1, SHARE);
				return;
			}
			error(WRONG_ARGS, comm);
			return;
		}
		cmd_newfile(p1, READ_WRITE);
		break;
	case 7:		
		//cat
		if (p1[0] == '\0' || p2[0] != '\0')
		{
			error(WRONG_ARGS, comm);
			return;
		}
		cmd_cat(p1);
		break;
	case 8:		
		//copy
		if (p2[0] == '\0')
		{
			error(WRONG_ARGS, comm);
			return;
		}
		cmd_copy(p1, p2);
		break;
	case 9:		
		//del
		if (p1[0] == '\0' || p2[0] != '\0')
		{
			error(WRONG_ARGS, comm);
			return;
		}
		cmd_del(p1);
		break;
	case 10:	
		//check
		if (p1[0] != '\0')
		{
			error(WRONG_ARGS, comm);
			return;
		}
		cmd_check();
		break;
	case 11:	
		//exit
		if (p1[0] != '\0')
		{
			error(WRONG_ARGS, comm);
			return;
		}
		cmd_exit();
		break;
	default:
		error(INVILID_CMD, comm);
	}
}


int main(int argc, char * argv[])
{
	//����
	load();
	cin.sync();
	system("cls");
	const unsigned int iCmdLength = MAX_COMMAND_LENGTH + MAX_PATH_LENGTH * 2 + 2;
	char commLine[iCmdLength];
	//�������
	char comm[MAX_COMMAND_LENGTH], p1[MAX_PATH_LENGTH], p2[MAX_PATH_LENGTH];		
	stringstream ssStream;
	while (1)	
	{
		//��ǰ·��
		cout << endl << curPath << ">";	
		cin.getline(commLine, iCmdLength);
		ssStream << commLine;
		ssStream >> comm >> p1 >> p2;
		//������Ϊ�մ�
		if (comm[0] == '\0')
		{
			ssStream.clear();
			continue;
		}
		//��������������
		if (!ssStream.eof())
		{
			error(TOO_MANY_ARGS);
			ssStream.str("");
			ssStream.clear();
			continue;
		}
		ssStream.clear();
		execute(comm, p1, p2);
	}
	return 1;
}
