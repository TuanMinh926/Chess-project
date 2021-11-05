#include "GameHandler.h"

GameHandler * gameHandlerNewGame(GhSettings settings) {
    //cấp phát vùng nhớ cho GameHandler
    //hàm trả về NULL nếu cấp phát không thành công
	GameHandler * gh = malloc(sizeof(GameHandler));
	if (gh == NULL) return NULL;

    //gán chế độ chơi 1 mình hay chơi 2 người
    //gán mức độ khó
    //gán bên trăng, đen
	gh->settings.gameMode = settings.gameMode;
	gh->settings.difficultyLevel = settings.difficultyLevel;
	gh->settings.userColor = settings.userColor;

    //vì là setting với new game
    //mặc định là chưa save game
	gh->gameIsSaved = false;
	
	//khởi tạo game
    //kích thước khởi tạo ArrayList quá khứ:
    //size 3 với mỗi người chơi = 6
    //1 size để rỗng dành các thao tác thêm , chèn của ArrayList
    //cộng thêm size độ khó của máy
    //nếu độ khó càng cao kích thước ArrayList quá khứ càng lớn
    //nếu kích thước ArrayList càng lớn thì thuật toán minimax càng thông minh hơn
    //nếu khởi tạo không thành công hàm trả về NULL
	// the size of the history allows using the minimax algorithm
	gh->game = gameCreate(GH_DEFAULT_HISTORY_SIZE + gh->settings.difficultyLevel);
	if (gh->game == NULL) return NULL;

	return gh;
}

bool gameHandlerRestartGame(GameHandler * gh) {
	Game * prevGame = gh->game;

    //khởi tạo lại game trờ lại ban đầu
    //nếu khởi tạo không thành công
    //giữ nguyên game hàm trả về false
	gh->game = gameCreate(GH_DEFAULT_HISTORY_SIZE + gh->settings.difficultyLevel);
	if (gh->game == NULL) {
		gh->game = prevGame;
		return false;
	}

    //gán giá trị mặc định lại cho trạng thái Save
	gh->gameIsSaved = false;

    //giải phóng game cũ
	gameDestroy(prevGame);
	return true;
}

bool gameHandlerIsUserTurn(GameHandler * gh) {
	ChessPlayer currentPlayer;

    //kiểm gia chế độ cơi 2 người 
	if (gh->settings.gameMode == GameModeMultiPlayer) return true;

    //lấy giá trị người chơi hiện tại
	currentPlayer = gameGetCurrentPlayer(gh->game);

    //đảm bảo người chơi đúng bên
	return (currentPlayer == White && gh->settings.userColor == UserColorWhite) ||
		(currentPlayer == Black && gh->settings.userColor == UserColorBlack);
}

void gameHandlerDestroy(GameHandler * gh) {
    //giải phóng GameHandler nếu khác NULL
	if (gh == NULL) return;

	gameDestroy(gh->game);
	free(gh);
}

GhSettings gameHandlerGetDefaultSettings() {
    //gán các giá trị mặc định
    //chế độ chơi 1 người
    //chế độ dễ
    //bên trắng
	GhSettings settings;

	settings.gameMode = GameModeSinglePlayer;
	settings.difficultyLevel = GameDifficultyEasy;
	settings.userColor = UserColorWhite;

	return settings;
}

void gameHandlerGameElementAddedToHistory(GameHandler * gh) {
    //đảm bảo kích thước ArrayListSize bằng với khai báo
    //nếu không sẽ lấy phần tử đầu tiền ra
	if (arrayListSize(gh->game->history) > GH_GAME_HISTORY_SIZE) {
		arrayListRemoveFirst(gh->game->history);
	}
}

void gameHandlerComputerTurn(GameHandler * gh) {
    //sử dụng nước đi thuật toán minimax đề xuất
    //di chuyển nước cờ
    //kiểm tra lại kích ArrayList quá khứ
	Move suggestMove = minimaxSuggestMove(gh->game, gh->settings.difficultyLevel);
	gameSetMove(gh->game, suggestMove.oldSquare, suggestMove.newSquare);
	gameHandlerGameElementAddedToHistory(gh);
}

void gameHandlerPrintGameSettingsToFileHandler(FILE * fh, GhSettings settings) {
    //hàm in các giá trị setting vào file save
	char * difficulty;

	fprintf(fh, "SETTINGS:\n");
	fprintf(fh, "GAME_MODE: %s\n", (settings.gameMode == GameModeSinglePlayer) ? "1-player" : "2-player");

	if (settings.gameMode == GameModeSinglePlayer) {
		switch (settings.difficultyLevel) {
		case GameDifficultyAmateur:
			difficulty = "amateur";
			break;
		case GameDifficultyEasy:
			difficulty = "easy";
			break;
		case GameDifficultyModerate:
			difficulty = "moderate";
			break;
		case GameDifficultyHard:
			difficulty = "hard";
			break;
		case GameDifficultyExpert:
			difficulty = "expert";
			break;
		}

		fprintf(fh, "DIFFICULTY: %s\n", difficulty);
		fprintf(fh, "USER_COLOR: %s\n", settings.userColor == UserColorBlack ? "black" : "white");
	}
}

bool gameHandlerSaveGame(GameHandler * gh, char * path) {
    //hàm kiểm tra save file thành công hay không
    //nếu thành công sẽ ghi file
	FILE * fh = fopen(path, "w");

	if (fh == NULL) return false;
	
	fprintf(fh, "%s\n", gameGetCurrentPlayerColorText(gh->game));
	gameHandlerPrintGameSettingsToFileHandler(fh, gh->settings);
	gamePrintGameBoardToFileHandler(fh, gh->game);

	if (fclose(fh) == 0) { // in success, fclose == 0
		gh->gameIsSaved = true;
		return true;
	}

	return false;
}

/*
Convert difficulty text to enum.
*/
static GhGameDifficultyLevel gameHandlerDifficultyTextToEnum(char * difficulty) {
    //hàm so sánh chuỗi tên độ khó 
    //trả về những độ khó tương ứng
	if (strcmp(difficulty, "easy") == 0) return GameDifficultyEasy;
	if (strcmp(difficulty, "moderate") == 0) return GameDifficultyModerate;
	if (strcmp(difficulty, "hard") == 0) return GameDifficultyHard;
	if (strcmp(difficulty, "expert") == 0) return GameDifficultyExpert;
	else return GameDifficultyAmateur;
}

GameHandler * gameHandlerLoadGame(char * path) {
    //hàm load game từ file
	bool success = false, currentPlayerBlack = false;
	int numberOfPlayers, i, lineNum;
	char line[GH_SAVE_FILE_MAX_LINE_LENGTH];
	char difficulty[GH_MAX_DIFFICULTY_LENGTH], userColor[GH_MAX_USER_COLOR_LENGTH];
	GameHandler * gh = NULL;
	GhSettings settings = gameHandlerGetDefaultSettings();
	FILE * fh = fopen(path, "r");

	if (fh == NULL) return NULL;

	// this loop (actually, one iteration) is used to prevent repeating the error handling code
	do {
		//lấy nhãn người đang chơi (trắng hoặc đen)
		// read first line - current player
		if (fscanf(fh, "%s\n", line) != 1) break;

		//so sánh chuỗi người chơi hiện tại
        //true nếu bên đen
        //false nếu bên trắng
		// set the current player. White always starts, so just change to black if it's the black player
		if (strcmp(line, "black") == 0) currentPlayerBlack = true;

		//bỏ qua dòng chữ  SETTINGS
		// SETTINGS line
		if (fgets(line, GH_SAVE_FILE_MAX_LINE_LENGTH - 1, fh) == NULL) break;

		//lấy số người chơi
		// number of players
		if (fscanf(fh, "GAME_MODE: %d-player\n", &numberOfPlayers) != 1) break;
		settings.gameMode = (numberOfPlayers == 1) ? GameModeSinglePlayer : GameModeMultiPlayer;

		if (numberOfPlayers == 1) {
			//lấy độ khó
            //sử dụng hàm so sánh chuỗi độ khó
            //gán giá trị độ khó
			// difficulty
			if (fscanf(fh, "DIFFICULTY: %s\n", difficulty) != 1) break;
			settings.difficultyLevel = gameHandlerDifficultyTextToEnum(difficulty);

			//lấy nhãn người chơi
            //so sánh nhãn có phải white nếu không là black
			// user color
			if (fscanf(fh, "USER_COLOR: %s\n", userColor) != 1) break;
			settings.userColor = (strcmp(userColor, "white") == 0) ? UserColorWhite : UserColorBlack;
		}

		//khởi tạo game với settings đã lưu
		// create the game handler
		gh = gameHandlerNewGame(settings);

		if (gh == NULL) break;

		//kiểm tra nếu là bên đen cần đổi bên chơi
		// check if need to switch players
		if (currentPlayerBlack) gameChangePlayer(gh->game);

		//đọc từng hàng bàng cờ theo định đạng và lưu lại nhãn trên bàn cờ
		// set board!
		for (i = BOARD_ROWS_NUMBER - 1; i >= 0; i--) {
			if (fscanf(fh, "%d| %c %c %c %c %c %c %c %c |\n", 
				&lineNum, 
				&gh->game->gameBoard[i][0], &gh->game->gameBoard[i][1],
				&gh->game->gameBoard[i][2], &gh->game->gameBoard[i][3],
				&gh->game->gameBoard[i][4], &gh->game->gameBoard[i][5],
				&gh->game->gameBoard[i][6], &gh->game->gameBoard[i][7]) 
				!= BOARD_COLUMNS_NUMBER + 1) break;
		}

		//kiểm tra đảm bảo load hết bàn cờ
		// finished scanning all lines successfully
		if (i == -1) success = true;
	} while (0);

	fclose(fh);
	
    //nếu đọc không thành công
    //giải phóng game lỗi
	if (!success) {
		gameHandlerDestroy(gh);
		return NULL;
	}

    //nếu đọc thành công
    //gán trạng thái đã save
    //kiểm tra con vua 2 bên có đang bị chiếu không
	gh->gameIsSaved = true;
	gh->game->isWhiteKingChecked = gameBoardKingIsChecked(gh->game->gameBoard, White);
	gh->game->isBlackKingChecked = gameBoardKingIsChecked(gh->game->gameBoard, Black);

	return gh;
}