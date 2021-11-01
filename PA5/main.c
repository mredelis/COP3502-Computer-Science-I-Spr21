/*=============================================================================
| COP 3502C Programming Assignment 5
| This program is written by: Edelis Molina
| Trie: Text completion program suggesting the most likely next letter
|=============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "leak_detector_c.h"

#define MAX_SIZE 2000000

struct trie
{
  int freq;     // similar to count
  int sum_freq;
  int cur_max_freq; 
  struct trie *next[26];
};


/* Function Prototypes */
struct trie* init();
void insert(struct trie* tree, char word[], int count, int k);
void printAll(struct trie* tree, char cur[]);
void prediction(struct trie* tree, FILE *ofp, char prefix[], int k);
int isEmpty(struct trie* root);
void freeDictionary(struct trie* tree);


/* Master of Ceremonies */
int main(void) {

  // printf("MAX Value for int: %d\n", INT_MAX);

  atexit(report_mem_leak); // Function "report_mem_leak" is called when the program terminates

  int n;  // count number of commands: 1 to insert and 2 for query
  int command, count;
  char word[MAX_SIZE];
  char prefix[MAX_SIZE];

  struct trie *myDictionary = init();

  FILE *ifp = fopen("in.txt", "r");
  FILE *ofp = fopen("out.txt", "w");

  if(ifp != NULL)
  {

    fscanf(ifp, "%d", &n); // number of lines
 
    for(int i = 0; i < n; i++)
    {
      // scan first command (1 or 2) of input file to insert a word or predict next character(s)
      fscanf(ifp, "%d", &command);

      if(command == 1) // insert
      {
        fscanf(ifp, "%s %d", word, &count);

        // Call the insert function
        insert(myDictionary, word, count, 0);
      }

      else if(command == 2) // predict
      {
        fscanf(ifp, "%s", prefix);
        // printf("Prefix..... %s\n", prefix);

        // Call the prediction function
        prediction(myDictionary, ofp, prefix, 0);
      }

      // printf("\n");
    }

    // // Printing all the words!
    // printf("Printing all the words: \n");
    // char myword[MAX_SIZE];
    // myword[0] = '\0';
    // printAll(myDictionary, myword);

  }

  fclose(ifp);
  fclose(ofp);
  freeDictionary(myDictionary);

  return 0;
}


// Creates a node
struct trie* init()
{
  // Create the struct, not a word
  struct trie *myTree = malloc(sizeof(struct trie));
  myTree->freq = 0;
  myTree->sum_freq = 0;
  myTree->cur_max_freq = 0;

  // Set each pointer to NULL
  for(int i = 0; i < 26; i++)
    myTree->next[i] = NULL;

  // Return a pointer to the new root
  return myTree;
}

// Insert function
void insert(struct trie* tree, char word[], int count, int k)
{
  // Down to the end, insert the word
  if(k == strlen(word))
  {
    tree->freq += count;
    tree->sum_freq += count;
    return;
  }

  // Increase sum_freq based on count
  tree->sum_freq += count;

  // See if the next place to go exists, if not, create it.
  int nextIndex = word[k] - 'a';

  if(tree->next[nextIndex] == NULL)
  {
    tree->next[nextIndex] = init();
  }

  // sum_freq of children. Calculate ahead
  int temp_child_sum_freq = tree->next[nextIndex]->sum_freq + count;

  if(tree->cur_max_freq < temp_child_sum_freq)
    tree->cur_max_freq = temp_child_sum_freq;

  insert(tree->next[nextIndex], word, count, k+1);
}


void printAll(struct trie* tree, char cur[])
{
  // Stop
  if(tree == NULL) return;

  // Print this node, if it's a word.
  if(tree->freq)
  {
    printf("%s\n", cur);
    printf("Word %s appears %d times\n", cur, tree->freq);
  }

  // Safer if we store this.
  int len = strlen(cur);

  // Recursively print all words in each subtree, in alpha order.
  for(int i = 0; i < 26; i++)
  {
    cur[len] = (char)('a' + i); // Visit all the 26 children
    cur[len + 1] = '\0';
    printAll(tree->next[i], cur);
  }

}

// Prints unknown word is the query is not a proper prefix (it's not shorter than the word)
// -or- Prints unknown word is the prefix does not appear in the Dictionary
// -or- Prints predicted character(s)
void prediction(struct trie* tree, FILE *ofp, char prefix[], int k)
{
  int idx = 0;
  static char retString[26];

  // Down to the end of the prefix, check the frequency of all of the children 
  // of the current node and match who has the highest freq based on the current node's
  // stored max freq
  if(k == strlen(prefix))
  {
 
    // leaf node. The word entered is not a prefix
    if(isEmpty(tree)) 
    {
      printf("unknown word\n"); // It's not a proper prefix
      fprintf(ofp, "unknown word\n");
      return;
    }

    for(int i = 0; i < 26; i++)
    {
      // if the child exists
      if(tree->next[i] != NULL)
      {
        // printf("i value ......%d\n", i);
        if(tree->cur_max_freq == tree->next[i]->sum_freq)
          retString[idx++] = 'a' + i;
      }

    }

    // Finish traversing the 26 children nodes, add '\0' to the end of the string
    retString[idx] = '\0';
    printf("%s\n", retString);
    fprintf(ofp, "%s\n", retString);

    return;
  }

  // If the next place doesn't exist, word is not present
  int nextIndex = prefix[k] - 'a';
  if(tree->next[nextIndex] == NULL)
  {
    printf("unknown word\n");   // Prefix does not exist
    fprintf(ofp, "unknown word\n");
    return;
  }

  return prediction(tree->next[nextIndex], ofp, prefix, k+1);
}


// Checks if all the 26 children are NULL or not.
int isEmpty(struct trie* root)
{
  for (int i = 0; i < 26; i++)
    if (root->next[i]) // if it is NULL
      return 0;
  return 1;
}


// Frees all the memory pointed to by tree
void freeDictionary(struct trie* tree)
{
  for(int i = 0; i < 26; i++)
  {
    if(tree->next[i] != NULL)
      freeDictionary(tree->next[i]);
  }

  free(tree);
}