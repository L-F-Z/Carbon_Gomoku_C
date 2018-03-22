#include "stdio.h"
#include "stdlib.h"
char board[15][31]={"���өөөөөөөөөөөөө�",
                    "�ĩ��������������",
					"�ĩ��������������",
					"�ĩ��������������",
					"�ĩ��������������",
					"�ĩ��������������",
					"�ĩ��������������",
					"�ĩ��������������",
					"�ĩ��������������",
					"�ĩ��������������",
					"�ĩ��������������",
					"�ĩ��������������",
					"�ĩ��������������",
					"�ĩ��������������",
					"���۩۩۩۩۩۩۩۩۩۩۩۩۩�"};
char chess[2][3]={"��","��"};
char name[4][6]={"���1","���2","���","����"};

void PrintBoard()
{
	int i,j;
	for (i=0; i<15; i++)
	{
		printf(" %2d",15-i);
		for (j=0; j<30; j++)
			printf("%c",board[i][j]);
		printf("\n");
	}
	printf("   ");
	for (i=0; i<15; i++)
		printf("%c ",'A'+i);
	printf("\n");
	return;
}

int main() {  
	FILE *fp;
	int player1,player2,step,i,x,y;  
	if((fp=fopen("Gomoku.log","r"))==NULL)
	{
		printf("Cannot Find Log/n");
		return 1;
    }
	fscanf(fp,"%d,%d,%d\n",&player1,&player2,&step);
	printf("             ������Ϣ\n           ִ�� - %s\n           ִ�� - %s\n           ʤ�� - ",name[player1-1],name[player2-1]);
	if (step%2)
		printf("%s\n",name[player1-1]);
	else
		printf("%s\n",name[player2-1]);
	printf("           ���� - %d\n\n",step);
	system("pause");
	system("cls");
	for (i=0; i<step; i++)
	{
		fscanf(fp,"%d,%d",&x,&y);
		board[x-1][2*y-2]=chess[i%2][0];
		board[x-1][2*y-1]=chess[i%2][1];
		printf("            - �� %d �� -\n             %s �� %d%c\n",i+1,chess[i%2],16-x,'A'+y-1);
		PrintBoard();
		printf("      %s - %s    %s - %s\n",name[player1-1],chess[0],name[player2-1],chess[1]);
		system("pause");
		system("cls");
	}
	printf("���ļ�¼չʾ����\n");
	fclose(fp);
	return 0;
}
