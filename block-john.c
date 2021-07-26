/*
*  Block Puzzle 
*  Create a program that erase blocks from the puzzle board
*  Author: John de Veyra
*  Date: 11/28/2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERASED '.'
#define EXIT_FAIL 0
#define EXIT_GOOD 1

#define DISPLAY_THIS(...) { printf("" __VA_ARGS__); \
							 return EXIT_FAIL;			   \
						   }

typedef struct Attributes{
    int width;
    int height;
    int min;
    int count;
    int score;
}PUZZLE;

typedef struct Blocks{
	char **in_matrix;	
	char *inBuff;
	int **out_matrix;
	int *outBuff;
	char *list[900];
}BLOCK_MATRIX;

const int dx[] = {+1, 0, -1, 0};
const int dy[] = {0, +1, 0, -1};

/*******
*
*	CreateMatrix(FILE **fptr,PUZZLE **puzzle, BLOCK_MATRIX **block)
*	Creates 2 matrices: main block and guide block
*	stores it in BLOCK_MATRIX struct
*
*******/
int CreateMatrix(FILE *fptr,PUZZLE *puzzle, BLOCK_MATRIX *block)
{
	int i;
	int j;
	char line[62] = {'0'};


	/* Allocates the main block */
	block->in_matrix = malloc(puzzle->height * sizeof(char *));
	if(block->in_matrix == NULL) 		return 0;
	block->inBuff = malloc(puzzle->width * puzzle->height * sizeof(char));
	if(block->inBuff == NULL) 			return 0;

	
	/* Allocates the guide block */
	block->out_matrix = malloc(puzzle->height * sizeof(int *));
	if(block->out_matrix == NULL) 		return 0;
	block->outBuff = malloc(puzzle->width * puzzle->height * sizeof(int));
	if(block->outBuff == NULL) 			return 0;
	

	/* Assign in and out matrix on allocated buff matrices */
	for(i=0;i < puzzle->height;i++){
		block->in_matrix[i] = block->inBuff + ((puzzle->width * sizeof(char)) * i);
		block->out_matrix[i] = block->outBuff + ((puzzle->width * sizeof(char)) * i);
	}

	for(i=0;i<puzzle->height;i++){
		fgets(line,puzzle->width*2 + 2,fptr);
		for(j=0;j<puzzle->width;j++){
			block->in_matrix[i][j]= line[j*2];
			block->out_matrix[i][j] = 0;
		}
	}
	return EXIT_GOOD;
}

/*****
*
*	FindBlocks(int x, int y,PUZZLE *puzzle,BLOCK_MATRIX *block,int label)
*	This functions uses Depth First Search
*	After parsing at end of tree, returns the count of the same elements
*
******/
int FindBlocks(int x, int y,PUZZLE *puzzle,BLOCK_MATRIX *block,int label)
{
	int dir;
	int Nx;
	int Ny;

	/* Bounds checking */
	if(x < 0 || x == puzzle->height || y < 0  || y == puzzle->width) 
		return EXIT_FAIL;
	
	/* Doesn't check if element is already labeled */
	if(block->out_matrix[x][y] != 0) 
		return EXIT_GOOD;

	/* Uses vectors to check neighbor elements */
	for(dir = 0;dir<4; ++dir){
		Nx = x+dx[dir];
  		Ny = y+dy[dir];
		
		/* Vector bounds checking */
		if(Nx == -1 || Ny == -1 || Nx == puzzle->height || Ny == puzzle->width) 
			continue;
		
		/* If same element is found, recursively calls, FindBlocks() to find same cells */
		/* This also labels the element, so that it won't be checked later */
		if(block->in_matrix[x][y] == block->in_matrix[Nx][Ny]){
			if(block->out_matrix[x][y] == 0) {
				
				block->out_matrix[x][y] = label + 1;
				/* adds address to list so that this will be erased later */
				block->list[puzzle->count] = &block->in_matrix[x][y];
				puzzle->count++;
			}
			FindBlocks(Nx,Ny,puzzle, block,label); 
			 			
		}
	}
	/* this returns the number of same elements */
	return puzzle->count;
}
/******
*
*	EraseFast(BLOCK_MATRIX *block,PUZZLE *puzzle)
*	This function erases all values stored in array of pointer
*	This erases all same neighbor elements
*	
*****/
int EraseFast(BLOCK_MATRIX *block,PUZZLE *puzzle)
{
	int x;
	for(x=0;x<puzzle->count;x++){
		puzzle->score = puzzle->score + *block->list[x] - '0';
		*block->list[x] = ERASED;
	}
	return EXIT_GOOD;
}
/******
*
*	EraseAll(PUZZLE *puzzle,BLOCK_MATRIX *block)
*	This function erases all elements if 1 is the minimum block to be erased.
*
*****/
int EraseAll(PUZZLE *puzzle,BLOCK_MATRIX *block)
{
	int i;
	int j;
	for(i=0;i<puzzle->height;i++){
		for(j=0;j<puzzle->width;j++){
			puzzle->score = puzzle->score + block->in_matrix[i][j] - '0';	
			block->in_matrix[i][j] = ERASED;
		}
	}
	return EXIT_GOOD;
}
/******
*
*	GravityBlock(char **matrix_in,int **matrix_out, int width, int height)
*	This function lets elements fall down above erased elements.
*	This function also sets all elements of guide block to '0'
*
********/
void GravityBlock(char **matrix_in,int **matrix_out, int width, int height)
{
	int i;
	int j;
	int s;
	char temp;
	for(j=0;j<width;j++){
		for(i=height-1;i>=1;i--){
			matrix_out[i][j] = 0;
			if(matrix_in[i][j] == ERASED){
				/* While erased is still not found, keep finding*/
				for(s=i-1;s != 0 && matrix_in[s][j] == ERASED;s--) { } 
				/* If erased is found, swap */
				temp = matrix_in[i][j];
				matrix_in[i][j] = matrix_in[s][j];
				matrix_in[s][j] = temp;
			}
		}
		matrix_out[0][j] = 0;
	}
}
/********
*
*	PrintMatrix(BLOCK_MATRIX *block,PUZZLE *puzzle)
*	This function prints the block.
*
******/
void PrintMatrix(BLOCK_MATRIX *block,PUZZLE *puzzle)
{ 
	int i;
	int j;
	for(i=0;i<puzzle->height;i++){
		for(j=0;j<puzzle->width;j++)
			printf("%c ", block->in_matrix[i][j]);
		printf("\n");
	}
}



/********
*
*	FindEraseGravity(PUZZLE *puzzle, BLOCK_MATRIX *block)
*	This function parses all elements in the block
*	Performs finding of connected and erasing of blocks
*	Finally, calls gravity function if there is blocks erased
*
*********/
int FindEraseGravity(PUZZLE *puzzle, BLOCK_MATRIX *block)
{
	int i;
	int j;
	int label 			= 1;
	int BlocksErased 	= 1;

	if(puzzle->min == 1)	return EraseAll(puzzle,block);
	

	while(BlocksErased != 0){		
		BlocksErased = 0;
		/* Use Depth First Search in checking for branches */
		for(i=0;i<puzzle->height;++i){
			for(j=0;j<puzzle->width;++j){
				if(block->in_matrix[i][j] != ERASED){
					if(FindBlocks(i,j,puzzle,block,label++) >= puzzle->min)
						BlocksErased += EraseFast(block,puzzle);
					puzzle->count = 0;
				}					
			}
		}
		/* If there are blocks erased, call gravity function */
		if(BlocksErased > 0) 
			GravityBlock(block->in_matrix,block->out_matrix, puzzle->width,puzzle->height);
	}
	return EXIT_GOOD;
}

/******
*
*	void FreeMemory(BLOCK_MATRIX *block)
*	Frees all allocated memory of blocks
*
********/
void FreeMemory(BLOCK_MATRIX *block)
{
	if(block->inBuff)		free(block->inBuff);
	if(block->in_matrix)	free(block->in_matrix);
	if(block->outBuff)		free(block->outBuff);
	if(block->out_matrix)	free(block->out_matrix);
}

int main(int argc, char *argv[])
{
	FILE 				*fptr;
	PUZZLE 				puzzle;
	BLOCK_MATRIX 		block;
	
	/* Incorrect Usage */
	if(argc != 2)						DISPLAY_THIS("Error Usage: block-john (filename.txt)");
	/* Opening File */
	if (!(fptr = fopen(argv[1],"r")))	DISPLAY_THIS("Error: File cannot be opened!\n");	
		
	/* Loop reads file until 0 0 0 attribute is read */
	while(1){
		/* Check correct header */
		if(3 != fscanf(fptr,"%d %d %d\n", &puzzle.width,&puzzle.height,&puzzle.min))
			DISPLAY_THIS("Error: Incorrect header!");	
		/* checks 0 0 0 end of puzzle data */
		if(puzzle.width == 0 && puzzle.height == 0 && puzzle.min == 0)	break;				
		/* Initialize same neighbor count and score to 0 */
		puzzle.count = 0;
		puzzle.score = 0;
		
		printf("Block size: %d x %d, Minimum blocks: %d\n", puzzle.width,puzzle.height,puzzle.min);
		
		if(!(CreateMatrix(fptr,&puzzle,&block))){
		 	FreeMemory(&block);
			DISPLAY_THIS("Error: Failed to allocated memory!\n");
		}

		/* Prints Original Blocks */
		printf("Original Blocks\n");
		PrintMatrix(&block,&puzzle);

		/* Perform Find Connected, Erase, Gravity Blocks */
		FindEraseGravity(&puzzle,&block);
		
		/* Prints Final Blocks and Score */
		printf("Final Blocks\n");
		PrintMatrix(&block,&puzzle);
		printf("Total point: %d\n\n", puzzle.score);
		/* Frees memory every after solve of block data */
		FreeMemory(&block);
	}
	if(fptr) fclose(fptr);
	DISPLAY_THIS("Program exits succesfully\n");
}