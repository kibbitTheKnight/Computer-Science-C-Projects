// Keira Taylor
// 4/5/17
// Boggle Solver written for COP 3502H Assignment in 2010
// 10/6/2015 - Edited for 2015 COP 3502 Fall Boggle Solution
// Framework constructed by Arup Guha and completed by Keira Taylor

/*** Framework for Spring 2017 Program ***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 4
#define NUMDIR 8
// Directions of movement in Boggle.
const int DX[] = {-1,-1,-1,0,0,1,1,1};
const int DY[] = {-1,0,1,-1,1,-1,0,1};

typedef struct trie {
    int isWord;
    struct trie* nextLetter[26];
} trie;


void readboard(char board[][SIZE]);

void solveAll(char board[][SIZE], trie* dictionary);
void solveIt(char prefix[], int used[][SIZE], int curX, int curY, char board[][SIZE], trie* dictionary);
int inbounds(int x, int y);

void insert(trie* root, char word[]);
trie* insertRec(trie* root, char word[], int index);
int isWord(char word[], trie* dictionary);
int isPrefix(char word[], trie* dictionary);
void freetree(trie* root);

int main() {

    FILE* ifp = fopen("dictionary.txt", "r");

    // Set up the dictionary structure.
    trie* dictionary = malloc(sizeof(trie));
    int i;
    for (i=0; i<26; i++)
        dictionary->nextLetter[i] = NULL;
    dictionary->isWord = 0;

    // Read in the number of words.
    int numWords;
    fscanf(ifp, "%d", &numWords);
    for (i=0; i<numWords; i++) {
        char word[100];
        fscanf(ifp, "%s", word);

        // Just insert the words that might count for boggle.
        if (strlen(word) >= 3 && strlen(word) <= 16)
            insert(dictionary, word);
    }
    fclose(ifp);

    // Start processing input cases.
    int numCases;
    scanf("%d", &numCases);

    // Go through each game.
    for (i=1; i<=numCases; i++) {

        printf("Words for Game #%d:\n", i);

        // Read in the board and solve.
        char board[SIZE][SIZE];
        readboard(board);
        solveAll(board, dictionary);
        printf("\n\n");
    }

    // Free this memory.
    freetree(dictionary);
    return 0;
}

// Reads in a single board from the file pointed to by ifp into board.
// The file pointer must be pointing to a SIZE x SIZE board with SIZE
// lines each with exactly SIZE contiguous letters.
void readboard(char board[][SIZE])  {

    char temp[SIZE+1];

    int i, j;

    // Go through each line.
    for (i=0; i<SIZE; i++) {

        scanf("%s", temp);

        // Set up each character.
        for (j=0; j<SIZE; j++)
            board[i][j] = temp[j];
    }

}

/*
    Given a board and dictionary, iterates through the board, feeding the
    letters at every spot into solveIt, which finds all words starting
    with that letter
*/
void solveAll(char board[][SIZE], trie* dictionary) {
    //declare and initialize variables
    int i = 0; int j = 0;

    //iterate through board
    for(i = 0; i < SIZE; i++)
    {
        for(j = 0; j < SIZE; j++)
        {
            //create prefix to store eventual 16 letter long word
            char prefix [SIZE * SIZE + 1];
            //create used to keep track of letters/spots already used
            int used [SIZE][SIZE] = {0};

            //set the first letter in prefix to be the (i,j) spot on the board
            prefix[0] = board[i][j];
            //add in null character
            prefix[1] = '\0';
            //set that spot to used
            used[i][j] = 1;

            //call solve it to find all valid words starting with that letter
            solveIt(prefix, used, i, j, board, dictionary);
        }
    }
}

// Prints out all valid words on the board that start with prefix, where the
// last letter used starts at coordinates (curX, curY). used stores which
// board squares have currently been used and board stores the playing board.
void solveIt(char prefix[], int used[][SIZE], int curX, int curY, char board[][SIZE], trie* dictionary) {
    //recording the length of the word for return cases
    int length = strlen(prefix);

    //return cases
    //if the word does not have a valid prefix, it will not be a valid word
    if(!isPrefix(prefix, dictionary))
    {
        return;
    }
    //if the word is greater than 16 letters long, it is not valid, because
    //it would have to use at least one board spot twice
    if(length > SIZE * SIZE)
    {
        return;
    }

    //prints out word if it is valid
    if(isWord(prefix, dictionary))
    {
        printf("%s\n", prefix);
    }

    //declare and initialize variables
    int dir = 0;
    int x = 0;
    int y = 0;
    int i = 0;

    //go through every direction
    for(dir = 0; dir < NUMDIR; dir++)
    {
        x = curX + DX[dir];
        y = curY + DY[dir];

        //if the location is in bounds and valid to move to
        if(inbounds(x, y) && (!used[x][y]))
        {
            //add new letter to word and add null value
            prefix[length] = board[x][y];
            prefix[length + 1] = '\0';

            //update used
            used[x][y] = 1;

            //recursively call function to find all other words with same
            //starting letters
            solveIt(prefix, used, x, y, board, dictionary);

            //reset used
            used[x][y] = 0;
        }
    }
}

// Returns 1 iff (x,y) are valid indexes to board. Returns 0 otherwise.
int inbounds(int x, int y) {
    return x >= 0 && x < SIZE && y >= 0 && y < SIZE;
}

// Wrapper function to insert word into the dictionary stored at root.
void insert(trie* root, char word[]) {
    root->nextLetter[(int)(word[0]-'a')] = insertRec(root->nextLetter[(int)(word[0]-'a')], word, 1);
}

// Recursive function that is given a pointer to location in the alphatree, the word
// to insert, and the index into that word for the next letter to insert.
trie* insertRec(trie* root, char word[], int index) {

    // The node where we want to insert hasn't been created yet.
    if (root == NULL) {

        trie* newnode = (trie*)malloc(sizeof(trie));
        int i;
        for (i=0; i<26; i++)
            newnode->nextLetter[i] = NULL;

        // We have a word, we can stop.
        if (index == strlen(word)) {
            newnode->isWord = 1;
        }
        else {
            newnode->isWord = 0;
            newnode->nextLetter[(int)(word[index]-'a')] = insertRec(newnode->nextLetter[(int)(word[index]-'a')], word, index+1);
        }

        return newnode;
    }

    // If root isn't NULL
    if (index == strlen(word))
        root->isWord = 1;
    else
        root->nextLetter[(int)(word[index]-'a')] = insertRec(root->nextLetter[(int)(word[index]-'a')], word, index+1);


    return root;
}

// Properly free the memory for our alphatree.
void freetree(trie* root) {

    if (root == NULL) return;
    int i;
    for (i=0; i<26; i++)
        freetree(root->nextLetter[i]);

    free(root);
}

// Returns 1 iff word is stored in dictionary.
int isWord(char word[], trie* dictionary) {

    int i;

    // Go through each letter.
    for (i=0; i<strlen(word); i++) {

        // If the node doesn't exist, it's definitely NOT a word.
        if (dictionary->nextLetter[(int)(word[i]-'a')] == NULL)
            return 0;

        // Go to the next letter.
        dictionary = dictionary->nextLetter[(int)(word[i]-'a')];
    }

    // We're at the end, check to see if it's a word!
    return dictionary->isWord;
}

// Returns 1 iff word is stored in dictionary.
int isPrefix(char word[], trie* dictionary) {

    int i;

    // Go through each letter.
    for (i=0; i<strlen(word); i++) {

        // If the node doesn't exist, it's definitely NOT a word.
        if (dictionary->nextLetter[(int)(word[i]-'a')] == NULL)
            return 0;

        // Go to the next letter.
        dictionary = dictionary->nextLetter[(int)(word[i]-'a')];
    }

    // We're at the end, what we've read through is definitely a prefix!
    return 1;
}
