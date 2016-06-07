#ifndef __BPlusTree_H__
#define __BPlusTree_H__

#define MAX_CHILD_NUMBER 29
#define DUPLICATE -1
#define NONEXIST -2
typedef struct WBPlusTreeNode {
        unsigned long int bitmap;
        unsigned char slot_array[MAX_CHILD_NUMBER+1];
	unsigned int isRoot, isLeaf;
	unsigned int key_num;
	unsigned int key[MAX_CHILD_NUMBER];
	void* child[MAX_CHILD_NUMBER + 1];
	struct WBPlusTreeNode* father;
	struct WBPlusTreeNode* next;
	struct WBPlusTreeNode* last;
        int unused[20];
} WBPlusTreeNode;

extern void WBPlusTree_Init();
//extern void WBPlusTree_Destroy();
extern int WBPlusTree_Insert(int, void*);
extern int WBPlusTree_GetTotalNodes();
extern int WBPlusTree_GetSplitCount();
extern int WBPlusTree_Delete(int);
//int split_count = 0;
#endif
