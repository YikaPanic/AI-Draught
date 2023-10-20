/* Program to print and play checker games.
  Signed by: Hoshizora
  Dated:  02/10/2021   

*/
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include <math.h>

/* some #define's from my sample solution ------------------------------------*/
#define BOARD_SIZE          8       // board size
#define ROWS_WITH_PIECES    3       // number of initial rows with pieces
#define ROWS_WITH_PIECES_B  5       // the begin number of initial rows with black pieces
#define CELL_EMPTY          '.'     // empty cell character
#define CELL_BPIECE         'b'     // black piece character
#define CELL_WPIECE         'w'     // white piece character
#define CELL_BTOWER         'B'     // black tower character
#define CELL_WTOWER         'W'     // white tower character
#define COST_PIECE          1       // one piece cost
#define COST_TOWER          3       // one tower cost
#define TREE_DEPTH          30       // minimax tree depth
#define COMP_ACTIONS        10      // number of computed actions
#define NUM_PIECE           12      // number of piece
#define DATA_SIZE           2       // each array with place of piece and type of piece (Piece or Tower)
#define MAX_STEP_LENTH      2       // max distance of one move
#define MAX_MOVE_TYPE       8       // how many types of possible move
#define ERROROFF            0       // stop print error information
#define ERRORON             1       // active print error information
#define PRINTOFF            0       // stop print cost
#define PRINTON             1       // active print cost
#define STRLEN              3       // the size used to store board location information 
#define INPUTLEN            5       // the size of input str

/* one type definition from my sample solution -------------------------------*/
typedef unsigned char board_t[BOARD_SIZE][BOARD_SIZE];
typedef char board_d[BOARD_SIZE][BOARD_SIZE][STRLEN];  // board type
typedef char player_d[NUM_PIECE][DATA_SIZE][STRLEN];  // data of player's piece

typedef struct{
    char from[STRLEN]; 
    char to[STRLEN];
}move_t;

typedef struct node node_t;

struct node{
    player_d white_board;
    player_d black_board;
    int depth;        
    int pre_cost;
    node_t* parent;
    node_t* child[NUM_PIECE*MAX_MOVE_TYPE];
    int num_child; // how many child a node has
    move_t move;
    move_t pre_move;
    int turn;
};



board_d board = {{{"A1\0"},{"B1\0"},{"C1\0"},{"D1\0"},{"E1\0"},{"F1\0"}\
                 ,{"G1\0"},{"H1\0"}},
                 {{"A2\0"},{"B2\0"},{"C2\0"},{"D2\0"},{"E2\0"},{"F2\0"}\
                 ,{"G2\0"},{"H2\0"}},
                 {{"A3\0"},{"B3\0"},{"C3\0"},{"D3\0"},{"E3\0"},{"F3\0"}\
                 ,{"G3\0"},{"H3\0"}},
                 {{"A4\0"},{"B4\0"},{"C4\0"},{"D4\0"},{"E4\0"},{"F4\0"}\
                 ,{"G4\0"},{"H4\0"}},
                 {{"A5\0"},{"B5\0"},{"C5\0"},{"D5\0"},{"E5\0"},{"F5\0"}\
                 ,{"G5\0"},{"H5\0"}},
                 {{"A6\0"},{"B6\0"},{"C6\0"},{"D6\0"},{"E6\0"},{"F6\0"}\
                 ,{"G6\0"},{"H6\0"}},
                 {{"A7\0"},{"B7\0"},{"C7\0"},{"D7\0"},{"E7\0"},{"F7\0"}\
                 ,{"G7\0"},{"H7\0"}},
                 {{"A8\0"},{"B8\0"},{"C8\0"},{"D8\0"},{"E8\0"},{"F8\0"}\
                 ,{"G8\0"},{"H8\0"}}};



/* function build ------------------------------------------------------------*/
void print_board(player_d white_piece, player_d black_piece);
void readin_op(player_d white_piece, player_d black_piece);
int updata_board(char from[STRLEN], char to[STRLEN], player_d white_piece, \
player_d black_piece, int *turn, int error);
int check(int x_from, int y_from, int x_to, int y_to, player_d white_piece, \
player_d black_piece, int *turn, int error);
int board_cost(player_d white_piece, player_d black_piece, int print_cost);
int stage_1(player_d white_piece, player_d black_piece, int* turn, int cost);
void search(node_t* root, int depth);
int find_possible_move(player_d white_board, \
player_d black_board,move_t moves[BOARD_SIZE], int turn, int depth);
int stage1check(char from[STRLEN], char to[STRLEN], player_d white_board, \
player_d black_board, int turn);
node_t* make_new_node(node_t* parent, move_t move);
void free_memory(node_t* root);


/* free all of the memory created by malloc from child to parents */
void free_memory(node_t* root){
    int i;
    if (root==NULL){
        return;
    }
    for (i=0;i<root->num_child;i++){
        free_memory(root->child[i]);        
    }
    free(root);
}


/* for each valid move，create new node and fill struct */
node_t* make_new_node(node_t* parent, move_t moves){
    int i, k;
    player_d white_board_copy;
    player_d black_board_copy;
    // initialize current board by copy the parent's board
    for (k=0;k<NUM_PIECE;k++){ 
        strcpy(white_board_copy[k][0], parent->white_board[k][0]);
        strcpy(white_board_copy[k][1], parent->white_board[k][1]);
        strcpy(black_board_copy[k][0], parent->black_board[k][0]);
        strcpy(black_board_copy[k][1], parent->black_board[k][1]);
    }
    node_t* node = (node_t*)malloc(sizeof(*node));
    node->parent = parent;
    node->num_child = 0;
    strcpy(node->move.from, moves.from);
    strcpy(node->move.to, moves.to);
    node->depth = parent->depth;
    node->turn = parent->turn;
    updata_board(moves.from, moves.to, white_board_copy, black_board_copy\
    , &(node->turn), ERROROFF); // apply the move and update the board
    node->turn += 1;
    for (i=0;i<NUM_PIECE;i++){ // after apply, fill node board
        strcpy(node->white_board[i][0], white_board_copy[i][0]);
        strcpy(node->white_board[i][1], white_board_copy[i][1]);
        strcpy(node->black_board[i][0], black_board_copy[i][0]);
        strcpy(node->black_board[i][1], black_board_copy[i][1]);
    }
    node->pre_cost = board_cost(node->white_board, node->black_board, \
    PRINTOFF);
    return node;
}


/* use white board and black board， copy to new white_board_copy 
and black_board_copy， check if the move is vvalid*/
int stage1check(char from[STRLEN], char to[STRLEN], player_d white_board,\
player_d black_board, int turn){
    int k;
    player_d white_board_copy;
    player_d black_board_copy;
    // initialize current board by copy the parent's board
    for (k=0;k<NUM_PIECE;k++){ 
        strcpy(white_board_copy[k][0], white_board[k][0]);
        strcpy(white_board_copy[k][1], white_board[k][1]);
        strcpy(black_board_copy[k][0], black_board[k][0]);
        strcpy(black_board_copy[k][1], black_board[k][1]);
    }
    if (updata_board(from, to, white_board_copy, black_board_copy, \
    &turn, ERROROFF)==1){ // if the move is valid, return 1
        return 1;
    }
    return 0;
}

/* by calling function: stage1check，check validations of 8 kinds move */
int find_possible_move(player_d white_board, player_d black_board,\
 move_t moves[NUM_PIECE * MAX_MOVE_TYPE], int turn, int depth){
    char from[STRLEN] = "00\0";
    char to[STRLEN] = "00\0";
    int i,j,k;
    int num_move=0;
    for (i=0;i<BOARD_SIZE;i++){
        for (j=0;j<BOARD_SIZE;j++){
            for (k=0;k<NUM_PIECE;k++){
                if (turn % 2 == 0){ // for white side
                    if (strcmp(board[i][j],white_board[k][0])==0){
                        strcpy(from,white_board[k][0]);
                        if (i-1>0 && i-1<8 && j+1>0 && j+1<8){
                            strcpy(to,board[i-1][j+1]); 
                            if (stage1check(from, to, white_board, \
                            black_board, turn)==1){
                                strcpy(moves[num_move].from,from);
                                strcpy(moves[num_move].to,to);
                                num_move+=1;
                            }
                        }
                        if (i-2>=0 && i-2<8 && j+2>=0 && j+2<8){                            
                            strcpy(to,board[i-2][j+2]); 
                            if (stage1check(from, to, white_board, \
                            black_board, turn)==1){
                                strcpy(moves[num_move].from,from);
                                strcpy(moves[num_move].to,to);
                                num_move+=1;
                            }
                        }
                        if (i+1>=0 && i+1<8 && j+1>=0 && j+1<8){   
                            strcpy(to,board[i+1][j+1]); 
                            if (stage1check(from, to, white_board, \
                            black_board, turn)==1){
                                strcpy(moves[num_move].from,from);
                                strcpy(moves[num_move].to,to);
                                num_move+=1;
                            }
                        }
                        if (i+2>=0 && i+2<8 && j+2>=0 && j+2<8){  
                            strcpy(to,board[i+2][j+2]); 
                            if (stage1check(from, to, white_board, \
                            black_board, turn)==1){
                                strcpy(moves[num_move].from,from);
                                strcpy(moves[num_move].to,to);
                                num_move+=1;
                            }
                        }
                        if (i+1>=0 && i+1<8 && j-1>=0 && j-1<8){  
                            strcpy(to,board[i+1][j-1]); 
                            if (stage1check(from, to, white_board, \
                            black_board, turn)==1){
                                strcpy(moves[num_move].from,from);
                                strcpy(moves[num_move].to,to);
                                num_move+=1;
                            }
                        }
                        if (i+2>=0 && i+2<8 && j-2>=0 && j-2<8){  
                            strcpy(to,board[i+2][j-2]);
                            if (stage1check(from, to, white_board, \
                            black_board, turn)==1){
                                strcpy(moves[num_move].from,from);
                                strcpy(moves[num_move].to,to);
                                num_move+=1;
                            }
                        }
                        if (i-1>=0 && i-1<8 && j-1>=0 && j-1<8){  
                            strcpy(to,board[i-1][j-1]); 
                            if (stage1check(from, to, white_board, \
                            black_board, turn)==1){
                                strcpy(moves[num_move].from,from);
                                strcpy(moves[num_move].to,to);
                                num_move+=1;
                            }
                        }
                        if (i-2>=0 && i-2<8 && j-2>=0 && j-2<8){
                            strcpy(to,board[i-2][j-2]); 
                            if (stage1check(from, to, white_board, \
                            black_board, turn)==1){
                               strcpy(moves[num_move].from,from);
                               strcpy(moves[num_move].to,to);
                               num_move+=1;
                            }
                        }
                    }
                }

                else if (turn % 2 != 0){ // for black side
                    if (strcmp(board[i][j],black_board[k][0])==0){
                        strcpy(from,black_board[k][0]);
                        if (i-1>0 && i-1<8 && j+1>0 && j+1<8){
                            strcpy(to,board[i-1][j+1]); 
                            if (stage1check(from, to, white_board, \
                            black_board, turn)==1){
                                strcpy(moves[num_move].from,from);
                                strcpy(moves[num_move].to,to);
                                num_move+=1;
                            }
                        }
                        if (i-2>=0 && i-2<8 && j+2>=0 && j+2<8){                            
                            strcpy(to,board[i-2][j+2]); 
                            if (stage1check(from, to, white_board, \
                            black_board, turn)==1){
                                strcpy(moves[num_move].from,from);
                                strcpy(moves[num_move].to,to);
                                num_move+=1;
                            }
                        }
                        if (i+1>=0 && i+1<8 && j+1>=0 && j+1<8){   
                            strcpy(to,board[i+1][j+1]); 
                            if (stage1check(from, to, white_board, \
                            black_board, turn)==1){
                                strcpy(moves[num_move].from,from);
                                strcpy(moves[num_move].to,to);
                                num_move+=1;
                            }
                        }
                        if (i+2>=0 && i+2<8 && j+2>=0 && j+2<8){  
                            strcpy(to,board[i+2][j+2]); 
                            if (stage1check(from, to, white_board, \
                            black_board, turn)==1){
                                strcpy(moves[num_move].from,from);
                                strcpy(moves[num_move].to,to);
                                num_move+=1;
                            }
                        }
                        if (i+1>=0 && i+1<8 && j-1>=0 && j-1<8){  
                            strcpy(to,board[i+1][j-1]); 
                            if (stage1check(from, to, white_board, \
                            black_board, turn)==1){
                                strcpy(moves[num_move].from,from);
                                strcpy(moves[num_move].to,to);
                                num_move+=1;
                            }
                        }
                        if (i+2>=0 && i+2<8 && j-2>=0 && j-2<8){  
                            strcpy(to,board[i+2][j-2]); 
                            if (stage1check(from, to, white_board, \
                            black_board, turn)==1){
                                strcpy(moves[num_move].from,from);
                                strcpy(moves[num_move].to,to);
                                num_move+=1;
                            }
                        }
                        if (i-1>=0 && i-1<8 && j-1>=0 && j-1<8){  
                            strcpy(to,board[i-1][j-1]); 
                            if (stage1check(from, to, white_board, \
                            black_board, turn)==1){
                                strcpy(moves[num_move].from,from);
                                strcpy(moves[num_move].to,to);
                                num_move+=1;
                            }
                        }
                        if (i-2>=0 && i-2<8 && j-2>=0 && j-2<8){
                            strcpy(to,board[i-2][j-2]); 
                            if (stage1check(from, to, white_board, \
                            black_board, turn)==1){
                               strcpy(moves[num_move].from,from);
                               strcpy(moves[num_move].to,to);
                               num_move+=1;
                            }
                        }
                    }
                }
            }
        }
    }    
    return num_move;
}

/* find all nodes by recursion using minmaxtree */
void search(node_t* root, int depth){
    move_t moves[NUM_PIECE*MAX_MOVE_TYPE];
    int num_move;
    if (depth >= TREE_DEPTH){
        return;
    }
    num_move=find_possible_move(root->white_board, root->black_board,\
     moves, root->turn, depth);
    for (int k=0; k<num_move; k++){
        node_t* new = make_new_node(root, moves[k]);
        root->child[root->num_child] = new;
        root->num_child += 1;
    } 
    // recursion start
    for (int i=0; i<root->num_child; i++){
        search(root->child[i],depth + 1);
    }
    if ((root->turn) % 2 != 0){ // next turn is black, find max
        int curr_cost = INT_MIN;
        for (int i=0; i< root->num_child; i++){
            if (root->child[i]->pre_cost > curr_cost){
                curr_cost = root->child[i]->pre_cost;
                if (depth==0){
                    strcpy(root->move.from, root->child[i]->move.from);
                    strcpy(root->move.to, root->child[i]->move.to);
                }
            }
        }
        root->pre_cost = curr_cost;
    }
    if ((root->turn) % 2 == 0){ // next turn is white, find min
        int curr_cost = INT_MAX;
        for (int i=0; i< root->num_child; i++){
            if (root->child[i]->pre_cost < curr_cost){
                curr_cost = root->child[i]->pre_cost;
                if (depth==0){
                    strcpy(root->move.from, root->child[i]->move.from);
                    strcpy(root->move.to, root->child[i]->move.to);
                }
            }
        }
        root->pre_cost = curr_cost;
    }
}

/* input is "A", stage1 start */
int stage_1(player_d white_piece, player_d black_piece, int* turn, int cost){
    node_t* root = (node_t*)malloc(sizeof(node_t));
    int i;
    // initialize current board by copy the parent's board
    for (i=0;i<NUM_PIECE;i++){ 
        strcpy(root->white_board[i][0], white_piece[i][0]);
        strcpy(root->white_board[i][1], white_piece[i][1]);
        strcpy(root->black_board[i][0], black_piece[i][0]);
        strcpy(root->black_board[i][1], black_piece[i][1]);
    }
    root->parent=NULL;
    root->num_child=0; // minmaxtree root(a) made
    root->pre_cost=cost;
    root->depth=0;
    root->turn=*turn;
    search(root,0); // by using minmaxtree to find best choice of next step
    updata_board(root->move.from, root->move.to, root->white_board, \
    root->black_board, &(root->turn), ERROROFF); // finish search, apply moves
    board_cost(root->white_board, root->black_board, PRINTOFF);
    if (root->pre_cost == INT_MAX){  // cost is INT_MAX, black win
        printf("BLACK WIN!\n");
        return 0;
    }
    if (root->pre_cost == INT_MIN){ // cost is INT_MIN, white win 
        printf("WHITE WIN!\n");
        return 0;
    }
    else{ // if no one win, print next action
        printf("=====================================\n");
        if (root->turn % 2 != 0){
            printf("*** BLACK ACTION #%d: %s-%s\n", root->turn, \
            root->move.from, root->move.to); 
        }
        else if (root->turn % 2 == 0){
            printf("*** WHITE ACTION #%d: %s-%s\n", root->turn, \
            root->move.from, root->move.to);
        }
    }
    board_cost(root->white_board, root->black_board, PRINTON); 
    print_board(root->white_board, root->black_board);
    *turn += 1;    
    for (i=0;i<NUM_PIECE;i++){ // update the piece
        strcpy(white_piece[i][0], root->white_board[i][0]);
        strcpy(white_piece[i][1], root->white_board[i][1]);
        strcpy(black_piece[i][0], root->black_board[i][0]);
        strcpy(black_piece[i][1], root->black_board[i][1]);
    }
    free_memory(root);
    return 1;
}
              
/* input blackboard and white board， calculate COST */
int board_cost(player_d white_piece, player_d black_piece, int print_cost){
    int i;
    int num_w=0, num_W=0, num_b=0, num_B=0; 

    for (i=0;i<NUM_PIECE;i++){
        if (strcmp(white_piece[i][1], "w")==0){
            num_w += 1;
        }
        else if (strcmp(white_piece[i][1], "W")==0){
            num_W += 1;
        }
    }
    for (i=0;i<NUM_PIECE;i++){
        if (strcmp(black_piece[i][1], "b")==0){
            num_b += 1;
        }
        else if (strcmp(black_piece[i][1], "B")==0){
            num_B += 1;
        }
    }
    if (num_w==0 && num_W==0){
        printf("BOARD COST: %d\n", INT_MAX);
        return INT_MAX;
    }
    if (num_b==0 && num_B==0){
        printf("BOARD COST: %d\n", INT_MIN);
        return INT_MIN;
    }
    if (print_cost == 1){
        printf("BOARD COST: %d\n", num_b + 3*num_B - num_w - 3*num_W);
    }
    return num_b + 3*num_B - num_w - 3*num_W;
}

/* input x from y from, x to y to, test validation : valid return1，
invalid return 0 */
int check(int x_from, int y_from, int x_to, int y_to, player_d white_piece,\
player_d black_piece, int* turn, int error){
	char from[STRLEN], to[STRLEN];
	int i,j,k,m,n;
    int a;
	int capture_x, capture_y;
	int captured = 0;
    char captured_piece[STRLEN];
    int has_found = 0;
	strcpy(from, board[y_from][x_from]);
	strcpy(to, board[y_to][x_to]);
    if (*turn % 2 == 0){ //turn white, can't hold black piece/tower
		for (j=0;j<NUM_PIECE;j++){
			if (strcmp(from,black_piece[j][0])==0){
                if (error==1){
                    printf("ERROR: Source cell holds opponent's piece/tower.\n");
                }				
				return 0;
			}
		}
	}
	else if (*turn % 2 != 0){ //turn black, can't hold white piece/tower
		for (k=0;k<NUM_PIECE;k++){
			if (strcmp(from,white_piece[k][0])==0){
                if (error==1){
                    printf("ERROR: Source cell holds opponent's piece/tower.\n");
                }				
				return 0;
			}
		}
	}
    for (a=0;a<NUM_PIECE;a++){
        if (*turn % 2 == 0){
            if (strcmp(from,white_piece[a][0]) == 0){
				has_found = 1;
                if (strcmp(white_piece[a][1],"w")==0){ //piece can't go back
		            if ((y_from-y_to)>0){
                        if (error==1){
                            printf("ERROR: Illegal action.\n");
                        }
			            return 0;
		            }
	            }
            }
        }
        else if (*turn % 2 != 0){
            // printf("%s and %s\n", from, black_piece[a][0]); // debug here
        	if (strcmp(from,black_piece[a][0]) == 0){
				has_found = 1;
                if (strcmp(black_piece[a][1], "b")==0){ //piece can't go back
		            if ((y_from-y_to)<0){
                        if (error==1){
                            printf("ERROR: Illegal action.\n");
                        }			                
			            return 0;
		            }
	            }
            }
        }
    }
    if (has_found == 0){  
        if (error == 1) {
            printf("ERROR: Source cell is empty.\n");
        }		
		return 0;
	}

	for (i=0;i<NUM_PIECE;i++){ //target cell can't be empty
	    if (strcmp(to,white_piece[i][0])==0 || \
        strcmp(to,black_piece[i][0])==0){
            if (error==1){
                printf("ERROR: Target cell is not empty.\n");
            }
            return 0;	    	
        }
	    
    }

    if (abs(x_from-x_to)>MAX_STEP_LENTH || abs(y_from-y_to)>MAX_STEP_LENTH \
    || abs(x_from-x_to) != abs(y_from-y_to)){ //test the move is in range
        if (error==1){
            printf("ERROR: Illegal action.\n");            
        }  
        return 0;      
	} 
    //test the capture is affective
	if (abs(x_from-x_to)==2 && abs(y_from-y_to)==2){ 
		capture_x=(x_from+x_to)/2;
		capture_y=(y_from+y_to)/2;
        strcpy(captured_piece,board[capture_y][capture_x]);
		if (*turn % 2 == 0){
			for (m=0;m<NUM_PIECE;m++){
				if (strcmp(captured_piece,black_piece[m][0])==0){
					captured = 1;
                    strcpy(black_piece[m][0], ".");
                    strcpy(black_piece[m][1], ".");
				}
            }
			if (captured == 0){
                if (error==1){
				    printf("ERROR: Illegal action.\n");				    
                }
                return 0;
			}   		    
        }
		else if (*turn % 2 != 0){
			for (n=0;n<NUM_PIECE;n++){
				if (strcmp(captured_piece,white_piece[n][0])==0){
					captured = 1;
                    strcpy(white_piece[n][0], "."); 
                    strcpy(white_piece[n][1], ".");
				    }
            }
			if (captured == 0){
                if (error==1){
				    printf("ERROR: Illegal action.\n");				    
                }
                return 0;
			}
 
	    }
    }
	return 1;
}

/* get information of from and to ，call check() function to test
 validation，if return 1, it is valid, update the board */
int updata_board(char from[STRLEN], char to[STRLEN], player_d white_piece, \
player_d black_piece, int* turn, int error){
    int a;
    int i,j,m,n;
    int x_from, y_from, x_to, y_to;
    int from_outside = 1;
    int to_outside = 1;
    for (i=0;i<BOARD_SIZE;i++){
        for (j=0;j<BOARD_SIZE;j++){
            if (strcmp(board[i][j],from)==0){
                from_outside = 0;
                x_from = j;
                y_from = i;
            }
        }
    }
    for (m=0;m<BOARD_SIZE;m++){
        for (n=0;n<BOARD_SIZE;n++){
            if (strcmp(board[m][n],to)==0){
                to_outside = 0;
                x_to = n;                            
                y_to = m;
            }
        }
    }
    if (from_outside == 1){ // source can't out of board 
        printf("ERROR: Source cell is outside of the board.\n");
        return 0;
    }
    if (to_outside == 1){ // target cell can't out of board
        printf("ERROR: Target cell is outside of the board.\n");
        return 0;
    }
	if (check(x_from, y_from, x_to, y_to, white_piece, black_piece,\
    turn, error) == 0 ){
		return 0;
	};
    for (a=0;a<NUM_PIECE;a++){
        if (*turn % 2 == 0){   
            if (strcmp(from,white_piece[a][0]) == 0){   
                // paste piece from source to targe                                     
                strcpy(white_piece[a][0],to); 
                // if reach the bottom, change piece to tower
                if (y_to == 7){ 
                    strcpy(white_piece[a][1], "W");
                }
            }
        }
	    else if (*turn % 2 != 0){
            if (strcmp(from,black_piece[a][0]) == 0){
                // paste piece from source to targe 
                strcpy(black_piece[a][0],to); 
                // if reach the bottom, change piece to tower
                if (y_to == 0){ 
                    strcpy(black_piece[a][1], "B");
                }
            }
        }
    }           
    return 1;
}

/* get input， call update_board() ，change board and print move,COST */
void readin_op(player_d white_piece, player_d black_piece){
    char input_txt[INPUTLEN];
    char from[STRLEN]="00\0", to[STRLEN]="00\0";
    int i;
    int turn=1;
    int cost;
    while (scanf("%s", input_txt)==1){
        for (i=0; i<2; i++){
            *(from+i)=input_txt[i];
            *(to+i)=input_txt[i+3];
        } 
        if (strcmp(from,"A") != 0 && strcmp(from,"P") != 0 \
        && strcmp(from,"\n") != 0 ){
            if (updata_board(from, to, white_piece, black_piece, \
            &turn, ERRORON) == 0){
                return;
            }
            printf("=====================================\n");
            if (turn % 2 != 0){
                printf("BLACK ACTION #%d: %s-%s\n", turn, from, to); 
            }
            else if (turn % 2 == 0){
                printf("WHITE ACTION #%d: %s-%s\n", turn, from, to);
            }
            cost=board_cost(white_piece, black_piece, PRINTON);
            print_board(white_piece, black_piece);
            turn += 1;
            if (cost == INT_MIN){
                printf("WHITE WIN!\n");
                return;
            }
            if (cost == INT_MAX){
                printf("BLACK WIN!\n");
                return;
            }
        }
        else if (strcmp(from,"A") == 0){
            if (stage_1(white_piece, black_piece, &turn, cost)==0){
                return;
            }      
        }
        else if (strcmp(from,"P") == 0){
            for (int k=0; k<COMP_ACTIONS; k++){
                if (stage_1(white_piece, black_piece, &turn, cost)==0){
                    return;
                }
            }
        }
    }
    return;
}        

/* get white board and black board and print one the board */
void print_board(player_d white_piece, player_d black_piece){
    int i, j;
    int m;
    int has_piece = 0;
    printf("     A   B   C   D   E   F   G   H\n");
    printf("   +---+---+---+---+---+---+---+---+\n");
    for (i=0;i<BOARD_SIZE;i++){
        printf(" %d |", i+1);
        for (j=0;j<BOARD_SIZE;j++){
            has_piece = 0;
            for (m=0;m<NUM_PIECE;m++){
                if ((strcmp(board[i][j],white_piece[m][0])) == 0){
                    printf(" %s |", white_piece[m][1]);
                    has_piece = 1;
                }
                else if (has_piece  != 1){
                    if ((strcmp(board[i][j],black_piece[m][0])) == 0){
                        printf(" %s |", black_piece[m][1]);
                        has_piece = 1;
                    }
                }
            }
            if (has_piece == 0){
                printf(" . |");
            }  
        }             
        printf("\n");
        printf("   +---+---+---+---+---+---+---+---+\n");    
    }
}
/* main function -------------------------------------------------------------*/
int
main(int argc, char *argv[]) {
    // YOUR IMPLEMENTATION OF STAGES 0-2
    int black_piece_num = 12;
    int white_piece_num = 12;
    player_d white_piece = {{"B1\0","w\0"},{"D1\0","w\0"},{"F1\0","w\0"}\
                            ,{"H1\0","w\0"},
                            {"A2\0","w\0"},{"C2\0","w\0"},{"E2\0","w\0"}\
                            ,{"G2\0","w\0"},
                            {"B3\0","w\0"},{"D3\0","w\0"},{"F3\0","w\0"}\
                            ,{"H3\0","w\0"}}; 
                            
    player_d black_piece = {{"A6\0","b\0"},{"C6\0","b\0"},{"E6\0","b\0"}\
                            ,{"G6\0","b\0"},
                            {"B7\0","b\0"},{"D7\0","b\0"},{"F7\0","b\0"}\
                            ,{"H7\0","b\0"},
                            {"A8\0","b\0"},{"C8\0","b\0"},{"E8\0","b\0"}\
                            ,{"G8\0","b\0"}};   

                            
    printf("BOARD SIZE: %dx%d\n", BOARD_SIZE, BOARD_SIZE);
    printf("#BLACK PIECES: %d\n", black_piece_num);
    printf("#WHITE PIECES: %d\n", white_piece_num);
    print_board(white_piece, black_piece);
    readin_op(white_piece, black_piece);
    return EXIT_SUCCESS;            // exit program with the success code
}
/* algorithms are fun! -------------------------------------------------------*/
/* THE END -------------------------------------------------------------------*/
