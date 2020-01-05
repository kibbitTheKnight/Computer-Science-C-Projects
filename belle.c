/*
    Keira Taylor
    March 12, 2017
    This program will simulate file management on a computer
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//constants
#define MAX_FILE 19
#define MAX_COMMANDS 100
#define DIR_LEN 1
#define CREATE 1
#define DELETE 2
#define REVERSE -1
#define Q_NUM_FILES 3
#define Q_NUM_SUBD 4
#define RIGHT 'R'
#define LEFT 'L'
#define ROOT_SIZE 17

//structs

typedef struct filenode{
    int isDir;
    char name[MAX_FILE + 1];
    struct filenode * left;
    struct filenode * right;
    struct filenode * parent;
    int numFiles;
    int numSubDir;
    int totalSize;
}filenode;

//function prototypes
filenode * init();
filenode * create(filenode * root, char * directions, int length, char * fName, int numBytes);
void updateNumDir(filenode * child, int numBytes, int increase);
void updateNumFiles(filenode * child, int numBytes, int increase);
int getNumFiles(filenode * root, char * directions, int length);
int getNumSubDir(filenode * root, char * directions, int length);
int getTotalSize(filenode * root, char * directions, int length);
filenode * traverse(filenode * root, char * directions, int length);

int erase(filenode * toRemove, char direction);

//main function
int main()
{
    //initialize the root
    filenode * root = init();

    //declare and initialize variables
    int numCommands = 0;
    int i = 0;
    int command;
    char directions [MAX_COMMANDS + 1];
    char name [MAX_FILE + 1];
    int numBytes = 0;

    //scan in numCommands
    scanf("%d", &numCommands);

    //loop through each command
    for(i = 0; i < numCommands; i++)
    {
        scanf("%d", &command);
        scanf("%s", directions);

        //perform each command based on its number
        if(command == CREATE)
        {
            //scan in name and numBytes
            scanf("%s", name);
            scanf("%d", &numBytes);
            //create a new node in root
            create(root, directions, strlen(directions), name, numBytes);
        }
        else if(command == DELETE)
        {
            //find the node to delete
            filenode * toRemove = traverse(root, directions, strlen(directions));
            //recursively remove it and all other nodes below it
            erase(toRemove, directions[strlen(directions) - 1]);
        }
        else if(command == Q_NUM_FILES)
        {
            //print the amount of files
            printf("%d\n", getNumFiles(root, directions, strlen(directions)));
        }
        else if(command == Q_NUM_SUBD)
        {
            //print amount of sub directories
            printf("%d\n", getNumSubDir(root, directions, strlen(directions)));
        }
        else
        {
            //print the total size of the file or directory
            printf("%d\n", getTotalSize(root, directions, strlen(directions)));
        }
    }

    //free root
    freeRoot(root);

    return 0;
}

//functions

//initializes root
filenode * init()
{
    //allocates space
    filenode * root = malloc(sizeof(filenode));

    //sets all parameters to default
    root -> isDir = 1;
    root -> left = NULL;
    root -> right = NULL;
    root -> parent = NULL;
    root -> numFiles = 0;
    root -> numSubDir = 0;
    root -> totalSize = ROOT_SIZE;

    //sets the root name to "C"
    strcpy(root->name, "C");

    //returns the root
    return root;
}
//follows the directions to locate the specified node
filenode * traverse(filenode * root, char * directions, int length)
{
    //create variable
    int i = 0;

    //create filenode * to hold the location directions point to
    filenode * temp = root;

    //iterates through the directions
    for(i = 0; i < length; i++)
    {
        if(directions[i] == LEFT)
        {
            temp = temp -> left;
        }
        else
        {
            temp = temp -> right;
        }
    }

    //returns the location based on the directions
    return temp;
}

//creates a directory or file and links it to its parent
//returns the root
filenode * create(filenode * root, char * directions, int length, char * fName, int numBytes)
{
    //creates root if it does not exist
    //should not have to be used in the scope of this program
    //but exists for error-catching
    if(root == NULL)
    {
        filenode * temp = malloc(sizeof(filenode));
        temp -> left = NULL;
        temp -> right = NULL;
        temp -> parent = NULL;
        temp -> numFiles = 0;
        temp -> numSubDir = 0;
        temp -> totalSize = ROOT_SIZE;
        strcpy(temp->name, fName);
        return root;
    }
    //traverse the tree to find the place to put the file
    filenode * tempParent = traverse(root, directions, length - 1);

    //create a new location in the tree
    filenode * temp = malloc(sizeof(filenode));

    //attach to tempParent
    if(directions[length - 1] == LEFT)
    {
        tempParent -> left = temp;
    }
    else
    {
        tempParent -> right = temp;
    }

    //determines if entry is file or directory
    //and updates tempParent accordingly
    if(strlen(fName) == DIR_LEN)
    {
        temp -> isDir = 1;
        updateNumDir(tempParent, numBytes, CREATE);
    }
    else
    {
        temp -> isDir = 0;
        updateNumFiles(tempParent, numBytes, CREATE);
    }

    //set temp's elements to default or the given information
    temp -> left = NULL;
    temp -> right = NULL;
    temp -> parent = tempParent;
    temp -> numFiles = 0;
    temp -> numSubDir = 0;
    temp -> totalSize = numBytes;
    strcpy(temp -> name, fName);

    //return root
    return root;
}

//adds/subtracts one to/from numSubDir of child and all of its parents, going up to the root
//also updates totalSize
//whether the function adds or subtracts is based off of if increase is 1 or -1
//1 adds to numSubDir and totalSize. -1 subtracts
void updateNumDir(filenode * child, int numBytes, int increase)
{
    //if child is null, search is over
    if(child == NULL)
    {
        return;
    }

    //otherwise, add/subtract 1 to/from numSubDir and update the parent
    (child -> numSubDir) += increase;

    //update totalSize
    if(child->isDir)
    {
        (child -> totalSize) += increase * numBytes;
    }

    //update parent
    updateNumDir(child -> parent, numBytes, increase);
}

//adds/subtracts one to/from numFiles of child and all of its parents, going up to the root
//also updates totalSize
//whether the function adds or subtracts is based off of if increase is 1 or -1
//1 adds to numSubDir and totalSize. -1 subtracts
void updateNumFiles(filenode * child, int numBytes, int increase)
{
    if(child == NULL)
    {
        return;
    }
    //otherwise, add/subtract 1 to/from numFiles and update the parent
    (child -> numFiles) += increase;
    //update totalSize
    if(child->isDir)
    {
        (child -> totalSize) += increase * numBytes;
    }
    //update parent
    updateNumFiles(child -> parent, numBytes, increase);
}

//deletes a file/directories from the tree and frees all of the files/directories beneath it
// indicates success, and 0 indicates failure
int erase(filenode * toRemove, char direction)
{
    //if the node doesn't exist, stop
    if(toRemove == NULL)
    {
        return 1;
    }

    //if it is a leaf, it is okay to remove
    if(isLeaf(toRemove))
    {
        //update all filenode parameters
        //all parent nodes of toRemove will no longer count it for numSubDir,
        //numFiles, and totalSize
        if(toRemove -> isDir)
        {
            updateNumDir(toRemove, toRemove -> totalSize, REVERSE);
        }
        else
        {
            updateNumFiles(toRemove, toRemove -> totalSize, REVERSE);
        }

        //make its parent not point to it
        if(direction == LEFT)
        {
            toRemove -> parent -> left = NULL;
        }
        else
        {
            toRemove -> parent -> right = NULL;
        }

        //free the node
        free(toRemove);

        return 1;
    }


    //remove left side
    erase(toRemove->left, LEFT);
    //remove right side
    erase(toRemove->right, RIGHT);

    //remove original node
    //uses isLeaf as error-catching to make sure all below has been deleted
    if(isLeaf(toRemove))
    {
        if(toRemove -> isDir)
        {
            updateNumDir(toRemove, toRemove -> totalSize, REVERSE);
        }
        else
        {
            updateNumFiles(toRemove, toRemove -> totalSize, REVERSE);
        }
        if(direction == LEFT)
            toRemove -> parent -> left = NULL;
        else
            toRemove -> parent -> right = NULL;

        free(toRemove);
    }
    else
    {
        //the last node to delete still has children. That means the function
        //did not execute correctly
        return 0;
    }

    return 1;
}
//frees the root by freeing each side first, then freeing it
//returns 1 for success, 0 for failure
int freeRoot(filenode * root)
{
    //if the root is invalid, the function fails
    if(root == NULL)
    {
        return 0;
    }
    //if left side exists, free it
    if(root -> left != NULL)
    {
        erase(root -> left, LEFT);
    }

    //if right side exists, free it
    if(root -> right != NULL)
    {
        erase(root -> right, RIGHT);
    }

    //free root
    free(root);

    return 1;
}

//returns whether the node is a leaf, where it has no children
//1 for true, 0 for false
int isLeaf(filenode * leaf)
{
    return (leaf -> left == NULL) && (leaf -> right == NULL);
}

//returns the number of files under the directory given by the directions
int getNumFiles(filenode * root, char * directions, int length)
{
    //if the root is the direction
    if(strcmp(directions, "C") == 0)
    {
        return root -> numFiles;
    }

    //if the directions point to a directory
    filenode * node = traverse(root, directions, length);

    return node -> numFiles;
}

//returns the number of subdirectories under the directory given by the directions
int getNumSubDir(filenode * root, char * directions, int length)
{
    //if the root is the direction
    if(strcmp(directions, "C") == 0)
    {
        return root -> numSubDir;
    }

    //if the directions point to a directory
    filenode * node = traverse(root, directions, length);

    return node -> numSubDir;
}

//returns the number of subdirectories in a file or directory given by the directions
int getTotalSize(filenode * root, char * directions, int length)
{
    //if the root is the direction
    if(strcmp(directions, "C") == 0)
    {
        return root -> totalSize;
    }

    //if the directions point to a file directory
    filenode * node = traverse(root, directions, length);

    return node -> totalSize;
}
