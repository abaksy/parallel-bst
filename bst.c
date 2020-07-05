#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include "bst.h"

extern pthread_mutex_t tree_lock;
extern node_t* g_root;
extern int search_retval;

void search(int val, node_t* root, node_t* parent)
{
    if(parent==NULL) //Currently at root
    {
        pthread_mutex_lock(&tree_lock);
        if(g_root == NULL)
        {
            search_retval = 0;
            pthread_mutex_unlock(&tree_lock);
            return;
        }
        pthread_mutex_lock(&(g_root->lock));
        root = g_root;
        pthread_mutex_unlock(&tree_lock);
    }
    if(val < root->data)
    {
        if(root->left == NULL)
        {
            search_retval = 0;
            pthread_mutex_unlock(&root->lock);
            return;
        }
        else
        {
            pthread_mutex_lock(&root->left->lock);
            pthread_mutex_unlock(&root->lock);
            search(val, root->left, root);
        }
    }
    else if(val > root->data)
    {
        if(root->right == NULL)
        {
            pthread_mutex_unlock(&root->lock);
            return;
        }
        else
        {
            pthread_mutex_lock(&root->right->lock);
            pthread_mutex_unlock(&root->lock);
            search(val, root->right, root);
        }
    }
    else
    {
        printf("Found %d\n", val);
        pthread_mutex_unlock(&root->lock);
    }
}

void insert(int val, node_t* root, node_t* parent, int threadIdx)
{
    if(parent==NULL) //Currently at root
    {
        pthread_mutex_lock(&tree_lock);
        if(g_root == NULL)
        {
            g_root = create_node(val, parent);
            pthread_mutex_unlock(&tree_lock);
            return;
        }
        pthread_mutex_lock(&g_root->lock);
        root = g_root;
        pthread_mutex_unlock(&tree_lock);
    }

    if(val < root->data)
    {
        if(root->left == NULL)
        {
            root->left = create_node(val, root);
            pthread_mutex_unlock(&root->lock);
        }
        else
        {
            pthread_mutex_lock(&root->left->lock);
            pthread_mutex_unlock(&root->lock);
            insert(val, root->left, root, threadIdx);
        }
    }
    else if(val > root->data)
    {
        if(root->right == NULL)
        {
            root->right = create_node(val, root);
            pthread_mutex_unlock(&root->lock);
        }
        else
        {
            pthread_mutex_lock(&root->right->lock);
            pthread_mutex_unlock(&root->lock);
            insert(val, root->right, root, threadIdx);
        }
    }
    else
    {
        printf("Duplicates not allowed!\n");
    }
}

node_t* create_node(int val, node_t* parent)
{
    node_t* newnode = (node_t*)malloc(sizeof(node_t));

    newnode->data = val;
    newnode->left = NULL;
    newnode->right = NULL;
    newnode->parent = parent;
    pthread_mutex_init(&newnode->lock, NULL);

    return newnode;
}

node_t* del_search(int val, node_t* root, int threadIdx)
{
    if( val == root->data)
    {
        return root;
    }
    else if(val < root->data)
    {
        if(root->left == NULL)
        {
            pthread_mutex_unlock(&root->lock);
            return NULL;
        }
        else
        {
            pthread_mutex_lock(&root->left->lock);
            if(val == root->left->data)
            {
                return root->left;
            }
            else
            {
                pthread_mutex_unlock(&root->lock);
                return del_search(val, root->left, threadIdx);
            }
        }
    }
    else
    {
        if(root->right == NULL)
        {
            pthread_mutex_unlock(&root->lock);
            return NULL;
        }
        else
        {
            pthread_mutex_lock(&root->right->lock);
            if(val == root->right->data)
            {
                return root->right;
            }
            else
            {
                pthread_mutex_unlock(&root->lock);
                return del_search(val, root->right, threadIdx);
            }
        }
    }
    return NULL;
}

int del(int val, node_t* root, int threadIdx)
{
    //printf("inside delete function deleting %d\n", val);
    node_t *curr, *parent, *i_s, *i_s_parent, *i_p, *i_p_parent;  //i_s == Inorder successor, i_p = Inorder predecessor

    pthread_mutex_lock(&tree_lock);
    if(g_root == NULL)
    {
        pthread_mutex_unlock(&tree_lock);
        return -1;
    }

    root = g_root;
    pthread_mutex_lock(&root->lock);

    if(val == root->data && root->left == NULL && root->right == NULL)  //Deleting the root node
    {
        free(root);
        g_root = NULL;
        pthread_mutex_unlock(&tree_lock);
        return 0;
    }

    curr = del_search(val, root, threadIdx);  //Search tree for node to delete

    if(curr==NULL)
    {
        printf("Could not find node with value %d while deleting\n", val);
        pthread_mutex_unlock(&tree_lock);
        return 0;
    }

    parent = curr->parent;

    if(curr->left == NULL && curr->right == NULL && parent == NULL)  //Only one node in tree
    {
        free(curr);
        g_root = NULL;
        pthread_mutex_unlock(&tree_lock);
        return 0;
    }

    pthread_mutex_unlock(&tree_lock);

    if(curr->left == NULL && curr->right == NULL) //Node to be deleted is a leaf node
    {
        if(curr->data < parent->data) //curr is left child of parent
        {
            free(curr);
            parent->left = NULL;
            pthread_mutex_unlock(&parent->lock);
            return 0;
        }
        else                         //curr is right child of parent
        {
            free(curr);
            parent->right = NULL;
            pthread_mutex_unlock(&parent->lock);
            return 0;
        }
    }

    if(parent != NULL)  //Deleting an internal node (internal ie. node has parent and children)
    {
        pthread_mutex_unlock(&parent->lock);
    }

    if(curr->right != NULL)  //Right branch exists
    {
        pthread_mutex_lock(&curr->right->lock);

        if(curr->right->left == NULL)  //No inorder successor for curr
        {
            i_s = curr->right;

            if(i_s->right != NULL)     
            {
                pthread_mutex_lock(&i_s->right->lock);
                curr->data = i_s->data;
                curr->right = i_s->right;
                i_s->right->parent = curr;
                pthread_mutex_unlock(&i_s->right->lock);
            }
            else
            {
                curr->data = i_s->data;
                curr->right = NULL;
            }
            free(i_s);
            pthread_mutex_unlock(&curr->lock);
            return 0;
        } 

        //Inorder successor exists for curr
        i_s = get_inorder_successor(curr);
        i_s_parent = i_s->parent;

        if(i_s->right != NULL)
        {
            pthread_mutex_lock(&i_s->right->lock);

            i_s_parent->left = i_s->right;
            i_s->right->parent = i_s_parent;
            curr->data = i_s->data;

            pthread_mutex_unlock(&i_s->right->lock);
        }
        else
        {
            curr->data = i_s->data;
            i_s_parent->left = NULL;
        }
        free(i_s);
        pthread_mutex_unlock(&i_s_parent->lock);
        pthread_mutex_unlock(&curr->lock);
        return 0;
    }

    if(curr->left != NULL) //Left branch exists
    {
        pthread_mutex_lock(&curr->left->lock);

        if(curr->left->right == NULL) //No inorder predecessor
        {
            i_p = curr->left;
            if(i_p->left != NULL)
            {
                pthread_mutex_lock(&i_p->left->lock);
                curr->data = i_p->data;
                curr->left = i_p->left;
                i_p->left->parent = curr;
                pthread_mutex_unlock(&i_p->left->lock);
            }
            else
            {
                curr->data = i_p->data;
                curr->left = NULL;
            }
            free(i_p);
            pthread_mutex_unlock(&curr->lock);
            return 0;
        }

        i_p = get_inorder_predecessor(curr);
        i_p_parent = i_p->parent;

        if(i_p->left != NULL)
        {
            pthread_mutex_lock(&i_p->left->lock);
            i_p_parent->right = i_p->left;
            i_p->left->parent = i_p_parent;
            curr->data =i_p->data;
            pthread_mutex_unlock(&i_p->left->lock);
        }
        else
        {
            curr->data = i_p->data;
            i_p_parent->right = NULL;
        }
        free(i_p);
        pthread_mutex_unlock(&i_p_parent->lock);
        pthread_mutex_unlock(&curr->lock);
        return 0;
    }
    return -1;
}

node_t* get_inorder_successor(node_t* node)
{
    node_t *parent, *successor;
    parent = node->right;
    successor = parent->left;

    pthread_mutex_lock(&successor->lock);
    while(successor->left != NULL)
    {
        successor = successor->left;

        pthread_mutex_unlock(&parent->lock);
        pthread_mutex_lock(&successor->lock);

        parent = successor->parent;
    }
    return successor;
}
node_t* get_inorder_predecessor(node_t* node)
{
    node_t *parent, *predecessor;
    parent = node->left;
    predecessor = parent->right;

    pthread_mutex_lock(&predecessor->lock);
    while(predecessor->right != NULL)
    {
        predecessor = predecessor->right;

        pthread_mutex_unlock(&parent->lock);
        pthread_mutex_lock(&predecessor->lock);

        parent = predecessor->parent;
    }
    return predecessor;
}

void traverse(node_t* root)
{
    if(root!=NULL)
    {
        traverse(root->left);
        printf("%d ", root->data);
        /*if(root->parent != NULL)
        {
            printf("child of %d\n", root->parent->data);
        }
        else
        {
            printf("Child of none\n");
        }*/
        traverse(root->right);
    }
}