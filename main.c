#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "wbptree.h"

#define true 1
#define false 0
#define MAX_KEY 2000000000

// file
char input_file[100];
char output_file[100];
char *buffer;
int fsize;
// record
int new_key, new_pos, new_len;
char new_st[12];
// data
const int TotalRecords = 2000000000;
int validRecords;
// test
int keys[250000000], key_num;

/**
* Read_Buffer(char *input_file) -> buffer
*/
inline void Read_Buffer(char *input_file) {
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
	buffer = (char*)malloc(sizeof(char) * fsize);
	if (buffer == NULL) {
		fputs("Memory Error\n", stderr);
		exit(2);
	}
	// read to buffer[]
	fread(buffer, 1, fsize, fin);
	fclose(fin);
}

/** Read and insert records into B+tree */
inline void Read_Data_And_Insert() {
	int rid = 0;
        int ret;
	int cur = 0;
	while (1) {
		while (cur < fsize && !('0' <= buffer[cur] && buffer[cur] <= '9')) cur++; // end of file
		if (cur == fsize) break;

		rid++;
		if (rid % 100000 == 0) printf("now inserting the %d th record ..\n", rid);
		new_key = 0;
		new_pos = cur;
		while (buffer[cur] != ' ' && buffer[cur] != '\t') {
			new_key = new_key * 10 + (buffer[cur] - '0');
			cur++;
		}
		cur++;
		new_len = 0;
		while (buffer[cur] == ' ') cur++; // util meet a character
		while (cur < fsize && buffer[cur] != '\n') {
			new_st[new_len++] = buffer[cur++];
		}
		new_st[new_len] = '\0';
		char* value = (char*)malloc(sizeof(char) * new_len);
		strcpy(value, new_st);
		keys[key_num++] = new_key;
                ret = WBPlusTree_Find(new_key);

                /*Non Duplicate key*/
                if(ret != -1)
                    continue;
                
		if (WBPlusTree_Insert(new_key, value) == true) validRecords++; 
	}
	free(buffer);
	buffer = NULL;
}


/** Show Help */
void ShowHelp() {
        printf("--------------------------------------\n");
	printf("  Input your operation:\n");
	printf("  1 :Build Tree\n");
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
        double diff_time;
        int built = false;

	// Read data to buffer
	Read_Buffer(input_file);
	// B+tree initialize
	WBPlusTree_Init();
	while (1) {
		ShowHelp();
                int request;
                scanf("%d", &request);
                switch (request) {

                    case 1: {

                                // Build B+tree from file
                                if (built == true) {
                                    printf("You have built the B+tree\n");
                                    break;
                                }
                                built = true;
                                clock_gettime(CLOCK_REALTIME,&start);
                                Read_Data_And_Insert();
                                clock_gettime(CLOCK_REALTIME,&end);
                                diff_s = end.tv_sec - start.tv_sec;
                                //diff_ms = round(end.tv_nsec / 1.0e6) - round(start.tv_nsec / 1.0e6);
                                WBPlusTree_Print();
                                printf("Valid Records inserted on WB+tree = %d\n", validRecords);
                                printf("Total number of WB+tree nodes = %d\n",WBPlusTree_GetTotalNodes());
                                printf("Total time : %d.%03ld seconds\n",(int)diff_s,diff_ms);
                                
                                printf("Total split count : %d\n",WBPlusTree_GetSplitCount());
                                break;
                            }
                    case 2: {
                                /*
                                // Searching
                                printf("input the key: ");
                                int key;
                                scanf("%d", &key);
                                start_time = clock();
                                WBPlusTree_Find(key);
                                end_time = clock();
                                printf("Query on a key, costs %lf s\n", (end_time - start_time) / CLOCKS_PER_SEC);
                                */
                                int key;
                                int find;
                                int ret;
                                long int total;
                                find =0;
                                for(key=1; key<100001; key++)
                                {
                                clock_gettime(CLOCK_REALTIME,&start);
                                ret = WBPlusTree_Find(key);
                                clock_gettime(CLOCK_REALTIME,&end);

                                diff_time = (end.tv_sec - start.tv_sec) + ((end.tv_nsec- start.tv_nsec)/1000000000);
                                if(ret == -1)
                                {
                                    printf("Duplicate!\n");
                                    break;
                                }
                                find++;
                                }
                                printf("Valid Records find on WB+tree = %d\n", find);
                                printf("Total number of WB+tree nodes = %d\n", WBPlusTree_GetTotalNodes());
                                //printf("Total time %ld s\n",total/CLOCKS_PER_SEC);
                                printf("Total time %d second(s) elapsed\n",diff_time);

                                printf("\n");


                                break;
                            }
                            
                    case 3: {
                                // Delete value on a key
                                printf("input key: ");
                                int key;
                                scanf("%d", &key);

                                clock_gettime(CLOCK_REALTIME,&start);
                                WBPlusTree_Delete(key);

                                clock_gettime(CLOCK_REALTIME,&end);
                                diff_time = (end.tv_sec - start.tv_sec) + ((end.tv_nsec- start.tv_nsec)/1000000000);
                                WBPlusTree_Print();
                                break;
                            }
                        case 4: {
                                    printf("input (key, value): ");
                                    scanf("%d %s", &new_key, new_st);
                                    char* value = (char*)malloc(sizeof(char) * new_len);
                                    strcpy(value, new_st);
                                    int pos = WBPlusTree_Find(new_key);
                                    printf("Return : %d\n",pos);
                                    if (pos == -1) {
                                        keys[key_num++] = new_key;
                                        clock_gettime(CLOCK_REALTIME,&start);
                                        WBPlusTree_Insert(new_key, value);
                                        clock_gettime(CLOCK_REALTIME,&end);
                                        diff_time = (end.tv_sec - start.tv_sec) + ((end.tv_nsec- start.tv_nsec)/1000000000);
                                        validRecords++;
                                        printf("Insert success.\n");
                                    } else {
                                        printf("Insert failed, the key already exist.\n");
                                    }
                                    WBPlusTree_Print();
                                    printf("Valid Records inserted on B+tree = %d\n", validRecords);
                                    printf("Total number of B+tree nodes = %d\n", WBPlusTree_GetTotalNodes());
                                printf("Total time %d second(s) elapsed\n",diff_time);

                                    printf("\n");
                                    break;
                                }
                        case 5: return;
                        case 6:{
                                    int num,j;
                                    int value;
                                    long int total,temp;
                                    char s[] = "dkdka";
                                    value = 4;
                                    total = 0;
                                    temp = 0;
                                   // srand((unsigned)time(NULL));
                                    for(j=1; j<=3; j++)
                                    {
                                        //start_time = clock();
                                        clock_gettime(CLOCK_REALTIME,&start);
                                       WBPlusTree_Insert(j,s);
                                        clock_gettime(CLOCK_REALTIME,&end);
                                        diff_time = (end.tv_sec - start.tv_sec) + ((end.tv_nsec- start.tv_nsec)/1000000000);


                                        validRecords++;
                                    }
                                    WBPlusTree_Print();
                                    printf("Valid Records inserted on B+tree = %d\n", validRecords);
                                    printf("Total number of B+tree nodes = %d\n", WBPlusTree_GetTotalNodes());
                
                                printf("Total time %d second(s) elapsed\n",diff_time);

                                printf("Total split count : %d\n",WBPlusTree_GetSplitCount());
                                    printf("\n");
                                    break;
                                }

                        default: break;
                }
        }
       WBPlusTree_Destroy();
}

void build_test();
void delete_test();

int main(int argc, char *argv[]) {
	// set input_file, output_file
	strcpy(input_file, "data/small-data.txt");
	strcpy(output_file, "data/out.txt");
	if (argc == 2) strcpy(input_file, argv[1]);
	
	MainLoop();
	
	
	return 0;
}

