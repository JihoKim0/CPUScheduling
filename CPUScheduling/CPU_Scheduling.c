#define _CRT_SECURE_NO_WARNINGS
/**************************************************************************************
 프로그램명 : CPU SCHEDULING
 프로그램의 구성 : printLine(), printGantt(), 큐 함수, 프로세스 구조체,
					fcfs(), sjf(), hrn(), nopreemptivePriority(), preemptivePriority(),
					roundRobin(), srt(), printMenu(), main()
 구성환경 : Windows 11, intel core i5 8th Gen
 개발환경 : Visual Studio 2017
 작성자 :JihoKim0
 출력 : 입력한 메뉴에 따른 CPU 동작의 간트차트, 대기시간, 응답시간, 반환시간
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
//프로세스 구조체(입력)
typedef struct process {
	char processID[10];	//프로세스ID
	int arriveTime;	//도착시간
	int execTime;	//실행시간
	float priority;	//우선순위
	int firstTime;	//첫 번째 반응
	int waitTime;	//대기시간
	int returnTime;	//반환시간
	int respondTime;	//응답시간
	int *gantt;	//간트차트 배열
}process;

//큐 함수 정의
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
		error("큐 포화상태");
		return;
	}
	q->data[++(q->rear)] = item;
}
int dequeue(queue *q)
{
	if (is_empty(q)) {
		error("큐 공백상태");
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
void printGantt(process *p)	//결과출력함수
{
	int i, j;
	float avgWait = 0, avgRespond = 0, avgReturn = 0;
	for (i = 0; i < MAX_SIZE; i++) {	//간트차트
		printLine(totalTime);
		printf("%s | ", p[i].processID);
		for (j = 0; j < totalTime; j++) {
			if (p[i].gantt[j] == 1)
				printf("■");
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
	//대기시간
	printf("\n[각 프로세스 별 대기시간]\n");
	for (i = 0; i < MAX_SIZE; i++) {
		printf("%s : %d\t", p[i].processID, p[i].waitTime);
		avgWait += (float)p[i].waitTime;
	}
	printf("\n평균 대기 시간 : %f\n", avgWait / (float)MAX_SIZE);
	//응답시간
	printf("\n[각 프로세스 별 응답시간]\n");
	for (i = 0; i < MAX_SIZE; i++) {
		printf("%s : %d\t", p[i].processID, p[i].respondTime);
		avgRespond += (float)p[i].respondTime;
	}
	printf("\n평균 응답 시간 : %f\n", avgRespond / (float)MAX_SIZE);
	//반환시간
	printf("\n[각 프로세스 별 반환시간]\n");
	for (i = 0; i < MAX_SIZE; i++) {
		printf("%s : %d\t", p[i].processID, p[i].returnTime);
		avgReturn += (float)p[i].returnTime;
	}
	printf("\n평균 반환 시간 : %f\n\n", avgReturn / (float)MAX_SIZE);
}
void fcfs(process *p)
{
	int i, j, time = 0, count = 0;	//time : 시간 변수, count : 프로세스 인덱스
	queue waitQ;	//대기큐 생성
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
	for (i = 0; i < MAX_SIZE; i++) {	//프로세스 수만큼 반복
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
	for (i = 0; i < MAX_SIZE; i++)	//각 우선순위 초기화
		p[i].priority = -1;

	//한 프로세스 실행 끝날 때 마다 우선순위 업데이트
	for (i = 0; i < MAX_SIZE; i++) {	//현재시각 도착한 프로세스들 중 우선순위 높은 것
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
		p[temp].priority = -2;	//우선순위 초기화
	}
	printGantt(p);
}
void nopreemptivePriority(process *p)	//비선점 우선순위
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
void preemptivePriority(process *p)	//선점 우선순위
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
	printf("1.FCFS\n2.SJF\n3.비선점 Priority\n");
	printf("4.선점 Priority\n5.RR\n6.SRT\n7.HRN\n");
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
	//파일입력으로 프로세스 수 읽기
	fscanf(fp, "%d", &MAX_SIZE);
	//입력받은 프로세스 수만큼 process 구조체 배열 동적할당
	process *p = (process *)malloc(MAX_SIZE * sizeof(process));
	//파일입력으로 프로세스 데이터 입력
	for (int i = 0; i < MAX_SIZE; i++) {
		fscanf(fp, "%s %d %d %d", p[i].processID, &p[i].arriveTime, &p[i].execTime, &p[i].priority);
		totalTime += p[i].execTime;	//전체 실행시간에 해당 프로세스의 실행시간 추가
		p[i].firstTime = 3;	//첫 번째 반응시간 일괄 지정
	}
	//도착순서대로 프로세스 정렬
	//파일입력으로 타임슬라이스 입력
	fscanf(fp, "%d", &timeslice);
	for (int i = 0; i < MAX_SIZE; i++)	//동적 할당 & 초기화
		p[i].gantt = (int*)calloc(totalTime, sizeof(int));
	printMenu();
	printf("원하는 메뉴를 선택해주세요 : ");
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