/* COP 3502C Assignment 1
This program is written by: Edelis Molina */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "leak_detector_c.h"

#define NAMEMAX 51

typedef struct monster
{
  // int id;  // This member is optional. If you use it, you can generate id. However,there is no input for this member
  char *name;
  char *element;
  int population;
} monster;

typedef struct region
{
  char *name;
  int nmonsters;
  int total_population;
  monster **monsters;
} region;

typedef struct itinerary
{
  int nregions;
  region **regions;
  int captures;
} itinerary;

typedef struct trainer
{
  char *name;
  itinerary *visits;
} trainer;


// Function prototypes
monster* createMonster(char *name, char *element, int population);
monster** readMonster(FILE *infile, int *monsterCount);

region** readRegions(FILE* infile, int *countRegions, monster** monsList, int monsterCount);

trainer* readTrainers(FILE* infile, int *trainerCount, region** regionList, int countRegions);
itinerary *createItinerary(FILE *infile, region **regionList, int countRegions);

void process_inputs(monster** monsList, int monsterCount, region** regionList, 
                    int regionCount, trainer* trainerList, int trainerCount);
                    
void release_memory(monster** monsList, int monsterCount, region** regionList, 
                    int regionCount, trainer* trainerList, int trainerCount);


// Master of Ceremonies
int main(void) {

  atexit(report_mem_leak);

  FILE *ifp = fopen("in.txt", "r");

  int mcount, rcount, trcount;

  if(ifp != NULL)
  {
    monster **monsList = readMonster(ifp, &mcount);
    region **regList = readRegions(ifp, &rcount, monsList, mcount);
    trainer *trainerList = readTrainers(ifp, &trcount, regList, rcount);
    process_inputs(monsList, mcount, regList, rcount, trainerList, trcount);
    release_memory(monsList, mcount, regList, rcount, trainerList, trcount);

    fclose(ifp);

  } 
  else {
    printf("Error opening the input file.");
    exit(-1);    
  }
  return 0;
}   


// Dynamically Allocated a Struct Monster
monster* createMonster(char *name, char *element, int population)
{
  monster *mons = (monster*)malloc(sizeof(monster));

  mons->population = population;

  mons->name = malloc(sizeof(char) * (strlen(name) + 1));
  strcpy(mons->name, name);

  mons->element = malloc(sizeof(char) * (strlen(element) + 1));
  strcpy(mons->element, element);

  return mons;
}


/* This function returns an array of MONSTER pointers where each MONSTER pointer points to a dynamically allocated monster */
monster** readMonster(FILE *infile, int *monsterCount)
{
  char name[NAMEMAX];
  char element[NAMEMAX];
  char buffer[NAMEMAX];
  int population;

  fscanf(infile, "%d%s", monsterCount, buffer); // Also reads the word monster into a buffer and ignores it

  monster **monsPtrArray = (monster**) malloc(sizeof(monster*) * *monsterCount);

  for(int i = 0; i < *monsterCount; i++)
  {
    fscanf(infile, "%s%s%d", name, element, &population);
    monsPtrArray[i] = createMonster(name, element, population);   
  }

  return monsPtrArray;
}



/* The function below returns an array of REGION pointers where each REGION pointer points
   to a dynamically allocated region */
region** readRegions(FILE* infile, int *countRegions, monster** monsList, int monsterCount)
{
  char rname[NAMEMAX];
  char mname[NAMEMAX];
  char buffer[NAMEMAX];
  int temp_population = 0;
  int rmcount;          // number of different monsters in a region
  

  fscanf(infile, "%d%s", countRegions, buffer);
  region **regPtrArray = (region **) malloc(sizeof(region*) * *countRegions);

  for(int i = 0; i < *countRegions; i++)
  {
    regPtrArray[i] = (region *) malloc(sizeof(region));

    fscanf(infile, "%s", rname);
    regPtrArray[i]->name = malloc(sizeof(char) * (strlen(rname)+ 1));
    strcpy(regPtrArray[i]->name, rname);

    fscanf(infile, "%d%s", &rmcount, buffer);
    regPtrArray[i]->nmonsters = rmcount;

    regPtrArray[i]->monsters = (monster **) malloc(sizeof(monster*) * rmcount);

    for(int j = 0; j < regPtrArray[i]->nmonsters; j++)
    {
      fscanf(infile, "%s", mname);
      
      for(int z = 0; z < monsterCount; z++)
      {
        if(strcmp(mname, monsList[z]->name) == 0)
        {
          regPtrArray[i]->monsters[j] = monsList[z];
          temp_population += monsList[z]->population;
        }         
      }
    }

    regPtrArray[i]->total_population = temp_population; 
    temp_population = 0;
  }

  return regPtrArray;
}


/* The function below returns a dynamically allocated array of trainers, filled up with the information from the file, and the trainer’s visits filed points to a dynamically allocated itinerary which is filled based on the passed regionList*/
trainer* readTrainers(FILE* infile, int *trainerCount, region** regionList, int countRegions)
{
  char trname[NAMEMAX];
  char rname[NAMEMAX];
  char buffer[NAMEMAX];  

  fscanf(infile, "%d%s", trainerCount, buffer);
  trainer *trPtrArray = (trainer *)malloc(sizeof(trainer) * *trainerCount);

  for(int i = 0; i < *trainerCount; i++)
  {
    fscanf(infile, "%s", trname);
    trPtrArray[i].name = malloc(sizeof(char) * (strlen(trname) + 1));
    strcpy(trPtrArray[i].name, trname);

    trPtrArray[i].visits = createItinerary(infile, regionList, countRegions);

  }

  return trPtrArray;

}


/* This function creates an dynamically allocated itinerary per each trainer */
itinerary *createItinerary(FILE *infile, region **regionList, int countRegions)
{
  char buffer[NAMEMAX]; 
  char tr_rname[NAMEMAX];

  itinerary *trVisits = (itinerary *)malloc(sizeof(itinerary));

  fscanf(infile, "%d%s", &trVisits->captures, buffer);
  fscanf(infile, "%d%s", &trVisits->nregions, buffer);

  trVisits->regions = (region **)malloc(sizeof(region *) * trVisits->nregions);

  for(int i = 0; i < trVisits->nregions; i++)
  {
    fscanf(infile, "%s", tr_rname);
    for(int j = 0; j < countRegions; j++)
    {
      if(strcmp(tr_rname, regionList[j]->name) == 0)
        trVisits->regions[i] = regionList[j];
    }
  }
  
  return trVisits;
}


/* This function processes all the data and produce the output */
void process_inputs(monster** monsList, int monsterCount, region** regionList, 
                    int regionCount, trainer* trainerList, int trainerCount)
{
  FILE *ofp = fopen("out.txt", "w");
  if(ofp != NULL)
  {
    for(int i = 0; i < trainerCount; i++)
    {
      printf("%s\n", trainerList[i].name);
      fprintf(ofp,"%s\n", trainerList[i].name);
      for(int j = 0; j < trainerList[i].visits->nregions; j++)
      {
        printf("%s\n", trainerList[i].visits->regions[j]->name);
        fprintf(ofp, "%s\n", trainerList[i].visits->regions[j]->name);

        for(int z = 0; z < trainerList[i].visits->regions[j]->nmonsters; z++)
        {
          float monsCaptured = round((float)(trainerList[i].visits->regions[j]->monsters[z]->population / 
                            (float)trainerList[i].visits->regions[j]->total_population) * 
                            (float)trainerList[i].visits->captures);

          if(monsCaptured != 0) // Output example does not include rounded value equal to 0
          {
            printf("%d %s\n", (int)monsCaptured, trainerList[i].visits->regions[j]->monsters[z]->name);
            fprintf(ofp,"%d %s\n", (int)monsCaptured, trainerList[i].visits->regions[j]->monsters[z]->name);
          }  
        }
      }
      printf("\n");
      // The below is a quick fix to pass test case 5 in Mimir. Do not fprint a new line at the end of the program
      if (i < trainerCount - 1)
        fprintf(ofp,"\n");
    }
    fclose(ofp);
  }
  else{
    printf("Output File cannot be opened.");
    exit(-1);
  }  
} 


/* This function frees-up all the memory*/
void release_memory(monster** monsList, int monsterCount, region** regionList, 
                    int regionCount, trainer* trainerList, int trainerCount)
{
  // Freeing array of "monsterCount" pointers
  for(int i = 0; i < monsterCount; i++)
  {
    free(monsList[i]->name);
    free(monsList[i]->element);
    free(monsList[i]);
  }
  free(monsList);

  for(int i = 0; i < regionCount; i++)
  {
    free(regionList[i]->name);
    free(regionList[i]->monsters);
    free(regionList[i]);

  }
  free(regionList);

  for(int i = 0; i < trainerCount; i++)
  {
    free(trainerList[i].name);
    free(trainerList[i].visits->regions);
    free(trainerList[i].visits);
  }
  free(trainerList);
}                    
