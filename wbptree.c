#include "wbptree.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define true 1
#define false 0
struct WBPlusTreeNode* Root;
void WBPlusTree_Print();
void Logging(WBPlusTreeNode*);
int MaxChildNumber = 29;
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
        p->bitmap = 0x00;
	TotalNodes++;
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

void Debug_Print(WBPlusTreeNode *Cur)
{
    int i,j;
    int count = 0;
    printf("**********Debug Print**************\n");
    printf("--------------Cur node!!----------------\n");
    for(i=0; i<Cur->key_num; i++)
    {
       // printf("Cur->key[%d] : %d ",i+1,Cur->key[Cur->slot_array[i+1]]);
        
        printf("Cur->key[%d] : %d ",i+1,Cur->key[Cur->slot_array[i+1]]);
        count++;
        if(count == Cur->key_num)
        {
            printf("\n");
        }

        
    }
        
    count = 0;
    printf("Cur->child[0] : %p ",Cur->child[0]);
    for(i=0; i<=Cur->key_num; i++)
    {
        printf("Cur->child[%d] : %p ",i+1,Cur->child[Cur->slot_array[i+1]+1]);
        
        count++;
        if(count == Cur->key_num)
        {
            printf("\n");
        }
       
    }
    for(i=0; i<=Cur->key_num; i++)
    {
        
        printf("Cur->slot_array[%d] : %d ",i,Cur->slot_array[i]);
        
        count++;
        if(count == Cur->key_num)
        {
            printf("\n");
        }
       
    }
     
    WBPlusTreeNode *t = Cur->child[0];
        printf("Cur->key_num : %d\n",Cur->key_num);
        printf("Cur->bitmap : %ld\n",Cur->bitmap);
        printf("My address : %p\n",Cur);
/*
    printf("t->key[0] : %d\n",t->key[t->slot_array[1]]);
    t = t->child[0];
    printf("t->key[0] : %d\n",t->key[t->slot_array[1]]);
    t = t->child[0];
    printf("t->key[0] : %d\n",t->key[t->slot_array[1]]);
    t = t->next;
    
    printf("t->key[0] : %d\n",t->key[t->slot_array[1]]);
    t = t->next;
    printf("t->key[0] : %d\n",t->key[t->slot_array[1]]);
    t = t->next;
    printf("t->key[0] : %d\n",t->key[t->slot_array[1]]);
*/
    printf("--------------End Cur node-------------\n");
   Cur = Cur->father;
  count = 0;
   printf("--------------Father Node!!----------------\n");
   if(Cur != NULL)
   {
   for(i=0; i<Cur->key_num; i++)
    {
        printf("father->key[%d] : %d ",i,Cur->key[Cur->slot_array[i+1]]);
        
        count++;
        if(count == Cur->key_num)
        {
            printf("\n");
        }

        
    }
    count = 0;
    printf("father->child[0]: %p\n",Cur->child[0]);
    for(i=0; i<Cur->key_num; i++)
    {
        printf("father->child[%d] : %p ",i,Cur->child[Cur->slot_array[i+1]+1]);
        
        count++;
        if(count == Cur->key_num)
        {
            printf("\n");
        }
       
    }
    for(i=0; i<=Cur->key_num; i++)
    {
        printf("father->slot_array[%d] : %d ",i,Cur->slot_array[i]);
        
        count++;
        if(count == Cur->key_num)
        {
            printf("\n");
        }
       
    }
        printf("father->key_num : %d\n",Cur->key_num);
        printf("father->bitmap : %ld\n",Cur->bitmap);
    printf("--------------End Cur node-------------\n");
    
   }
    printf("***************End****************\n");
}
void Merge_Node(WBPlusTreeNode *,int, void*);

void Logging(WBPlusTreeNode* Cur)
{
    
        clflush(&Cur); mfence();
        WBPlusTreeNode *log = (struct WBPlusTreeNode*)malloc(sizeof(struct WBPlusTreeNode));

        memmove(log,Cur,sizeof(struct WBPlusTreeNode));
        //clflush(log); mfence();
        clflush(&log); mfence();
       
  
}
void Insert(WBPlusTreeNode*, int,void*);
void Split(WBPlusTreeNode* Cur) {

    Logging(Cur);
    WBPlusTreeNode* Temp = New_WBPlusTreeNode();
    int Mid = MaxChildNumber >> 1;
    //printf("Mid : %d\n",Mid);
    Temp->isLeaf = Cur->isLeaf;
   // Temp->key_num = MaxChildNumber - Mid;
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
        Temp->slot_array[0] = Mid-1;
        Temp->key_num = Mid-1;


        /*Moving child pointer to temp*/
        child = Temp->child[i];
        child->father = Temp;
        Logging(Temp);
        
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
       // Temp->next = Cur->next;
       // Cur->next = Temp;
        Logging(Temp);
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
        Logging(Root);


    }
    else
    {
        Logging(Root);

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
    unsigned long int bit = 1;
    int t;
    int temp;
/*
    if(Cur->bitmap & 1 ==0)
    {

    }
*/
    if(Cur->bitmap & 1 == 0)
    {
        sleep(10);
    }
    ins = slot_binary_search(Cur,key);
    //printf("ins : %d\n",ins);


    Cur->bitmap = Cur->bitmap - 1;
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
                //printf("NONONONONONONO\n");
                temp = value;
                prev = Cur->child[0];
                temp->next = prev->next;
                prev->next = temp;

            }
            else
            {
                // printf("hihi\n");
                //printf("ins : /p\n",ins);
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
void Merge_Node(WBPlusTreeNode *Cur,int key,void *value)
{
    int i, ins,u;
    unsigned long int bit = 1;
    int t;
    int temp;
    ins = slot_binary_search(Cur,key);
//    printf("ins : %d\n",ins);

    Cur->bitmap = Cur->bitmap - 1;

    
    bit = bit << 1;


    u = 0;
    while(1)
    {
        if(u == MAX_CHILD_NUMBER)
        {
            break;
        }
            temp = Cur->bitmap & bit;

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


    
    if(Cur->isLeaf == false && ins == 1)
    {
        Cur->child[0] = value;
    }
    else if(Cur->isLeaf == false)
    {
        Cur->child[u+1] = value;
    }

    if(Cur->isLeaf)
    {
        Cur->child[u] = value;
    }


    Cur->slot_array[0] = Cur->key_num; 




    for(i = Cur->key_num; i>=ins; i--)
    {
        Cur->slot_array[i+1] = Cur->slot_array[i];
    }


    Cur->slot_array[ins] = u;
    //printf("what is ins : %d\n",ins);
    //printf("what is slot_array[ins] : %d\n",Cur->slot_array[ins]);
    //printf("Cur->key_num: %d\n",Cur->key_num);
    //printf("Cur->key[%d] : %d\n",u,Cur->key[Cur->slot_array[ins]]);

   /* 
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
                printf("NONONONONONONO\n");
                temp = value;
                prev = Cur->child[0];
                temp->next = prev->next;
                prev->next = temp;

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
    }*/

    Cur->bitmap= Cur->bitmap + 1 +  (1 << u+1);
}
unsigned int Finding_Moving_Child(WBPlusTreeNode *father, WBPlusTreeNode *Cur)
{
    int index = slot_binary_search(father,Cur->key[Cur->slot_array[1]]);
    index--;

 //   printf("Finding index : %d\n",index);
  //  printf("Cur->key : %d\n",Cur->key[Cur->slot_array[1]]);

    //WBPlusTreeNode *temp = Cur->child[0];
   // printf("temp:%d\n",temp->key[temp->slot_array[1]]);
    {
        
        if(index == 0)
        {
            return father->key[father->slot_array[index+1]];
        }/*
        if(temp->key[temp->slot_array[1]] == father->key[father->slot_array[index]])
        {
       // printf("hi");
            return father->key[father->slot_array[index]];
        }*/
        else
        {
            return father->key[father->slot_array[index]];
        }
        

        //index--;
        

    }
}

void Re_Delete(WBPlusTreeNode *Cur, int key)
{
    int ins;
    int bit_pos = 0;
    int i;
    //WBPlusTreeNode *temp = New_WBPlusTreeNode();


    ins = slot_binary_search(Cur,key);

    ins--;
    //printf("Cal Redelete\n");
    int j; 
    
    /*for(j = 0; j<Cur->key_num; j++)
    {
        printf("Cur->key[%d] : %d\n",j,Cur->key[Cur->slot_array[j+1]]);
    }*/

   // printf("ins: %d\n",ins);
    //printf("key: %d\n",key);
    bit_pos = 1 << (Cur->slot_array[ins]+1);
    //Findiing bit position
    Cur->bitmap = Cur->bitmap - bit_pos;
    Cur->key[Cur->slot_array[ins]] = 0;

    //printf("Cur->key_num:%d\n",Cur->key_num);
   // printf("Re delete ins:%d\n",ins);
   // printf("Cur key[!]:%d\n",Cur->key[Cur->slot_array[ins]]);
    
    for(i=ins; i<Cur->key_num; i++)
    {
        Cur->slot_array[i] = Cur->slot_array[i+1];
    }
    Cur->slot_array[Cur->key_num] = 0;
    Cur->key_num = Cur->key_num - 1;
    Cur->slot_array[0] = Cur->key_num;
    

    /* clear slot_aray == 0
       if(Cur->key_num == 0)
       {
       Cur->slot_array[i-1] = 0;

       }*/

}
WBPlusTreeNode* Find_Child(WBPlusTreeNode* Cur, int key){

    int index = slot_binary_search(Cur,key);

    if(index == 1)
    {
        //printf("Child 0!\n");
        return Cur->child[0];
    }
    else
    {
        //printf("Child not 0!\n");
        return Cur->child[Cur->slot_array[index-1]+1];
    }

}

void Delete(WBPlusTreeNode*, int);

int Merge(WBPlusTreeNode *Cur)
{
    WBPlusTreeNode *father = Cur->father;
    int index = slot_binary_search(father,Cur->key[Cur->slot_array[1]]);
    int j,temp,Move_key,Root_up_key;
    WBPlusTreeNode *next,*prev,*t;
    //printf("index merge : %d\n",index);
    //printf("father->key_num : %d\n",father->key_num);
    index--;


    if(Cur->isLeaf == false)
    {
        if(index == 0){


                //printf("Non leaf merge!!\n");
                next = father->child[father->slot_array[index+1]+1];
                prev = NULL;

               // printf("hi\n");
               // printf("next: %d\n",next->key[next->slot_array[1]]);
                //printf("Cur->father :%d\n",father->key[father->slot_array[1]]);

                for(j=0; j < next->key_num; j++)
                {

                    Merge_Node(Cur,next->key[next->slot_array[j+1]],next->child[next->slot_array[j+1]+1]);
                    t = next->child[next->slot_array[j+1]+1];
                    t->father = Cur;
                }
                Move_key = Finding_Moving_Child(father,next);
                temp = Move_key;
                WBPlusTreeNode *child = Find_Child(next,Move_key);
                child->father = Cur;

                Merge_Node(Cur,Move_key,child);

                
                Cur->next = next->next;
                
                //Debug_Print(Cur);
                /*Adjusting Cur child Postion in Non_leaf*/
                Move_key = Finding_Moving_Child(father,Cur);
                child = Find_Child(father,Move_key);
                Merge_Node(father,Move_key,child);
                Re_Delete(father,Move_key);
                child->father = father;
                /************************/

                //Debug_Print(Cur);
                


                
                /*Remove original father key*/
                Re_Delete(father,temp);
                
                free(next);
                if(father->isRoot && father->key_num == 0)
                {
                    //printf("Free Root!\n");
                    //printf("Root : %p\n",Root);
                    //printf("father : %p\n",father);
                    WBPlusTreeNode *t = father->child[0];
                    //printf("t->key[0]: %d\n",t->key[t->slot_array[1]]);
           
                 //   Debug_Print(Cur);
                    Root = father->child[0];
                    
                    Root->isRoot = true;
                    Root->father = NULL;
                    free(father);
                }

                return true;

        }

    else if(index == father->key_num)
    {
        //printf("hi3\n");
        //printf("Non leaf merge!!\n");
        if(index == 1)
        {
            prev = father->child[0];
        }
        else
        {
            prev = father->child[father->slot_array[index-1]+1];//prev
        }
        next = NULL;
        //printf("father key : %d\n",father->key[father->slot_array[1]]);
        //printf("prev :%d\n",prev->key[prev->slot_array[1]]);
        //Debug_Print(prev);
        //Debug_Print(Cur);

        //printf("what is t : %p\n",Cur->child[Cur->slot_array[1]+1]);

        Merge_Node(prev,Cur->key[Cur->slot_array[1]],Cur->child[Cur->slot_array[1]+1]);

        t = Cur->child[Cur->slot_array[1]+1];
        t->father = prev;

     //   Debug_Print(prev);
       // Debug_Print(Cur);
        Move_key = Finding_Moving_Child(father,Cur);
        WBPlusTreeNode *child = Find_Child(Cur,Move_key);
        child->father = prev;

       // printf("what is Move_key : %d\n",Move_key);
        //printf("child : %p\n",child);
        Merge_Node(prev,Move_key,child);
        Re_Delete(father,Move_key);
       
       // Debug_Print(prev);
       // Debug_Print(Cur);

        //sleep(1000);

        //  Re_Child(father,prev->key[prev->slot_array[1]],true);

        prev->next = Cur->next;
        free(Cur);
        if(father->isRoot && father->key_num == 0)
        {
          //  printf("Delete Root\n");
            Root = father->child[0];
            Root->isRoot = true;
            Root->father = NULL;
            free(father);
        }



        return true;
    }
        else
        {   //printf("hi2\n"); 
            //printf("Non leaf merge!!\n");
            if(index == 1)
            {
                prev = father->child[0];
            }
            else
            {
                prev = father->child[father->slot_array[index-1]+1];
            }

            next = father->child[father->slot_array[index+1]+1];

            //printf("prev :%d\n",prev->key[prev->slot_array[1]]);
            //printf("next :%d\n",next->key[next->slot_array[1]]);


                if(prev->key_num <=2 &&prev->key_num < next->key_num)
                {
                    printf("Come here impossible\n");
                     sleep(100);

                    for(j=0; j<next->key_num; j++)
                    {

                        Merge_Node(Cur,next->key[next->slot_array[j+1]],next->child[next->slot_array[j+1]+1]);
                    }

                    Re_Delete(father,next->key[next->slot_array[1]]);
                       free(next);
                    if(father->isRoot && father->key_num == 0)
                    {
                        Root = father->child[father->slot_array[1]];
                        Root->isRoot = true;
                        Root->father = NULL;
                        free(father);
                    }
                    return true;
                }
                

               // if(prev->key_num == next->key_num || prev->key_num > next->key_num)
                else
                {
                    //printf("hi\n");
                    Merge_Node(prev,Cur->key[Cur->slot_array[1]],Cur->child[Cur->slot_array[1]+1]);
                    t = Cur->child[Cur->slot_array[1]+1];
                    t->father = prev;
                    //printf("what is t %p\n",t);
                    Move_key = Finding_Moving_Child(father,Cur);

                    WBPlusTreeNode *child = Find_Child(Cur,Move_key);
                    child->father = prev;
                    //printf("what is Move_key : %d\n",Move_key);
                    //printf("what is child: %p\n",child);
                    Merge_Node(prev,Move_key,child);
                    Re_Delete(father,Move_key);
                    prev->next = Cur->next;
                      
                    free(Cur);
                    if(father->isRoot && father->key_num == 0)
                    {
                        Root = father->child[0];
                        Root->isRoot = true;
                        Root->father = NULL;
                        free(father);
                    }
                    return true;
                }



    }

    }
    else{
/**********************/
    if(index == 0){

        next = father->child[father->slot_array[index+1]+1];

        //next = Cur->next;

        prev = NULL;

        //printf("hi\n");
        //printf("Normal Merge!\n");
        //printf("next: %d\n",next->key[next->slot_array[1]]);
        //printf("next: %p\n",next);
        // Insert(next,Cur->key[Cur->slot_array[1]],Cur->child[Cur->slot_array[1]]);
        // Delete(father,next->key[next->slot_array[1]]);


        //printf("Cur key:%d\n",Cur->key[Cur->slot_array[1]]);
        for(j=0; j < next->key_num; j++)
        {

            Merge_Node(Cur,next->key[next->slot_array[j+1]],next->child[next->slot_array[j+1]]);
            t = next->child[next->slot_array[j+1]];
            t->father = Cur->father;
        }
        /* 
           for(j=0; j < Cur->key_num; j++)
           {

           printf("Cur key+:%d\n",Cur->key[Cur->slot_array[j+1]]);
           }*/

        Re_Delete(father,next->key[next->slot_array[1]]);
        //memmove(newnode,Cur,sizeof(struct WBPlusTreeNode));
        //Insert(father,newnode->key[newnode->slot_array[1]],newnode);

        /*Adjusting the newnode child position in Non_leaf Node*/
        /*************************************************/


        Merge_Node(father,Cur->key[Cur->slot_array[1]],Cur);
       // Debug_Print(Cur);
        Re_Delete(father,Cur->key[Cur->slot_array[1]]);

        //Debug_Print(Cur);
        /*Re_Delete Becase We keep the key number and child number +1 */
        /************************************************/

        Cur->next = next->next;
        //


        // Re_Delete(father,next->key[next->slot_array[1]]);
        free(next);
        //  free(Cur);
        /* int i = slot_binary_search(father,next->key[next->slot_array[1]]);
           i--;
           if(father->key[father->slot_array[i]] == next->key[next->slot_array[1]])
           {

           }*/
        
        return true;


    }

    else if(index == father->key_num)
    {
        //printf("Normal Merge!\n");
        //printf("hi3\n");
        if(index == 1)
        {
            prev = father->child[0];
        }
        else
        {
            prev = father->child[father->slot_array[index-1]+1];//prev
        }
        next = NULL;
        //printf("father key : %d\n",father->key[father->slot_array[1]]);
        //printf("prev :%d\n",prev->key[prev->slot_array[1]]);


        Merge_Node(prev,Cur->key[Cur->slot_array[1]],Cur->child[Cur->slot_array[1]]);
        t = Cur->child[Cur->slot_array[1]];
        t->father = prev->father;
        Re_Delete(father,Cur->key[Cur->slot_array[1]]);

//        Debug_Print(prev);

        /*Adjusting the newnode child position in Non_leaf Node*/
        /*************************************************/
      //  Merge_Node(father,prev->key[prev->slot_array[1]],prev);

       // Re_Delete(father,prev->key[prev->slot_array[1]]);
        /*Re_Delete Becase We keep the key number and child number +1 */
        /************************************************/




        //Re_Child(father,prev->key[prev->slot_array[1]],false);
        prev->next = Cur->next;
        free(Cur);
        return true;
    }
    else
    {   //printf("hi2\n"); 
        //printf("Normal Merge!\n");
        if(index == 1)
        {
            prev = father->child[0];
        }
        else
        {
            prev = father->child[father->slot_array[index-1]+1];
        }

        next = father->child[father->slot_array[index+1]+1];

        //printf("prev :%d\n",prev->key[prev->slot_array[1]]);
        //printf("next :%d\n",next->key[next->slot_array[1]]);
        //printf("prev->key_num : %d\n",prev->key_num);
        //printf("next->key_num : %d\n",next->key_num);

        
            if(prev->key_num <=2 && prev->key_num < next->key_num)
            {
                //printf("Impossible\n");
                //sleep(100);

            for(j=0; j<next->key_num; j++)
            {

            Insert(Cur,next->key[next->slot_array[j+1]],next->child[next->slot_array[j+1]]);
            Re_Delete(father,next->key[next->slot_array[1]]);
            Cur->next = NULL;
            }
            Re_Delete(father,next->key[next->slot_array[1]]);
            free(next);
            }
            //if(prev->key_num == next->key_num || prev->key_num > next->key_num)
            else
            {
                Merge_Node(prev,Cur->key[Cur->slot_array[1]],Cur->child[Cur->slot_array[1]]);
                t = Cur->child[Cur->slot_array[1]];
                t->father = prev->father;
                Re_Delete(father,Cur->key[Cur->slot_array[1]]);
                //Re_Child(father,Cur->key[Cur->slot_array[1]],false);
                prev->next = Cur->next;
                   free(Cur);
                return true;
            }

        


    }
    }

}


void replace(WBPlusTreeNode *Cur,int key,unsigned char position)
{
    int i = slot_binary_search(Cur,key);
    if(position == 3)
    {
        /*if we borrow value from right, position --*/
    i--;
    }
    /*Because of Left/right overwrting must be diffrent*/
    
    Cur->key[Cur->slot_array[i]] = key;
    //printf("Replace !!\n");

    //printf("Replace i %d\n",i);
    //printf("key :%d\n",key);
    

    
}
int Redistribute(WBPlusTreeNode *Cur)
{
    WBPlusTreeNode *father = Cur->father;
    int index = slot_binary_search(father,Cur->key[Cur->slot_array[1]]);
    int Root_up_key,Move_key;
    index--;
    WBPlusTreeNode *next,*prev,*child,*child2;
   // printf("Redistribute!!\n");
     //       printf("index : %d\n",index);
//    printf("father->key_num : %d\n",father->key_num);

  //  printf("cur key :%d\n",Cur->key[Cur->slot_array[1]]);
  //  printf("father->key[0]: %d\n",father->key[father->slot_array[1]]);
  //  printf("Cur->key[Cur->slot_array[0] : %d\n",Cur->key[Cur->slot_array[1]]);




    if(Cur->isLeaf == false)
    {
       // printf("----------------non----leaf------\n");
        
        if(index == 0)
        {
            //printf("hi\n");
            next = father->child[father->slot_array[index+1]+1];
            prev = NULL;
            if(next->key_num <= 2)
            {
                return false;
            }
            else
            {
                //printf("next :%d\n",next->key[next->slot_array[1]]);
                
                
                 Move_key = Finding_Moving_Child(father,next);
                 Root_up_key = next->key[next->slot_array[1]];

                child = Find_Child(next,Move_key);
                                
                //printf("Move_key : %d\n",Move_key);
                child->father = Cur;
               // printf("Cur : %p\n",Cur);
               // printf("child : %p\n",child);
                //printf("child->father : %p\n",child->father);
                Merge_Node(Cur,Move_key,child);
               // printf("child key : %d\n",child->key[child->slot_array[1]]);
                
                child2 = Find_Child(next,Root_up_key);
                //printf("Root_up_key : %d\n",Root_up_key);
               // printf("child2 : %p\n",child2);
                child2->father = next;
                Re_Delete(next,next->key[next->slot_array[1]]);
                Merge_Node(next,Root_up_key,child2);
                //Debug_Print(next);
                replace(father,next->key[next->slot_array[1]],3);
                Re_Delete(next,next->key[next->slot_array[1]]);
               // Debug_Print(next);

               
                //printf("next->slot_array[1]] :%d\n",next->key[next->slot_array[1]]);
                
                return true;
            }
        }
        else if(index == father->key_num)
        {
           // printf("hi1\n");

           // sleep(10);

            if(index == 1)
            {
                
                prev = father->child[0];
            }
            else
            {
                prev = father->child[father->slot_array[index-1]+1];//prev
            }
            next = NULL;

            //printf("father key : %d\n",father->key[father->slot_array[1]]);
            //printf("prev :%d\n",prev->key[prev->slot_array[1]]);

            if(prev->key_num  <= 2)
                return false;
            else
            {

                //printf("what is i : %d\n",i);
                //
                //replace(Cur,father->key[father->slot_array[1]],3);
                //replace(father,prev->key[prev->slot_array[prev->key_num]],4);

                Root_up_key = Finding_Moving_Child(father,Cur);
                child = Find_Child(Cur,Root_up_key);
                child->father = Cur;
                
                Move_key = prev->key[prev->slot_array[prev->key_num]];
                child2 = Find_Child(prev,Move_key);
                child2->father = Cur;
                
                Merge_Node(Cur,Move_key,child2);
                Merge_Node(Cur,Root_up_key,child);


               

                Re_Delete(Cur,Move_key);
                replace(father,Move_key,4);
                Re_Delete(prev,Move_key);
            
               // printf("%d\n",father->key[father->slot_array[1]]);
              //  WBPlusTreeNode *n = Cur->child[n->slot_array[in]+1];
                //printf("key : %d\n",n->key[n->slot_array[1]]);
               // Re_Delete(prev,prev->key[prev->slot_array[prev->key_num]]);
                // Insert(father,prev->key[prev->slot_array[prev->key_num]],Cur);


                return true;
            }
        }
        else
        {
            //printf("hi2\n");
            if(index == 1)
            {
                prev = father->child[0];
            }
            else
            {
                prev = father->child[father->slot_array[index-1]+1];
            }

            next = father->child[father->slot_array[index+1]+1];

            //printf("prev :%d\n",prev->key[prev->slot_array[1]]);
            //printf("next :%d\n",next->key[next->slot_array[1]]);
            if(prev->key_num <= 2 && next->key_num <= 2)
                return false;
            else
            {
                if(prev->key_num <=2 && prev->key_num < next->key_num)
                {
                    //printf("wow\n");
                    Move_key = Finding_Moving_Child(father,next);
                    child = Find_Child(next,Move_key);
                    child->father = Cur;
                    Merge_Node(Cur,Move_key,child);
 
                    Root_up_key = next->key[next->slot_array[1]];
                   
                    replace(father,next->key[next->slot_array[1]],3);

                    child2 = Find_Child(next,Root_up_key);
                    child2->father = next;
                    
                    Re_Delete(next,next->key[next->slot_array[1]]);
                    Merge_Node(next,Root_up_key,child2);
                    Re_Delete(next,next->key[next->slot_array[1]]);
                    
                    
                   // printf("next->key[next->slot_array[1] : %d\n",next->key[next->slot_array[1]]);
                   // printf("Move_key : %d\n",Move_key);
                    return true;
                }
                else
                {
                   // printf("Here\n");
                    //sleep(10);
                    //Insert(Cur,prev->key[prev->slot_array[prev->key_num]],prev->child[prev->slot_array[prev->key_num]]);
                    //Re_Delete(prev,prev->key[prev->slot_array[prev->key_num]]);
                    //replace(father,prev->key[prev->slot_array[prev->key_num]],4);
                    //Re_Delete(prev,prev->key[prev->slot_array[prev->key_num]]);

          

                     Root_up_key = Finding_Moving_Child(father,Cur);


                    child = Find_Child(Cur,Root_up_key);
                    child->father = Cur;
                   // Merge_Node(Cur,Root_up_key,child);


                     Move_key = prev->key[prev->slot_array[prev->key_num]];
                 //   printf("Move_key :%d\n",Move_key);
                    child2 = Find_Child(prev,Move_key);
                    child2->father = Cur;
                    Merge_Node(Cur,Move_key,child2);
                    Merge_Node(Cur,Root_up_key,child);

                    Re_Delete(prev,Move_key);
                    Re_Delete(Cur,Move_key);


                    replace(father,Move_key,4);



                    return true;
                }

            }
        }



    }

    else{
/************************/
        //printf("Normal redistribute\n");
    if(index == 0)
    {
        //printf("hi\n");

        next = father->child[father->slot_array[index+1]+1];
        prev = NULL;
        //printf("next->key %d\n",next->key[next->slot_array[1]]);
        if(next->key_num <= 2)
        {
            return false;
        }
        else
        {
          //  printf("next :%d\n",next->key[next->slot_array[1]]);

            Merge_Node(Cur,next->key[next->slot_array[1]],next->child[next->slot_array[1]]);
            Re_Delete(next,next->key[next->slot_array[1]]);
            replace(father,next->key[next->slot_array[1]],3);
            return true;
        }
    }
    else if(index == father->key_num)
    {
       // printf("hi1\n");


        if(index == 1)
        {
         //   printf("exi!\n");
            prev = father->child[0];
        }
        else
        {
           // printf("dxei!\n");
            prev = father->child[father->slot_array[index-1]+1];//prev
        }
       // printf("index : %d\n",index);

        next = NULL;

        //printf("father key : %d\n",father->key[father->slot_array[1]]);
       // printf("prev :%d\n",prev->key[prev->slot_array[1]]);

        if(prev->key_num  <= 2)
            return false;
        else
        {
         //   printf("hi error!!\n");

            
            Merge_Node(Cur,prev->key[prev->slot_array[prev->key_num]],prev->child[prev->slot_array[prev->key_num]]);
            replace(father,prev->key[prev->slot_array[prev->key_num]],4);
            Re_Delete(prev,prev->key[prev->slot_array[prev->key_num]]);
            // Insert(father,prev->key[prev->slot_array[prev->key_num]],Cur);


            return true;
        }
    }
    else
    {
        //printf("hi2\n");
        if(index == 1)
        {
            prev = father->child[0];
        }
        else
        {
            prev = father->child[father->slot_array[index-1]+1];
        }

        next = father->child[father->slot_array[index+1]+1];
       // next = Cur->next;
       // printf("prev :%d\n",prev->key[prev->slot_array[1]]);
       // printf("next :%d\n",next->key[next->slot_array[1]]);
        if(prev->key_num <= 2 && next->key_num <= 2)
            return false;
        else
        {
            if(prev->key_num == 2 && prev->key_num < next->key_num)
            {
                //sleep(10);
                //Debug_Print(Cur);
                //Debug_Print(next);
                Merge_Node(Cur,next->key[next->slot_array[1]],next->child[next->slot_array[1]]);
                replace(father,next->key[next->slot_array[2]],3);
                Re_Delete(next,next->key[next->slot_array[1]]);
                //Debug_Print(Cur);
                //Debug_Print(next);

                
            }
            else
            {
                //printf("Here\n");
                Merge_Node(Cur,prev->key[prev->slot_array[prev->key_num]],prev->child[prev->slot_array[prev->key_num]]);
                //Re_Delete(prev,prev->key[prev->slot_array[prev->key_num]]);
                replace(father,prev->key[prev->slot_array[prev->key_num]],4);
                Re_Delete(prev,prev->key[prev->slot_array[prev->key_num]]);
                return true;
            }

        }
    }

    //printf("index : %d\n",index);

    //printf("cur key :%d\n",Cur->key[Cur->slot_array[1]]);
    //printf("father : %p\n",father);
    // printf("father key : %d\n",father->key[father->slot_array[1]]);
    // printf("next :%d\n",next->key[next->slot_array[1]]);
   // printf("prev :%d\n",prev->key[prev->slot_array[1]]);
   // printf("next->key_num: %d\n",next->key_num);
    //printf("prev->key_num: %d\n",prev->key_num);
   // sleep(5);
    
    }

}
void Delete(WBPlusTreeNode *Cur, int key){
   
    int ins;
    int bit_pos = 0;
    int i;
   WBPlusTreeNode *father;

    
    ins = slot_binary_search(Cur,key);

    ins--;

    //printf("what is ins: %d\n",ins);
    /*:L
    if(Cur->isRoot)
    {
        printf("Root!!\n");
    }*/
   Cur->bitmap = Cur->bitmap - 1;

    bit_pos = 1 << (Cur->slot_array[ins]+1);
    //Findiing bit position
    Cur->bitmap = Cur->bitmap+1 - bit_pos;
   // Cur->key[Cur->slot_array[ins]] = 0;

   clflush(&Cur->bitmap); mfence();
    
   
   for(i=ins; i<Cur->key_num; i++)
    {

        Cur->slot_array[i] = Cur->slot_array[i+1];
        //Cur->slot_array[i] = 0;
    }
    Cur->slot_array[Cur->key_num] = 0;
    
    /*
    for(i=0; i< Cur->key_num; i+=8 )
    {
        clflush(&Cur->slot_array[i]);
    }
    mfence();*/
    Cur->key_num = Cur->key_num - 1;
    //Cur->slot_array[i+1] = 0;
    Cur->slot_array[0] = Cur->key_num;
    /*
    clflush(&Cur->key_num);
    clflush(&Cur->slot_array[0]);
    mfence();*/
    /* clear slot_aray == 0
    if(Cur->key_num == 0)
    {
        Cur->slot_array[i-1] = 0;
        
    }*/

/*
    if(Cur->isLeaf == false){
        delChild = Cur->child[ins+1];
        WBPlusTreeNode* firstChild = (WBPlusTreeNode*)(Cur->child[Cur->slot_array[1]]);
        if(firstChild->isLeaf == true){
            WBPlusTreeNode* temp = (WBPlusTreeNode*)delChild;
            WBPlusTreeNode *prevChild = Cur->child[Cur->slot_array[ins]];
            WBPlusTreeNode *nextChild = temp; 
            if(prevChild != NULL) prevChild->next = nextChild;
            if(nextChild != NULL) nextChild->last = prevChild;
        

        }
    }
*/  
   // printf("ins : %d\n",ins);
    
    if(ins == 1 && !Cur->isRoot){
//        printf("Normal ins : %d\n",ins);

        WBPlusTreeNode *temp;
        temp = Cur;
                
        while(1){
            int i = slot_binary_search(temp, key);
            i--;
           if(temp->isRoot == true)
            {
                if(temp->key[temp->slot_array[i]] == key)
                {
                    temp->key[temp->slot_array[i]] = Cur->key[Cur->slot_array[1]];
                //    printf("Cur->key[Cur->slot_array[1]] : %d\n",Cur->key[Cur->slot_array[1]]);
                 //   printf("root ins : %d\n",ins);

                }
                break;
            }
            if(temp->key[temp->slot_array[i]] == key)
            {
               // printf("temp->key[0] : %d\n",temp->key[temp->slot_array[1]]);
                temp->key[temp->slot_array[i]] = Cur->key[Cur->slot_array[1]];
               // printf("temp->key[Cur->slot_array[1]] : %d\n",temp->key[temp->slot_array[i]]);
               // printf("Second ins : %d\n",ins);

                temp = temp->father;
                continue;
            }
                temp = temp->father;
        }

    }


     WBPlusTreeNode *temp = Cur;
    if(temp->key_num  <= 1 && !temp->isRoot)
    {

       // printf("key!!\n");
        //printf("root : %p\n",Root);
        //printf("Root->key_num: %d\n",Root->key_num);
        //printf("temp : %p\n",temp);
        //printf("temp->key_num: %d\n",temp->key_num);
       // Debug_Print(Root);
       // Debug_Print(temp);
       // WBPlusTree_Print();
        if(Redistribute(temp) == false)
        {
         //   printf("Redistibute fail!\n"); 
          //  printf("Merge!!\n");
            Merge(temp);
        }
    }
/*Checking the last leaf deletion*/
    father = Cur->father;

    

    if(father != NULL)
    {
        while(1)
        {

            Cur = Cur->father;


            if(Cur->key_num <=1 && !Cur->isRoot)
            {
            //    printf("Upper!\n");
                if(Redistribute(Cur) == false)
                {
              //      printf("Non-leaf merge!!\n");
                    Merge(Cur);
                }

            }
            if(Cur->isRoot)
            {
                break;
            }

        }
    }
    if(father != NULL && father->isRoot && father->key_num == 0)
    {
       // printf("Free Root!\n");
       // printf("shit\n");
        Root = father->child[0];
        Root->isRoot = true;
        Root->father = NULL;
        Root->isLeaf = true;
        free(father);
        return ;

    }
   // printf("***********************\n");
   // printf("Delete success\n");
   // printf("***********************\n");

    

}



WBPlusTreeNode* Find(int key) {
    WBPlusTreeNode* Cur = Root;
    //printf("In Find what is Root->key[0] :%d\n",Cur->key[Cur->slot_array[1]]);
    //printf("In find what is key : %d\n",key);
    
    if(!Cur->isRoot || Root == NULL)
    {
       // printf("No Root\n");
        exit(-1);
    }
   // Debug_Print(Root);

       while (1) {
        if (Cur->isLeaf == true)
        {
            //      printf("Cur address in leaf : %p\n",Cur);
           // printf("first hi\n");
            
            break;
        }
           // printf("Cur key :%d\n",Cur->key[Cur->slot_array[1]]);
        if(key < Cur->key[Cur->slot_array[1]])
        {
           // printf("first\n");
            Cur = Cur->child[0];
            //printf("Cur key :%d\n",Cur->key[Cur->slot_array[1]]);
         //   WBPlusTreeNode *node = Cur->child[0];dddd
          //  printf("node : %p\n",node);
            //printf("node->key : %d\n",node->key[node->slot_array[1]]);
           // Debug_Print(Cur);
        }
        else
        {
           // printf("hi\n");
           // printf("thrid in key :%d\n",key);
            int i = slot_binary_search(Cur, key);


            
        
      //      printf("beafore -- Cur->slot_array[i] : %d\n",Cur->slot_array[i]);
            //printf("what is i : %d\n",i);
           i--;
          //  printf("------------------\n");
           // printf("Cur->slot_array[i] : %d\n",Cur->slot_array[i]);
           // printf("!!!!!!!!!!!!!!!!!!!!!!!\n");
           
          //  printf("i !! i :%d\n",i);
           // Debug_Print(Cur);

            Cur = Cur->child[Cur->slot_array[i]+1];


//            WBPlusTreeNode *t = Cur;
          //  printf("t: %p\n",t);
          //  printf("t->key[0] : %d\n",t->key[1]);
            
        }
    }
      //  printf("risk\n");
        return Cur;
    
}

/** Destroy subtree whose root is Cur, By recursion */
/*
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
}*/

int WBPlusTree_Delete(int key){
    int ret,i;
    //printf("ret!!\n");
    
    WBPlusTreeNode* Leaf = Find(key);
   // printf("Root : %p\n",Root);
   // printf("Find key !! :%d\n",key);
   // printf("Leaf : %p\n",Leaf);
    i = slot_binary_search(Leaf,key);
    i--;

   // printf("Find in key : %d\n",Leaf->key[Leaf->slot_array[i]]);
/* 
    if(key == 15)
    {
        printf("Leaf->key[Leaf->slot_array[1] :%d\n",Leaf->key[Leaf->slot_array[1]]);
        printf("key is 15 and index : %d\n",i);
        sleep(5);
    }*/
    if(Leaf->key[Leaf->slot_array[i]] == key){

       // printf("Find!\n");
        Delete(Leaf,key);
       
        /*
        if(Leaf->isLeaf && Leaf->isRoot && Root->key_num == 0)
        {
            free(Root);
            printf("Root : %p\n",Root);
            printf("Free Root Nothing in Node\n");
        }*/
        return 1;
    }
    else{
        //printf("Not find\n");
        return -2;
    }
    
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
	if (Leaf->key[Leaf->slot_array[i]] == key)
        {
            return -1;
        }
        else
        {
	
       // printf("Leaf->pos[i] : %d\n",Leaf->pos[i]);
        return i;
        }
}
/*
void WBPlusTree_Destroy() {
	if (Root == NULL) return;
	Destroy(Root);
	Root = NULL;
	printf("Done.\n");
}
*/
/** Interface: Initialize */
void WBPlusTree_Init() {
	unsigned char i;
	Root = New_WBPlusTreeNode();
	Root->isRoot = true;
	Root->isLeaf = true;
        Root->bitmap = 0x00 | 1;
        TotalNodes = 0x00;
        for(i=0; i<MAX_CHILD_NUMBER-1; i++)
        {
            Root->slot_array[i] = 0x00;

        }

}

/** Interface: print the tree (DEBUG use)*/
void WBPlusTree_Print() {
	int cnt = 0;
        int flag =0 ;
        struct WBPlusTreeNode* root_first = Root;
        struct WBPlusTreeNode* Leaf = NULL;
        WBPlusTreeNode *before,*origin;
        //printf("Cur->slot_array size : %4d\n",sizeof(Leaf->slot_array));
        //printf("slot_array[65] :%d\n",Leaf->slot_array[65]);
        printf("Print start!!\n");
        while (root_first->isLeaf == false) {

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
            printf("root_first->bitmap : %ld\n",root_first->bitmap);
            printf("root_first->child[0] :%p\n",root_first->child[0]);
            printf("Root address : %p\n",Root);
            printf("My_addrss : %p\n",root_first);
            printf("Next : %p\n",root_first->next);
            printf("father : %p\n",root_first->father);
            
            if(root_first->isRoot == true )
            {
                root_first = root_first->child[0];
                before = root_first;
            
            }
            else
            {
            root_first = root_first->next;
            if(root_first == NULL)
            {
                root_first = before->child[0];
                before = root_first;
            }
            
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
            Leaf = Leaf->child[0];
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
            printf("Leaf->bitmap :%ld\n",Leaf->bitmap);
            printf("Leaf->key_num :%d\n",Leaf->key_num);
            printf("First key: %d\n",Leaf->key[Leaf->slot_array[1]]);
           printf("Address of leaf : %p\n",Leaf);
           printf("Address of next : %p\n",Leaf->next);
           printf("Address of father: %p\n",Leaf->father);
            //printf("Leaf->child[0] : %s\n",Leaf->child[0]);
            printf("*******************leaf*****************\n");
            Leaf = Leaf->next;
        }
        printf("Total nodes : %d\n",TotalNodes);

       // Debug_Print(Root);
        
}


/** Interface: Total Nodes */
int WBPlusTree_GetTotalNodes() {
	return TotalNodes;
}

int WBPlusTree_GetSplitCount(){
    return split_count;
}
