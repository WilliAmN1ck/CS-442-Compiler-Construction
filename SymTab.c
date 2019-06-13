/* Author:      Nick Williams
   Created:     02/01/18
   Resources:   lecture
*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#include "SymTab.h"

/* The symbol table entry structure proper.
*/
/*struct SymEntry {
  struct SymTab * table;
  char * name;
  int attrKind;
  void * attributes;
  struct SymEntry * next;
};*/

/* The symbol table structure proper. The hash table array Contents
   is dynamically allocated according to size
*/
/*struct SymTab {
  struct SymTab * parent;
  char * scopeName;
  int size;
  int count;
  struct SymEntry ** contents;
};*/

//initialize vars to null..? or initalize to the args if they arent null?
struct SymTab *
CreateSymTab(int size, char * scopeName, struct SymTab * parentTable) {
  struct SymTab * table = malloc(sizeof(struct SymTab));
  table->parent = NULL; //find size of parentTable and allocate that space into parentTable
  if (parentTable != NULL) {
    //table->parent = malloc(sizeof(struct SymTab));
    table->parent = parentTable;
      //printf("DEBUG: created table: %s and parent table is: %s\n",scopeName, parentTable->scopeName);
  } else {
    //still allocate space for ability to look up parent table..
    table->parent = NULL;
  }

  //same for scopeName
  table->scopeName = NULL;
  if(scopeName != NULL) {
    table->scopeName = strdup(scopeName);
  }
  table->size = size;
  table->count = 0;
  table->contents = NULL;
  if (size > 0 ) {
    table->contents = malloc(sizeof(struct SymEntry *) * (size+1));
    for(int i = 0; i <= size; i++) {
      table->contents[i] = NULL;
    }
  }
  return table;
}

struct SymTab *
DestroySymTab(struct SymTab *aTable) {
  if (aTable != NULL) { //if it exists, destroy it
    if (aTable->contents != NULL) { //if there are things to delete..

      for(int i = 0 ; i < aTable->size+1; i++) {

        if (aTable->contents[i] == NULL) { //nothing here
          //keep walking through list
        } else {
          struct SymEntry * entry = aTable->contents[i]; //get a reference
          //free allocated space in the entries

          while(entry) {
            if(entry->name != NULL) {
              entry->name = NULL;
              free(entry->name);
            }
            if(entry->attributes != NULL) {
              //entry->attributes = NULL;
              //free(entry->attributes);
            }
            if(entry->table != NULL) {
              //entry->table = NULL;
              //free(entry->table);
            }
            if(entry->next != NULL) { //at least one more entry left...
              struct SymEntry * temp = malloc(sizeof(struct SymEntry));
              temp = entry;
              entry = entry->next;
              temp = NULL;
              free(temp);
            } else { //no other entries after this...done here
              entry = NULL;
              free(entry);
              //return
            }
          }
          if(entry != NULL) {
            entry = NULL;
            free(entry);
          }

        }
        if(aTable->contents[i] != NULL) {
          aTable->contents[i] = NULL;
          free(aTable->contents[i]);
        }
      }//end for
    } //end if
    if (aTable->contents) {
      aTable->contents = NULL;
      free(aTable->contents);
    }
    if (aTable->scopeName) {
      aTable->scopeName = NULL;
      free(aTable->scopeName);
    }
    if(aTable->parent) {
      //aTable->parent = NULL;
      //free(aTable->parent);
    }
    return aTable;
    //else table did not exist
  } else { //aTable did not exist
    //printf("DEBUG: tried destroying a table that did not exist!\n");
    return NULL;
  }
  //printf("DEBUG: table succesfully deleted\n");
  return aTable;
}

void
DestroyEntry(struct SymEntry * entry) {
  while(entry) {
    if(entry->name != NULL) {
      entry->name = NULL;
      free(entry->name);
    }
    if(entry->attributes != NULL) {
      entry->attributes = NULL;
      free(entry->attributes);
    }
    if(entry->table != NULL) {
      entry->table = NULL;
      free(entry->table);
    }
    if(entry->next != NULL) { //at least one more entry left...
      entry = entry->next;
    } else { //no other entries after this...done here
      //return
    }
  }

  return;
}

unsigned int
HashName(int size, const char *name) {
  int length = strlen(name);
  int hash = 0;
  for(int i = 0; i < length; i ++) {
    hash = hash + name[i];
  }
  hash = hash%size;
  return hash;
}

struct SymEntry *
FindHashedName(struct SymTab *aTable, int hashValue, const char *name) {
  struct SymEntry * curEntry = NULL;
  curEntry = aTable->contents[hashValue]; //grab first index of array
  char * curName = NULL;
  while(curEntry != NULL) {
    curName = curEntry->name;
    //start walking down the linked list until at end or find it
    if(strcmp(name, curName) == 0) { //found it
      return curEntry;
    } else { //go to the next entry in the list
      curEntry = curEntry->next;
    }
  } //end of while
  //curEntry will be null if not found
  return curEntry;
}

struct SymEntry *
LookupName(struct SymTab *aTable, const char * name) {
  //printf("DEBUG: in lookupname for %s\n", name);
  if(aTable == NULL|| name  == NULL|| aTable->size == 0) { //did not find
    return NULL;
  }
  //else...
  int hash = HashName(aTable->size, name);
  if (hash > aTable->size) { //hash is greater than count so don't look here
    return LookupName(aTable->parent, name);
  } else {
    struct SymEntry * curEntry= FindHashedName(aTable, hash, name);
    if(curEntry != NULL) {
      return curEntry;
    } else { //did not find in this table, keep looking
      return LookupName(aTable->parent, name);
    }
  } //end of else
}

struct SymEntry *
EnterName(struct SymTab *aTable, const char *name) {
  //check if if fits or can be added to the table
  if(aTable == NULL || name == NULL) {
    return NULL;
  }
  //else
  struct SymEntry * entry = NULL;
  //can add to table
  entry = malloc(sizeof(struct SymEntry));
  entry->name = strdup(name);

  //table, attrkind, attrs, next
  //entry->table = malloc(sizeof(struct SymTab));

  entry->table = aTable;
  entry->attrKind = 0; //will be changed next in setAttr method
  entry->attributes = NULL;
  //hash and find where to put it. then change the links so This
  //entry is the new head of the list
  entry->next = NULL;
  struct SymEntry * nextEntry = NULL;
  int hash = HashName(aTable->size, name);
  if (aTable->contents[hash] == NULL) { //no first entry
    nextEntry = NULL;
  } else {
    nextEntry = aTable->contents[hash];
  }
  aTable->contents[hash] = entry;
  entry->next = nextEntry;
  aTable->count = aTable->count + 1;
  return entry;
}


void
SetAttr(struct SymEntry *anEntry, int kind, void *attributes) {
  //this may need work... could be an array or
  if(anEntry != NULL) {
    anEntry->attributes = malloc(sizeof(attributes));
    anEntry->attributes = attributes;
    anEntry->attrKind = kind;
  }
}

int
GetAttrKind(const struct SymEntry *anEntry) {
  int retVal = 0;
  if(anEntry != NULL) {
    retVal = anEntry->attrKind;
  }
  return retVal;
}

void *
GetAttr(const struct SymEntry *anEntry) {
  struct SymEntry * entry = NULL;
  if(anEntry != NULL) {
    return anEntry->attributes;
  } else {
    //printf("DEBUG: returning null from getAttr\n");
    return entry;
  }
}

const char *
GetName(const struct SymEntry *anEntry) {
  return anEntry->name;
}

struct SymTab *
GetTable(const struct SymEntry *anEntry) {
  return anEntry->table;
}

const char *
GetScopeName(const struct SymTab *aTable) {
  return aTable->scopeName;
}

int GetPathSize(struct SymTab * aTable) {
  int size = 0;
//  printf("getting size of path\n");
  while(aTable != NULL && aTable->scopeName != NULL) {
    size += strlen(aTable->scopeName)+1; //+1 for the >
    //printf("aTable's scopeName is: %s\n",aTable->scopeName);
    aTable = aTable->parent;
  }

  return size;
}

//iterate through all to find size.. then realloc it to the whole size.
char *
GetScopeNamePath(const struct SymTab *aTable) {
  //printf("DEBUG: in get scopename path\n");
  if(aTable == NULL || aTable->scopeName == NULL) { //not a valid table
    //printf("DEBUG: Returning null from getScopeNamePath due to null table\n");
    return NULL;
  }
  int size = strlen(aTable->scopeName) + GetPathSize(aTable->parent)+1;
  //may need to add a +1 to size here toa ccount for null terminator...

  //printf("size of the path is: %d\n",size);
  char * path = malloc(size); //+1 for the > separators and last +1
  //for null terminator
  char * temp = NULL;
  strcpy(path, aTable->scopeName);
  aTable = aTable->parent;
  while(aTable != NULL && aTable->scopeName != NULL) {
    temp = strdup(path);

    path = strcpy(path, aTable->scopeName);

    strcat(path,">");
    strcat(path, temp);
    free(temp);
    if(aTable->parent == NULL || aTable->parent->scopeName == NULL) {
      aTable = NULL; //break
    } else {
      aTable = aTable->parent;
    }
  }
  return path;
}

struct SymTab *
GetParentTable(const struct SymTab *aTable) {
  return aTable->parent;
}

struct SymEntry ** entryArray = NULL;
int entryArraySize = 0;

//clean this up... the if's are basically all pointless...
void ProvisionArray(struct SymTab * aTable, bool includeParents) {
  int reqSize = 0;
  if(aTable == NULL) {
    return;
  } //else...
  if(includeParents) {
    while(aTable != NULL) { //loop
      reqSize += aTable->count;
      aTable = aTable->parent;
    }
  } else {
    reqSize = aTable->count;
  }
  if (entryArraySize < reqSize) { //if the array needs to be reallocated..
    if(entryArray == NULL) {//entryArray is null so need to malloc
      //printf("DEBUG: first time allocating entry array to size %d\n", reqSize);
      entryArray = malloc((reqSize +1) * sizeof(struct SymEntry *));
    } else {
      //printf("DEBUG: reallocating entry array to size: %d\n", reqSize);
      entryArray = realloc(entryArray, (reqSize+1) * sizeof(struct SymEntry *));
    }
  } else { //entryArraySize > reqSize... loop through and null/free those?
    for(int i = reqSize; i < entryArraySize; i++) {//loop through and null the spots not being used
      entryArray[i] = NULL;
    }
    if(entryArray == NULL) { //null so malloc
      //printf("DEBUG: allocated space for entry array after one was created\n");
      entryArray = malloc((reqSize + 1) * sizeof(struct SymEntry *));
    } else { //not null so realloc
      //printf("DEBUG: reallocated space for entry array after one was created\n");
      entryArray = realloc(entryArray, (reqSize + 1) * sizeof(struct SymEntry *));
    }
  }
  //init array
  for(int i = 0; i < reqSize; i++) {
    entryArray[i] = NULL;
  }
  entryArraySize = reqSize;
}

//use the struct ** entryArray here?
//Pretty sure this is the helper to getEntries
void
CollectEntries(struct SymTab * aTable, bool includeParents, entryTestFunc testFunc) {
  // enumerate table collecting SymEntry pointers, if testFunc provided used to
  // select entries, null terminate the array
  if(aTable == NULL || aTable->scopeName == NULL) {
    return;
  }
  //resize the entryArray if needed

  int numEntries = 0;
  struct SymEntry * curEntry = NULL;
  //check i the row
  int numAdded = 0;
  while(aTable != NULL && aTable->scopeName != NULL) { //while the table isnt null or the cmdTable
    int numRows = aTable->size; //for looping through all the rows
    for(int i = 0; i < numRows; i++) { //go through each row
      curEntry = aTable->contents[i];
      while (curEntry != NULL) { //loop through each list of entries
        if (testFunc == NULL) { //testFunc NULL so all entries included
          entryArray[numAdded] = malloc(sizeof(curEntry));
          entryArray[numAdded] = curEntry;
          numAdded++;
        } else {//use the testFunc to see which entries to include
          //need to call entryTestFunc with a SymEntry as an arg
          if(testFunc(curEntry)) { //if the function rtns true, include the entry
            entryArray[numAdded] = malloc(sizeof(curEntry));
            entryArray[numAdded] = curEntry;
            numAdded++;
          }
        }
        if(curEntry->next != NULL) {
          curEntry = curEntry->next;
        } else {
          curEntry = NULL;
        }
      }
    }//end for
    if(aTable->parent != NULL && includeParents) { //need to look at parent table
      aTable = aTable->parent;
    } else { //dont look anymore
      aTable = NULL;
    }
  }
  if(curEntry != NULL) {
    curEntry = NULL;
    free(curEntry);
  }
}

//use the struct ** entryArray here?
struct SymEntry **
GetEntries(struct SymTab * aTable, bool includeParents, entryTestFunc testFunc) {
  if(entryArray != NULL) {
    for(int i = 0; i < entryArraySize; i++){
      entryArray[i] = NULL;
      free(entryArray[i]);
    }
    entryArray = NULL;
    free(entryArray);
    entryArraySize = 0;
  }
  if(aTable == NULL) {
    return entryArray;
  } else {
    ProvisionArray(aTable, includeParents);
    CollectEntries(aTable, includeParents, testFunc);
  }
  if(entryArraySize > 0) {

  }
  //null terminate
  entryArray[entryArraySize] = NULL; //null terminate array
  return entryArray;
}

struct Stats *
Statistics(struct SymTab *aTable) {
  struct Stats * theStats = NULL;
  int numEntries = 0;
  if (aTable != NULL) { //table is not null
    theStats = malloc(sizeof(struct Stats)); //init the stats to 0 first
    theStats->minLen = INT_MAX;
    theStats->maxLen = 0;
    theStats->avgLen = 0;
    theStats->maxLen = 0;
    //check each row in the contents array
    int numRows = aTable->size;
    int totalEntries = 0;
    for (int i = 0; i < numRows; i++) {
      //check rows and accumulate stats
      int curChainLength = 0;
      if(aTable->contents[i] != NULL) {
        //walk down chain in another method... count the length and return it
        curChainLength = CountChainLength(aTable->contents[i]);
        totalEntries += curChainLength;
        //could be another method... (checkStats(symentry and curchainlength))
        if(theStats->minLen > curChainLength) {
          theStats->minLen = curChainLength;
        }
        if (theStats->maxLen < curChainLength) {
          theStats->maxLen = curChainLength;
        }
      } else {
        theStats->minLen = 0;
      }
    }
    theStats->avgLen = totalEntries / numRows;
    theStats->entryCnt = totalEntries;
  } else {//table is null
    //do not need to do anything really...
    return theStats;
  }
  return theStats;
}

int CountChainLength (struct SymEntry * entry) {
  int length = 0; //start length at 0 since method is called at 1st entry
  while(entry != NULL) {
    length++;
    entry = entry->next;
  }
  return length;
}
