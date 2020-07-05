# Parallelized Binary Search Tree using CPU threads

To run: 
./run.sh inp1.txt (or inp2.txt or inp3.txt)
Please ensure that run.sh has executable permissions

The binary search tree consists of a linked list like structure consisting of nodes with pointer links
The node has the following members:
1) Data value
2) Pointer to left and right nodes
3) Pointer to parent node
4) mutex lock for that node

Each node has a mutex lock that is activated whenever it is to be edited by a particular thread and 
deactivated whenever that node is not edited.

## Insert:
Inserts the data value into the appropriate position of the BST.
If data is less than the root value, the node is inserted recursively into the left subtree.
The root lock is released and the left subtree is locked by the current thread. 

If data is greater than the root value, the node is inserted recursively into the right subtree
The root lock is released and the right subtree is locked by the current thread. 
If data is equal to the root value then the function prevents insertion of duplicate values

## Delete:
The following cases are handled by the delete function:
1) inorder successor exists: The node value is swapped with the inorder successor and the inorder successor node is deleted
2) inorder predecessor exists: The node value is swapped with the inorder predecessor and the inorder predecessor node is deleted

## Search:
If data is less than the root value, the node is searched recursively in the left subtree
If data is greater than the root value, the node is searched recursively in the right subtree
If data is equal to the root value then the function returns true
If none of these are true then the function returns false

## Traverse:
The inorder traversal of the BST is used to demonstrate this.
First the left subtree of the root is processed recursively, followed by the root itself and then the 
right subtree is processed recursively.
