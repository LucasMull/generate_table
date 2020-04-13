#include "src/data_fetch.h"
#include "src/manipulate_data.h"

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#define IDSIZE 999999999
#define AGENCYSIZE 99999 
#define ACCOUNTSIZE 99999

void getUsers(FILE* f_out, t_BLOCK *addr[]);

int main(void)
{
    srand(time(NULL)); //generate unique seed for generator on each run

    char *locale = setlocale(LC_CTYPE, ""); //set locale to UTF8

    FILE *f_name; //list of brazilian names
    FILE *f_surnam; //list of brazilian surnames
    FILE *f_out; //generated individuals

    size_t i;    

    t_HEAP HEAP;
    t_BLOCK b_name, b_surnam, b_ID, b_agency, b_account;

    
    if ( !locale ){
        fprintf(stderr,"Couldn't set locale");
    } else {
        fprintf(stderr,"Locale is set to %s\n",locale);
        free(locale);
    }
    
    initBLOCK(&b_name, &HEAP, 0); //init and assign b_name to HEAP.addr[0]
    initBLOCK(&b_surnam, &HEAP, 0); //init and assign b_surnam to HEAP.addr[1]
    initBLOCK(&b_ID, &HEAP, 12); //init and assign b_ID to HEAP.addr[2]
    initBLOCK(&b_agency, &HEAP, 7); //init and assign b_agency to HEAP.addr[3]
    initBLOCK(&b_account, &HEAP, 7); //init and assign b_account to HEAP.addr[4]

    f_name = fopen("content/nomes.txt", "r"); 
    f_surnam = fopen("content/sobrenomes.txt", "r"); 
    if ( !f_name )
        fprintf(stderr,"Couldn't open nomes.txt");
    if ( !f_surnam )
        fprintf(stderr,"Couldn't open sobrenomes.txt");
  
    fileToBLOCK(f_name, HEAP.addr[0]);
    fclose(f_name);    
    fileToBLOCK(f_surnam, HEAP.addr[1]);    
    fclose(f_surnam);

    NumsToBLOCK(HEAP.addr[2], 700000000, IDSIZE, DBSIZE);
    NumsToBLOCK(HEAP.addr[3], 10000, AGENCYSIZE, 5);
    NumsToBLOCK(HEAP.addr[4], 10000, ACCOUNTSIZE, DBSIZE);
    

    f_out = fopen("data.csv", "w");
    getUsers(f_out, HEAP.addr); //the brain
    fclose(f_out);

    for ( i=0; i<=HEAP.size; ++i )
        freeBLOCK(HEAP.addr[i], 0);

    return 0;
}

//This function needs to be updated and be made into a new library, not modularized enough, basically compiles every information and print output into file
void getUsers(FILE* f_out, t_BLOCK *addr[])
{
    t_tree T[addr[3]->size];    
    t_subj subject;

    char *child;
    char *str1, *str2;
    size_t i, rand_i, rand_j;
    
    for ( i=0; i < addr[3]->size; ++i ){
        initTree(T+i, addr[3]->data[i]);
    }
    
    shuffleArray(addr[2]);
    for ( i=0; i<DBSIZE; ++i ){
        // THIS CREATES ATTRIBUTE 1
        subject.attribute_0 = fetchLinear(addr[2], i);
        // THIS CREATES ATTRIBUTE 3
        str1 = pickRandom(addr[0]);
        str2 = pickRandom(addr[1]);
        joinStrings(subject.attribute_3,str1,str2);
        if (strlen(str2) <= 3){
            str2 = pickRandom(addr[1]);
            joinStrings(subject.attribute_3,subject.attribute_3,str2);
        }

        //THIS CREATES ATTRIBUTE 1 AND 2 (1 is a requisite for 2)
        rand_i = rand()%addr[3]->size;
        do{
            rand_j = rand()%addr[4]->size;
            while ( rand_j < addr[4]->size ){
                if ( child = uniqueChild(T+rand_i,addr[4]->data[rand_j]) )
                    break;
                ++rand_j;
            }
        } while (!child);
        insertChild(T+rand_i, child);
        subject.attribute_1 = addr[3]->data[rand_i];
        subject.attribute_2 = child;
         
        // PRINTS DATABASE TO OUTPUT STREAM 
        fprintf(f_out,"%s,%s,%s,%s\n",subject.attribute_0,
                                      subject.attribute_1, 
                                      subject.attribute_2, 
                                      subject.attribute_3);
    }
    free(subject.attribute_1);
    free(subject.attribute_2);

    for ( i=0; i<addr[3]->size; ++i ){
        eraseTree(T[i].root, T+i);
    }
}
