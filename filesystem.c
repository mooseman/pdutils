
    

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "errno.h"

#define BLOCK_SIZE 512
#define THE_DISK "./Drive2MB"

int blockOffset = 0;
int logicalOffset = 0;

int blockCount;

typedef struct node {
  int type;
  char * name;
  int firstBlock;
  int blockCount;
  struct node *previous;
  struct node *next;
  struct node *parent;
  struct node *contents;
}  node;

typedef struct block {
  int used;
  struct block * next;
  int nextInFile;
} block;

block * freeList;
node * rootNode;

int readBlock(char * disk, int blockNumber, void *block) {
  FILE * file = fopen(disk, "r");
  fseek(file, 512 * blockNumber + blockOffset, SEEK_SET);
  int irc = fread(block, 1, BLOCK_SIZE, file);
  fclose(file);  
  return irc;  
}

int writeBlock(char * disk, int blockNumber, void *block) {
  FILE * file = fopen(disk, "r+");

  fseek(file, 512 * blockNumber + blockOffset, SEEK_SET);
  int irc = fwrite(block, 1, BLOCK_SIZE, file);
  fclose(file);
  return irc;
}

block * getBlock(int blockNumber) {
  int i = 0;
  
  block * currentBlock = freeList;

  while (i < blockNumber) {
    currentBlock = currentBlock -> next;
    i++;
  }

  return currentBlock;
}

node * getLastNode(node * currentNode) {
  while (currentNode->next != NULL) {
    currentNode = currentNode->next;
  }
  return currentNode;
}

void writeBlockList(char * disk, block * currentBlock) {
  FILE * file = fopen(disk, "r+");

  int i = 0;

  while (i < blockCount) {
    fseek(file, i * BLOCK_SIZE, SEEK_SET);
    fwrite(currentBlock, sizeof(struct block), 1, file);
    currentBlock = currentBlock->next;
    i++;
  }

  fclose(file);
}

void readBlockList(char *disk, block * initialBlock) {
  FILE *file = fopen(disk, "r");

  int i = 0;

  while (i < blockCount) {
    fseek(file, 0, SEEK_SET);
    fread(initialBlock, sizeof(struct block), 1, file);
    initialBlock->next = (block *)malloc(sizeof(struct block));
    initialBlock = initialBlock->next;
    i++;
  }

  fclose(file);
}

void writeLogicalFileSystem(char * disk, node * initialNode) {
  // TODO: Check size limits...
  FILE *file = fopen(disk, "r+");
  fseek(file, logicalOffset, SEEK_SET);
  fwrite(initialNode, sizeof(struct node), 1, file);
  fclose(file);
}

void readLogicalFileSystem(char * disk, node * initialBlock) {
  FILE *file = fopen(disk, "r");
  fseek(file, logicalOffset, SEEK_SET);
  fread(initialBlock, sizeof(struct node), 1, file);
  fclose(file);
}

node * createDirectory(char * name, node * parent) {
  node * directory = malloc(sizeof(struct node));

  // TODO: Check name length.
  // TODO: Check to make sure parent is a directory.
  directory->name = name;
  directory->type = 1;
  directory->parent = parent;

  if (parent == rootNode) {
    node * lastInDirectory = getLastNode(parent);
    lastInDirectory->next = directory;
    directory->previous = lastInDirectory;
  } else if (parent->contents != NULL) {
    node * lastInDirectory = getLastNode(parent->contents);
    lastInDirectory->next = directory;
    directory->previous = lastInDirectory;
  } else {
    parent->contents = directory;
  }
  
  writeLogicalFileSystem(THE_DISK, rootNode);

  return directory;
}

node * writeFileData(char *disk, void * data, char * fileName, node * parent) {
  int blocks = (strlen(data) + BLOCK_SIZE - 1) / BLOCK_SIZE;

  int i = 0;
  int j = 0;

  block * currentBlock = freeList;

  int * blocksToUse = (int*)malloc(sizeof(int)*blocks);
  
  int z = 0;
  while (z < blocks) {
    blocksToUse[z] = -1;
    z++;
  }

  while (i < blocks) {
    printf("Checking free list for block: %d\n", i+1);
    while (currentBlock != NULL) {
      if (currentBlock->used == 0) {
        printf("Found one! It's: %d\n", j);
        blocksToUse[i] = j; 
        j++;
        currentBlock = currentBlock->next;
        break;
      } else {
        j++;
        currentBlock = currentBlock->next;
      }
    }
    
    i++;
  }

  z = 0;
  int success = 1;
  while (z < blocks) {
    if (blocksToUse[z] == -1) {
      success = 0;
      break;
    }
    z++;
  }

  if (success == 0) {
    printf("Not enough free space!\n");
  } else {
    char * original = (char*)data;

    int i = 0;
    while (i < blocks) {
      char * bytes = (char *)malloc(sizeof(char)* BLOCK_SIZE);
      strncpy(bytes, original + i * BLOCK_SIZE, BLOCK_SIZE);

      block * theBlock = getBlock(blocksToUse[i]);
      theBlock->used = 1;
      if (i != blocks -1 )
        theBlock->nextInFile = blocksToUse[i + 1];

      writeBlock(THE_DISK, blocksToUse[i], (void *) bytes);

      free(bytes);

      printf("Using block: %d Next in file: %d\n", blocksToUse[i], theBlock->nextInFile);

      i++;
    }

    node * fileNode = (node *)malloc(sizeof(struct node));
    fileNode->name = fileName;
    fileNode->firstBlock = blocksToUse[0];
    fileNode->blockCount = blocks;
    fileNode->type = 0;
    

    if (parent == NULL) {
      node * lastNode = getLastNode(rootNode);
      lastNode->next = fileNode;
      fileNode->previous = lastNode;  
    } else {
      printf("Directory!\n");
      if (parent->contents == NULL) {
        printf("Contents is null!\n");
        parent->contents = fileNode;
        fileNode->parent = parent;
      } else {
        node * lastNode = getLastNode(parent->contents);
        lastNode->next = fileNode;
        fileNode->previous = lastNode;
      }
    }

    writeLogicalFileSystem(THE_DISK, rootNode);

    return fileNode;
  }

  // TODO: Check size of logical filesystem too before creating the file!

}

void * readFileData(node * fileNode) {
  printf("Reading: %s\n", fileNode->name);

  void * data = malloc(fileNode->blockCount * BLOCK_SIZE);

  block * currentBlock = getBlock(fileNode->firstBlock);
  int currentBlockIndex = fileNode->firstBlock;
  
  int i = 0;

  printf("%d node count\n", fileNode->blockCount);

  while (i < fileNode->blockCount) {
    readBlock(THE_DISK, currentBlockIndex, data + i * BLOCK_SIZE);

    currentBlockIndex = currentBlock->nextInFile;
    currentBlock = currentBlock->next;
    i++;
  }

  printf("Pointer: %s\n", (char*)data);  

  return data;
}

void deleteNode(node * theNode) {

  if (theNode->previous != NULL && theNode->next != NULL) {
    theNode->previous->next = theNode->next;
    theNode->next->previous = theNode->previous;
  } else if (theNode->previous != NULL) {
    theNode->previous->next = NULL;
  } else {
    theNode->parent->contents = NULL;
  }

  if (theNode->type == 1) {

    node * nextInDir = theNode->contents;
    node * prevInDir;

    while (nextInDir != NULL) {

      if (nextInDir->type == 0) {
        deleteNode(nextInDir);  
      } else {
        deleteNode(nextInDir);
      }

      nextInDir = nextInDir->next;
    }
  
  } else {
   
    block * currentBlock = getBlock(theNode->firstBlock);
    block * previousBlock;
    
    while (currentBlock->nextInFile != -1) {
      currentBlock->used = 0;
      previousBlock = currentBlock;
      currentBlock = getBlock(currentBlock->nextInFile);
      previousBlock->nextInFile = -1;
    }
  }

  //free(theNode);

  // TODO: Fix this.
  //writeLogicalFilesystem(THE_DISK, rootNode);

}

void printBlockList(block *block) {
  int i = 0;
  do {
    printf("Block: %d Used: %d\n", i, block->used);
    block = block->next;
    i++;
  } while (block != NULL);
}

void printFileSystem(node *currentNode, int index) {
  
  if (index == 0) {
    printf("\n\nFILESYSTEM LAYOUT\n=================\n");
  }

  if (currentNode == rootNode) { currentNode = rootNode->next; }

  while (currentNode != NULL) {
    
    int i = 0;
    while (i < index) {
      printf("\t");
      i++;
    }

    printf("%s", currentNode-> name);
    
    if (currentNode->type == 1) {
      printf(" [DIR]");
    }

    printf("\n");

    if (currentNode->contents != NULL) {
      printFileSystem(currentNode->contents, index+2);
    }

    currentNode = currentNode->next;
  }
}

int formatDisk() {
  FILE * file = fopen(THE_DISK, "r");
  fseek(file, 0L, SEEK_END);
  int size = ftell(file);
  printf("Size of your disk is: %d\n", size);

  blockCount = size/(BLOCK_SIZE + sizeof(struct block));

  printf("Total blocks: %d\n", blockCount);
  fclose(file);

  // Figure out how to calculate the amount of blocks needed. For now it is simply static.
  int new_size = size - sizeof(struct block) * blockCount;
  printf("New disk size is: %d\n", new_size);

  blockOffset = size - new_size;
  logicalOffset = size - 512;
   
  // Create our block list...
  int i = 1;
  block * currentBlock = (block *)malloc(sizeof(struct block));
  freeList = currentBlock;
  while (i < blockCount) {
    currentBlock->next = (block *)malloc(sizeof(struct block));
    currentBlock->used = 12;
    currentBlock = currentBlock->next;
    currentBlock->nextInFile = -1;
    i++;
  }

  writeBlockList(THE_DISK, freeList);

  rootNode->name = "Root";
  rootNode->type = 1;

  writeLogicalFileSystem(THE_DISK, rootNode);  
}

int main() {

  


  FILE * file = fopen(THE_DISK, "r");
  fseek(file, 0L, SEEK_END);
  int size = ftell(file);
  printf("Size of your disk is: %d\n", size);

  blockCount = size/(BLOCK_SIZE + sizeof(struct block));

  printf("Total blocks: %d\n", blockCount);
  fclose(file);  

  freeList = (block *)malloc(sizeof(struct block));

  readBlockList(THE_DISK, freeList);

  rootNode = (node *)malloc(sizeof(struct node));

  readLogicalFileSystem(THE_DISK, rootNode);
    

  if (freeList->next == NULL) {
    printf("This disk appears to be raw data, formatting disk...\n");
    formatDisk();
  } else {
    printBlockList(freeList);
    printFileSystem(rootNode, 0);
  }



  /*char *test = "Here we go this is a ";
  
  int testint = 1234;
  int *testValue = &testint;

  node * charFile = writeFileData(THE_DISK, test, "test.txt", NULL);
  node * intFile = writeFileData(THE_DISK, testValue, "test.int", NULL);

  node * directory = createDirectory("Text", rootNode);
  node * directoryTwo = createDirectory("AnotherDirectory", directory);

  writeFileData(THE_DISK, test, "test2.txt", directory);
  writeFileData(THE_DISK, test, "test3.txt", directoryTwo);


  //i = 0;
  //while (i < 10000) {
  //  writeFileData(THE_DISK, testValue, "test.int", NULL);
  //  i++;
  //}

  void * readOutput = readFileData(charFile);
  printf("Pointer: %s\n", (char*)readOutput);
  free(readOutput);

  printf("Going to print filesystem!\n");
  printFileSystem(rootNode, 0);*/


  //deleteNode(charFile);
  //deleteNode(directoryTwo);


  return 0;
}


  
