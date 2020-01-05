/*
    Keira Taylor
    March 4, 2017
    This program will help a person desperate to escape from a corn maze
    It prints out the amount of steps needed to escape, or -1 if inescapable
*/

//preprocessor directives
#include<stdio.h>
#include<stdlib.h>

//structs
typedef struct location{
    int row;
    int col;
    int stepNum;
    struct location * next;
}location;

typedef struct queue{
    location * front;
    location * back;
    int size;
}queue;

//constants
#define NUM_DIR 4
#define VALID_SPACE 0
#define END 1
#define BLOCKED 2
#define NO_ROUTE -1

const int DX[] = {-1, 0, 0, 1};
const int DY[] = {0, -1, 1, 0};

//globals
int rows;
int cols;

//function prototypes
int** readInput(int * startX, int * startY);
int** initVisited(int startX, int startY);
int addAdjacent(queue * qPtr, location * lPtr, int** maze, int** visited);
int isInBounds(int r, int c);
int isValid(int x, int y, int** maze, int** visited);
int isVisited(int x, int y, int** visited);

void init(queue * ptr);
int enqueue(queue * ptr, int x, int y, location * prev);
int dequeue(queue * ptr);

//main function
int main()
{
    //declare and initialize variables
    int i = 0;
    int j = 0;
    int numCases = 0;
    int startX = 0;
    int startY = 0;
    int isOut = 0;
    rows = 0;
    cols = 0;

    //get numCases
    scanf("%d", &numCases);

    //main loop of program
    for(i = 0; i < numCases; i++)
    {
        //create and initialize queue
        queue * ptr = (queue*)malloc(sizeof(queue));
        init(ptr);

        //scan how big the maze is
        scanf("%d%d", &rows, &cols);
        //create the arrays for maze and visited
        int** maze = readInput(&startX, &startY);
        int** visited = initVisited(startX, startY);

        //create a location struct for the starting position
        //and add it to the queue
        enqueue(ptr, startX, startY, NULL);

        //loop through queue
        while(ptr -> front != NULL)
        {
            //adds all items adjacent to the first location in ptr
            addAdjacent(ptr, ptr->front, maze, visited);

            //checks if the border of the maze was reached
            if(maze[ptr->front->row][ptr->front->col] == 1)
            {
                isOut = 1;
                break;
            }

            //dequeues the first location in ptr
            dequeue(ptr);
        }

        //if the while loop completed before the border was found
        //there was no route out and isOut == 0
        if(isOut)
        {
            //print results
            printf("%d\n", ptr->front->stepNum);
        }
        else
        {
            //print results
            printf("%d\n", NO_ROUTE);
        }

        //free everything
        for(j = 0; j < rows; j++)
        {
            free(maze[j]);
            free(visited[j]);
        }
        free(maze);
        free(visited);
        //free queue and everything inside of it
        while(ptr -> front != NULL)
        {
            dequeue(ptr);
        }
        free(ptr);

        //reset isOut
        isOut = 0;
    }

    //free everything

    return 0;
}
//functions

//Reads in the maze grid and puts it into an integer array
//because integer arrays are much easier to work with
//stores the start in startX and startY
int** readInput(int * startX, int * startY)
{
    //declare and initialize variables
    int i = 0;
    int j = 0;

    //loop through to allocate space and record chars
    char** wMaze = malloc(sizeof(char*)*rows);
    int** iMaze = malloc(sizeof(int*)*rows);
    for (i=0; i<rows; i++)
    {
        wMaze[i] = malloc(sizeof(char)*(cols+1));
        iMaze[i] = malloc(sizeof(int)*(cols));
        scanf("%s", wMaze[i]);
    }

    //go through array and convert chars to ints in new int array
    for(i = 0; i < rows; i++)
    {
        for(j = 0; j < cols; j++)
        {
            if(wMaze[i][j] == '-')
            {
                iMaze[i][j] = 0;
            }
            else if(wMaze[i][j] == '~')
            {
                iMaze[i][j] = 1;
            }
            else if(wMaze[i][j] == 'X')
            {
                iMaze[i][j] = 2;
            }
            else //this is the start location
            {
                *startX = i;
                *startY = j;
                iMaze[i][j] = 3;
            }
        }
    }

    //free char array
    for(i = 0; i < rows; i++)
    {
        free(wMaze[i]);
    }
    free(wMaze);

    //return int array
    return iMaze;
}

//creates the visited array based on the starting point and amount of
//rows and cols. Unvisited cells are -1. The starting spot is 0
//all other numbers represent the amount of steps need to get there
int** initVisited(int startX, int startY)
{
    //creates variables and allocates space
    int i = 0;
    int j = 0;
    int ** visited = malloc(rows * sizeof(int));

    for(i = 0; i < rows; i++)
    {
        visited[i] = malloc(cols * sizeof(int));
    }

    //sets all spots to unvisited (-1)
    for(i = 0; i < rows; i++)
    {
        for(j = 0; j < cols; j++)
        {
            visited[i][j] = -1;
        }
    }

    //sets the starting spot to 0
    visited[startX][startY] = 0;

    return visited;

}

//adds all viable adjacent locations to the queue
int addAdjacent(queue * qPtr, location * lPtr, int** maze, int** visited)
{
    //declare and initialize variables
    int i = 0;
    int r = 0;
    int c = 0;
    int didEnqueue = 0; //keeps track if anything was enqueued

    //loop through all directions
    for(i = 0; i < NUM_DIR; i++)
    {
        r = lPtr->row + DX[i];
        c = lPtr->col + DY[i];

        //if the cell in that direction can be visited, add to the queue
        if(isValid(r, c, maze, visited))
        {
            enqueue(qPtr, r, c, lPtr);
            //place the number of steps at that spot in visited
            visited[r][c] = qPtr->back->stepNum;
            //addAdjacent was successful in adding at least one location
            didEnqueue = 1;
        }
    }
    //returns if any items were added to the queue
    return didEnqueue;
}

//checks to see if the location is valid to add to the queue
//if it is valid, returns 1. Else, 0
int isValid(int r, int c, int** maze, int** visited)
{
    //declare and initialize char
    int cell = 0;

    //checks that the location is in bounds
    if(isInBounds(r, c))
    {
        //assigns the location's spot in the grid to cell
        //printMaze(maze);
        cell = maze[r][c];
    }
    else
    {
        return 0;
    }

    //checks to see if the cell has been visited already
    if(!isVisited(r, c, visited))
    {
        //checks to see if the cell can be walked on
        if(cell != BLOCKED)
        {
            return 1;
        }
    }
    return 0;
}

//checks to see if r and c are within the bounds of maze
int isInBounds(int r, int c)
{
    return r < rows && c < cols && r >= 0 && c >= 0;
}

//checks to see if the location has been visited
int isVisited(int r, int c, int** visited)
{
    if(visited[r][c] == -1)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

//queue-related functions

// Initializes an empty queue
void init(queue * ptr)
{
    ptr -> front = NULL;
    ptr -> back = NULL;
    ptr -> size = 0;
}

//Adds a new location to the queue. If not possible, returns 0
//If successful, returns 1
int enqueue(queue * ptr, int r, int c, location * prev)
{
    //create temporary location
    location * temp;

    //allocate space for temp
    temp = (location*)malloc(sizeof(location));

    //error-catching
    if(temp != NULL)
    {
        //add temp to the back of queue
        temp -> row = r;
        temp -> col = c;
        temp -> next = NULL;
        //the stepNum of temp is exactly 1 more than the location
        //that created it
        if(prev != NULL)
            temp -> stepNum = prev -> stepNum + 1;
        else
            temp -> stepNum = 0;

        //if queue isn't empty, set the last location's next to temp
        if(ptr -> back != NULL)
        {
            ptr -> back -> next = temp;
        }

        //the new end of the queue is temp
        ptr -> back = temp;

        //if temp is the only location in queue, it is also the front
        if(ptr -> front == NULL)
        {
            ptr -> front = temp;
        }

        //the enqueue was successful
        //increase the size by 1
        ptr -> size++;
        return 1;
    }

    //the enqueue was not successful. Return 0
    return 0;
}

//Removes the first location from queue. If unsuccessful (queue is empty)
//returns a 0
int dequeue(queue * ptr)
{
    //create temporary location
    location * temp;

    //check if queue is empty
    if(ptr -> front == NULL)
    {
        return 0;
    }

    //store the current front of queue in temp
    temp = ptr -> front;

    //make new front the next location in the queue
    ptr -> front = ptr -> front -> next;

    //if dequeue makes the queue empty, change back to reflect that
    if(ptr -> front == NULL)
    {
        ptr -> back = NULL;
    }

    //free deleted location
    free(temp);

    //return 1 for success and decrease queue size
    ptr -> size--;
    return 1;
}
