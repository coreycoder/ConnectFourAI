#include<iostream>
#include<cstring>
#include<cmath>
#include<algorithm>
#include<vector>
#include<ctime>

using namespace std;

//This is the State class. It holds the current state of the game as a two-dimensional 
//character array. It also has all the methods to do manipulations on the game board
//and search for the best possible move using alpha-beta pruning.
class State {

//private fields of State class
private:
	const int ROWS = 8;
	const int COLS = 8;
	int best_row = 0;
	int best_col = 0;
	char **board = NULL;
	char *upper_letters = NULL;
	char *lower_letters = NULL;
	vector<vector<int>> threes_horizontal;
	vector<vector<int>> threes_vertical;
	int threes_vertical_row= 0;
	int threes_horizontal_row = 0;
	clock_t start_time;
	double allowed_time;

//public methods of state class.
public:
	State(double);
	~State();
	void enterMove(char[], char);
	bool checkWinner(char);
	void printState();
	void alphaBetaSearch(char, char[]);
	int maxSearch(int, int, char, int, int);
	int minSearch(int, int, char, int, int);
	int evaluation();
	int checkThrees(char);
	int checkTwos(char);
	bool checkAlone(int, int);
	bool checkValidMove(char[]);
	bool checkBlocking(int, int);
};

State::State(double time) {

	//Maximum seconds per computer move
	allowed_time = time;

	//dynamically allocate space for the game board
	board = new char *[ROWS];
	for (int i = 0; i < ROWS; i++) {
		*(board + i) = new char[COLS];
	}
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			*(*(board + i) + j) = '-';
		}
	}

	//dynamically allocate and initialize letter arrays for the row position
	upper_letters = new char[ROWS];
	for (int i = 65; i < (65 + ROWS); i++) {
		char temp = i;
		*(upper_letters + (i - 65)) = temp;
	}
	lower_letters = new char[ROWS];
	for (int i = 97; i < (97 + ROWS); i++) {
		char temp = i;
		*(lower_letters + (i - 97)) = temp;
	}

	//allocate a space of at least 10 for 2 vectors.
	//These vectors will be used to make sure a 2-in-a-row
	//isnt already part of a 3-in-a-row
	threes_horizontal.reserve(10);
	threes_vertical.reserve(10);

}


State::~State() {

	//deallocate and delete the board 
	for (int i = 0; i < ROWS; i++) { 
		delete[] * (board + i);
	}
	delete[] board;
	board = NULL;

	//deallocate and delete the letter arrays;
	delete[] upper_letters;
	upper_letters = NULL;
	delete[] lower_letters;
	lower_letters = NULL;
}

//This method takes in a character array that holds the move and
//the sign of that move. 
void State::enterMove(char move[], char sign) {

	char letter = *(move + 0);
	char temp = *(move + 1);
	int col = (temp - '0') - 1;


	//If the letter appears in either of the letter arrays, 
	//then the row position is the position in the array
	int row = 0;
	for (int i = 0; i < ROWS; i++) {
		if (*(upper_letters + i) == letter || *(lower_letters + i) == letter) {
			row = i;
			break;
		}
	}

	*(*(board + row) + col) = sign;
}

//This method takes a given sign (either X or O)
//and checks to see if that sign makes a 4-in-a-row.
//It checks the board first horizonally, then vertically.
bool State::checkWinner(char sign) {

	bool win = false;

	//check horizontal
	for (int i = 0; i < ROWS; i++) {
		int count = 0;
		for (int j = 0; j < COLS; j++) {
			if (*(*(board + i) + j) == sign) {
				count++;
			}
			else {
				count = 0;
			}
			if (count >= 4) {
				win = true;
				return win;
			}
		}
	}

	//check vertical
	for (int j = 0; j < COLS; j++) {
		int count = 0;
		for (int i = 0; i < ROWS; i++) {
			if (*(*(board + i) + j) == sign) {
				count++;
			}
			else {
				count = 0;
			}
			if (count >= 4) {
				win = true;
				return win;
			}
		}
	}
	return win;
}

//This method simply prints the current state of the board
void State::printState() {
	for (int i = 0; i < ROWS; i++) {
		if (i == 0) {
			cout << "  ";
		}
		cout << (i + 1) << " ";
	}
	cout << endl;
	for (int i = 0; i < ROWS; i++) {
		cout << *(upper_letters + i) << " ";
		for (int j = 0; j < COLS; j++) {
			cout << *(*(board + i) + j) << " ";
		}
		cout << endl;
	}
}

//This method takes the current coordinate of the alpha-beta
//algorithm and checks to see if that coordinate is touching
//another move on the board. If the coordinate is only touching
//empty spaces, it is not considered a good or helpful move and 
//is thus ignored. This method accounts for corners and edges as well.
bool State::checkAlone(int i, int j) {

	bool alone = false;

	if (i == 0 ) {
		if (j == 0) {
			if (*(*(board + (i + 1)) + j) == '-' && *(*(board + i) + (j + 1)) == '-') {
				alone = true;
			}
		}
		else if (j == 7) {
			if (*(*(board + (i + 1)) + j) == '-' && *(*(board + i) + (j - 1)) == '-') {
				alone = true;
			}
		}
		else if (*(*(board + (i + 1)) + j) == '-' && *(*(board + i) + (j - 1)) == '-' && *(*(board + i) + (j + 1)) == '-') {
				alone = true;
		}
	}
	else if (j == 0) {
		if (i == 7) {
			if (*(*(board + (i - 1)) + j) == '-' && *(*(board + i) + (j + 1)) == '-') {
				alone = true;
			}
		}
		else if (*(*(board + (i - 1)) + j) == '-' && *(*(board + (i + 1)) + j) == '-' && *(*(board + i) + (j + 1)) == '-') {
			alone = true;
		}
	}
	else if (i == 7) {
		if (j == 7) {
			if (*(*(board + (i - 1)) + j) == '-' && *(*(board + i) + (j - 1)) == '-') {
				alone = true;
			}
		}
		else if (*(*(board + (i - 1)) + j) == '-' && *(*(board + i) + (j - 1)) == '-' && *(*(board + i) + (j + 1)) == '-') {
			alone = true;
		}
	}
	else if (j == 7) {
		if (*(*(board + (i - 1)) + j) == '-' && *(*(board + (i + 1)) + j) == '-' && *(*(board + i) + (j - 1)) == '-') {
			alone = true;
		}
	}
	else if (*(*(board + (i - 1)) + j) == '-' && *(*(board + (i + 1)) + j) == '-' && *(*(board + i) + (j + 1)) == '-' && *(*(board + i) + (j - 1)) == '-') {
		alone = true;
	}


	return alone;

}

//This is the alpha-beta pruning algorithm. It takes the computer's sign and the move it made.
//It first starts with the maximizer method and then recursively calls the min and so on.
//The values of alpha and beta are negative infinity and infinity respectively.
//This method also has a clock timer that keeps track of how long the algorithm has run.
//If the time surpasses the allowed length, the algorithm returns the best value so far.
//This algorithm runs at a ply depth of 5.
void State::alphaBetaSearch(char sign, char move[]) {
	
	//These are class variables that hold the positions of the best found move
	//made by the maximizer method.
	best_row = 0;
	best_col = 0;
	int a = -1000000;
	int b = 1000000;
	int depth = 5;
	start_time = 0;
	start_time = clock();
	int v = maxSearch(a, b, sign, depth, depth);

	for (int i = 0; i < ROWS; i++) {
		if (i == best_row){
			*(move + 0) = *(upper_letters + i);
		}
	}
	best_col++;
	*(move + 1) = best_col + '0';

	cout << "Computer has made the move: " << *(move + 0) << *(move + 1) << endl;

	enterMove(move, sign);

}

//This is the maximizer method for the alpha-beta pruning algorithm. 
int State::maxSearch(int alpha, int beta, char sign, int depth, int original_depth) {

	//If either of the player has own in this state, return its score
	if (checkWinner('X'))
		return 100000;
	else if (checkWinner('O'))
		return -100000;
	//When it has reached a leaf node, return the score based on the evaluation function.
	if (depth == 0) {
		return evaluation();
	}

	int v = -1000000;
	//Current time to compare with the allowed time
	double time = ((double)(clock() - start_time)) / CLOCKS_PER_SEC;

	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			if (time > allowed_time) {
				if (v == -1000000)
					v = -v;
				return v;
			}
			if (checkAlone(i, j))
				continue;
			//If the current coordinate is empty, add the sign of the current player to it 
			//to create a new successor state
			if (*(*(board + i) + j) == '-') {
				*(*(board + i) + j) = sign;
				//Change the sign before entering the minimizer method
				if (sign == 'O')
					sign = 'X';
				else if (sign == 'X')
					sign = 'O';
				int old_v = v;
				v = max(v, minSearch(alpha, beta, sign, depth - 1, original_depth));
				if (v > old_v && depth == original_depth) {
					//cout << "score: " << v << endl;
					best_row = i;
					best_col = j;
				}
				//Remove the move made previously
				*(*(board + i) + j) = '-';
				//Change the sign back after returning from minimizer method.
				if (sign == 'O')
					sign = 'X';
				else if (sign == 'X')
					sign = 'O';
				if (v >= beta) {
					return v;
				}
				alpha = max(alpha, v);
			}
		}
	}
	return v;

}

//This is the minimizer method for the alpha-beta pruning algorithm.
int State::minSearch(int alpha, int beta, char sign, int depth, int original_depth) {

	//If either of the player has own in this state, return its score
	if (checkWinner('X'))
		return 100000;
	else if (checkWinner('O'))
		return -100000;

	//When it has reached a leaf node, return the score based on the evaluation function.
	if (depth == 0) {
		return evaluation();
	}
	int v = 1000000;
	//Current time to compare with the allowed time
	double time = ((double)(clock() - start_time)) / CLOCKS_PER_SEC;

	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			if (time > allowed_time) {
				if (v == 1000000)
					v = -v;
				return v;
			}
				
			if (checkAlone(i, j))
				continue;
			//If the current coordinate is empty, add the sign of the current player to it 
			//to create a new successor state
			if (*(*(board + i) + j) == '-') {
				*(*(board + i) + j) = sign;
				//Change the sign before entering the maximizer method
				if (sign == 'O')
					sign = 'X';
				else if (sign == 'X')
					sign = 'O';
				int old_v = v;
				v = min(v, maxSearch(alpha, beta, sign, depth - 1, original_depth));
				//Remove the move made previously
				*(*(board + i) + j) = '-';
				//Change the sign back after returning from maximizer method.
				if (sign == 'O')
					sign = 'X';
				else if (sign == 'X')
					sign = 'O';
				if (v <= alpha) {
					return v;
				}
				beta = min(beta, v);

			}
		}
	}
	return v;
}

//This is the evaluation function for the leaf nodes of the alpha-beta pruning algorithm.
//This method finds the number of 3-in-a-rows and 2-in-a-rows for each player. Then it
//gives a certain weight to each of the number of 3's and 2's of each player. For instance, here
//the opponent has a higher negative weight than the computer, making this more of a defensive
//computer player.
int State::evaluation() {

	int good_threes = checkThrees('X');
	int good_twos = checkTwos('X');
	int bad_threes = checkThrees('O');
	int bad_twos = checkTwos('O');

	int total = 0;
	total += (good_threes * 100);
	total += (good_twos * 10);
	total -= (bad_threes * 500);
	total -= (bad_twos * 50);
	return total;

}

//This is the method that checks for the number of 3-in-a-rows given the sign of 
//the current player. These 3-in-a-rows include those with spaces in them.
int State::checkThrees(char sign) {

	//This allocates space for a two-dimensional vector to be used to hold
	//the positions of any 3-in-a-rows.
	//These vectors will be used to make sure the 2-in-a-rows found later
	//are not part of these 3-in-a-rows.
	for (int i = 0; i < 10; i++) {
		threes_horizontal.push_back(vector<int>());
		threes_vertical.push_back(vector<int>());
	}

	char opponent;
	if (sign == 'O')
		opponent = 'X';
	else if (sign == 'X')
		opponent = 'O';

	//starting position of the 3-in-a-row
	int start = 0;
	//Number of spaces so far
	int space = 0;
	//The position of the space in a 3-in-a-row
	int space_position = 0;
	//Total number of 3-in-a-rows
	int threes = 0;

	//check horizontal
	for (int i = 0; i < ROWS; i++) {
		int count = 0;
		for (int j = 0; j < COLS; j++) {
			if (*(*(board + i) + j) == sign && space < 2) {
				if (count == 0) {
					start = j;
				}
				count++;
				
			}
			else if (*(*(board + i) + j) != sign && space >= 1) {
				count = 0;
				space = 0;
				space_position = 0;
			}
			else if (*(*(board + i) + j) != sign && space < 2) {
				space_position = j;
				space++;
			}
			if (count >= 3) {
				if (space == 1) {
					if (*(*(board + i) + (space_position)) == '-')
						threes += 10;
					else if (*(*(board + i) + (space_position)) == opponent)
						threes -= 10;
				}
				else {
					if (start == 0) {
						if (*(*(board + i) + (j + 1)) == opponent) {
							threes -= 10;
						}
						else if (*(*(board + i) + (j + 1)) == '-') {
							threes += 10;
						}
					}
					else if (start == 5) {
						if (*(*(board + i) + (start - 1)) == opponent) {
							threes -= 10;
						}
						else if (*(*(board + i) + (start - 1)) == '-') {
							threes += 10;
						}
					}
					else {
						if (*(*(board + i) + (start - 1)) == opponent && *(*(board + i) + (j + 1)) == opponent) {
							threes -= 11;
						}
						else if (*(*(board + i) + (start - 1)) == opponent) {
							threes += 10;
						}
						else if (*(*(board + i) + (j + 1)) == opponent) {
							threes += 10;
						}
						else if ((*(*(board + i) + (start - 1)) == '-' && *(*(board + i) + (j + 1)) == '-')) {
							threes += 15;
						}
					}
				}
				count = 0;
				space_position = 0;
				space = 0;
				//Add the coordinates of the 3-in-a-row to the vector to be used later
				threes_horizontal[threes_horizontal_row].push_back(i);
				threes_horizontal[threes_horizontal_row].push_back(start);
				threes_horizontal_row++;

			}
		}
	}

	//check vertical
	for (int j = 0; j < COLS; j++) {
		int count = 0;
		for (int i = 0; i < ROWS; i++) {
			if (*(*(board + i) + j) == sign && space < 2) {
				if (count == 0) {
					start = i;
				}
				count++;
			}
			else if (*(*(board + i) + j) != sign && space >= 1) {
				count = 0;
				space = 0;
				space_position = 0;
			}
			else if (*(*(board + i) + j) != sign && space < 2) {
				space++;
				space_position = i;
			}
			if (count >= 3) {
				if (space == 1) {
					if (*(*(board + (space_position)) + j) == '-')
						threes += 10;
					else if (*(*(board + (space_position)) + j) == opponent)
						threes -= 10;
				}
				else {
					if (start == 0) {
						if (*(*(board + (i + 1)) + j) == opponent) {
							threes -= 10;
						}
						else if (*(*(board + (i + 1)) + j) == '-') {
							threes += 10;
						}
					}
					else if (start == 5) {
						if (*(*(board + (start - 1)) + j) == opponent) {
							threes -= 10;
						}
						else if (*(*(board + (start - 1)) + j) == '-') {
							threes += 10;
						}
					}
					else {
						if (*(*(board + (start - 1)) + j) == opponent && *(*(board + (i + 1)) + j) == opponent) {
							threes -= 11;
						}
						else if (*(*(board + (start - 1)) + j) == opponent) {
							threes += 10;
						}
						else if (*(*(board + (i + 1)) + j) == opponent) {
							threes += 10;
						}
						else if ((*(*(board + (start - 1)) + j) == '-' && *(*(board + (i + 1)) + j) == '-')) {
							threes += 15;
						}
					}
				}
				count = 0;
				space = 0;
				space_position = 0;
				threes_vertical[threes_vertical_row].push_back(start);
				threes_vertical[threes_vertical_row].push_back(j);
				threes_vertical_row++;
			}
		}
	}

	return threes;

}

//This method checks for the number of 2-in-a-rows for each player, given the sign.

int State::checkTwos(char sign) {
	char opponent;
	if (sign == 'O')
		opponent = 'X';
	else if (sign == 'X')
		opponent = 'O';

	int start;

	int space = 0;
	int space_position = 0;
	int twos = 0;

	int threes_check_horizontal = 0;
	int threes_check_vertical = 0;


	//check horizontal
	for (int i = 0; i < ROWS; i++) {
		int count = 0;
		for (int j = 0; j < COLS; j++) {
			int exists = false;

			//This makes sure the 2-in-a-row is not part of any 3-in-a-rows found earlier.
			for (int k = 0; k < threes_horizontal.size(); k++) {
				if (threes_horizontal[k].size() > 0) {
					if (threes_horizontal[k][0] == i && (threes_horizontal[k][1] == j || (threes_horizontal[k][1] + 1) == j)) {
						count = 0;
						space = 0;
						exists = true;
						break;
					}
				}
			}
			if (exists)
				break;
			if (*(*(board + i) + j) == sign && space < 2) {
				if (count == 0) {
					start = j;
				}
				count++;

			}
			else if (*(*(board + i) + j) != sign && space >= 1) {
				count = 0;
				space = 0;
				space_position = 0;
			}
			else if (*(*(board + i) + j) != sign && space < 2) {
				space++;
				space_position = j;
			}
			if (count >= 2) {
				if (space == 1) {
					if (*(*(board + i) + (space_position)) == '-')
						twos += 10;
					else if (*(*(board + i) + (space_position)) == opponent)
						twos -= 10;
				}
				else {
					if (start == 0) {
						if (*(*(board + i) + (j + 1)) == opponent) {
							twos -= 10;
						}
						else if (*(*(board + i) + (j + 1)) == '-') {
							twos += 10;
						}
					}
					else if (start == 6) {
						if (*(*(board + i) + (start - 1)) == opponent) {
							twos -= 5;
						}
						else if (*(*(board + i) + (start - 1)) == '-') {
							twos += 5;
						}
					}
					else {
						if (*(*(board + i) + (start - 1)) == opponent && *(*(board + i) + (j + 1)) == opponent) {
							twos -= 8;
						}
						else if (*(*(board + i) + (start - 1)) == opponent) {
							twos -= 6;
						}
						else if (*(*(board + i) + (j + 1)) == opponent) {
							twos -= 6;
						}
						else if ((*(*(board + i) + (start - 1)) == '-' && *(*(board + i) + (j + 1)) == '-')) {
							twos += 7;
						}
					}
				}
				count = 0;
				space = 0;
				space_position = 0;
			}
		}
	}

	//check vertical
	for (int j = 0; j < COLS; j++) {
		int count = 0;
		for (int i = 0; i < ROWS; i++) {
			bool exists = false;
			for (int k = 0; k < threes_vertical.size(); k++) {
				if (threes_vertical[k].size() > 0) {
					if ((threes_vertical[k][0] == i || (threes_vertical[k][0] + 1) == i) && threes_vertical[k][1] == j) {
						count = 0;
						space = 0;
						exists = true;
						break;
					}
				}
			}
			if (exists)
				break;
			if (*(*(board + i) + j) == sign && space < 2) {
				if (count == 0) {
					start = i;
				}
				count++;
			}
			else if (*(*(board + i) + j) == opponent) {
				count = 0;
				space = 0;
			}
			else if (*(*(board + i) + j) == '-' && space >= 1) {
				count = 0;
				space = 0;
			}
			else if (*(*(board + i) + j) == '-' && space < 2) {
				space++;
			}
			if (count >= 2) {
				if (space == 1) {
					if (*(*(board + (space_position)) + j) == '-')
						twos += 10;
					else if (*(*(board + (space_position)) + j) == opponent)
						twos -= 10;
				}
				else {
					if (start == 0) {
						if (*(*(board + (i + 1)) + j) == opponent) {
							twos -= 10;
						}
						else if (*(*(board + (i + 1)) + j) == '-') {
							twos += 10;
						}
					}
					else if (start == 6) {
						if (*(*(board + (start - 1)) + j) == opponent) {
							twos -= 10;
						}
						else if (*(*(board + (start - 1)) + j) == '-') {
							twos += 10;
						}
					}
					else {
						if (*(*(board + (start - 1)) + j) == opponent && *(*(board + (i + 1)) + j) == opponent) {
							twos -= 8;
						}
						else if (*(*(board + (start - 1)) + j) == opponent) {
							twos -= 6;
						}
						else if (*(*(board + (i + 1)) + j) == opponent) {
							twos -= 6;
						}
						else if ((*(*(board + (start - 1)) + j) == '-' && *(*(board + (i + 1)) + j) == '-')) {
							twos += 7;
						}
					}
				}
				count = 0;
				space = 0;
				space_position = 0;
			}
		}
	}


	//Deallocate and delete the helping vectors used 
	for (int i = 0; i < 10; i++) {
		threes_horizontal.erase(threes_horizontal.end() - 1);
		threes_vertical.erase(threes_vertical.end() - 1);
	}
	threes_horizontal_row = 0;
	threes_vertical_row = 0;

	return twos;

}

//This method makes sure the move submitted by the player is valid.
//It makes sure the move that the player makes is not out of bounds
//or the piece is not currently empty.
bool State::checkValidMove(char move[]) {

	bool valid = true;

	char letter = *(move + 0);
	char temp = *(move + 1);
	int col = (temp - '0') - 1;
	if (col < 0 || col > 7) {
		return false;
	}
	int row = 0;
	for (int i = 0; i < ROWS; i++) {
		if (*(upper_letters + i) == letter || *(lower_letters + i) == letter) {
			row = i;
			valid = true;
			break;
		}
	}
	if (*(*(board + row) + col) != '-') {
		return false;
	}
	return valid;
}



int main() {

	int flag = 0;
	char human = 'O';
	char computer = 'X';
	int turn;
	char *move = NULL;
	char sign;
	char input;
	cout << "Would you like to go first (y/n): ";
	cin >> input;
	if (input == 'y')
		turn = 1;
	else
		turn = 0;

	int first = 0;

	int time;
	cout << "How much time would you like the computer to take per move? (in seconds): ";
	cin >> time;
	
	State state((double)time);
	state.printState();

	while (true) {

		if (turn == 1) {
			sign = human;
			move = new char[3];
			cout << "type in a move: ";
			if (flag == 0 || flag == 1)
				cin.ignore();
			cin.getline(move, 3);
			if (!state.checkValidMove(move)) {
				cout << "This move is invalid. Try again." << endl;
				flag = 2;
				continue;
			}
			state.enterMove(move, sign);
			state.printState();
			//this is for debugging purposes
			//cout << "good three: " << state.checkThrees('X') << endl;
			//cout << "good two: " << state.checkTwos('X') << endl;
			//cout << "bad three: " << state.checkThrees('O') << endl;
			//cout << "bad two: " << state.checkTwos('O') << endl;
			bool win = state.checkWinner(sign);
			if (win) {
				cout << "\n\nYOU WON" << endl;
				break;
			}
			first++;
		}
		else {
			sign = computer;
			move = new char[3];
			state.alphaBetaSearch(sign, move);
			state.printState();
			//This is for debugging purposes
			//cout << "good three: " << state.checkThrees('X') << endl;
			//cout << "good two: " << state.checkTwos('X') << endl;
			//cout << "bad three: " << state.checkThrees('O') << endl;
			//cout << "bad two: " << state.checkTwos('O') << endl;
			bool win = state.checkWinner(sign);
			if (win) {
				cout << "\n\nComputer wins. boohoo." << endl;
				break;
			}
			flag = 2;
			if (first == 0) {
				flag = 1;
				first++;
			}
			
		}

		
		turn = abs(turn - 1);

	}
	delete[] move;
	move = NULL;
	system("pause");
	return 0;

}

