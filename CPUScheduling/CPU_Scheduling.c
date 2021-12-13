#define _CRT_SECURE_NO_WARNINGS
/**************************************************************************************
 ���α׷��� : CPU SCHEDULING
 ���α׷��� ���� : printLine(), printGantt(), ť �Լ�, ���μ��� ����ü,
					fcfs(), sjf(), hrn(), nopreemptivePriority(), preemptivePriority(),
					roundRobin(), srt(), printMenu(), main()
 ����ȯ�� : Windows 11, intel core i5 8th Gen
 ����ȯ�� : Visual Studio 2017
 �ۼ��� :JihoKim0
 ��� : �Է��� �޴��� ���� CPU ������ ��Ʈ��Ʈ, ���ð�, ����ð�, ��ȯ�ð�
***************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

int MAX_SIZE = 0;
int totalTime = 0;
int timeslice = 0;
typedef int element;
typedef struct {
	int front;
	int rear;
	element *data;
}queue;
//���μ��� ����ü(�Է�)
typedef struct process {
	char processID[10];	//���μ���ID
	int arriveTime;	//�����ð�
	int execTime;	//����ð�
	float priority;	//�켱����
	int firstTime;	//ù ��° ����
	int waitTime;	//���ð�
	int returnTime;	//��ȯ�ð�
	int respondTime;	//����ð�
	int *gantt;	//��Ʈ��Ʈ �迭
}process;

//ť �Լ� ����
void error(char *message)
{
	fprintf(stderr, "%s\n", message);
	exit(1);
}
void init_queue(queue *q)
{
	q->rear = -1;
	q->front = -1;
}
int is_full(queue *q)
{
	if (q->rear == MAX_SIZE - 1)
		return 1;
	else
		return 0;
}
int is_empty(queue *q)
{
	if (q->front == q->rear)
		return 1;
	else
		return 0;
}
void enqueue(queue *q, int item)
{
	if (is_full(q)) {
		error("ť ��ȭ����");
		return;
	}
	q->data[++(q->rear)] = item;
}
int dequeue(queue *q)
{
	if (is_empty(q)) {
		error("ť �������");
		return 0 - 1;
	}
	int item = q->data[++(q->front)];
	return item;
}
void printLine(void)
{
	for (int i = 0; i < (totalTime * 2 + 5); i++)
		printf("-");
	printf("\n");
}
void printGantt(process *p)	//�������Լ�
{
	int i, j;
	float avgWait = 0, avgRespond = 0, avgReturn = 0;
	for (i = 0; i < MAX_SIZE; i++) {	//��Ʈ��Ʈ
		printLine(totalTime);
		printf("%s | ", p[i].processID);
		for (j = 0; j < totalTime; j++) {
			if (p[i].gantt[j] == 1)
				printf("��");
			else
				printf("  ");
		}
		printf("\n");
		printLine(totalTime);
	}
	printf("time");
	for (i = 0; i < totalTime; i++) {
		if (i % 10 == 0)
			printf("%d                  ", i);
	}
	//���ð�
	printf("\n[�� ���μ��� �� ���ð�]\n");
	for (i = 0; i < MAX_SIZE; i++) {
		printf("%s : %d\t", p[i].processID, p[i].waitTime);
		avgWait += (float)p[i].waitTime;
	}
	printf("\n��� ��� �ð� : %f\n", avgWait / (float)MAX_SIZE);
	//����ð�
	printf("\n[�� ���μ��� �� ����ð�]\n");
	for (i = 0; i < MAX_SIZE; i++) {
		printf("%s : %d\t", p[i].processID, p[i].respondTime);
		avgRespond += (float)p[i].respondTime;
	}
	printf("\n��� ���� �ð� : %f\n", avgRespond / (float)MAX_SIZE);
	//��ȯ�ð�
	printf("\n[�� ���μ��� �� ��ȯ�ð�]\n");
	for (i = 0; i < MAX_SIZE; i++) {
		printf("%s : %d\t", p[i].processID, p[i].returnTime);
		avgReturn += (float)p[i].returnTime;
	}
	printf("\n��� ��ȯ �ð� : %f\n\n", avgReturn / (float)MAX_SIZE);
}
void fcfs(process *p)
{
	int i, j, time = 0, count = 0;	//time : �ð� ����, count : ���μ��� �ε���
	queue waitQ;	//���ť ����
	init_queue(&waitQ);
	waitQ.data = (int*)calloc(MAX_SIZE, sizeof(int));
	for (i = 0; i < MAX_SIZE; i++)
		enqueue(&waitQ, i);
	for (i = 0; i < MAX_SIZE; i++) {
		count = waitQ.data[i];
		if (p[count].arriveTime <= time) {
			p[count].waitTime = time - p[count].arriveTime;
			for (j = 0; j < p[count].execTime; j++) {
				p[count].gantt[time] = 1;
				time++;
				p[count].firstTime--;
				if (p[count].firstTime == 0)
					p[count].respondTime = time - p[count].arriveTime;
			}
			p[count].returnTime = time - p[count].arriveTime;
			dequeue(&waitQ);
		}
	}
	printGantt(p);
	free(waitQ.data);
}
void sjf(process *p)
{
	int temp = 0, time = 0;
	int i, j;
	for (i = 0; i < MAX_SIZE; i++) {	//���μ��� ����ŭ �ݺ�
		for (j = 0; j < MAX_SIZE; j++) {
			if (p[temp].arriveTime<time && p[temp].execTime > p[j].execTime)
				temp = j;
		}
		p[temp].waitTime = time - p[temp].arriveTime;
		for (j = 0; j < p[temp].execTime; j++) {
			p[temp].gantt[time] = 1;
			time++;
			p[temp].firstTime--;
			if (p[temp].firstTime == 0)
				p[temp].respondTime = time - p[temp].arriveTime;
		}
		p[temp].returnTime = time - p[temp].arriveTime;
		p[temp].execTime = 100;
	}
	printGantt(p);
}
void hrn(process *p)
{
	int i, j, temp;
	int time = 0;
	for (i = 0; i < MAX_SIZE; i++)	//�� �켱���� �ʱ�ȭ
		p[i].priority = -1;

	//�� ���μ��� ���� ���� �� ���� �켱���� ������Ʈ
	for (i = 0; i < MAX_SIZE; i++) {	//����ð� ������ ���μ����� �� �켱���� ���� ��
		temp = 0;
		for (j = 0; j < MAX_SIZE; j++) {
			if (p[j].priority == -2)
				continue;
			p[j].waitTime = time - p[j].arriveTime;
			p[j].priority = ((float)p[j].waitTime + (float)p[j].execTime) / (float)p[j].execTime;
			if ((p[j].arriveTime <= time) && (p[temp].priority < p[j].priority))
				temp = j;
		}
		p[temp].waitTime = time - p[temp].arriveTime;
		for (j = 0; j < p[temp].execTime; j++) {
			p[temp].gantt[time] = 1;
			time++;
			p[temp].firstTime--;
			if (p[temp].firstTime == 0)
				p[temp].respondTime = time - p[temp].arriveTime;
		}
		p[temp].returnTime = time - p[temp].arriveTime;
		p[temp].priority = -2;	//�켱���� �ʱ�ȭ
	}
	printGantt(p);
}
void nopreemptivePriority(process *p)	//���� �켱����
{
	int i, j, temp;
	int time = 0;
	for (i = 0; i < MAX_SIZE; i++) {
		temp = 0;
		for (j = 1; j < MAX_SIZE; j++) {
			if (p[j].arriveTime <= time && p[temp].priority > p[j].priority) {
				temp = j;
			}
		}
		p[temp].waitTime = time - p[temp].arriveTime;
		for (j = 0; j < p[temp].execTime; j++) {
			p[temp].gantt[time] = 1;
			time++;
			p[temp].firstTime--;
			if (p[temp].firstTime == 0)
				p[temp].respondTime = time - p[temp].arriveTime;
		}
		p[temp].returnTime = time - p[temp].arriveTime;
		p[temp].priority = 100;
	}
	printGantt(p);
}
void preemptivePriority(process *p)	//���� �켱����
{
	int i, j, k, temp, time = 0, count = 0, turn = 0;
	while (time < totalTime) {
		temp = 0;
		for (i = 1; i < MAX_SIZE; i++)
			if ((p[i].arriveTime <= time) && (p[i].priority < p[temp].priority))
				temp = i;

		if (p[temp].waitTime == 0)
			p[temp].waitTime = time - p[temp].arriveTime;
		else
			p[temp].waitTime += time - (p[temp].returnTime + p[temp].arriveTime);

		if (p[temp].execTime < timeslice)
			turn = p[temp].execTime;
		else
			turn = timeslice;
		for (i = 0; i < turn; i++) {
			p[temp].gantt[time] = 1;
			time++;
			p[temp].execTime--;
			p[temp].firstTime--;
			if (p[temp].firstTime == 0)
				p[temp].respondTime = time - p[temp].arriveTime;
		}
		p[temp].returnTime = time - p[temp].arriveTime;
		if (p[temp].execTime == 0)
			p[temp].priority = 100;
	}
	printGantt(p);
}
void roundRobin(process *p)
{
	int i;
	int time = 0, turn = 0, count = 0;
	while (time < totalTime)
	{
		if ((p[count].arriveTime > time) || (p[count].execTime == 0)) {
			count++;
			if (count == MAX_SIZE)
				count = 0;
			continue;
		}
		if (p[count].waitTime == 0)
			p[count].waitTime = time - p[count].arriveTime;
		else
			p[count].waitTime += time - p[count].returnTime + p[count].arriveTime;

		if (p[count].execTime < timeslice)
			turn = p[count].execTime;
		else
			turn = timeslice;

		for (i = 0; i < turn; i++) {
			p[count].gantt[time] = 1;
			time++;
			p[count].execTime--;
			p[count].firstTime--;
			if (p[count].firstTime == 0)
				p[count].respondTime = time - p[count].arriveTime;
		}
		p[count].returnTime = time - p[count].arriveTime;
		count++;
		if (count == MAX_SIZE)
			count = 0;
	}
	printGantt(p);
}
void srt(process *p)
{
	int i, j, k, temp, time = 0, count = 0, turn = 0;
	while (time < totalTime) {
		temp = 0;
		for (i = 1; i < MAX_SIZE; i++)
			if ((p[i].arriveTime <= time) && p[i].execTime < p[temp].execTime)
				temp = i;

		if (p[temp].execTime >= 100) {
			time++;
			continue;
		}

		if (p[temp].waitTime == 0)
			p[temp].waitTime = time - p[temp].arriveTime;
		else
			p[temp].waitTime += time - (p[temp].returnTime + p[temp].arriveTime);

		if (p[temp].execTime < timeslice)
			turn = p[temp].execTime;
		else
			turn = timeslice;
		for (i = 0; i < turn; i++) {
			p[temp].gantt[time] = 1;
			time++;
			p[temp].execTime--;
			p[temp].firstTime--;
			if (p[temp].firstTime == 0)
				p[temp].respondTime = time - p[temp].arriveTime;
		}
		p[temp].returnTime = time - p[temp].arriveTime;
		if (p[temp].execTime == 0)
			p[temp].execTime = 100;
	}

	printGantt(p, totalTime);
}
void printMenu(void)
{
	printf("*******MENU*******\n");
	printf("1.FCFS\n2.SJF\n3.���� Priority\n");
	printf("4.���� Priority\n5.RR\n6.SRT\n7.HRN\n");
	printf("******************\n");
}
int main(void)
{
	system("mode con cols=140 lines=40");
	int menu;
	FILE *fp = fopen("process.txt", "r");
	if (fp == NULL) {
		printf("FILE OPEN ERROR");
		return 0;
	}
	//�����Է����� ���μ��� �� �б�
	fscanf(fp, "%d", &MAX_SIZE);
	//�Է¹��� ���μ��� ����ŭ process ����ü �迭 �����Ҵ�
	process *p = (process *)malloc(MAX_SIZE * sizeof(process));
	//�����Է����� ���μ��� ������ �Է�
	for (int i = 0; i < MAX_SIZE; i++) {
		fscanf(fp, "%s %d %d %d", p[i].processID, &p[i].arriveTime, &p[i].execTime, &p[i].priority);
		totalTime += p[i].execTime;	//��ü ����ð��� �ش� ���μ����� ����ð� �߰�
		p[i].firstTime = 3;	//ù ��° �����ð� �ϰ� ����
	}
	//����������� ���μ��� ����
	//�����Է����� Ÿ�ӽ����̽� �Է�
	fscanf(fp, "%d", &timeslice);
	for (int i = 0; i < MAX_SIZE; i++)	//���� �Ҵ� & �ʱ�ȭ
		p[i].gantt = (int*)calloc(totalTime, sizeof(int));
	printMenu();
	printf("���ϴ� �޴��� �������ּ��� : ");
	scanf("%d", &menu);
	switch (menu) {
	case 1:
		fcfs(p);
		break;
	case 2:
		sjf(p);
		break;
	case 3:
		nopreemptivePriority(p);
		break;
	case 4:
		preemptivePriority(p);
		break;
	case 5:
		roundRobin(p);
		break;
	case 6:
		srt(p);
		break;
	case 7:
		hrn(p);
		break;
	default:
		printf("INPUT ERROR\n");
		break;
	}
	system("PAUSE");
	fclose(fp);
	free(p);
	return 0;
}