/*=============================================================================
| COP 3502C Programming Assignment 3
| This program is written by: Edelis Molina
|=============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "leak_detector_c.h"

/* Global variable declaration. My position */
int iniX;
int iniY;


typedef struct coordinates
{
  int x, y;
  double distance;
}coordinate;


/* Function prototypes */
coordinate* ReadData(FILE *ifp, int nCount);

void sortFunction(coordinate *coordArr, int lenght, int t);
void insertionSort(coordinate *coordArr, int low, int high);
void mergeSort(coordinate *coordArr, int l, int r, int t);
void merge(coordinate *coordArr, int l, int mid, int r);

int compareTo(coordinate *ptrPt1, coordinate *ptrPt2);
void processQueries(coordinate *coordArr, coordinate *queryArr, int nCount, int sCount, FILE *ofp);
int binarySearch(coordinate *coordArr, coordinate queryItem, int lenght);


/* MASTER OF CEREMONIES */
int main(void) {

  int nCount; //number  of monster coordinates
  int sCount; //number of points to search for
  int t;      //threshold  to  determine  using  Merge  Sort  or Insertion Sort.

  coordinate *coordArr;
  coordinate *queryArr;


  atexit(report_mem_leak); // Check for memory leak   

  FILE *ifp = fopen("in3.txt", "r");
  FILE *ofp = fopen("out.txt", "w");

  if(ifp != NULL)
  {
    fscanf(ifp, "%d %d %d %d %d", &iniX, &iniY, &nCount, &sCount, &t);
    
    // Read in monster's coordinates
    coordArr = ReadData(ifp, nCount);
    // Read in query's coordinates
    queryArr = ReadData(ifp, sCount);
    
    // Sort monsters locations based on distance from me
    sortFunction(coordArr, nCount, t);

    for(int i = 0; i < nCount; i++)
    {
      printf("%d %d\n", coordArr[i].x, coordArr[i].y);
      fprintf(ofp, "%d %d\n", coordArr[i].x, coordArr[i].y);
    }

    // Handling of queries
    processQueries(coordArr, queryArr, nCount, sCount, ofp);

    fclose(ifp);
    fclose(ofp);

    free(coordArr);
    free(queryArr);
  }

  else {
    printf("Error opening the input file.");
    exit(-1);    
  }

  return 0;
}


// Reads data from input file and return a pointer to a struct coord array
coordinate* ReadData(FILE *ifp, int size)
{
  int x, y;

  // Create space for "size" struct coordinate elements
  coordinate *coordArr = malloc(sizeof(coordinate) * size);

  for(int i = 0; i < size; i++)
  {
    fscanf(ifp, "%d %d", &x, &y);
    coordArr[i].x = x;
    coordArr[i].y = y;
    coordArr[i].distance = (double)sqrt(pow((x-iniX), 2) + pow((y-iniY), 2));
  }

  return coordArr;
}


/* Wrapper function */
// Call Merge Sort or Insertion Sort depending upon t value
void sortFunction(coordinate *coordArr, int nCount, int t)
{
  int low = 0;
  int high = nCount - 1;

  if(nCount <= t)
    insertionSort(coordArr, low, high);
  else
    mergeSort(coordArr, low, high, t); // slightly modified 
}


/* Insertion Sort Function */
// It's called when the numbers of elements of the array is less than the threshold value, t.
void insertionSort(coordinate *coordArr, int low, int high)
{
  int i, j;
  int n = high - low + 1; // # of elements

  int handDist, handX, handY;

  for(i = low + 1; i <= high; i++)
  {
    coordinate temp;
    temp.distance = coordArr[i].distance;
    temp.x = coordArr[i].x;
    temp.y = coordArr[i].y;

    coordinate *handPtr = &temp;

    for(j = i - 1; j > low-1; j--) // loop to handle values before hand
    {
      //flag =-1 Pt1 < Pt2
      //flag = 0 Pt1 = Pt2 same points
      //flag = 1 Pt1 > Pt2
      int flag = compareTo(&coordArr[j], handPtr); 

      if(flag > 0) // coordArr[j] > coordArr[i]. slide
      {
        coordArr[j + 1].distance = coordArr[j].distance;
        coordArr[j + 1].x = coordArr[j].x;
        coordArr[j + 1].y = coordArr[j].y;
      }
      else
        break;
    }

    coordArr[j + 1].distance = handPtr->distance;
    coordArr[j + 1].x = handPtr->x;
    coordArr[j + 1].y = handPtr->y;
  }

}


/* Merge Sort.*/
// If the subarray had t or fewer elements, proceed with Insertion Sort 
void mergeSort(coordinate *coordArr, int l, int r, int t)
{
  int n; // numbers of elements in the subarray
  n = r - l + 1;


  if(n <= t)
  {
    insertionSort(coordArr, l, r);
  }
  else
  {
    int mid = (l + r)/2;

    // Sort first half
    mergeSort(coordArr, l, mid, t);
    // Sort second half
    mergeSort(coordArr, mid + 1, r, t);

    // printf("\nTesting l= %2d  mid= %2d  r= %2d\n", l, mid, r);
    merge(coordArr, l, mid, r);
  }

}


// Merges two subarrays of coordArr[].
// First subarray is coordArr[l...m]
// Second subarray is coord[m+1...r]
void merge(coordinate *coordArr, int l, int mid, int r)
{
  int i, j, k;
  int n1 = mid - l + 1; // number of elements left subarray
  int n2 = r - mid;     // number of elements right subarray

  /* Create temp subarrays */
  coordinate *L = malloc(sizeof(coordinate) * n1);
  coordinate *R = malloc(sizeof(coordinate) * n2);

  // copy data to temp struct coordinate subarrays L[] and R[]
  for(i = 0; i < n1; i++)
  {
    L[i].x = coordArr[l + i].x;
    L[i].y = coordArr[l + i].y;
    L[i].distance = coordArr[l + i].distance;
  }
  for(j = 0; j < n2; j++)
  {
    R[j].x = coordArr[mid + 1 + j].x;
    R[j].y = coordArr[mid + 1 + j].y;
    R[j].distance = coordArr[mid + 1 + j].distance;
  }

  /* Merge the temp arrays back into arr[l..r]*/
  i = 0; // Initial index of first subarray
  j = 0; // Initial index of second subarray
  k = l; // Initial index of merged subarray

  //flag =-1 Pt1 < Pt2
  //flag = 0 Pt1 = Pt2 same points
  //flag = 1 Pt1 > Pt2

  while(i < n1 && j < n2)
  {
    if((compareTo(&L[i], &R[j])) <= 0)
    {
      coordArr[k].x = L[i].x;
      coordArr[k].y = L[i].y;
      coordArr[k].distance = L[i].distance;
      i++;
    }
    else
    {
      coordArr[k].x = R[j].x;
      coordArr[k].y = R[j].y;
      coordArr[k].distance = R[j].distance;
      j++;
    }

    k++;
  }

  // Copy the remaining elements of L[], if there are any
  while(i < n1)
  {
    coordArr[k].x = L[i].x;
    coordArr[k].y = L[i].y;
    coordArr[k].distance = L[i].distance;
    i++;
    k++;
  }

  // Copy the remaining elements of R[], if there are any
  while(j < n2)
  {
    coordArr[k].x = R[j].x;
    coordArr[k].y = R[j].y;
    coordArr[k].distance = R[j].distance;
    j++;
    k++;
  }

  free(L);
  free(R);   

}


// Receives a sorted coordinate array of monster locations and
// a coordinate array of queries to process
void processQueries(coordinate *coordArr, coordinate *queryArr, int nCount, int sCount, FILE *ofp)
{
  int rank;

  for(int i = 0; i < sCount; i++)
  {
    rank = binarySearch(coordArr, queryArr[i], nCount);
    // (rank == -1) ? printf("%d %d not found\n", queryArr[i].x, queryArr[i].y)
    //              : printf("%d %d found at rank %d\n", queryArr[i].x, queryArr[i].y, rank + 1);
    if(rank == -1)
    {
      printf("%d %d not found\n", queryArr[i].x, queryArr[i].y);
      fprintf(ofp, "%d %d not found\n", queryArr[i].x, queryArr[i].y);
    }
    else
    {
      printf("%d %d found at rank %d\n", queryArr[i].x, queryArr[i].y, rank + 1);
      fprintf(ofp, "%d %d found at rank %d\n", queryArr[i].x, queryArr[i].y, rank + 1);
    }
  }
}


/* Binary Search */
// Returns -1 if item is not found or index of the item in the array
int binarySearch(coordinate *coordArr, coordinate queryItem, int lenght)
{
  int low = 0;
  int high = lenght - 1;
  int mid;

  while(low <= high)
  {
    mid = (low + high)/2;

    //flag =-1 Pt1 < Pt2
    //flag = 0 Pt1 = Pt2 same points
    //flag = 1 Pt1 > Pt2
    
    // check if point is at mid
    if(compareTo(&coordArr[mid], &queryItem) == 0)
      return mid;
    // if item is greater, ignore left half
    if(compareTo(&coordArr[mid], &queryItem) < 0)
      low = mid + 1;
    // if item is smaller, ignore right half
    else
      high = mid - 1;
  }

  // if code reaches here, item is not present
  return -1;
}


// Returns -1 if the point pointed to by ptrPt1 is closer to me than the point pointed to by ptrPt2
// Returns 0 if the two locations pointed to by both pointers are identical locations
// Returns 1 if point pointed to by ptrPt1 is farther from me the point pointed to by ptrPt2
int compareTo(coordinate *ptrPt1, coordinate *ptrPt2)
{
  if(ptrPt1->distance < ptrPt2->distance)
    return -1;
  else if(ptrPt1->distance == ptrPt2->distance)
  {
    if(ptrPt1->x < ptrPt2->x)
      return -1;
    else if(ptrPt1->x > ptrPt2->x)
      return 1;
    else
    {
      if(ptrPt1->y < ptrPt2->y)
        return -1;
      else if(ptrPt1->y > ptrPt2->y)
        return 1;
    }

    // if code reaches here, distance of the points from me is the same
    // and their x and y coordinates are the same, meaning the two locations
    // pointed to by ptrPt1 & ptrPt2 are identical
    return 0;
  }

  return 1;
}