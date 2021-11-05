#include "Game.h"

/*
Sets the game board and the pieces to initial game state.
Assumes gameBoard is not null.
*/
static void setupInitialGameBoard(ChessBoard gameBoard) {
	//chessboard mảng 2 chiều 8x8
	//khởi tạo giá trị _ với 4 hàng giữa chessboard
	// _ _ _ _ _ _ _ _
	// _ _ _ _ _ _ _ _
	// _ _ _ _ _ _ _ _
	// _ _ _ _ _ _ _ _
	for (int i = 2; i < BOARD_ROWS_NUMBER - 2; i++) {
		for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
			gameBoard[i][j] = BOARD_EMPTY_CELL;
		}
	}

	//với mỗi ký tự đại diện 1 quân cờ
	//khởi tạo 2 hàng đầu mảng với chuỗi ký tự "rnbqkbnr" "mmmmmmmm"
	// r n b q k b n r
	// m m m m m m m m
	strncpy(gameBoard[0], BOARD_WHITE_FIRST_ROW_INITAL_PIECES, BOARD_COLUMNS_NUMBER);
	strncpy(gameBoard[1], BOARD_WHITE_SECOND_ROW_INITIAL_PIECES, BOARD_COLUMNS_NUMBER);

	//khởi tạo 2 hàng cuối mảng với chuỗi ký tự "RNBQKBNR" "MMMMMMMM"
	// R N B Q K B N R
	// M M M M M M M M
	strncpy(gameBoard[BOARD_ROWS_NUMBER - 1], BOARD_BLACK_FIRST_ROW_INITAL_PIECES, BOARD_COLUMNS_NUMBER);
	strncpy(gameBoard[BOARD_ROWS_NUMBER - 2], BOARD_BLACK_SECOND_ROW_INITIAL_PIECES, BOARD_COLUMNS_NUMBER);

	//khởi tạo mảng chessboard 8x8
	// r n b q k b n r
	// m m m m m m m m
	// _ _ _ _ _ _ _ _
	// _ _ _ _ _ _ _ _
	// _ _ _ _ _ _ _ _
	// _ _ _ _ _ _ _ _
	// R N B Q K B N R
	// M M M M M M M M
}

Game * gameCreate(int historySize) {
	Game *game;

    //nếu historySize âm
    //trả về hàm trả về NULL
	if (historySize <= 0) return NULL;

    //cấp phát vùng nhớ động cho game theo kích thước game
    //nếu cấp phát không thành công hàm trả về NULL
	game = malloc(sizeof(Game));
	if (game == NULL) return NULL;

    //gán giá trị mặc định quân trắng đi trước
    //giá trị mặc định kiểm tra con vua hai bên là false
	game->currentPlayer = White;
	game->isBlackKingChecked = false;
	game->isWhiteKingChecked = false;

	//sử dụng hàm khởi tạo bàn cờ
	setupInitialGameBoard(game->gameBoard);

    //sử dụng hàm khởi tạo ArrayList cấp phát vùng nhớ với số lượng vùng nhớ là historySize
	game->history = arrayListCreate(historySize);

    //nếu khởi tạo không thành công giải phóng vùng nhớ cấp phát game
    //hàm trả về NULL
	if (game->history == NULL) {
		free(game);
		return NULL;
	}

    //khởi tạo thành công các
    //vùng nhớ được cấp phát
    //thiết lập giá trị mặc định quân nào đi trước, trạng thái của 2 con vua
    //bàn cờ ban đầu
    //ArrayList
    //trả về game sau khi khởi tạo
	return game;
}

void gameDestroy(Game * game) {
    //nếu game bằng NULL không cần phải giải phóng vùng nhớ
	if (game == NULL) return;

    //giải phóng vùng nhớ ArrayList với kích thước history của game
    //sau đó giải phóng game
	arrayListDestroy(game->history);
	free(game);
}

bool gameIsSquareValid(BoardSquare s)
{
    //kiểm tra đảm bảo
    //số dòng, số cột >=0
    //số dòng, số cột <8
	return s.row >= 0 && s.col >= 0 && s.row < BOARD_ROWS_NUMBER && s.col < BOARD_COLUMNS_NUMBER;
}

static bool gameIsSquareEmpty(ChessBoard gameBoard, BoardSquare s) {
    //kiểm tra ô trên bàn cờ có trống hay không
    //nếu trống sẽ bằng _
	return gameBoard[s.row][s.col] == BOARD_EMPTY_CELL;
}

void gamePrintGameBoardToFileHandler(FILE * fh, Game * game) {
    //in bàn cờ vào file
    //in thêm số thứ tự hàng vào GameBoard khởi tạo
	//8| r n b q k b n r |
	//7| m m m m m m m m |
	//6| _ _ _ _ _ _ _ _ |
	//5| _ _ _ _ _ _ _ _ |
	//4| _ _ _ _ _ _ _ _ |
	//3| _ _ _ _ _ _ _ _ |
	//2| R N B Q K B N R |
	//1| M M M M M M M M |
	for (int i = BOARD_ROWS_NUMBER - 1; i >= 0; i--) {
		fprintf(fh, "%d| ", i + 1);
		for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
			fprintf(fh, "%c ", game->gameBoard[i][j]);
		}
		fprintf(fh, "|\n");
	}

    //in thêm 2 hàng cuối thể hiện tên cột trên bàn cờ
	//8| r n b q k b n r |
	//7| m m m m m m m m |
	//6| _ _ _ _ _ _ _ _ |
	//5| _ _ _ _ _ _ _ _ |
	//4| _ _ _ _ _ _ _ _ |
	//3| _ _ _ _ _ _ _ _ |
	//2| R N B Q K B N R |
	//1| M M M M M M M M |
    //  -----------------
    //   A B C D E F G H

	fprintf(fh, "  -----------------\n");
	fprintf(fh, "   A B C D E F G H\n");
}

GAME_MESSAGE gamePrintBoard(Game * game) {
    //nếu game không khởi tạo thành công
    //trả về trạng thái tham số truyền không hợp lệ = 7
	if (game == NULL) return GAME_INVALID_ARGUMENT;

    //in bàn cờ vào file
	gamePrintGameBoardToFileHandler(stdout, game);

    //trả về thao tác game hoàn tất = 10
	return GAME_SUCCESS;
}

static void getWhitePawnLegalMoves(ChessBoard gameBoard,
	bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER], BoardSquare s) {
	
	int row, col;

    //hàm con tốt đi đúng luật
	//với lên 1 ô: tăng số dòng lên 1
    //nếu số dòng < 8 và ô đi đến trống _ 
    //gán giá trị ô đó trong mảng = 1 
	row = s.row + 1, col = s.col;
	if (row < BOARD_ROWS_NUMBER && gameBoard[row][col] == BOARD_EMPTY_CELL) {
		movesBoard[row][col] = true;

		//với lên 2 ô
        //nếu số dòng con tốt là vị trí ban đầu = 1 và ô đi đến trống
        //gán giá trị ô đó trong mảng = 1 
		row = s.row + 2;
		if (s.row == 1 && gameBoard[row][col] == BOARD_EMPTY_CELL) {
			movesBoard[row][col] = true;
		}
	}

	//với đi xéo bên phải tăng số dòng, số cột 1
	//nếu số dòng < 8 và số cột <8
	//nếu ô đi đến có chứa chữ viết hoa
    //gán giá trị ô đó trong mảng = 1 
	row = s.row + 1, col = s.col + 1;
	if (row < BOARD_ROWS_NUMBER && col < BOARD_COLUMNS_NUMBER &&
		isupper(gameBoard[row][col])) {
		movesBoard[row][col] = true;
	}

	//với đi xéo bên trái tăng số dòng 1, số cột -1
	//nếu số dòng < 8 và số cột >= 0
	//nếu ô đi đến có chứa chữ viết hoa
    //gán giá trị ô đó trong mảng = 1 
	row = s.row + 1, col = s.col - 1;
	if (row < BOARD_ROWS_NUMBER && col >= 0 && isupper(gameBoard[row][col])) {
		movesBoard[row][col] = true;
	}
}

static void getWhiteBishopLegalMoves(ChessBoard gameBoard,
	bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER], BoardSquare s) {

	int row, col;

	//phạm vi xéo phải đến hết bàn cờ
	//nếu các ô trong phạm vi vẫn trống _ sẽ tiếp tục gán = true
	//gán đến hết bàn cờ hoặc gặp ô chứa chữ viết hoa sẽ gán = true và ngừng lại
	for (row = s.row + 1, col = s.col + 1; row < BOARD_ROWS_NUMBER && col < BOARD_COLUMNS_NUMBER; row++, col++) {
		if (gameBoard[row][col] == BOARD_EMPTY_CELL) movesBoard[row][col] = true;

		else {
			if (isupper(gameBoard[row][col])) movesBoard[row][col] = true;

			break;
		}
	}

	//phạm vi xéo trái đến hết bàn cờ
	//nếu các ô trong phạm vi vẫn trống _ sẽ tiếp tục gán = true
	//gán đến hết bàn cờ hoặc gặp ô chứa chữ viết hoa sẽ gán = true và ngừng lại
	for (row = s.row + 1, col = s.col - 1; row < BOARD_ROWS_NUMBER && col >= 0; row++, col--) {
		if (gameBoard[row][col] == BOARD_EMPTY_CELL) movesBoard[row][col] = true;

		else {
			if (isupper(gameBoard[row][col])) movesBoard[row][col] = true;

			break;
		}
	}

	//phạm vi xéo phải đằng sau đến hết bàn cờ
	//nếu các ô trong phạm vi vẫn trống _ sẽ tiếp tục gán = true
	//gán đến hết bàn cờ hoặc gặp ô chứa chữ viết hoa sẽ gán = true và ngừng lại
	for (row = s.row - 1, col = s.col + 1; row >= 0 && col < BOARD_COLUMNS_NUMBER; row--, col++) {
		if (gameBoard[row][col] == BOARD_EMPTY_CELL) movesBoard[row][col] = true;

		else {
			if (isupper(gameBoard[row][col])) movesBoard[row][col] = true;

			break;
		}
	}

	//phạm vi xéo trái đằng sau đến hết bàn cờ
	//nếu các ô trong phạm vi vẫn trống _ sẽ tiếp tục gán = true
	//gán đến hết bàn cờ hoặc gặp ô chứa chữ viết hoa sẽ gán = true và ngừng lại
	for (row = s.row - 1, col = s.col - 1; row >= 0 && col >= 0; row--, col--) {
		if (gameBoard[row][col] == BOARD_EMPTY_CELL) movesBoard[row][col] = true;

		else {
			if (isupper(gameBoard[row][col])) movesBoard[row][col] = true;

			break;
		}
	}
}

static void getWhiteRookLegalMoves(ChessBoard gameBoard,
	bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER], BoardSquare s) {

	int row, col;

	//phạm vi đi thằng đến hết bàn cờ
	//nếu các ô trong phạm vi vẫn trống _ sẽ tiếp tục gán = true
	//gán đến hết bàn cờ hoặc gặp ô chứa chữ viết hoa sẽ gán = true và ngừng lại
	for (row = s.row + 1, col = s.col; row < BOARD_ROWS_NUMBER; row++) {
		if (gameBoard[row][col] == BOARD_EMPTY_CELL) movesBoard[row][col] = true;

		else {
			if (isupper(gameBoard[row][col])) movesBoard[row][col] = true;

			break;
		}
	}

	//phạm vi đi lùi đến hết bàn cờ
	//nếu các ô trong phạm vi vẫn trống _ sẽ tiếp tục gán = true
	//gán đến hết bàn cờ hoặc gặp ô chứa chữ viết hoa sẽ gán = true và ngừng lại
	for (row = s.row - 1, col = s.col; row >= 0; row--) {
		if (gameBoard[row][col] == BOARD_EMPTY_CELL) movesBoard[row][col] = true;

		else {
			if (isupper(gameBoard[row][col])) movesBoard[row][col] = true;

			break;
		}
	}

	//phạm vi đi bên phải đến hết bàn cờ
	//nếu các ô trong phạm vi vẫn trống _ sẽ tiếp tục gán = true
	//gán đến hết bàn cờ hoặc gặp ô chứa chữ viết hoa sẽ gán = true và ngừng lại
	for (row = s.row, col = s.col + 1; col < BOARD_COLUMNS_NUMBER; col++) {
		if (gameBoard[row][col] == BOARD_EMPTY_CELL) movesBoard[row][col] = true;

		else {
			if (isupper(gameBoard[row][col])) movesBoard[row][col] = true;

			break;
		}
	}

	//phạm vi đi bên trái đến hết bàn cờ
	//nếu các ô trong phạm vi vẫn trống _ sẽ tiếp tục gán = true
	//gán đến hết bàn cờ hoặc gặp ô chứa chữ viết hoa sẽ gán = true và ngừng lại
	for (row = s.row, col = s.col - 1; col >= 0; col--) {
		if (gameBoard[row][col] == BOARD_EMPTY_CELL) movesBoard[row][col] = true;

		else {
			if (isupper(gameBoard[row][col])) movesBoard[row][col] = true;

			break;
		}
	}
}

static void getWhiteKnightLegalMoves(ChessBoard gameBoard,
	bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER], BoardSquare s) {

	int row, col;
	//mảng BoardSquare chứa 8 hướng đi của con ngựa
	BoardSquare potentialMoves[8] = {
		{ s.row + 1, s.col - 2 }, // 2 trái, 1 lên
		{ s.row + 2, s.col - 1 }, // 1 trái, 2 lên
		{ s.row - 1, s.col - 2 }, // 2 trái, 1 xuống
		{ s.row - 2, s.col - 1 }, // 1 trái, 2 xuống
		{ s.row + 1, s.col + 2 }, // 2 phải, 1 lên
		{ s.row + 2, s.col + 1 }, // 1 phải, 2 lên
		{ s.row - 1, s.col + 2 }, // 2 phải, 1 xuống
		{ s.row - 2, s.col + 1 }, // 1 phải, 2 xuống
	};

	//load phạm vi 8 hướng
	//đảm bảo số dòng số cột nằm trong bàn cờ
	//nếu thỏa gán ô đó = true
	for (int i = 0; i < 8; i++) {
		row = potentialMoves[i].row, col = potentialMoves[i].col;

		if (row >= 0 && row < BOARD_ROWS_NUMBER && 
			col >= 0 && col < BOARD_COLUMNS_NUMBER && 
			!islower(gameBoard[row][col])) {

			movesBoard[row][col] = true;
		}
	}
}

static void getWhiteQueenLegalMoves(ChessBoard gameBoard,
	bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER], BoardSquare s) {

	//với con hậu là tổng hợp phạm vi giữa con tượng, con xe
	//nên sử dụng hàm lấy phạm vi con tượng, con xe
	getWhiteBishopLegalMoves(gameBoard, movesBoard, s);
	getWhiteRookLegalMoves(gameBoard, movesBoard, s);
}

static void getWhiteKingLegalMoves(ChessBoard gameBoard,
	bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER], BoardSquare s) {

	int row, col;

	//mảng BoardSquare chứa 8 hướng đi của con vua
	BoardSquare potentialMoves[8] = {
		{ s.row + 1, s.col - 1 }, // đi xéo trái trên
		{ s.row + 1, s.col }, // đi lên
		{ s.row + 1, s.col + 1 }, // đi xéo phải trên
		{ s.row - 1, s.col - 1 }, // đi xéo trái dưới
		{ s.row - 1, s.col }, // đi lùi
		{ s.row - 1, s.col + 1 }, // đi xéo phải dưới
		{ s.row, s.col - 1 }, // đi trái
		{ s.row, s.col + 1 }, // đi phải
	};

	//load phạm vi 8 hướng
	//đảm bảo số dòng số cột nằm trong bàn cờ
	//nếu thỏa gán ô đó = true
	for (int i = 0; i < 8; i++) {
		row = potentialMoves[i].row, col = potentialMoves[i].col;

		if (row >= 0 && row < BOARD_ROWS_NUMBER &&
			col >= 0 && col < BOARD_COLUMNS_NUMBER &&
			!islower(gameBoard[row][col])) {

			movesBoard[row][col] = true;
		}
	}
}

static void getWhitePieceLegalMoves(ChessBoard gameBoard,
	bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER], BoardSquare s) {

	//tùy theo ký tự mbrnqk của bàn cờ bên trắng để nhận diện con cờ
	//rồi lấy hàm phạm vi với quân cờ tương ứng
	switch (gameBoard[s.row][s.col]) {
	case PIECE_PAWN:
		getWhitePawnLegalMoves(gameBoard, movesBoard, s);
		break;
	case PIECE_BISHOP:
		getWhiteBishopLegalMoves(gameBoard, movesBoard, s);
		break;
	case PIECE_ROOK:
		getWhiteRookLegalMoves(gameBoard, movesBoard, s);
		break;
	case PIECE_KNIGHT:
		getWhiteKnightLegalMoves(gameBoard, movesBoard, s);
		break;
	case PIECE_QUEEN:
		getWhiteQueenLegalMoves(gameBoard, movesBoard, s);
		break;
	case PIECE_KING:
		getWhiteKingLegalMoves(gameBoard, movesBoard, s);
		break;
	}
}

static char changeCaseOfChar(char c) {
	//đổi ngược chữ cái thường thành hoa và ngược lại
	if (isupper(c)) return tolower(c);
	if (islower(c)) return toupper(c);

	return c;
}

static void rotateBoardAndSwitchColors(ChessBoard gameBoard) {
	//đổi chữ hoa trên bàn cờ thành chữ thường và ngược lại
	//chỉ đổi kiểu chữ cái 1 bên bàn cờ
	char temp;

	for (int i = 0; i < BOARD_ROWS_NUMBER / 2; i++) { // divided by 2 to prevent double swapping
		for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
			temp = changeCaseOfChar(gameBoard[i][j]);
			gameBoard[i][j] = changeCaseOfChar(gameBoard[BOARD_ROWS_NUMBER - 1 - i][BOARD_COLUMNS_NUMBER - 1 - j]);
			gameBoard[BOARD_ROWS_NUMBER - 1 - i][BOARD_COLUMNS_NUMBER - 1 - j] = temp;
		}
	}
}

static void rotateMovesBoard(bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER]) {
	//đổi phía phạm vi đi trên mảng bool moveBoard
	//chỉ đổi phạm vi 1 bên bàn cờ
	bool temp;

	for (int i = 0; i < BOARD_ROWS_NUMBER / 2; i++) {
		for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
			temp = movesBoard[i][j];
			movesBoard[i][j] = movesBoard[BOARD_ROWS_NUMBER - 1 - i][BOARD_COLUMNS_NUMBER - 1 - j];
			movesBoard[BOARD_ROWS_NUMBER - 1 - i][BOARD_COLUMNS_NUMBER - 1 - j] = temp;
		}
	}
}

static BoardSquare getBlackKingSquare(ChessBoard gameBoard) {
	//khởi tạo vị trí con cờ
	BoardSquare s = { -1, -1 };

	//tìm vị trí con vua đen
	//lưu lại vị trí
	for (int i = BOARD_ROWS_NUMBER - 1; i >= 0; i--) { // starts from the end - optimization
		for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
			if (gameBoard[i][j] == toupper(PIECE_KING)) {
				s.row = i;
				s.col = j;
				
				return s;
			}
		}
	}

	//trường hợp không tìm thấy trả về cột, dòng -1 -1 
	return s;
}

static bool checkBlackKingCheckState(ChessBoard gameBoard) {
	BoardSquare kingSquare = getBlackKingSquare(gameBoard);
	bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER] = { {false} };

	//trường hợp nếu phạm vi bất kỳ quân cờ bên trắng trùng với tọa độ con vua đen
	//trả về true
	for (int i = 0; i < BOARD_ROWS_NUMBER; i++) {
		for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
			if (islower(gameBoard[i][j])) {
				gameGetLegalMoves(gameBoard, movesBoard, (BoardSquare) { i, j });
				if (movesBoard[kingSquare.row][kingSquare.col]) return true;
			}
		}
	}
	
	//nếu không nằm vô bất kỳ phạm vi quân cờ trắng
	//trả về false
	return false;
}

bool gameBoardKingIsChecked(ChessBoard gameBoard, ChessPlayer player) {
	bool retValue;

	//trường hợp bên đen đi sẽ kiểm tra lại con có bị chiếu chưa
	if (player == Black) return checkBlackKingCheckState(gameBoard);

	//trường hợp bên trắng đi cần đổi bên
	//kiểm tra con vua đen bị bí chưa
	//trả về bên trắng như cũ
	rotateBoardAndSwitchColors(gameBoard);
	retValue = checkBlackKingCheckState(gameBoard);
	rotateBoardAndSwitchColors(gameBoard);

	//trả về true nếu vua đen bị chiếu
	//trả về false nếu vua đen không bị chiếu
	return retValue;
}

void gameGetLegalMoves(ChessBoard gameBoard, bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER], BoardSquare s) {

	//kiểm tra vị trí có phải của quân cờ trắng
	bool isWhitePiece = islower(gameBoard[s.row][s.col]);

	//trường hợp không phải quân cờ trắng
	//cần đổi bên cờ để đảm bảo quân đen đi đúng luật
	if (!isWhitePiece) {
		rotateBoardAndSwitchColors(gameBoard);
		s.row = BOARD_ROWS_NUMBER - 1 - s.row;
		s.col = BOARD_COLUMNS_NUMBER - 1 - s.col;
	}

	getWhitePieceLegalMoves(gameBoard, movesBoard, s);

	//cần trả đổi bên lại như cũ
	//đổi phạm vi quân cờ về cho quân đen
	if (!isWhitePiece) {
		rotateBoardAndSwitchColors(gameBoard);
		rotateMovesBoard(movesBoard);
	}
}

void gameChangePlayer(Game * game) {
	//đổi lượt chơi bên đen thành trắng và ngược lại
	if (game->currentPlayer == White) game->currentPlayer = Black;
	else game->currentPlayer = White;
}

void gameForceSetMove(Game * game, BoardSquare from, BoardSquare to) {	
	//lưu lại các thông tin của con cờ trên bàn
	//vị trí đang đứng, vị trí đến	
	//lưu giá trị (nhãn) mà con cờ đến trên bàn cờ
	//lưu giá trị có phải quân cờ trắng
	//lưu giá trị có phải quân cờ đen
	HistoryElement histElement = { from, to, game->gameBoard[to.row][to.col],
		game->isWhiteKingChecked, game->isBlackKingChecked };

	//thêm vào cuối ArrayList;
	//lưu trữ giá trị quá khứ của con cờ vào ArrayList
	arrayListAddLast(game->history, histElement);

	//gán giá trị (nhãn) của con cờ hiện đứng lên ô có vị đến
	//gán vị trí đang đứng trước về giá trị ô trống _
	game->gameBoard[to.row][to.col] = game->gameBoard[from.row][from.col];
	game->gameBoard[from.row][from.col] = BOARD_EMPTY_CELL;

	//đổi bên người chơi
	gameChangePlayer(game);

	//lưu lại giá trị kiểm tra co vua 2 bên có bị chiếu không
	game->isBlackKingChecked = gameBoardKingIsChecked(game->gameBoard, Black);
	game->isWhiteKingChecked = gameBoardKingIsChecked(game->gameBoard, White);
}

static void gameForceUndoPrevMove(Game * game) {
	//cho phép đi lại bằng cách
	//lấy giá trị quá khứ đã lưu trong ArrayList
	//xóa giá trị lấy ra đó
	HistoryElement histElement = arrayListGetLast(game->history);
	arrayListRemoveLast(game->history);

	//lấy giá trị(nhãn) hiện tại gán cho quá khứ 
	game->gameBoard[histElement.oldSquare.row][histElement.oldSquare.col] = game->gameBoard[histElement.newSquare.row][histElement.newSquare.col];

	//trả giá trị(nhãn) hiện tại về như cũ
	game->gameBoard[histElement.newSquare.row][histElement.newSquare.col] = histElement.prevElementOnNewCell;

	//đổi bên
	gameChangePlayer(game);

	//kiểm tra lại vua 2 bên có bị chiếu chưa
	game->isBlackKingChecked = histElement.isBlackKingChecked;
	game->isWhiteKingChecked = histElement.isWhiteKingChecked;
}

bool gameIsCurrentPlayerChecked(Game * game) {
	//kiểm tra hiện tại đang tới lượt chơi bên nào
	if (gameGetCurrentPlayer(game) == White) {
		return game->isWhiteKingChecked;
	}

	return game->isBlackKingChecked;
}

static bool gameIsBlackPieceThreatened(ChessBoard gameBoard, BoardSquare s) {
	//khởi tạo mảng false 8x8
	bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER] = { {false} };

	//trường hợp nếu phạm vi bất kỳ quân cờ bên trắng trùng với tọa độ con cờ đen
	//trả về true
	for (int i = 0; i < BOARD_ROWS_NUMBER; i++) {
		for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
			if (islower(gameBoard[i][j])) {
				gameGetLegalMoves(gameBoard, movesBoard, (BoardSquare) { i, j });
				if (movesBoard[s.row][s.col]) return true;
			}
		}
	}

	//nếu không nằm vô bất kỳ phạm vi quân cờ trắng
	//trả về false
	return false;
}

static bool gameIsPieceThreatened(ChessBoard gameBoard, BoardSquare s) {
	bool retValue;
	
	//nếu là quân cờ đen kiểm tra có nằm trong phạm vi của cờ trắng hay không
	if (isupper(gameBoard[s.row][s.col])) return gameIsBlackPieceThreatened(gameBoard, s);

	//nếu không
	//đổi bên bàn cờ thành đen
	//kiểm tra có nằm trong phạm vi của cờ trắng hay không
	//đổi bên lại như cũ
	rotateBoardAndSwitchColors(gameBoard);
	retValue = gameIsBlackPieceThreatened(gameBoard, 
		(BoardSquare) { BOARD_ROWS_NUMBER - 1 - s.row, BOARD_COLUMNS_NUMBER - 1 - s.col });
	rotateBoardAndSwitchColors(gameBoard);

	//trả về true nếu cờ đen trong phạm vi cờ trắng
	//trả về false nếu cờ đen không trong phạm vi cờ đen
	return retValue;
}

static void gameDoesMoveCreateKingThreatOrPieceThreat(Game * game, BoardSquare from, BoardSquare to, 
	bool * moveCreatesKingThreat, bool * moveCreatesPieceThreat) {

	//lưu giá trị có phải quân cờ trắng hay không
	bool isWhitePiece = islower(game->gameBoard[from.row][from.col]);

	//sử dụng hàm di chuyển quân cờ trước để kiểm tra
	gameForceSetMove(game, from, to);

	//nếu là quân trắng kiểm tra lại con vua có đang bị chiếu
	//nếu là quân đen kiểm tra lại con vua có đang bị chiếu
	//kiểm tra quân cờ có nằm trong phạm vi bên còn lại không
	*moveCreatesKingThreat = isWhitePiece ? game->isWhiteKingChecked : game->isBlackKingChecked;
	*moveCreatesPieceThreat = gameIsPieceThreatened(game->gameBoard, to);

	//trả lại vị trí cũ
	gameForceUndoPrevMove(game);
}


void gameGetMovesByTypes(Game * game, bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER],
	MovesBoardWithTypes movesBoardWithTypes, BoardSquare s) {
	bool moveCaptures, moveCreatesPieceThreat = false, moveCreatesKingThreat = false;

	//khởi tạo các biến bool
	//moveCaptures: xác định có phải ô trống không
	//moveCreatesPieceThreat: xác định nước đi có thể bị ăn không
	//moveCreatesKingThreat: xác định nước đi có thể bị chiếu không
	for (int i = 0; i < BOARD_ROWS_NUMBER; i++) {
		for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
			//gán các ô không nằm phạm vi di chuyển con cờ gán = 0 mảng bool movesBoardWithTypes
			//ngược lại với mảng movesBoard
			if (!movesBoard[i][j]) movesBoardWithTypes[i][j] = BoardSquareInvalidMove;
			
			//trường hợp là ở phạm vi quân cờ
			else {
				//kiểm tra vị trí đó có quân cờ nào không
				//sử dụng hàm di chuyển trước để xem các vua 2 bên có đị chiếu hay quần cờ nào nằm trong phạm vi không
				moveCaptures = game->gameBoard[i][j] != BOARD_EMPTY_CELL;
				gameDoesMoveCreateKingThreatOrPieceThreat(game, s, (BoardSquare) { i, j }, 
					&moveCreatesKingThreat, &moveCreatesPieceThreat);

				//nếu trường hợp biến moveCreatesKingThreat = true
				//có nghĩa nước đi này chiếu con vua
				//gán vị trí nước đi chiếu trên movesBoardWithTypes = 5
				//sau đó kiểm tra đến ô tiếp theo
				if (moveCreatesKingThreat) {
					movesBoardWithTypes[i][j] = BoardSquareKingThreatMove;
					continue;
				}

				//trường hợp nếu ô là 1 con cờ 
				//và nước đi có thể bị ăn bởi quân cờ khác do nằm trên phạm vi di chuyển
				//gán vị trí nước đi có thể bị ăn trên movesBoardWithTypes = 4
				if (moveCaptures && moveCreatesPieceThreat) movesBoardWithTypes[i][j] = BoardSquareCaptureAndThreatMove;

				//trường hợp chỉ là một con cờ
				//với nước đi có thể ăn con cờ khác
				//gán vị trí nước đi có thể ăn con cờ khác trên movesBoardWithTypes = 2
				else if (moveCaptures) movesBoardWithTypes[i][j] = BoardSquareCaptureMove;

				//trường hợp là ô trống nhưng con cờ đi đến ô đó có thể ăn con khác
				//gán vị trí nước đi bình thường trên movesBoardWithTypes = 3
				else if (moveCreatesPieceThreat) movesBoardWithTypes[i][j] = BoardSquareThreatMove;

				//trường hợp còn lại là có thể đi đến bình thường
				//gán vị trí nước đi bình thường trên movesBoardWithTypes = 1
				else movesBoardWithTypes[i][j] = BoardSquareValidMove;
			}
		}
	}
}

ChessPlayer gameGetCurrentPlayer(Game * game) {
	//bắt đầu với bên trắng
	if (game == NULL) return White;

	//trả về giá trị người chơi hiện tại
	return game->currentPlayer;
}

ChessPlayer gameGetOtherPlayer(Game * game) {
	//bắt đầu với bên trắng
	if (game == NULL) return White;

	//trả về giá trị người chơi còn lại
	if (game->currentPlayer == White) return Black;
	return White;
}

/*
Checks if the given piece is a piece of the current player. 
*/
bool gameIsPieceOfCurrentPlayer(Game * game, BoardSquare s) {
	//trường hợp nếu người chơi là trắng thì tìm con cờ chữ thường trên bàn cờ
	if (gameGetCurrentPlayer(game) == White) return islower(game->gameBoard[s.row][s.col]);

	//ngược lại
	return isupper(game->gameBoard[s.row][s.col]);
}

/*
Like get moves by types, but doesn't require a movesBoard as an argument.
*/
static void gameGetMovesByTypesWrapper(Game * game, MovesBoardWithTypes movesBoardWithTypes, BoardSquare s) {
	//khởi tạo ma trận full false
	bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER] = { {false} };

	//lấy phạm vi đi của con cờ
	gameGetLegalMoves(game->gameBoard, movesBoard, s);
	
	//lấy cấp độ được đánh bởi nước đi
	gameGetMovesByTypes(game, movesBoard, movesBoardWithTypes, s);
}

GAME_MESSAGE gameSetMove(Game * game, BoardSquare from, BoardSquare to)
{
	MovesBoardWithTypes movesBoardWithTypes = { {BoardSquareInvalidMove} };
	BoardSquareMoveType moveType;

	//các điều kiện đảm bảo game khi di chuyển quân cờ
	//đảm bảo game được cấp phát thành công
	//đảm bảo quân cờ ở trong bàn cờ
	//đảm bảo người chơi cầm đúng quân cờ
	if (game == NULL) return GAME_INVALID_ARGUMENT;
	if (!gameIsSquareValid(from) || !gameIsSquareValid(to)) return GAME_INVALID_SQUARE;
	if (!gameIsPieceOfCurrentPlayer(game, from)) return GAME_INVALID_PIECE;

	//lấy cấp độ được đánh bởi nước đi dựa theo phạm vi quân cờ đối phương
	gameGetMovesByTypesWrapper(game, movesBoardWithTypes, from);

	//lưu trữ cấp độ nước đi
	moveType = movesBoardWithTypes[to.row][to.col];

	switch (moveType)
	{
	case BoardSquareValidMove:

	//cấp độ có thể đi đến ô trống để ăn quân cờ đối phương
	//cần di chuyển quân cờ
	case BoardSquareThreatMove:
		gameForceSetMove(game, from, to);
		return GAME_MOVE_SUCCESS;
	case BoardSquareCaptureMove:

	//cấp độ có thể đi đến ô trống để ăn quân cờ đối phương nhưng sẽ bị ăn lại
	//vẫn di chuyển quân cờ
	case BoardSquareCaptureAndThreatMove:
		gameForceSetMove(game, from, to);
		return GAME_MOVE_SUCCESS_CAPTURE;

	//cấp độ không nằm trong phạm vi di chuyển thì trả về không thể di chuyển
	case BoardSquareInvalidMove:
		return GAME_MOVE_INVALID;

	//cấp độ con vua bị chiếu sẽ trả về nguy hiểm cho con vua	
	case BoardSquareKingThreatMove:
		return GAME_MOVE_FAILED_KING_THREAT;
	default:
		break;
	}

	return GAME_MOVE_INVALID; // just for compilation
}

GAME_MESSAGE gameUndoPrevMove(Game * game) {
	//trường hợp game không được cấp phát trả về tham số không hợp lệ
	if (game == NULL) return GAME_INVALID_ARGUMENT;

	//trường hợp ArrayList quá khứ trống sẽ trả về không thể đi lại 
	if (arrayListIsEmpty(game->history)) return GAME_UNDO_NO_HISTORY;

	//sử dụng hàm đi lại
	//trả về đi lại thành công 
	gameForceUndoPrevMove(game);
	return GAME_UNDO_SUCCESS;
}

/*
Switch the player colors. Rotates the board and changes the current player.
*/
static void gameSwitchPlayersColors(Game * game) {
	//xoay lượt người chơi 
	rotateBoardAndSwitchColors(game->gameBoard);
	gameChangePlayer(game);
}

/*
Returns true iff the piece on square s has a valid move (a move that creates a threat 
on the current player is not valid).
*/
static bool gamePieceHasNextMove(Game * game, BoardSquare s) {
	MovesBoardWithTypes movesBoardWithTypes = { {BoardSquareInvalidMove} };

	//đánh giá cấp độ nước đi của con cờ
	gameGetMovesByTypesWrapper(game, movesBoardWithTypes, s);

	//kiểm tra tránh những ô không thể đi và làm con vua bị chiếu
	for (int i = 0; i < BOARD_ROWS_NUMBER; i++) {
		for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
			if (gameIsValidMove(movesBoardWithTypes[i][j])) return true;
		}
	}

	//không thể đi thêm false
	return false;
}

/*
True iff the current player has next move to do.
*/
static bool gameCurrentPlayerHasValidMoves(Game * game) {
	// Kiểm tra quân cờ trắng có thể đi tiếp hay không.
	bool switchPlayers = (gameGetCurrentPlayer(game) == Black);
	bool hasNextMove = false;
	
	if (switchPlayers) gameSwitchPlayersColors(game);

	for (int i = 0; i < BOARD_ROWS_NUMBER && !hasNextMove; i++) {
		for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
			if (islower(game->gameBoard[i][j])) {
				if (gamePieceHasNextMove(game, (BoardSquare) { i, j })) {
					hasNextMove = true;
					break;
				}
			}
		}
	}

	if (switchPlayers) gameSwitchPlayersColors(game);

	return hasNextMove;
}

GAME_CHECK_WINNER_MESSAGE gameCheckWinner(Game * game) {
	//trường hợp người chơi có thể đi tiếp
	if (gameCurrentPlayerHasValidMoves(game)) return GAME_CHECK_WINNER_CONTINUE;
	
	//trường hợp người chơi chiếu bí
	if (gameIsCurrentPlayerChecked(game)) return GAME_CHECK_WINNER_CURRENT_PLAYER_LOSE;

	//trường hợp người chơi hòa
	return GAME_CHECK_WINNER_DRAW;
}

GAME_MESSAGE gameGetMovesWrapper(Game * game, BoardSquare s, MovesBoardWithTypes movesBoardWithTypes) {
	//trường hợp báo tin ô không thể đi
	if (!gameIsSquareValid(s)) return GAME_INVALID_SQUARE;

	//trường hợp báo ô trống
	if (gameIsSquareEmpty(game->gameBoard, s)) return GAME_INVALID_PIECE;

	//trường hợp còn lại để đánh giá cấp độ mức đi
	gameGetMovesByTypesWrapper(game, movesBoardWithTypes, s);
	return GAME_SUCCESS;
}

char * gameGetCurrentPlayerColorText(Game * game) {
	//trường người chơi hiện tại bên trắng trả về "white"
	if (gameGetCurrentPlayer(game) == White) return "white";

	//trường người chơi hiện tại bên đen trả về "black"
	return "black";
}

bool gameIsValidMove(BoardSquareMoveType moveType) {
	//kiểm tra tránh trường hợp bị ô không thể đi và đi bị chiếu
	return !(moveType == BoardSquareInvalidMove || moveType == BoardSquareKingThreatMove);
}