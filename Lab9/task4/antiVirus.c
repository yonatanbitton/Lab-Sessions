#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct virus {
	int length;
    char signature[]; 
};

typedef struct link link;
typedef struct virus virus;

void PrintHex(char* buffer, int length); 


struct link {
    virus* v;
    link* next;
};


/* Add a new link with the given data to the list 
    (either at the end or the beginning, depending on what your TA tells you),
    and return a pointer to the list (i.e., the first link in the list).
    If the list is null - create a new entry and return a pointer to the entry. */
link* list_append(link* virus_list, virus* data){
    link* curr = virus_list;
    link* insert_virus_node=(struct link*)malloc(sizeof(struct link));
    insert_virus_node->v=data;
    insert_virus_node->next=NULL;
    if (curr==NULL) return insert_virus_node;
    /* Else: list is not null, the curr will be the first node */
    while ((curr->next)!= NULL)
        curr = curr->next;
    curr->next=insert_virus_node;
    return virus_list; /* The first node will still be the first node */
}

link* getVirusList(FILE* input){
    link* virus_list_from_input;
    virus_list_from_input=NULL;
    int finish=0;
    if (input==NULL){
        printf("File Not Found.\r\n");
    }
    else {
         virus* bigVirus=malloc(843);
         bigVirus->length=839;
         fread((char*)bigVirus+4,1,839,input);
		 virus_list_from_input=list_append(virus_list_from_input,bigVirus);
    }
    return virus_list_from_input;
}

void detect_virus(char *buffer, link *virus_list, unsigned int size){
        link* curr = virus_list;
        int i;
        int found=0;
        for (i=0; i<size; i++)
        {
            while (curr!=NULL){
                if (memcmp(curr->v->signature, buffer+i, curr->v->length)==0){  
                	found=1;            
                    printf("Starting location at file: %d\n",i);
                }
                curr=curr->next;
            }
            curr=virus_list;
        }
        if (found==0) printf("Virus not found on this file!\n");
}

int main(int argc, char* argv[]){
    FILE *input;
    input = fopen("signatures", "r");
    if (input==NULL) { 
        printf("File not found\n\n");
        exit(0);
    }
    link* virus_list_from_input = getVirusList(input); /* Returns a pointer to the virus list created from the input given */
    fclose(input);
    int buffer_size=10000;
    char buffer[buffer_size];
    int realSize=0;
    FILE* virus_file;
    virus_file=fopen("ELFexec","r");
    if (virus_file==NULL) { 
        printf("File not found\n\n");
        exit(0);
    }
    realSize=fread(buffer, 1,buffer_size, virus_file);
    detect_virus(buffer, virus_list_from_input, realSize);
    fclose(virus_file);

    return 0;
}
