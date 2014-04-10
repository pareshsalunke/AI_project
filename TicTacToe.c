#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#define String_Length 80

#define True 1
#define False 0

#define Squares 20
typedef char Square_Type;
typedef Square_Type Board_Type[Squares];
#define Empty ' '

#define Maximum_Moves Squares                /* Maximum moves in a game */
#define Infinity Maximum_Moves + 1        /* Higher value than any score */
#define Heuristic_Infinity 10000        /* Higher than any heuristic */

/* Types of search */
#define Normal 1
#define Heuristic_Search 2
#define Number_of_Searches 2

#define Min_Depth 1
#define Max_Depth 20

long Nodes;                                /* Nodes searched with minimax */

/* Array describing the eight combinations of three squares in a row */
#define Possible_Wins 17
const int Four_in_a_Row[Possible_Wins][4] = {
    {0, 1, 2, 3}, 
    {1, 2, 3, 4}, 
    {5, 6, 7, 8}, 
    {6, 7, 8, 9}, 
    {10, 11, 12, 13},
    {11, 12, 13, 14}, 
    {15, 16, 17, 18}, 
    {16, 17, 18, 19}, 
    {0, 5, 10, 15},
    {1, 6, 11, 16}, 
    {2, 7, 12, 17}, 
    {3, 8, 13, 18}, 
    {4, 9, 14, 19}, 
    {0, 6, 12, 18},
    {1, 7, 13, 19}, 
    {3, 7, 11, 15}, 
    {4, 8, 12, 16}
};

/* Array used in heuristic formula for each move
   Rows are the number of X's in a row, columns are the number of O's */
const int Heuristic_Array[4][4] = {
    {     0,   -10,  -100, -1000 },
    {    10,     0,     0,     0 },
    {   100,     0,     0,     0 },
    {  1000,     0,     0,     0 }
};

/* Structure to hold a move and its heuristic */
typedef struct {
    int Square;
    int Heuristic;
} Move_Heuristic_Type;

/* Clear the board */
void Initialize(Board_Type Board) {
    int I;
    for (I = 0; I < Squares; I++)
        Board[I] = Empty;
}

/* If a player has won, return the winner. If the game is a tie,
   return 'C' (for cat). If the game is not over, return Empty. */
Square_Type Winner(Board_Type Board) {
    int I;
    for (I = 0; I < Possible_Wins; I++) {
        Square_Type Possible_Winner = Board[Four_in_a_Row[I][0]];
        if (Possible_Winner != Empty &&
            Possible_Winner == Board[Four_in_a_Row[I][1]] &&
            Possible_Winner == Board[Four_in_a_Row[I][2]] &&
            Possible_Winner == Board[Four_in_a_Row[I][3]])
            return Possible_Winner;
    }

    for (I = 0; I < Squares; I++)
        if (Board[I] == Empty)
            return Empty;

    return 'C';
}

/* Return the other player */
Square_Type Other(Square_Type Player) {
    return Player == 'X' ? 'O' : 'X';
}

/* Make a move on the board */
void Play(Board_Type Board, int Square, Square_Type Player) {
    Board[Square] = Player;
}

/* Print the board */
void Print(Board_Type Board) {
    int I;
    for (I = 0; I < Squares; I += 5) {
        if (I > 0)
            printf("---+---+---+---+---\n");
        printf(" %c | %c | %c | %c | %c\n", Board[I], Board[I + 1], Board[I + 2], Board[I + 3], Board[I + 4]);
    }
    printf("\n");
}

/* Return heuristic that determines the order in which the moves are searched
   and the values of terminal nodes in the heuristic search */
int Evaluate(Board_Type Board, Square_Type Player) {
    int I;
    int Heuristic = 0;
    for (I = 0; I < Possible_Wins; I++) {
        int J;
        int Players = 0, Others = 0;
        for (J = 0; J < 4; J++) {
            Square_Type Piece = Board[Four_in_a_Row[I][J]];
            if (Piece == Player)
                Players++;
            else if (Piece == Other(Player))
                Others++;
        }
        Heuristic += Heuristic_Array[Players][Others];
    }
    return Heuristic;
}

/* Comparision function for sorting the moves by descending heuristic */
int Sort_Comp(const void *A, const void *B) {
    return ((Move_Heuristic_Type *) B)->Heuristic -
           ((Move_Heuristic_Type *) A)->Heuristic;
}




/* Return the score of the best move found for a board
   The square number of the move is returned in *Square */
int Best_Move(Board_Type Board, Square_Type Player, int *Square, int Move_Nbr,
              int Alpha, int Beta) {
    int Best_Square = -1;
    int Moves = 0;
    int I;
    Move_Heuristic_Type Move_Heuristic[Squares];

    /* Find the heuristic for each move and sort moves in descending order */
    for (I = 0; I < Squares; I++)
        if (Board[I] == Empty) {
            Play(Board, I, Player);
            Move_Heuristic[Moves].Heuristic = Evaluate(Board, Player);
            Move_Heuristic[Moves].Square = I;
            Moves++;
            Play(Board, I, Empty);
        }
    qsort(Move_Heuristic, Moves, sizeof(Move_Heuristic_Type), Sort_Comp);

    for (I = 0; I < Moves; I++) {
        int Score;
        int Sq = Move_Heuristic[I].Square;

            Nodes++;

        /* Make a move and get its score */
        Play(Board, Sq, Player);

        switch (Winner(Board)) {
        case 'X':
            Score = (Maximum_Moves + 1) - Move_Nbr;
            break;
        case 'O':
            Score = Move_Nbr - (Maximum_Moves + 1);
            break;
        case 'C':
            Score = 0;
            break;
        default:
            Score = Best_Move(Board, Other(Player), Square, Move_Nbr + 1,
                              Alpha, Beta);
            break;
        }

        Play(Board, Sq, Empty);

        /* Perform alpha-beta pruning */
        if (Player == 'X') {
            if (Score >= Beta) {
                *Square = Sq;
                return Score;
            } else if (Score > Alpha) {
                Alpha = Score;
                Best_Square = Sq;
            }
        } else {
            if (Score <= Alpha) {
                *Square = Sq;
                return Score;
            } else if (Score < Beta) {
                Beta = Score;
                Best_Square = Sq;
            }
        }
    }

    *Square = Best_Square;
    if (Player == 'X')
        return Alpha;
    else
        return Beta;
}

/* Return the score of the move that maximizes the minimum
   heuristic function Depth moves ahead
   The square number of the move is returned in *Square */
int Heuristic_Move(Board_Type Board, Square_Type Player, int *Square,
                   int Move_Nbr, int Depth, int Alpha, int Beta) {
    int Best_Square = -1;
    int Moves = 0;
    int I;
    Move_Heuristic_Type Move_Heuristic[Squares];

    /* Find the heuristic for each move and sort moves in descending order */
    for (I = 0; I < Squares; I++)
        if (Board[I] == Empty) {
            Play(Board, I, Player);
            Move_Heuristic[Moves].Heuristic = Evaluate(Board, Player);
            Move_Heuristic[Moves].Square = I;
            Moves++;
            Play(Board, I, Empty);
        }
    qsort(Move_Heuristic, Moves, sizeof(Move_Heuristic_Type), Sort_Comp);

    for (I = 0; I < Moves; I++) {
        int Score;
        int Sq = Move_Heuristic[I].Square;

            Nodes++;

        /* Make a move and get its score */
        Play(Board, Sq, Player);

        if (Depth == 1 || Winner(Board) != Empty)
            Score = Evaluate(Board, 'X');
        else
            Score = Heuristic_Move(Board, Other(Player), Square, Move_Nbr + 1,
                                         Depth - 1, Alpha, Beta);

        Play(Board, Sq, Empty);

        /* Perform alpha-beta pruning */
        if (Player == 'X') {
            if (Score >= Beta) {
                *Square = Sq;
                return Score;
            } else if (Score > Alpha) {
                Alpha = Score;
                Best_Square = Sq;
            }
        } else {
            if (Score <= Alpha) {
                *Square = Sq;
                return Score;
            } else if (Score < Beta) {
                Beta = Score;
                Best_Square = Sq;
            }
        }
    }

    *Square = Best_Square;
    if (Player == 'X')
        return Alpha;
    else
        return Beta;
}

/* Provide an English description of the score returned by Best_Move */
void Describe(int Score) {
    if (Score < 0)
        printf("You have a guaranteed win.\n");
    else if (Score == 0)
        printf("I can guarantee a tie.\n");
    else
        printf("I have a guaranteed win by move %d.\n",
               Maximum_Moves - Score + 1);
}

/* Have the human or the computer move */
void Move(Board_Type Board, Square_Type Player, int Move_Nbr,
          int Type_of_Search, int Depth) {
    int Square;

    if (Player == 'X') {
        int Score;
        Nodes = 1;
        switch(Type_of_Search) {
        
        case Normal:
            Score = Best_Move(Board, 'X', &Square, Move_Nbr,
                              -Infinity, Infinity);
            break;
        case Heuristic_Search:
            Score = Heuristic_Move(Board, 'X', &Square, Move_Nbr, Depth,
                                   -Heuristic_Infinity, Heuristic_Infinity);
            break;
        }
        printf("%ld node%s examined.\n", Nodes, Nodes==1 ? "" : "s");
        Play(Board, Square, 'X');
        printf("Move #%d - X moves to %d\n", Move_Nbr, Square + 1);
        if (Type_of_Search == Heuristic_Search)
            printf("My evaluation for this move is %d.\n", Score);
        else
            Describe(Score);
    } else {
        char Answer[String_Length];
        do {
            printf("Move #%d - What is O's move? ", Move_Nbr);
            gets(Answer);
            Square = atoi(Answer) - 1;
        } while (!(0 <= Square && Square < Squares) || Board[Square] != Empty);
        Play(Board, Square, 'O');
    }
}

/* Play a game of tic-tac-toe */
int Game() {
    Square_Type Player;
    char Answer[String_Length];
    Board_Type Board;
    int Move_Nbr = 1;
    int Type_of_Search, Depth;

    Initialize(Board);

    printf("\nTypes of minimax search available:\n");
    printf("(In order of increasing complexity and increasing speed)\n");
    printf("  1) Alpha-beta pruning, move ordering\n");
    printf("  2) Heuristic search\n\n");
    printf("Type Q to quit.\n\n");
    do {
        printf("What type of search do you want (1-2 or Q)? ");
        gets(Answer);
        if (toupper(Answer[0]) == 'Q')
            return False;
        Type_of_Search = atoi(Answer);
    } while (!(1 <= Type_of_Search && Type_of_Search <= Number_of_Searches));

    if (Type_of_Search == Heuristic_Search)
        do {
            printf("Depth of heuristic search (%d-%d)? ", Min_Depth, Max_Depth);
            gets(Answer);
            Depth = atoi(Answer);
        } while (!(Min_Depth <= Depth && Depth <= Max_Depth));

    printf("\nDo you want to move first? ");
    gets(Answer);
    Player = (toupper(Answer[0]) == 'Y') ? 'O' : 'X';

    while(Winner(Board) == ' ') {
        Print(Board);
        Move(Board, Player, Move_Nbr, Type_of_Search, Depth);
        Player = Other(Player);
        Move_Nbr++;
    }
    Print(Board);

    if (Winner(Board) != 'C')
        printf("%c wins!\n", Winner(Board));
    else
        printf("It's a tie.\n");
   
    return True;
}

/* Display instructions and play a round of tic-tac-toe games */
int main() {
    printf("Welcome to Tic-Tac-Toe!\n\n");
    printf("Here is the board numbering:\n");
    printf(" 1 | 2 | 3 | 4 | 5  \n");
    printf("---+---+------------\n");
    printf(" 6 | 7 | 8 | 9 | 10 \n");
    printf("---+---+------------\n");
    printf(" 11| 12| 13| 14| 15 \n");
    printf("---+---+------------\n");
    printf(" 16| 17| 18| 19| 20 \n");
    printf("\n");
    printf("Computer plays X, you play O.\n");

    while (Game())
        ;
   
    return 0;
}
