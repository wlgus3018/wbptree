#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "wbptree.h"

#define true 1
#define false 0
#define MAX_KEY 2500000000
#define BILLION 1000000000L

// file
char input_file[100];
char input_file2[100];
char *buffer,*buffer2,*buffer3;
int fsize;
// record
int new_key, new_pos, new_len;
char new_st[12];
// data
const int TotalRecords = 2500000000;
int validRecords;
// test
int keys[250000000], key_num;

/**
* Read_Buffer(char *input_file) -> buffer
*/
inline char* Read_Buffer(char *buff, char *input_file) {
	FILE* fin = fopen(input_file, "rb");
	if (fin == NULL) {
		fputs("File Error\n", stderr);
		exit(1);
	}
	// obtain file size
	fseek(fin, 0, SEEK_END);
	fsize = ftell(fin);
	rewind(fin);
	// allocate memory
	buff = (char*)malloc(sizeof(char) * fsize);
	if (buff == NULL) {
		fputs("Memory Error\n", stderr);
		exit(2);
	}
	// read to buffer[]
	fread(buff, 1, fsize, fin);
	fclose(fin);
        return buff;
}

/** Read and insert records into B+tree */
inline void Read_Data_And_Insert(char *buff) {
    int rid = 0;
    int ret;
    int cur = 0;
    while (1) {
        while (cur < fsize && !('0' <= buff[cur] && buff[cur] <= '9')) cur++; // end of file
        if (cur == fsize) break;

        rid++;

        new_key = 0;
        new_pos = cur;
        while (buff[cur] != ' ' && buff[cur] != '\t') {
            new_key = new_key * 10 + (buff[cur] - '0');
            cur++;
        }
        cur++;
        new_len = 0;
        while (buff[cur] == ' ') cur++; // util meet a character
        while (cur < fsize && buff[cur] != '\n') {
            new_st[new_len++] = buff[cur++];
        }
        new_st[new_len] = '\0';
        char* value = (char*)malloc(sizeof(char) * new_len);
        strcpy(value, new_st);
        ret = WBPlusTree_Find(new_key);

        /*Non Duplicate key*/
        if(ret == DUPLICATE)
        {
          //  sleep(10);
            continue;
        }
        if (WBPlusTree_Insert(new_key, value) == true) {
            validRecords++;
           // if (rid % 100000 == 0) printf("now inserting the %d th record ..\n", rid);

        }
        keys[key_num++] = new_key;

    }
    free(buff);
    buff = NULL;
}
inline long long unsigned int Read_Data_And_Search(char *buff) {
    int rid = 0;
    int ret;
    int cur = 0;
    struct timespec start,end;
    long long unsigned int diff_time,total = 0;
    while (1) {
        while (cur < fsize && !('0' <= buff[cur] && buff[cur] <= '9')) cur++; // end of file
        if (cur == fsize) break;

        rid++;

        new_key = 0;
        new_pos = cur;
        while (buff[cur] != ' ' && buff[cur] != '\t') {
            new_key = new_key * 10 + (buff[cur] - '0');
            cur++;
        }
        cur++;
        new_len = 0;
        while (buff[cur] == ' ') cur++; // util meet a character
        while (cur < fsize && buff[cur] != '\n') {
            new_st[new_len++] = buff[cur++];
        }

        clock_gettime(CLOCK_REALTIME,&start);
        ret = WBPlusTree_Find(new_key);
        clock_gettime(CLOCK_REALTIME,&end);

        diff_time = BILLION * (end.tv_sec - start.tv_sec)+(end.tv_nsec - start.tv_nsec);
        total += diff_time;
        if(ret == DUPLICATE)
            continue;

        
    }
    free(buff);
    buff = NULL;
    return total;
}

inline long long unsigned int Read_Data_And_Delete(char *buff) {
    int rid = 0;
    int ret;
    int cur = 0;
    int count = 0;
    struct timespec start,end;
    long long unsigned int diff_time,total = 0;
    while (1) {
        while (cur < fsize && !('0' <= buff[cur] && buff[cur] <= '9')) cur++; // end of file
        if (cur == fsize) break;

        rid++;

        new_key = 0;
        new_pos = cur;
        while (buff[cur] != ' ' && buff[cur] != '\t') {
            new_key = new_key * 10 + (buff[cur] - '0');
            cur++;
        }
        cur++;
        new_len = 0;
        while (buff[cur] == ' ') cur++; // util meet a character
        while (cur < fsize && buff[cur] != '\n') {
            new_st[new_len++] = buff[cur++];
        }

        

        clock_gettime(CLOCK_REALTIME,&start);
        ret = WBPlusTree_Delete(new_key);
        clock_gettime(CLOCK_REALTIME,&end);
        diff_time = BILLION * (end.tv_sec - start.tv_sec)+(end.tv_nsec - start.tv_nsec);
        total += diff_time;
        //validRecords--;
        
        
        if(ret == -2)
        {
            //printf("Don't have !\n");
            count++;
            continue;
        }
    }
    free(buff);
    buff = NULL;
    printf("Non valid record : %d\n",count);
    return total;
}





/** Show Help */
void ShowHelp() {
        printf("--------------------------------------\n");
	printf("  Input your operation:\n");
	printf("  0 :Build Tree\n");
	printf("  1 :Experiment\n");
	printf("  2 :Query on a key(future work)\n");
	printf("  3 :Delete value on a key\n");
	printf("  4 :Insert new record\n");
	printf("  5 :Quit\n");
    printf("  6 :Debug\n");
        printf("--------------------------------------\n");
}

void MainLoop() {
	struct timespec start,end;
        long diff_ms;
        time_t diff_s;
        long long unsigned int diff_time;
        int built1,built2 = false;

	// Read data to buffer
	// B+tree initialize
	WBPlusTree_Init();
	while (1) {
		ShowHelp();
                int request;
                scanf("%d", &request);
                switch (request) {


                    case 0: {
                                long long unsigned int total = 0;

	                    buffer = Read_Buffer(buffer,input_file);
                                // Build B+tree from file
                                if (built1 == true) {
                                    printf("You have built the WB+tree\n");
                                    break;
                                }
                                built1 = true;
                                clock_gettime(CLOCK_REALTIME,&start);
                                Read_Data_And_Insert(buffer);
                                clock_gettime(CLOCK_REALTIME,&end);
                                diff_time = BILLION * (end.tv_sec - start.tv_sec)+(end.tv_nsec - start.tv_nsec);
                                //WBPlusTree_Print();
                                total += diff_time;
                                printf("Valid Records inser/ted on WB+tree = %d\n", validRecords);
                                printf("Total number of WB+tree nodes = %d\n",WBPlusTree_GetTotalNodes());
                                printf("Total time %llu msecond(s) elapsed\n",total/1000000);
                                
                                printf("Total split count : %d\n",WBPlusTree_GetSplitCount());
                                break;
                            }
                    case 1: {
                                long long unsigned int total = 0;

                                buffer2 = Read_Buffer(buffer2,input_file2);
                                if (built2 == true) {
                                    printf("You have epxeriment\n");
                                    break;
                                }
                                built2 = true;
                                clock_gettime(CLOCK_REALTIME,&start);
                                Read_Data_And_Insert(buffer2);
                                clock_gettime(CLOCK_REALTIME,&end);
                                diff_time = BILLION * (end.tv_sec - start.tv_sec)+(end.tv_nsec - start.tv_nsec);
                                WBPlusTree_Print();
                                total += diff_time;
                                printf("Valid Records inser/ted on WB+tree = %d\n", validRecords);
                                printf("Total number of WB+tree nodes = %d\n",WBPlusTree_GetTotalNodes());
                                printf("Total time %llu msecond(s) elapsed\n",total/1000000);

                                printf("Total split count : %d\n",WBPlusTree_GetSplitCount());
                                printf("Node size %lu byte\n",sizeof(struct WBPlusTreeNode));
                                break;
                            }
                    case 2: {
                                long long unsigned int total = 0;
                                //buffer3 = Read_buffer(buffer3,input_file2);
                                
                                buffer2 = Read_Buffer(buffer2,input_file2);
                                total =  Read_Data_And_Search(buffer2);

                                printf("Total number of WB+tree nodes = %d\n", WBPlusTree_GetTotalNodes());
                                //printf("Total time %ld s\n",total/CLOCKS_PER_SEC);
                                printf("Total time %llu msecond(s) elapsed\n",total/1000000);

                                printf("\n");


                                break;
                            }

                    case 3: {
                                printf("input key: ");
                                int key;
                                int result;
                                scanf("%d", &key);

                                clock_gettime(CLOCK_REALTIME,&start);
                                result = WBPlusTree_Delete(key);

                                clock_gettime(CLOCK_REALTIME,&end);
                                if(result == NONEXIST){
                                    printf("|key : %d |->Doesn't exist in WB+tree! \n",key);
                                }
                                diff_time = (end.tv_sec - start.tv_sec) + ((end.tv_nsec- start.tv_nsec)/1000000000);
                                WBPlusTree_Print();
                                printf("result is %d\n",result);
                               
                                /*
                                long long unsigned int total = 0;

                                buffer2 = Read_Buffer(buffer2,input_file2);
                               total = Read_Data_And_Delete(buffer2);
                                
                                printf("Valid Records on WB+tree = %d\n", validRecords);
                                printf("Total time %llu msecond(s) elapsed\n",total/1000000);

                                printf("Node size %lu byte\n",sizeof(struct WBPlusTreeNode));
                                break;
                                */

                                break;
                            }
                        case 4: {
                                    printf("input (key, value): ");
                                    scanf("%d %s", &new_key, new_st);
                                    char* value = (char*)malloc(sizeof(char) * new_len);
                                    strcpy(value, new_st);
                                    int result = WBPlusTree_Find(new_key);
                                    if (result != DUPLICATE) {
                                        keys[key_num++] = new_key;
                                        clock_gettime(CLOCK_REALTIME,&start);
                                        WBPlusTree_Insert(new_key, value);
                                        clock_gettime(CLOCK_REALTIME,&end);
                                        diff_time = BILLION * (end.tv_sec - start.tv_sec)+(end.tv_nsec - start.tv_nsec);
                                        validRecords++;
                                        printf("Insert success.\n");
                                    } else {
                                        printf("Insert failed, the key already exist.\n");
                                    }
                                    WBPlusTree_Print();
                                    printf("Valid Records inserted on WB+tree = %d\n", validRecords);
                                    printf("Total number of WB+tree nodes = %d\n", WBPlusTree_GetTotalNodes());
                                printf("Total time %llu nanosecond(s) elapsed\n",(long long unsigned int)diff_time);

                                    printf("\n");
                                    break;
                                }
                        case 5: return;
                        case 6:{
                                    int num,j;
                                    int value;
                                    long long int total;
                                    char s[] = "dkdka";
                                    value = 4;
                                    total = 0;
                                    srand((unsigned)time(NULL));
                                    for(j=1; j<=500000; j++)
                                    {
                                        //num = rand() % 1000000 + 1 ;
                                        //start_time = clock();
                                        clock_gettime(CLOCK_REALTIME,&start);
                                       WBPlusTree_Insert(j,s);
                                        clock_gettime(CLOCK_REALTIME,&end);
                                        diff_time = BILLION *(end.tv_sec - start.tv_sec) + end.tv_nsec- start.tv_nsec;

                                        total = diff_time + total;


                                        validRecords++;
                                    }
                                   // WBPlusTree_Print();
                                    printf("Valid Records inserted on WB+tree = %d\n", validRecords);
                                    printf("Total number of WB+tree nodes = %d\n", WBPlusTree_GetTotalNodes());
                
                                printf("Total time %llu msecond(s) elapsed\n",total/1000000);

                                printf("Total split count : %d\n",WBPlusTree_GetSplitCount());
                                    printf("\n");
                                    break;
                                }

                        default: break;
                }
        }
       WBPlusTree_Destroy();
}


int main(int argc, char *argv[]) {
	// set input_file, output_file
	strcpy(input_file, "data/small-data.txt");
        strcpy(input_file2, "data/rand-data.txt");
	//if (argc == 2) strcpy(input_file, argv[1]);
	
	MainLoop();

	
	return 0;


