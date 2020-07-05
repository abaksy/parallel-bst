#ifndef BST_PTHREADS_H
#define BST_PTHREADS_H

#include<pthread.h>

typedef struct node{
    int data;
    struct node* left;
    struct node* right;
    struct node* parent;
    pthread_mutex_t lock;
}node_t;

void insert(int val, node_t* root, node_t* parent, int threadIdx);
void search(int val, node_t* root, node_t* parent);
node_t* create_node(int val, node_t* parent);
node_t* del_search(int val, node_t* root, int threadIdx);
node_t* get_inorder_successor(node_t* node);
node_t* get_inorder_predecessor(node_t* node);
int del(int val, node_t* root, int threadIdx);
void traverse(node_t* root);
#endif