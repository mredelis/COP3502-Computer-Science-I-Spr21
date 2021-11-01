/*=============================================================================
| COP 3502C Programming Assignment 4
| This program is written by: Edelis Molina
| Binary Search Tree
|=============================================================================*/

#include <stdio.h>
#include <stdlib.h>  
#include <string.h> 
#include "leak_detector_c.h"
  
#define MAXLEN 50 // Max lenght of the strings

/* Global Var Declaration */
FILE *ofp;

typedef struct itemNode
{
  char name[MAXLEN];
  int count;
  struct itemNode *left;
  struct itemNode *right;
}itemNode;

typedef struct treeNameNode
{
  char treeName[MAXLEN];
  struct treeNameNode *left;
  struct treeNameNode *right;
  itemNode *theTree;
}treeNameNode;

/* ==================== Function Prototypes ==================== */
treeNameNode* createtreeNameNode(char *treeName);
treeNameNode* buildNameTree(treeNameNode *root, treeNameNode *temp);
treeNameNode* searchtreeNameNode(treeNameNode *root, char treeName[MAXLEN]);
itemNode* createItemNode(char *itemName, int cnt);
itemNode* buildItemTree(itemNode *root, itemNode *tmp);
void traverse_in_traverse(treeNameNode *root);

// Traverse functions
void inOrder(treeNameNode *root);
void inOrderItem(itemNode *root_Item);

// Query Functions
int search_in_itemTree(itemNode *root, char *itemNameBuffer);
int item_before(itemNode *root, char *itemNameBuffer);
void height_balance(itemNode *root, treeNameNode *treeName_node); 
int height(itemNode *node_ptr);  // Function to aid height_balance
int max(int a, int b);           // Function to aid height_balance
int count(itemNode *root);
itemNode* reduce(itemNode *root, char *itemNameBuffer, int redVal);
void delete_Tree(itemNode *node);

// Delete function for ITEM node. Not recursive
itemNode *delete(itemNode *root, char *itemName);
// Functions to aid delete.
itemNode* findNode(itemNode *current_ptr, char *itemName);
itemNode* parent(itemNode *root, itemNode *node);
int isLeaf(itemNode *node);
int hasOnlyLeftChild(itemNode *node);
int hasOnlyRightChild(itemNode *node);
itemNode* minVal(itemNode *root);

// Delete function for NAME node. Recursive approach
treeNameNode *delRec_NameNode(treeNameNode *root, char *treeName);
int isLeaf_2(treeNameNode *node);
int hasOnlyLeftChild_2(treeNameNode *node);
int hasOnlyRightChild_2(treeNameNode *node);
treeNameNode* minVal_2(treeNameNode *root);

void freeMemory(treeNameNode *root);
void deallocate(itemNode *root);


/* ==================== Master of Ceremonies ================ */
int main(void) {

  int N, I, Q; // First line of input file
  char treeNameBuffer[MAXLEN];
  char itemNameBuffer[MAXLEN];
  char queryBuffer[MAXLEN];

  int itemCnt, treeItemsCnt, itemsBeforeCnt, reduceVal;
  int i;

  treeNameNode *treeName_root = NULL;
  treeNameNode *treeName_node = NULL;
  treeNameNode *temp;

  itemNode *tmp;

  atexit(report_mem_leak);

  FILE *ifp = fopen("in2.txt", "r");
  ofp = fopen("out.txt", "w");

  if(ifp != NULL)
  {
    fscanf(ifp, "%d %d %d", &N, &I, &Q); // Read in first line of in.txt

    // Create and fill Name Tree
    for(i = 0; i < N; i++)
    {
      fscanf(ifp, "%s", treeNameBuffer);
      temp = createtreeNameNode(treeNameBuffer);
      treeName_root = buildNameTree(treeName_root, temp);
    }
    inOrder(treeName_root); // Print NAME tree in order
    printf("\n"); fprintf(ofp, "\n");

    // Create and fill Item Tree
    for(i = 0; i < I; i++)
    {
      fscanf(ifp, "%s %s %d", treeNameBuffer, itemNameBuffer, &itemCnt);

      tmp = createItemNode(itemNameBuffer, itemCnt);
  
      treeName_node = searchtreeNameNode(treeName_root, treeNameBuffer);
      
      // treeName_node->theTree is the ROOT of the ITEM tree
      treeName_node->theTree = buildItemTree(treeName_node->theTree, tmp);  
    }

    // For testing
    traverse_in_traverse(treeName_root);

    // Process Queries
    for(i = 0; i < Q; i++)
    {
      fscanf(ifp, "%s", queryBuffer);

      // Search Query
      if(!strcmp(queryBuffer, "search"))
      {
        fscanf(ifp, "%s %s", treeNameBuffer, itemNameBuffer);

        treeName_node = searchtreeNameNode(treeName_root, treeNameBuffer);

        if(treeName_node == NULL)
        {
          printf("%s does not exist\n", treeNameBuffer);
          fprintf(ofp, "%s does not exist\n", treeNameBuffer);
        }
        else
        {
          itemCnt = search_in_itemTree(treeName_node->theTree, itemNameBuffer);

          if(itemCnt == 0)
          {
            printf("%s not found in %s\n", itemNameBuffer, treeNameBuffer);
            fprintf(ofp, "%s not found in %s\n", itemNameBuffer, treeNameBuffer);
          }
          else
          {
            printf("%d %s found in %s\n", itemCnt, itemNameBuffer, treeNameBuffer);
            fprintf(ofp, "%d %s found in %s\n", itemCnt, itemNameBuffer, treeNameBuffer);
          }
        }
      }

      // Item_before Query. Counts # of item before a given item name 
      if(!strcmp(queryBuffer, "item_before"))
      {
        fscanf(ifp, "%s %s", treeNameBuffer, itemNameBuffer);

        treeName_node = searchtreeNameNode(treeName_root, treeNameBuffer);

        if(treeName_node == NULL) printf("%s node in NAME tree does not exist\n", treeNameBuffer);

        if(search_in_itemTree(treeName_node->theTree, itemNameBuffer) == 0)
          printf("%s ITEM node not found in %s NAME tree\n", itemNameBuffer, treeNameBuffer);
        
        else
        {
          itemsBeforeCnt = item_before(treeName_node->theTree, itemNameBuffer);
          printf("item before %s: %d\n", itemNameBuffer, itemsBeforeCnt);
          fprintf(ofp, "item before %s: %d\n", itemNameBuffer, itemsBeforeCnt);
        }  
      }

      if(!strcmp(queryBuffer, "height_balance"))
      {
        fscanf(ifp, "%s", treeNameBuffer);
        treeName_node = searchtreeNameNode(treeName_root, treeNameBuffer);

        if(treeName_node == NULL) printf("%s node in NAME tree does not exist\n", treeNameBuffer);

        else
          height_balance(treeName_node->theTree, treeName_node);
      }
      
      // Count Query to print total # of item in a tree
      if(!strcmp(queryBuffer, "count"))
      {
        fscanf(ifp, "%s", treeNameBuffer);

        treeName_node = searchtreeNameNode(treeName_root, treeNameBuffer);
        if(treeName_node == NULL) printf("%s does not exist\n", treeNameBuffer);

        else
        {
          treeItemsCnt = count(treeName_node->theTree);
          printf("%s count %d\n", treeNameBuffer, treeItemsCnt);
          fprintf(ofp, "%s count %d\n", treeNameBuffer, treeItemsCnt);
        }
      }

      // Reduce the count of an item in a given tree. If count becomes < 0, delete the item
      if(!strcmp(queryBuffer, "reduce"))
      {
        // Get tree NAME node, ITEM node & val to reduce the ITEM node count
        fscanf(ifp, "%s %s %d", treeNameBuffer, itemNameBuffer, &reduceVal);

        // Didn't find tree NAME node
        treeName_node = searchtreeNameNode(treeName_root, treeNameBuffer);
        if(treeName_node == NULL) printf("REDUCE QUERY: %s does not exist\n", treeNameBuffer);

        // Didn't find the node to delete in the ITEM tree 
        if(!search_in_itemTree(treeName_node->theTree, itemNameBuffer))
          printf("REDUCE QUERY: %s not found in %s\n", itemNameBuffer, treeNameBuffer);

        // Proceed to reduce item count
        else
        {
          treeName_node->theTree = reduce(treeName_node->theTree, itemNameBuffer, reduceVal);
          // new_itemCnt = reduce(treeName_node->theTree, itemNameBuffer, reduceVal);
          printf("%s reduced\n", itemNameBuffer);
          fprintf(ofp, "%s reduced\n", itemNameBuffer);
        }
      }

      // Delete an item from a given tree
      if(!strcmp(queryBuffer, "delete"))
      { 
        // Get tree NAME node and ITEM node to delete from the tree NAME
        fscanf(ifp, "%s %s", treeNameBuffer, itemNameBuffer);

        // Didn't find tree NAME node
        treeName_node = searchtreeNameNode(treeName_root, treeNameBuffer);
        if(treeName_node == NULL) printf("DELETE QUERY: %s does not exist\n", treeNameBuffer);

        // Didn't find the node to delete in the ITEM tree 
        if(!search_in_itemTree(treeName_node->theTree, itemNameBuffer))
          printf("DELETE QUERY: %s not found in %s\n", itemNameBuffer, treeNameBuffer);

        // Proceed to delete the item node
        else
        {
          treeName_node->theTree = delete(treeName_node->theTree, itemNameBuffer);
          printf("%s deleted from %s\n", itemNameBuffer, treeNameBuffer);
          fprintf(ofp, "%s deleted from %s\n", itemNameBuffer, treeNameBuffer);
          // printf("tree Name is pointing to: %s\n", treeName_node->theTree->name);
        }
      }

      if(!strcmp(queryBuffer, "delete_name"))
      {
        fscanf(ifp, "%s", treeNameBuffer);

        // Check if the tree NAME node exits and return a pointer to it
        treeName_node = searchtreeNameNode(treeName_root, treeNameBuffer);
        if(treeName_node == NULL) printf("DELETE_NAME QUERY: %s does not exist\n", treeNameBuffer);

        else
        {
          delete_Tree(treeName_node->theTree);
          treeName_node->theTree = NULL;
          printf("%s deleted\n", treeName_node->treeName);
          fprintf(ofp, "%s deleted\n", treeName_node->treeName);
          treeName_root = delRec_NameNode(treeName_root, treeNameBuffer);
        }
      }

    } // End of for loop for Queries processing 
  } // End of if checking if ifp != NULL

  // For testing after processing all the queries
  // printf("\n\n");
  // inOrder(treeName_root); 
  // printf("\n");
  // traverse_in_traverse(treeName_root);

  freeMemory(treeName_root);

  fclose(ifp);
  fclose(ofp);

  return 0;
}

/* ==================== Functions Implementation ==================== */

// Create and Return a pointer to a treeNameNode
treeNameNode* createtreeNameNode(char *treeName)
{
  // Allocate space for the tree Name node
  treeNameNode *tmp = malloc(sizeof(treeNameNode));

  // Fill in the tree Name Node
  strcpy(tmp->treeName, treeName);
  tmp->left = NULL;
  tmp->right = NULL;
  tmp->theTree = NULL;

  return tmp; // Return a pointer to the created node
}

// Take the root of the tree and insert a treeNameNode in the tree
// Return the root of the treeNameNode
treeNameNode* buildNameTree(treeNameNode *root, treeNameNode *temp)
{
  if(root == NULL)
    return temp;

  // int strcmp(const char *s1, const char *s2);
  // If s2 comes before s1, an integer greater than 0 is returned.

  if(strcmp(root->treeName, temp->treeName) > 0)
  {
    if(root->left == NULL)
      root->left = temp;
    else
      root->left = buildNameTree(root->left, temp);
  }
  //s2 (tree Name node) to be inserted is equal or comes after alphabetically than s1
  else 
  {
    if(root->right == NULL)
      root->right = temp;
    else
      root->right = buildNameTree(root->right, temp);
  }

  return root;
}


// Takes a name string and search this name in the name tree and returns that node
treeNameNode* searchtreeNameNode(treeNameNode *root, char treeName[MAXLEN])
{
  if(root != NULL)
  {
    // Name is at the root
    if(strcmp(root->treeName, treeName) == 0)
      return root;
    
    // Search to the left
    if(strcmp(root->treeName, treeName) > 0)
      return searchtreeNameNode(root->left, treeName);

    // Search to the right
    else
      return searchtreeNameNode(root->right, treeName);
  }
  else
    return NULL;
}


// Create and Return a pointer to an item Node
itemNode* createItemNode(char *itemName, int cnt)
{
  // Allocate space for the item node
  itemNode *tmp = malloc(sizeof(itemNode));

  // Fill in the item Node
  strcpy(tmp->name, itemName);
  tmp->count = cnt;
  tmp->left = NULL;
  tmp->right = NULL;

  return tmp; // Return a pointer to the created node
}


// Takes a pointer to a name Tree node where the ITEM node needs to be inserted
itemNode* buildItemTree(itemNode *root, itemNode *tmp)
{
  if(root == NULL)
    return tmp;

  // Insert to the left. temp->name comes before than root->name
  if(strcmp(root->name, tmp->name) > 0)
  {
    if(root->left == NULL)
      root->left = tmp;
    else
      root->left = buildItemTree(root->left, tmp);
  }
  // Insert to the right. 
  else 
  {
    if(root->right == NULL)
      root->right = tmp;
    else
      root->right = buildItemTree(root->right, tmp);
  }

  return root;

}


// Takes the root of the NAME Tree and prints the data of the name Tree
// and the corresponding ITEM tress
void traverse_in_traverse(treeNameNode *root)
{
  if(root != NULL)
  {
    traverse_in_traverse(root->left);
    printf("===%s===\n", root->treeName); 
    fprintf(ofp, "===%s===\n", root->treeName);
    inOrderItem(root->theTree);
    printf("\n"); fprintf(ofp, "\n"); 
    traverse_in_traverse(root->right);
  }
}

// In order traversal of tree NAME 
void inOrder(treeNameNode *root)
{
  if(root != NULL)
  {
    inOrder(root->left);
    printf("%s ", root->treeName);
    fprintf(ofp, "%s ", root->treeName);
    inOrder(root->right);
  }
}

// In order traversal of TREE item
void inOrderItem(itemNode *root_Item)
{
  if(root_Item != NULL)
  {
    inOrderItem(root_Item->left);
    printf("%s ", root_Item->name);
    fprintf(ofp, "%s ", root_Item->name);
    inOrderItem(root_Item->right);
  }

}

/* ==================== Query Functions ==================== */

// It's guarantee from main() call that the NAME tree exits
// Now this function finds if the item exits in the tree
// Returns 0 if item is NOT found in the tree
// Returns item count if item is found in the tree
int search_in_itemTree(itemNode *root, char *itemNameBuffer)
{
  if(root != NULL)
  {
    // Check if item is at root
    if(strcmp(root->name, itemNameBuffer) == 0)
      return root->count;
    // Search to the left
    if(strcmp(root->name, itemNameBuffer) > 0)
      return search_in_itemTree(root->left, itemNameBuffer);
    // Or....search to the right
    return search_in_itemTree(root->right, itemNameBuffer);
  }
  else
    return 0;
}


// It's guarantee from main() call that the NAME tree exits
// Returns total number of items in a given tree
int count(itemNode *root)
{
  if(root == NULL)
    return 0;
  else
    return root->count + count(root->left) + count(root->right);
}


// Retuns the root of the NAME tree
// If the reduce value makes the ITEM node count <= 0, delete ITEM node
// Ohterwise, reduce item count by redVal 
itemNode *reduce(itemNode *root, char *itemNameBuffer, int redVal)
{
  // root = treeName_node->theTree
  itemNode *red_Node;

  // red_Node is a pointer to the ITEM node to be reduced
  red_Node = findNode(root, itemNameBuffer);

  // New count is less than 0, delete node
  if((red_Node->count - redVal) <= 0) 
  {
    root = delete(root, itemNameBuffer);
    return root;   
  }

  // Reduce ITEM node count
  else
  {
    red_Node->count = red_Node->count - redVal;
    return root;
  }
}


// It's guarantee from main() call that the NAME tree exits and ITEM node exists
// Returns the # if ITEM nodes coming before a given item name (alphabetically)
int item_before(itemNode *root, char *itemNameBuffer)
{
  if(root == NULL)
    return 0; // Base case
  
  // if root->name comes before (alphabetically) than itemNameBuffer, a negative int is returned
  if(strcmp(root->name, itemNameBuffer) < 0)  
    return 1 + item_before(root->left, itemNameBuffer) + item_before(root->right, itemNameBuffer);

  else
   return item_before(root->left, itemNameBuffer) + item_before(root->right, itemNameBuffer);

}


// If the diff between the height of the left subtree and the height of the right subtree
// is more than 1, the tree is imbalanced
void height_balance(itemNode *root, treeNameNode *treeName_node)
{
  int lh; // height of left subtree
  int rh; // height of right subtree

  // If the item tree is empty (with no node)
  if(root == NULL) printf("Item tree for |%s| is empty\n", treeName_node->treeName);

  // Find the height of left and right subtrees
  lh = height(root->left);
  rh = height(root->right);

  printf("%s: left height %d, right height %d, difference %d, ", treeName_node->treeName, lh, rh, abs(lh - rh));
  fprintf(ofp, "%s: left height %d, right height %d, difference %d, ", treeName_node->treeName, lh, rh, abs(lh - rh));
  if(abs(lh - rh) >= 2)
  {
    printf("not balanced\n");
    fprintf(ofp, "not balanced\n");
  }
  else
  {
    printf("balanced\n");
    fprintf(ofp, "balanced\n");
  }
  
}

int height(itemNode *node_ptr)
{
  if(node_ptr == NULL) // per instructions, a tree with no node has height = -1
    return -1;

  // If tree is not empty then height = 1 + max of left height and right heights
  return 1 + max(height(node_ptr->left), height(node_ptr->right));
}

// Max function
int max(int a, int b)
{
  return (a >= b) ? a
                  : b;
}


// It's guarantee from main() call that the NAME tree exits and ITEM node exists. Not recursive
itemNode *delete(itemNode *root, char *itemName)
{
  itemNode *delNode;
  itemNode *prnt;
  itemNode *saveNode;
  itemNode *new_delNode;

  // to swap the content of delNode and minVal to the right of new_delNode
  // when delNode has two children
  char save_name[MAXLEN];
  int save_val;

  delNode = findNode(root, itemName);  // returns an itemNode pointer to the node to delete

  prnt = parent(root, delNode);        // returns the parent of this node.

  //===== CASE 1. When node to be deleted is a leaf node
  if(isLeaf(delNode))
  {
    // deleting the only node in the tree. If parent is NULL and it's a leaf node,
    // then the node to delete is the root of the tree
    if(prnt == NULL)
    {
      free(root); 
      return NULL;
    }

    // Deletes node if it's a left child
    if(strcmp(prnt->name, delNode->name) > 0)
    {
      free(prnt->left);
      prnt->left = NULL;
    }

    // Deletes node if it's a right child
    else
    {
      free(prnt->right);
      prnt->right = NULL;
    }

    return root;

  } // End of if statement when node to delete is a leaf


  //===== CASE 2.1. When node to be deleted has only left child
  if(hasOnlyLeftChild(delNode))
  {
    // Deleting root node
    if(prnt == NULL)
    {
      saveNode = delNode->left;
      free(delNode);
      return saveNode; // new root node of the tree
    }

    // Deleting node if it's a left child
    if(strcmp(prnt->name, delNode->name) > 0)
    {
      saveNode = prnt->left;         // node to be deleted
      prnt->left = prnt->left->left; // introduce parent with grandchild
      free(saveNode);
    }

    // Deleting node if it's a right child
    else
    {
      saveNode = prnt->right;
      prnt->right = prnt->right->left; // introduce parent with grandchild
      free(saveNode);      
    }

    return root;

  } // End of if statement when node to delete has only left child


  //===== CASE 2.2. When node to be deleted has only right child
  if(hasOnlyRightChild(delNode))
  {
    // Deleting root node
    if(prnt == NULL)
    {
      saveNode = delNode->right;
      free(delNode);
      return saveNode;
    }

    // Deleting node if it's a left child
    if(strcmp(prnt->name, delNode->name) > 0)
    {
      saveNode = prnt->left;
      prnt->left = prnt->left->right;  // introduce parent with grandchild
      free(saveNode);
    }

    // Deleting node if it's a right child
    else
    {
      saveNode = prnt->right;
      prnt->right = prnt->right->right; // introduce parent with grandchild
      free(saveNode);
    }

    return root;

  } // End of if statement when node to delete has only right child


  //===== CASE 3. When node to be deleted has two children
  // Find the minVal node to the right of the delNode to delete
  // (actually it will take the place of delNode)
  new_delNode = minVal(delNode->right);
  strcpy(save_name, new_delNode->name);
  save_val = new_delNode->count;

  // the min Val will fall in deleting a leaf node or deleting a node with only right child
  delete(root, new_delNode->name);  

  // Cpy content of saved node (orginal was deleted in the step above) into the node to be deleted
  strcpy(delNode->name, save_name);
  delNode->count = save_val;

  return root;
}

// Receive the root of the ITEM tree and ITEM name to search for (key)
// Return a pointer to the ITEM node containing the key  
itemNode* findNode(itemNode *root, char *itemName)
{
  if(root != NULL)
  {
    // Check if item is at the root
    if(!strcmp(root->name, itemName))
      return root;
    // Search to the left
    if(strcmp(root->name, itemName) > 0)
      return findNode(root->left, itemName);
    // Or....search to the right
    return findNode(root->right, itemName);
  }
  else
    return NULL; // from previous check, the itemName is in the ITEM tree, but return NULL to handle exceptions
}


// Returns the parent of the node pointed to by node in the tree rooted at root.
// If the node is the root of the tree, or the node doesn't exist in the tree, null will be returned.
itemNode* parent(itemNode *root, itemNode *node)
{
  if(root == NULL || root == node) // null cases
    return NULL;
  
  // When root is parent of the node
  if(root ->left == node || root->right == node)
    return root;

  // Search for node's parent in the left side of the tree
  if(strcmp(root->name, node->name) > 0)  // node comes before than root alphabetically
    return parent(root->left, node);

  // Or seach on the right side
  else if(strcmp(root->name, node->name) < 0)
    return parent(root->right, node);

  return NULL; // to handle any exception  
}


// Returns 1 if node is a leaf node, 0 otherwise.
int isLeaf(itemNode *node)
{
  return (node->left == NULL && node->right == NULL);
}

// Returns 1 if node has only a left child, 0 otherwise.
int hasOnlyLeftChild(itemNode *node)
{
  return (node->left != NULL && node->right == NULL);
}

// Returns 1 if node has only a right child, 0 otherwise.
int hasOnlyRightChild(itemNode *node)
{
  return (node->left == NULL && node->right != NULL);
}

// Return a pointer to the node storing the minVal
itemNode* minVal(itemNode *root)
{
  // Root stores the min val, meaning the tree does not have left subtree
  if(root->left == NULL)
    return root;

  // The leftmost node stores the min val
  else
    return minVal(root->left);
}


// Takes the root of the item tree and traverses the tree in post-order to delete each node of the tree
void delete_Tree(itemNode *node)
{
  if(node == NULL)
    return; // This will make treeName_node->theTree = NULL;

  delete_Tree(node->left);
  delete_Tree(node->right);

  free(node);
}


// This function deletes a node from the NAME tree after deleting the entire ITEM tree for this NAME category
treeNameNode *delRec_NameNode(treeNameNode *root, char *treeName)
{

  // Wiht the recursive approach, it's not necessary to find the parent of the node to be deleted
  // Keep calling recursive delete fuction until the node to delete is at the root
  if(root == NULL) return NULL;

  // Base case, found node to delete
  if(!strcmp(root->treeName, treeName))
  {
    // Leaf node.
    if(isLeaf_2(root))
    {
      free(root);
      return NULL;
    }
    // Left child
    else if(hasOnlyLeftChild_2(root))
    {
      treeNameNode *ret = root->left;
      free(root);
      return ret;
    }
    // Right child
    else if(hasOnlyRightChild_2(root))
    {
      treeNameNode *ret = root->right;
      free(root);
      return ret;
    }
    // 2 children
    else
    {
      treeNameNode *new_delNode = minVal_2(root->right);

      char save_name[MAXLEN];
      // save content of node with min val to the right of root. This node will be deleted and its content copied into root
      strcpy(save_name, new_delNode->treeName);  

      root->right = delRec_NameNode(root->right, save_name);

      strcpy(root->treeName, save_name);  // restore content

      return root;
    }
  } // End of base case

  // Go left
  if(strcmp(root->treeName, treeName) > 0)  // treeName comes before root->treeName alphabetically
    root->left = delRec_NameNode(root->left, treeName);

  // Go right
  else 
    root->right = delRec_NameNode(root->right, treeName);

  return root;
}


int isLeaf_2(treeNameNode *node)
{
  return (node->left == NULL && node->right == NULL);
}

int hasOnlyLeftChild_2(treeNameNode *node)
{
  return (node->left != NULL && node->right == NULL);
}

int hasOnlyRightChild_2(treeNameNode *node)
{
  return (node->left == NULL && node->right != NULL);
}

treeNameNode* minVal_2(treeNameNode *root)
{
  if(root->left == NULL)
    return root;
  return minVal_2(root->left);
}


// Free up memory 
void freeMemory(treeNameNode *root)
{
  if(root == NULL)
    return;
  
  // Go to the left most node first and deallocate the ITEM tree pointed to root->theTree
  freeMemory(root->left); 
  freeMemory(root->right);
  deallocate(root->theTree);
  // printf("\n Deleting node NAME: %s\n", root->treeName);
  free(root);
}


void deallocate(itemNode *root)
{
  if(root == NULL)
    return;

  deallocate(root->left);
  deallocate(root->right);
  // printf("\n Deleting node: %s\n", root->name);
  free(root);
}