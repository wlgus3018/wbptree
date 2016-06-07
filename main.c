#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "wbptree.h"

#define true 1
#define false 0
#define MAX_KEY 50000000
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
int TotalRecords = 50000000;
int validRecords;
// test
unsigned int keys[50000000], key_num;

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
            //printf("key :%d\n",new_key);
            //sleep(10);
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
        int count =0;
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
        if(ret == -1)
        {
        //    printf("It have\n");
            count++; 
      //  printf("Count: %d\n",count);        
            continue;
        }
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
    int out = 1;
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

        

       
      /* 
        printf("---------------------------\n");
        printf("new_key : %d\n",new_key);
        printf("---------------------------\n");
      
        */
        // sleep(1);
        clock_gettime(CLOCK_REALTIME,&start);
        ret = WBPlusTree_Delete(new_key);
        clock_gettime(CLOCK_REALTIME,&end);
        diff_time = BILLION * (end.tv_sec - start.tv_sec)+(end.tv_nsec - start.tv_nsec);
        total += diff_time;
    
        
        out++;
        /*
        if(out == 19)
        {
            return ;
        }
*/

        if(ret == -2)
        {
           // printf("Don't have !\n");
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
	printf("  2 :Search\n");
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
                        //       WBPlusTree_Print();
                                total += diff_time;
                                printf("Valid Records inser/ted on WB+tree = %d\n", validRecords);
                                printf("Total number of WB+tree nodes = %d\n",WBPlusTree_GetTotalNodes());
                                printf("Total time %llu msecond(s) elapsed\n",total/1000000);

                                printf("Total split count : %d\n",WBPlusTree_GetSplitCount());
                                printf("Node size %lu byte\n",sizeof(struct WBPlusTreeNode));
                                //exit(0);
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
                              /* 
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
                                */
                               
                                long long unsigned int total = 0;

                                buffer2 = Read_Buffer(buffer2,input_file2);
                               total = Read_Data_And_Delete(buffer2);
                                
                              //  WBPlusTree_Print();
                                printf("Valid Records on WB+tree = %d\n", validRecords);
                                printf("Total time %llu msecond(s) elapsed\n",total/1000000);

                                printf("Node size %lu byte\n",sizeof(struct WBPlusTreeNode));
                                exit(0);
                                break;

                                
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
                        case 5: 
                                WBPlusTree_Print();
                                exit(0);
                                break;
                        case 6:{
                                    int num,j;
                                    int value;
                                    long long int total;
                                    int key[87];
                                    key[1]=34;key[2]=244;key[3]=9;key[4]=10;key[5]=168;
                                    key[6]=297;key[7]=280;key[8]=228;key[9]=133;
                                    key[10]=150;key[11]=247;key[12]=279;key[13]=119;key[14]=260;
                                    key[15]=115;key[16]=208;key[17]=300;key[18]=35;key[19]=184;
                                    key[20]=76;key[21]=144;key[22]=136;key[23]=244;key[24]=72;
                                    key[25]=176;key[26]=235;key[27]=112;key[28]=129;key[29]=15;
                                    key[30]=100;key[31]=256;key[32]=296;key[33]=226;key[34]=99;
                                    key[35]=199;key[36]=32;key[37]=114;key[38]=71;key[39]=281;
                                    key[40]=166;key[41]=9;key[42]=162;key[43]=53;key[44]=220;
                                    key[45]=131;key[46]=211;key[47]=73;key[48]=217;key[49]=84;
                                    key[50]=134;key[51]=224;key[52]=232;key[53]=121;key[54]=159;
                                    key[55]=25;key[56]=106;key[57]=203;key[58]=90;key[59]=170;
                                    key[60]=156;key[61]=149;key[62]=54;key[63]=124;key[64]=21;
                                    key[65]=231;key[66]=216;key[67]=118;key[68]=67;key[69]=87;
                                    key[70]=291;key[71]=122;key[72]=7;key[73]=285;key[74]=75;
                                    key[75]=222;key[76]=289;key[77]=230;key[78]=109;key[79]=69;
                                    key[80]=271;key[81]=29;key[82]=252;key[83]=270;key[84]=116;
                                    key[85]=22;key[86]=24;key[87]=174;

                             
                                    value = 4;
                                    total = 0;
                                    srand((unsigned)time(NULL));
                                    for(j=1; j<=18; j++)
                                    {
                                        //num = rand() % 1000000 + 1 ;
                                        //start_time = clock();
                                        clock_gettime(CLOCK_REALTIME,&start);
                                     //  WBPlusTree_Insert(j,s);
                                        printf("--------key[%d] : %d\n",j,key[j]);
                                        WBPlusTree_Delete(key[j]);
                                        clock_gettime(CLOCK_REALTIME,&end);
                                        diff_time = BILLION *(end.tv_sec - start.tv_sec) + end.tv_nsec- start.tv_nsec;

                                        total = diff_time + total;


                                        validRecords++;
                                    }
                                    WBPlusTree_Print();
                                    printf("Valid Records inserted on WB+tree = %d\n", validRecords);
                                    printf("Total number of WB+tree nodes = %d\n", WBPlusTree_GetTotalNodes());
                
                                printf("Total time %llu msecond(s) elapsed\n",total/1000000);

                                printf("Total split count : %d\n",WBPlusTree_GetSplitCount());
                                    printf("\n");
                                    exit(0);
                                    break;
                                }

                        default: break;
                }
        }
      // WBPlusTree_Destroy();
}


int main(int argc, char *argv[]) {
	// set input_file, output_file
	strcpy(input_file, "data/small-data.txt");
        strcpy(input_file2, "data/rand-data.txt");
	//if (argc == 2) strcpy(input_file, argv[1]);
	
	MainLoop();

	
	return 0;
}

