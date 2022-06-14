// Need a quick reminder of forward chaining on linked lists so doing that here
#include <stdlib.h>
#include <stdio.h>



typedef struct node{
    int data;
    struct node* next;
}node;


int main(){
    // got rid of frees as forgot the will get popped off, messed with offsets
    node* node1 = malloc(sizeof(node1));
    node1->data = 1;
    node* node2 = malloc(sizeof(node2));
    node2->data = 2;
    node1->next = node2;
    node* node3 = malloc(sizeof(node3));
    node3->data = 3;
    node2->next = node3;
    node3->next = NULL;
   
    node** tracer = malloc(sizeof(tracer));
    tracer = &node1;
    printf("tracer data: %d\n", (*tracer)->data);
    tracer = &node1->next;
    //free(node1);
    printf("node1 freed\n");
    printf("tracer data: %d\n", (*tracer)->data);
    tracer = &node2->next;
    //free(node2);
    printf("node2 freed\n");
    printf("tracer data: %d\n", (*tracer)->data);
   // free(node3);
    printf("Node3 freed\n");
   // free(tracer);

}