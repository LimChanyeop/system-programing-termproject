#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#define MAXLIST 30
#define READ_SIZE 512
#define MAXLENGTH 300
#define OPEN_FLAGS O_RDWR
#define SIZE 128

int copytxt(const char *command); 
void* randomth(void* arg);
void addmenu_k(); void chcnt_k(); 	//kor 메뉴추가및 메뉴갯수변경함수
void addmenu_j(); void chcnt_j(); 	//jpn 메뉴추가및 메뉴갯수변경함수
void addmenu_c(); void chcnt_c(); 	//chn 메뉴추가및 메뉴갯수변경함수
void addmenu_w(); void chcnt_w(); 	//west 메뉴추가및 메뉴갯수변경함수
char *itoa(int, char *, unsigned);	// 정수값을 아스키코드로 바꾸는 함수

int main()
{
    DIR* dp;
    int op = 0;
    char name[100] = " ";
    const char* user_name;
    int fd;
    char buff[15];
    ssize_t ss = 0;
    int ratio[4];
    int* retValue;
    int error;
    int type;
    pid_t childpid;
    pthread_t ranTid;
	
    char probability[10];	// 확률을 저장할 분자열
    int option2;		// 먹을지 안먹을지
    int pro;		// 확률을 저장할 정수값
    char* buf;		// write 에 쓰이는 변수
    int fd2;


    //사용자 아이디 입력
    printf("아이디를 입력하세요: ");
    scanf("%s", name);
    user_name = name;

    if ((dp = opendir(user_name)) == NULL)//opendir, 아이디가 없을 경우 디렉토리 생성
    {
        mkdir("new", 0776);//new라는 디렉토리 생성
        copytxt("cp *.txt new"); //new에 txt파일(현재 디렉토리 내) 전부 복사
        if (-1 == rename("new", user_name)); //디렉토리 이름을 사용자 아이디로 변경
        printf("사용자 정보를 등록하였습니다.\n");
    }

    closedir(dp);

    if(chdir(user_name) == -1)		// 작업 디렉토리 변경
	    perror("Failed to change current working directory to user_name");
    // !
    printf("메뉴 추천  => 1 입력 , 메뉴 편집 => 2 입력 : ");
    while(getchar() != '\n');
    scanf("%d", &op);

    // 메뉴 추천
    if (op == 1) {
        
        fd = open("./probability.txt", OPEN_FLAGS);
        read(fd, buff, 12);

        ratio[0] = atoi(strtok(buff, ","));
        ratio[1] = atoi(strtok(NULL, ","));
        ratio[2] = atoi(strtok(NULL, ","));
        ratio[3] = atoi(strtok(NULL, ",")); // transfer into ratio (int)

        	
        if (error = pthread_create(&ranTid, NULL, randomth, (void*)ratio)) {
            fprintf(stderr, "FAILED to create Thread : %s", strerror(error));
            return 1;
        }

        if (error = pthread_join(ranTid, (void**)&retValue)) {
            fprintf(stderr, "FAILED to join Thread : %s", strerror(error));
            return 1;
        }

	type = *retValue;
	
	close(fd);
        
    }
    // 메뉴 추가
    else if (op == 2)
    {
        char food_type;
        printf("메뉴를 추가할 분류를 고르시오: \n");
        printf("1. 한식: k/ 2. 일식: j/ 3. 중식: c/ 4. 양식: w --> ");
        scanf(" %c", &food_type);
        switch (food_type) {
        case 'k':
            addmenu_k();
            chcnt_k();
            break;
        case 'j':
            addmenu_j();
            chcnt_j();
            break;
        case 'c':
            addmenu_c();
            chcnt_c();
            break;
        case 'w':
            addmenu_w();
            chcnt_w();
            break;
        }
	return 0;
    }
    // 최종 선택 여부
 
    if((fd = open("./probability.txt", O_RDWR)) == -1)
    {
	    perror("Failded to open probability.txt");
	    return 1;
    }

    while (1)
    {
        printf("추천된 음식을 선택하시겠습니까?\n");
        printf("(1. 선택한다, 2. 재 추천) : ");
        scanf("%d", &option2);

	if (option2 == 1)
        {
            if (type == 1)
                lseek(fd, 0, SEEK_SET);		// 한식 확률 위치
            else if (type == 2)
                lseek(fd, 3, SEEK_SET);		// 일식 확률 위치
            else if (type == 3)
                lseek(fd, 6, SEEK_SET);		// 중식 확률 위치
            else
                lseek(fd, 9, SEEK_SET);		// 양식 확률 위치

            read(fd, probability, 2);
            pro = atoi(probability);
            pro++;

	    if (pro == 100)
                pro = 99;
            else if (pro == 9)
                pro = 10;

            buf = itoa(pro, probability, 10);
            lseek(fd, -2, SEEK_CUR);
            write(fd, buf, strlen(buf));

            printf("\n선택이 되었습니다.\n");
            break;
	}
        else if (option2 == 2)
        {
            if (type == 1)
                lseek(fd, 0, SEEK_SET);
            else if (type == 2)
                lseek(fd, 3, SEEK_SET);
            else if (type == 3)
                lseek(fd, 6, SEEK_SET);
            else
                lseek(fd, 9, SEEK_SET);

            read(fd, probability, 2);
            pro = atoi(probability);
            pro--;

            if (pro == 100)
                pro = 99;
            else if (pro == 9)
                pro = 10;

            buf = itoa(pro, probability, 10);
            lseek(fd, -2, SEEK_CUR);
            write(fd, buf, strlen(buf));

            printf("\n재 추천을 선택했습니다.\n");

	    fd2 = open("./probability.txt", OPEN_FLAGS);
            read(fd2, buff, 12);

	    ratio[0] = atoi(strtok(buff, ","));
	    ratio[1] = atoi(strtok(NULL, ","));
	    ratio[2] = atoi(strtok(NULL, ","));
	    ratio[3] = atoi(strtok(NULL, ",")); // transfer into ratio (int)

	    if (error = pthread_create(&ranTid, NULL, randomth, (void*)ratio)) {
	        fprintf(stderr, "FAILED to create Thread : %s", strerror(error));
	        return 1;
	    }

	    if (error = pthread_join(ranTid, (void**)&retValue)) {
	        fprintf(stderr, "FAILED to join Thread : %s", strerror(error));
	        return 1;
	    }

	    type = *retValue;

	    close(fd2);
        }
        else
            printf("\n잘못 선택하셨습니다. 다시 선택해주세요.\n\n");
    }

    printf("\n감사합니다. 다음에 또 이용해주세요.\n");
    close(fd);

    if(chdir("..") == -1)
        perror("Failed to change current working directory to ..");

    return 0;
}

void* randomth(void* arg) {
    int ratio_sum;
    int rd;
    int result;
    int fd;
    int i;
    int num;//리스트 메뉴 갯수
    ssize_t ss = 0;
    char* name;
    char longlist[MAXLENGTH];
    char* list[MAXLIST]; //30*15
    int len_longlist;
    int* argv;

    srand((unsigned)time(NULL));
    argv = (int*)arg;
    ratio_sum = argv[0] + argv[1] + argv[2] + argv[3];
    rd = (rand() % ratio_sum) + 1;

    if (rd < argv[0]) {
        result = 1;
    }
    else if (rd < argv[0] + argv[1]) {
        result = 2;
    }
    else if (rd < argv[0] + argv[1] + argv[2]) {
        result = 3;
    }
    else
        result = 4;


    if (result == 1)
        fd = open("./kor.txt", OPEN_FLAGS);
    if (result == 2)
        fd = open("./jpn.txt", OPEN_FLAGS);
    if (result == 3)
        fd = open("./chn.txt", OPEN_FLAGS);
    if (result == 4)
        fd = open("./west.txt", OPEN_FLAGS);

    ss = read(fd, longlist, 1000);


    list[0] = NULL;
    if (NULL == (list[0] = strtok(longlist, ",")))
        fprintf(stderr, "Tokenizer initiate ERROR");
    
    num = atoi(list[0]);

    for (i = 1; i < num + 1; i++) {
        if (NULL == (name = strtok(NULL, ",")))
            break;
        list[i] = (char*)malloc(sizeof(char) * (strlen(name) + 1));
        strcpy(list[i], name);

    }

    rd = (rd % num) + 1;


    printf("\n식사 메뉴 추천 결과 : %s\n", list[rd]);


    for (i = 1; i < num + 1; i++) {
        free(list[i]);
    }
    if (0 == close(fd));
    

    int* th_return = (int*)malloc(sizeof(int));
    *th_return = result;
    return th_return;
}


void addmenu_k() {
    int fd;
    char food_name[SIZE];
    printf("추가할 메뉴를 입력하시오: ");
    while(getchar() != '\n');
    fgets(food_name, SIZE, stdin);
    if ((fd = open("kor.txt", O_WRONLY, 0644)) == -1) {
        printf("OPEN ERROR!\n");
    }
    lseek(fd, -1, SEEK_END);
    if (write(fd, food_name, strlen(food_name)) == -1) {
        printf("WRITE ERROR!\n");
    }
    lseek(fd, -1, SEEK_END);
    write(fd, ",", 1);
    close(fd);
}

void addmenu_j()
{
    int fd;
    char food_name[SIZE];
    printf("추가할 메뉴를 입력하시오: ");
    while(getchar() != '\n');
    fgets(food_name, SIZE, stdin);
    if ((fd = open("jpn.txt", O_WRONLY, 0644)) == -1) {
        printf("OPEN ERROR!\n");
    }
    lseek(fd, -1, SEEK_END);
    if (write(fd, food_name, strlen(food_name)) == -1) {
        printf("WRITE ERROR!\n");
    }
    lseek(fd, -1, SEEK_END);
    write(fd, ",", 1);
    close(fd);
}

void addmenu_c()
{
    int fd;
    char food_name[SIZE];
    printf("추가할 메뉴를 입력하시오: ");
    while(getchar() != '\n');
    fgets(food_name, sizeof(food_name), stdin);
    if ((fd = open("chn.txt", O_WRONLY, 0644)) == -1) {
        printf("OPEN ERROR!\n");
    }
    lseek(fd, -1, SEEK_END);
    if (write(fd, food_name, strlen(food_name)) == -1) {
        printf("WRITE ERROR!\n");
    }
    lseek(fd, -1, SEEK_END);
    write(fd, ",", 1);
    close(fd);
}

void addmenu_w()
{
    int fd;
    char food_name[SIZE];
    printf("추가할 메뉴를 입력하시오: ");
    while(getchar() != '\n');
    fgets(food_name, SIZE, stdin);
    if ((fd = open("west.txt", O_WRONLY, 0644)) == -1) {
        printf("OPEN ERROR!\n");
    }
    lseek(fd, -1, SEEK_END);
    if (write(fd, food_name, strlen(food_name)) == -1) {
        printf("WRITE ERROR!\n");
    }
    lseek(fd, -1, SEEK_END);
    write(fd, ",", 1);
    close(fd);
}

void chcnt_k()
{
    FILE* fp = fopen("kor.txt", "r+");
    int cnt;
    fscanf(fp, "%d", &cnt);
    fseek(fp, 0, SEEK_SET);
    fprintf(fp, "%d", ++cnt);
    fclose(fp);
}

void chcnt_j()
{
    FILE* fp = fopen("jpn.txt", "r+");
    int cnt;
    fscanf(fp, "%d", &cnt);
    fseek(fp, 0, SEEK_SET);
    fprintf(fp, "%d", ++cnt);
    fclose(fp);
}

void chcnt_c()
{
    FILE* fp = fopen("chn.txt", "r+");
    int cnt;
    fscanf(fp, "%d", &cnt);
    fseek(fp, 0, SEEK_SET);
    fprintf(fp, "%d", ++cnt);
    fclose(fp);
}

void chcnt_w()
{
    FILE* fp = fopen("west.txt", "r+");
    int cnt;
    fscanf(fp, "%d", &cnt);
    fseek(fp, 0, SEEK_SET);
    fprintf(fp, "%d", ++cnt);
    fclose(fp);
}

char* itoa(int val, char* buf, unsigned radix)
{
    char* p;
    char* firstdig;
    char temp;
    unsigned digval;

    p = buf;

    if (radix == 10 && val < 0)
    {
        *p++ = '-';
        val = (unsigned int)(-(int)val);
    }

    firstdig = p;

    do
    {
        digval = (unsigned)(val % radix);
        val /= radix;

        if (digval > 9)
            *p++ = (char)(digval - 10 + 'a');
        else
            *p++ = (char)(digval + '0');
    } while (val > 0);

    *p-- = '\0';

    do
    {
        temp = *p;
        *p = *firstdig;
        *firstdig = temp;
        --p;
        ++firstdig;
    } while (firstdig < p);

    return buf;
}

int copytxt(const char* command)
{
    pid_t child_pid;
    int status;

    child_pid = fork();
    if (child_pid == 0)
    {
        if (execl("/bin/sh", "sh", "-c", command, (char*)NULL) == -1)
        {
            _exit(127);
        }
    }
    else if (child_pid > 0)
    {
        waitpid(child_pid, &status, 0);
        return  WEXITSTATUS(status);
    }
    else
    {
        return -1;
    }
}
