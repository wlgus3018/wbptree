#include "wbptree.h"
#include <stdio.h>
#include <stdlib.h>

#define true 1
#define false 0
struct WBPlusTreeNode* Root;

int MaxChildNumber = 5;
int TotalNodes;
int split_count = 0;
void clflush(volatile void *p)
{
    asm volatile("clflush (%0)" :: "r"(p));
}
void mfence(void)
{
    asm volatile("mfence" ::: "memory");
}

WBPlusTreeNode* New_WBPlusTreeNode() {
    int i;	
    struct WBPlusTreeNode* p = (struct WBPlusTreeNode*)malloc(sizeof(struct WBPlusTreeNode));
	p->isRoot = false;
	p->isLeaf = false;
	p->key_num = 0;
	//p->child[0] = NULL;
	p->father = NULL;
    	p->next = NULL;
	p->last = NULL;
        p->bitmap = 0;
	TotalNodes++;
        //p->key_pos = 0;
        for(i=0; i<MAX_CHILD_NUMBER; i++)
        {
            p->slot_array[i] = 0;
            p->key[i] = 0;
            p->child[i] = NULL;
        }
        p->slot_array[i] = 0;
        p->child[i] = NULL;
        return p;
}
inline int slot_binary_search(WBPlusTreeNode* Cur, int key)
{
    int l = 1, r = Cur->key_num;
   // printf("in slot binarry key : %d\n",Cur->key[Cur->slot_array[l]]);
    //printf("in slot binary : %d\n",Cur->slot_array[l]);
    //printf("Cur->key_num : %d\n",Cur->key_num);
    //printf("in slot key : %d\n",key);
    if (key < Cur->key[Cur->slot_array[l]]) return l;
    if (Cur->key[Cur->slot_array[r]] <= key) return r+1;
    while (l < r-1) {
        int mid = (l + r) >> 1;
        if (Cur->key[Cur->slot_array[mid]] > key)
        {
            r = mid;
        }
        else{
            l = mid;
        }
    }  
    return l + 1;

}

void Insert(WBPlusTreeNode*, int,void*);
void Split(WBPlusTreeNode* Cur) {
    // copy Cur(Mid .. MaxChildNumber) -> Temp(0 .. Temp->key_num)
    WBPlusTreeNode* Temp = New_WBPlusTreeNode();
    int Mid = MaxChildNumber >> 1;
    //printf("Mid : %d\n",Mid);
    Temp->isLeaf = Cur->isLeaf;
    Temp->key_num = MaxChildNumber - Mid;
    //printf("Temp->key_num : %d\n",Temp->key_num);
    int i;
    split_count++;

    //printf("split Cur->bitmap : %d\n",Cur->bitmap);
    Temp->bitmap = 0;
    Temp->bitmap = Temp->bitmap + 1;
  
   /*Non leaf Split*/ 
    if(Cur->isLeaf == false)
    {
     WBPlusTreeNode* child = New_WBPlusTreeNode();
        //printf("Is leaf == false so Non_leaf spilt\n");
          //  TotalNodes--;
        Mid = Mid + 1; 

        for(i = 0; i < Mid-1; i++)
        {
            Temp->key[i] = Cur->key[Cur->slot_array[i+Mid+1]];

          //  printf("Cur->key[Cur->slot_array[i+Mid]] : %d\n",Cur->key[Cur->slot_array[i+Mid]]);
            
       //     printf("Cur->slot_array[i+Mid]] :%d\n",Cur->slot_array[i+Mid]);
            Temp->child[i] = Cur->child[Cur->slot_array[i+Mid]+1];
            Temp->slot_array[i+1] = i;
          
            Temp->bitmap = Temp->bitmap + (1<< i+1);

            Cur->bitmap = Cur->bitmap & (~(1 << Cur->slot_array[Mid+i]+1));
           child = Temp->child[i];

            child->father = Temp;

            //printf("what is Cur->slot_array[%d] : %d\n",Mid+i,Cur->slot_array[Mid+i]);
            //printf("what is Cur->bitmap : %d\n",Cur->bitmap);
            //printf("what is Cur->slot_array[%d] : %d\n",Mid+i,Cur->slot_array[Mid+i]);
            //printf("what is Temp->bitmap : %d\n",Temp->bitmap);
            //printf("what is : %d\n",Cur->key[15]);
        }

        Cur->bitmap = Cur->bitmap & (~(1 << Cur->slot_array[Mid+i]+1));
        Temp->child[i] = Cur->child[Cur->slot_array[i+Mid]+1];
        Cur->key_num = Mid-1;
        Cur->slot_array[0] = Mid-1;
        Temp->slot_array[0] = Mid;
        Temp->key_num = Mid-1;

        /*Moving child pointer to temp*/
        child = Temp->child[i];
        child->father = Temp;
    }
    else /*Leaf Split(*/
    {
        Mid = Mid + 1;
        for(i = 0; i < Mid; i++)
        {
            Temp->key[i] = Cur->key[Cur->slot_array[i+Mid]];

            //printf("Cur->key[Cur->slot_array[i+Mid]] : %d\n",Cur->key[Cur->slot_array[i+Mid]]);
            Temp->child[i] = Cur->child[Cur->slot_array[i+Mid]];
            Temp->slot_array[i+1] = i;
/*
            if(Temp->isLeaf)
            {
                Temp->pos[i] = Cur->pos[Cur->slot_array[i+Mid]];
            }
*/
            Temp->bitmap = Temp->bitmap + (1<< i+1);

            Cur->bitmap = Cur->bitmap & (~(1 << Cur->slot_array[Mid+i]+1));

            //printf("what is Cur->slot_array[%d] : %d\n",Mid+i,Cur->slot_array[Mid+i]);
            //printf("what is Cur->bitmap : %d\n",Cur->bitmap);
            //printf("what is Cur->slot_array[%d] : %d\n",Mid+i,Cur->slot_array[Mid+i]);
            //printf("what is Temp->bitmap : %d\n",Temp->bitmap);
            //printf("what is : %d\n",Cur->key[15]);
        }
    
        Cur->key_num = Mid-1;
        Cur->slot_array[0] = Mid-1;
        Temp->slot_array[0] = Mid;
        Temp->key_num = Mid;
    }
    if (Cur->isRoot)
    {
       // printf("in here ---\n");
        // Create a new Root, the depth of Tree is increased
        Root = New_WBPlusTreeNode();
        Root->key_num = 1;
        Root->isRoot = true;
        Root->key[0] = Cur->key[Cur->slot_array[Mid]];
       // printf("---\n");
       // printf("Cur->key[Cur->slot_array[Mid] : %d\n", Cur->key[Cur->slot_array[Mid]]);
        //printf("---\n");
        Root->child[0] = Cur;
        //Root->key[1] = Temp->key[Cur->slot_array[1]];
        Root->child[1] = Temp;
        /* Bitmap Checking*/
        Root->bitmap = Root->bitmap + 1;
        Root->bitmap = Root->bitmap + (Root->bitmap << 1);
        //Root->bitmap = Root->bitmap + (1 << 2);

        //printf("Root->bitmap : %d\n",Root->bitmap);
        Root->isLeaf = false;
        Root->slot_array[0] = Root->key_num;
        Root->slot_array[1] = 0;
        //Root->slot_array[2] = 1;
        //Root->slot_array[2] = 1;
        Cur->father = Temp->father = Root;
        Cur->isRoot = false;

        Cur->next = Temp;
        Temp->last = Cur;


    }
    else
    {

          //  printf("Cur->father : %p\n",Cur->father);
                //printf("Current->father : %d\n",Cur->father->bitmap);
        //printf("wow Cur->slot_array[Mid] : %d\n",Cur->key[Cur->slot_array[Mid]]);
        // Try to insert Temp to Cur->father
        Temp->father = Cur->father;
        Insert(Cur->father,Cur->key[Cur->slot_array[Mid]],(void*)Temp);
        //printf("inserting : %d\n",Cur->key[Cur->slot_array[Mid]]);
    }
   //printf("hi\n");
   
}

void Insert(WBPlusTreeNode* Cur, int key, void* value){
    int i, ins,u;
    int bit = 1;
    int t;
    int temp;
    

    if(Cur->bitmap & 1 == 0)
    {
        sleep(10);
    }
    ins = slot_binary_search(Cur,key);
    //printf("ins : %d\n",ins);

    
    if(Cur->bitmap & 1 > 0)
    {
        Cur->bitmap = Cur->bitmap - 1;
    }
    clflush(&Cur->bitmap); mfence();


    bit = bit << 1;

   // printf("Cur->bitmap : %d\n",Cur->bitmap);
   // printf("bit :%d\n",bit);
    
    u = 0;
    while(1)
    {
        if(u == MAX_CHILD_NUMBER)
        {
            break;
        }
        temp = Cur->bitmap & bit;
       // printf("temp : %d\n",temp);

     //   printf("In-Cur->bitmap : %d\n",Cur->bitmap);
        if(temp == 0)
        {
            break;
        }
        else
        {
            bit = bit << 1;
            u++;
        }
    }

  //  printf("what is bit %d\n",bit);
//    printf("what is u %d\n",u);
  
    Cur->key_num = Cur->key_num + 1;
    Cur->key[u] = key;

   // Cur->pos[u] = pos;

    if(Cur->isLeaf == false)
    {
        Cur->child[u+1] = value;
        clflush(&Cur->child[u+1]);
    }
    else
    {
    Cur->child[u] = value;
    }


    Cur->slot_array[0] = Cur->key_num; 
    clflush(&Cur->key_num);
    clflush(&Cur->key[u]);
    clflush(&Cur->child[u]);

   // clflush(&Cur->pos[u]);
    clflush(&Cur->slot_array[0]); 


    
    for(i = Cur->key_num; i>=ins; i--)
    {
        Cur->slot_array[i+1] = Cur->slot_array[i];
    }


    Cur->slot_array[ins] = u;
    //printf("what is ins : %d\n",ins);
    //printf("what is slot_array[ins] : %d\n",Cur->slot_array[ins]);
    //printf("Cur->key_num: %d\n",Cur->key_num);
    //printf("Cur->key[%d] : %d\n",u,Cur->key[Cur->slot_array[ins]]);

    for(i = ins-1; i>=1; i--)
    {
        Cur->slot_array[i] = Cur->slot_array[i];
    }
   
    for(i=0; i< Cur->key_num; i+=8 )
    {
        clflush(&Cur->slot_array[i]);
    }
    mfence();
    //printf("what is Cur->bitmap: %d\n",Cur->bitmap);
    Cur->bitmap= Cur->bitmap + 1 +  (1 << u+1);
    //printf("what is Cur->bitmap(modifided): %d\n",Cur->bitmap);
    clflush(&Cur->bitmap); mfence();
    //printf("key[%d] : %d\n",0,Cur->key[0]);
    //printf("key[%d] : %d\n",1,Cur->key[1]);
    //printf("slot_array[%d] : %d\n",0,Cur->slot_array[0]);
    //printf("slot_array[%d] : %d\n",1,Cur->slot_array[1]);
    //printf("slot_array[%d] : %d\n",2,Cur->slot_array[2]);


    /*Adjusting child pointer*/
    if(Cur->isLeaf == false)
    {
       // sleep(10);
        WBPlusTreeNode* comp;
        WBPlusTreeNode* temp;
        WBPlusTreeNode* prev;
        {
            if(ins == 1)
            {
              //  printf("hi\n");
                //sleep(10);
                temp = value;
                prev = Cur->child[0];
                temp->next = prev->next;
                prev->next = temp;

                //sleep(10);
            }
            else if(ins == Cur->key_num)
            {
            //    printf("hi2\n");
             //   printf("what is ins %d\n",ins);
               
               // printf("%d\n",Cur->key[Cur->slot_array[ins]]);
               

                temp = value;
                prev = Cur->child[Cur->slot_array[ins-1]+1];

                prev->next = value;
                temp->next = NULL;
            }
            else
            {
               // printf("hihi\n");
                //printf("ins : %d\n",ins);
                temp = value;
                prev = Cur->child[Cur->slot_array[ins-1]+1];
                
               // printf("%d\n",prev->key[prev->slot_array[1]]);
                temp->next = prev->next;
                prev->next = temp;
                
            }
        }
    }





    /*split*/
    if (Cur->key_num == MaxChildNumber)
    {// children is full
        //printf("split!\n");
        //printf("Cur->key_num : %d\n",Cur->key_num);
        Split(Cur);
    }
}

void Delete(WBPlusTreeNode*, int);
void Delete(WBPlusTreeNode *Cur, int key){
   
    int ins;
    int bit_pos = 0;
    int i;

    WBPlusTreeNode *temp = New_WBPlusTreeNode();

    ins = slot_binary_search(Cur,key);
    ins--;
    bit_pos = 1 << (Cur->slot_array[ins]+1);
    //Findiing bit position
    Cur->bitmap = Cur->bitmap - bit_pos;
    Cur->key[Cur->slot_array[ins]] = 0;
    clflush(&Cur->bitmap); mfence();
    clflush(&Cur->key[Cur->slot_array[ins]]);
    printf("ins :%d\n",ins);
    printf("slot_array[ins] : %d\n",Cur->slot_array[ins]);
    printf("key_num:%d\n",Cur->key_num);

    for(i=ins+1; i<=Cur->key_num; i++)
    {

        Cur->slot_array[i-1] = Cur->slot_array[i];
        Cur->slot_array[i] = 0;
    }
    Cur->key_num = Cur->key_num - 1;
    Cur->slot_array[0] = Cur->key_num;
    clflush(&Cur->key_num);
   

    //printf("Cur->father : %d\n",temp->key[temp->slot_array[1]]);

/*

    if(Cur->isLeaf)
    {
        int index;
        temp = Cur->father;
        index = slot_binary_search(temp,key);
        index--;
        if(temp->key[temp->slot_array[index]] == key){
            bit_pos = 1 << (temp->slot_array[index]+1);
            temp->bitmap = temp->bitmap - bit_pos;
            //temp->key[Cur->slot_array[ins]] = 0;
            //clflush(&Cur->bitmap); mfence();
            //clflush(&Cur->key[Cur->slot_array[ins]]);
            printf("ins :%d\n",ins);
            printf("slot_array[ins] : %d\n",temp->slot_array[ins]);
            printf("key_num:%d\n",temp->key_num);
            printf("temp->bitmap : %d\n",temp->bitmap);

            //temp->child[temp->slot_array[index]+1] = NULL;
               for(i=index+1; i<=temp->key_num; i++)
               {

               temp->slot_array[i-1] = temp->slot_array[i];
               temp->slot_array[i] = 0;
            //   temp->key[slot_array[index]] = temp->key[slot_array[index]+1];
            // temp->key[i] = 0;
            }*/
            //temp->key_num = temp->key_num - 1;
            //temp->slot_array[0] = temp->key_num;
            //printf("Cur :%d\n",Cur->key[Cur->slot_array[1]]);
            /*
               for(i=0; i<5; i++)
               {
               printf("%p \n",temp->child[i]);
               }
            Insert(temp,Cur->key[Cur->slot_array[1]],Cur);
*/
        
    
}



WBPlusTreeNode* Find(int key) {
    WBPlusTreeNode* Cur = Root;
    //printf("In Find what is Root->key[0] :%d\n",Cur->key[Cur->slot_array[1]]);
    //printf("In find what is key : %d\n",key);
    while (1) {
        if (Cur->isLeaf == true)
        {
      //      printf("Cur address in leaf : %p\n",Cur);
        //    printf("first hi\n");
            break;
        }
        if(key < Cur->key[Cur->slot_array[1]])
        {
          //  printf("first\n");
            Cur = Cur->child[0];
        }
        else
        {
           // printf("hi\n");
            //printf("thrid in key :%d\n",key);
            int i = slot_binary_search(Cur, key);



        
      //      printf("beafore -- Cur->slot_array[i] : %d\n",Cur->slot_array[i]);
        //    printf("what is i : %d\n",i);
           i--;
          //  printf("------------------\n");
           // printf("Cur->slot_array[i] : %d\n",Cur->slot_array[i]);
           // printf("!!!!!!!!!!!!!!!!!!!!!!!\n");
           // printf("i !! i :%d\n",i);
            Cur = Cur->child[Cur->slot_array[i]+1];

        }
    }
        //printf("risk\n");
        return Cur;
    
}

/** Destroy subtree whose root is Cur, By recursion */
void Destroy(WBPlusTreeNode* Cur) {
	if (Cur->isLeaf == true) {
		int i;
		for (i = 0; i < Cur->key_num; i++)
			free(Cur->child[i]);
	} else {
		int i;
		for (i = 0; i < Cur->key_num; i++)
			Destroy(Cur->child[i]);
	}
	free(Cur);
}

int WBPlusTree_Delete(int key){
    WBPlusTreeNode* Leaf = Find(key);
    Delete(Leaf,key);
    return true;
}
int WBPlusTree_Insert(int key, void* value) {
	WBPlusTreeNode* Leaf = Find(key);
	Insert(Leaf, key, value);
	return true;
}
int WBPlusTree_Find(int key) {
	WBPlusTreeNode* Leaf = Find(key);
	int i = slot_binary_search(Leaf, key);
       // printf("Bplustree_find %d\n",i);
        i--;
       // printf("Leaf->key[i] : %d\n",Leaf->key[Leaf->slot_array[i]]);
       // printf("Find key : %d\n",key);
	if (Leaf->key[Leaf->slot_array[i]] != key) return -1; // don't have this key

	
       // printf("Leaf->pos[i] : %d\n",Leaf->pos[i]);
        return i;
}

void WBPlusTree_Destroy() {
	if (Root == NULL) return;
	printf("Now destroying B+tree ..\n");
	Destroy(Root);
	Root = NULL;
	printf("Done.\n");
}

/** Interface: Initialize */
void WBPlusTree_Init() {
	int i;
        WBPlusTree_Destroy();
	Root = New_WBPlusTreeNode();
	Root->isRoot = true;
	Root->isLeaf = true;
        Root->bitmap = 0;
        TotalNodes = 0;
        for(i=0; i<MAX_CHILD_NUMBER-1; i++)
        {
            Root->slot_array[i] = 0;

        }

}

/** Interface: print the tree (DEBUG use)*/
void WBPlusTree_Print() {
	int cnt = 0;

        struct WBPlusTreeNode* root_first = Root;
        struct WBPlusTreeNode* Leaf = NULL;
        //printf("Cur->slot_array size : %4d\n",sizeof(Leaf->slot_array));
        //printf("slot_array[65] :%d\n",Leaf->slot_array[65]);
        while (root_first != NULL && root_first->isLeaf == false) {

            int i;
            printf("-----Print key-----\n");
    
            for(i = 0; i<MAX_CHILD_NUMBER; i++)
            {
                printf("%4d", root_first->key[i]);
                if (++cnt % MAX_CHILD_NUMBER  == 0) printf("\n");
            }
            printf("\n");
            printf("-----Print slot_array-----\n");
            cnt = 0;
            for(i= 0; i<MAX_CHILD_NUMBER+1; i++)
            {
                printf("%4d ",root_first->slot_array[i]);
                if (++cnt % MAX_CHILD_NUMBER+1 == 0) printf("\n");

            }
            cnt = 0;
            printf("\n");
            for(i=0; i<MAX_CHILD_NUMBER+1; i++)
            {
                printf("%p ",root_first->child[i]);
                if (++cnt % MAX_CHILD_NUMBER+1 == 0) printf("\n");
            }
            printf("\n");
            printf("root_first->key[0] :%d\n",root_first->key[root_first->slot_array[1]]);
            printf("root_first->key_num : %d\n",root_first->key_num);
            printf("root_first->bitmap : %d\n",root_first->bitmap);
            printf("root_first->child[0] :%p\n",root_first->child[0]); 
            if(root_first->isRoot == true)
            {
                root_first = root_first->child[0];
            }
            else
            {
                root_first  = root_first->next;
            }
            printf("*************************Non-leaf************************\n");
        }


        Leaf = Root;
        while(1)
        {
        if(Leaf->isLeaf == true)
            break;
        else
        {
            Leaf = Leaf->child[Leaf->slot_array[1]];
        }
        }
        while (Leaf != NULL) {

            int i;
            printf("-----Print key-----\n");

            for(i = 0; i<MAX_CHILD_NUMBER; i++)
            {
                printf("%4d ", Leaf->key[i]);
                if (++cnt % MAX_CHILD_NUMBER+1 == 0) printf("\n");
            }
            printf("\n");
            printf("-----Print slot_array-----\n");
            cnt = 0;
            for(i= 0; i<MAX_CHILD_NUMBER+1; i++)
            {
                printf("%4d ",Leaf->slot_array[i]);
                if (++cnt % MAX_CHILD_NUMBER+1  == 0) printf("\n");

            }
            cnt = 0;
            printf("\n");
            for(i=0; i<MAX_CHILD_NUMBER+1; i++)
            {
                printf("%s ",(char *)Leaf->child[i]);
                if (++cnt % MAX_CHILD_NUMBER+1 == 0) printf("\n");
            }
            printf("\n");
            printf("Leaf->bitmap :%d\n",Leaf->bitmap);
            printf("Leaf->key_num :%d\n",Leaf->key_num);
            printf("First key: %d\n",Leaf->key[Leaf->slot_array[1]]);
           printf("Address of leaf : %p\n",Leaf);
            //printf("Leaf->child[0] : %s\n",Leaf->child[0]);
            printf("*******************leaf*****************\n");
            Leaf = Leaf->next;
        }
        printf("Total nodes : %d\n",TotalNodes);
}


/** Interface: Total Nodes */
int WBPlusTree_GetTotalNodes() {
	return TotalNodes;
}

int WBPlusTree_GetSplitCount(){
    return split_count;
}
