#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct virus {
    unsigned short length;
    char name[16];
    char signature[]; 
};

typedef struct link link;
typedef struct virus virus;

void PrintHex(char* buffer, int length); 


struct link {
    virus* v;
    link* next;
};


/* Print the data of every link in list. Each item followed by a newline character. */
void list_print(link *virus_list){
    link* curr = virus_list;
    int k=1;
    while(curr != NULL){
        printf("Virus name: %s\n",curr->v->name);
        printf("Virus size: %d\n",curr->v->length);
        printf("signature:\n");
        PrintHex(curr->v->signature, curr->v->length);
        printf("\n");
        curr = curr->next;
        k++;

    }
}

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

/* Free the memory allocated by the list. I allocated place for each virus and for each link!*/
void list_free(link *virus_list) {
    if (virus_list == NULL) return;
    else {
        list_free(virus_list->next);
        free(virus_list->v);
        free(virus_list);
    }
}


void PrintHex(char* buffer, int length){
    int j;
    for (j=0; j<length-1; j++)
        printf("%02X ", buffer[j] & 0x00FF);
    printf("%02X", buffer[length-1] & 0x00FF);
    printf("\n");
}

link* getVirusList(FILE* input){
    link* virus_list_from_input;
    virus_list_from_input=NULL;
    int finish=0;
    if (input==NULL){
        printf("File Not Found.\r\n");
    }
    else {
        char header_buf[2];
        fread(header_buf, 1, 2, input);
        int big_indian = 0;
        int null_terminated = 0;
        if (header_buf[0] == 1) big_indian = 1;
        if (header_buf[1] == 1) null_terminated = 1;
        while (finish == 0) {
            char length_buf[2];
            if (fread(length_buf,1,2,input)<2) finish=1;
            int length = (length_buf[0]*100 + length_buf[1]);
            virus* curr_virus=malloc(length);
            length = ((length) - 2 - 16); /* The length is the actuall length read, minus 2 for the length representation, minus 16 for the name */
            curr_virus->length=length;
            if (fread((char*)curr_virus+2,1,16+length,input)<(16+length)) { 
                finish=1; 
                free(curr_virus); 
                break;
            }
            virus_list_from_input=list_append(virus_list_from_input,curr_virus);

        }
    }
    return virus_list_from_input;
}

void detect_virus(char *buffer, link *virus_list, unsigned int size){
        link* curr = virus_list;
        int i;
        for (i=0; i<size; i++)
        {
            while (curr!=NULL){
                if (memcmp(curr->v->signature, buffer+i, curr->v->length)==0){              
                    printf("Starting location at file: %d\n",i);
                    printf("Virus name: %s\n",curr->v->name);
                    printf("Virus size: %d\n",curr->v->length);     
                }
                curr=curr->next;
            }
            curr=virus_list;
        }
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
    virus_file=fopen("signatures","r");
    if (virus_file==NULL) { 
        printf("File not found\n\n");
        exit(0);
    }
    realSize=fread(buffer, 1,buffer_size, virus_file);
    detect_virus(buffer, virus_list_from_input, realSize);
    list_free(virus_list_from_input);
    fclose(virus_file);

    return 0;
}
