#ifndef __BPlusTree_H__
#define __BPlusTree_H__

#define MAX_CHILD_NUMBER 5
#define DUPLICATE -1
#define NONEXIST -2
typedef struct WBPlusTreeNode {
	int isRoot, isLeaf;
	int key_num;
	int key[MAX_CHILD_NUMBER];
	void* child[MAX_CHILD_NUMBER + 1];
        long int bitmap;
	struct WBPlusTreeNode* father;
	struct WBPlusTreeNode* next;
	struct WBPlusTreeNode* last;
        unsigned char slot_array[MAX_CHILD_NUMBER+1];
        int unused[14];
} WBPlusTreeNode;

extern void WBPlusTree_Init();
extern void WBPlusTree_Destroy();
extern int WBPlusTree_Insert(int, void*);
extern int WBPlusTree_GetTotalNodes();
extern int WBPlusTree_GetSplitCount();
//extern void WBPlusTree_Delete(int);
//int split_count = 0;
#endif
