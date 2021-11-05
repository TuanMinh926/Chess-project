#include "GuiHelpers.h"

SDL_Texture * guiTextureFromBMP(SDL_Renderer * rend, char * imagePath) {
	//SDL_Surface lưu trữ dữ liệu từ ảnh
	//khởi tạo SDL_Texture
	SDL_Surface * surface = SDL_LoadBMP(imagePath);
	SDL_Texture * texture = NULL;

	//nếu lấy dữ liệu ảnh không thành công
	//in lỗi
	//hàm trả về NULL
	if (surface == NULL) {
		printf("ERROR: loading BMP failed: %s\n", SDL_GetError());
		return NULL;
	}
	
	//tạo ảnh từ dữ liệu lưu trữ
	//dữ liệu lưu trữ không cần thiết nữa giải phóng dữ liệu
	texture = SDL_CreateTextureFromSurface(rend, surface);
	SDL_FreeSurface(surface);

	//nếu tạo ảnh không thành công
	//báo lỗi
	if (texture == NULL) printf("ERROR: texture creation failed: %s\n", SDL_GetError());

	return texture;
}

void guiPushUserEvent(GuiUserEventCode code, void * data1, void * data2) {
	SDL_Event e;
	SDL_memset(&e, 0, sizeof(e));
	
	//gán loại sự kiện người chơi
	//gán list các trạng thái của người chơi
	//lưu dữ liệu của 2 người chơi
	e.type = SDL_USEREVENT;
	e.user.code = code;
	e.user.data1 = data1;
	e.user.data2 = data2;
	
	//thêm dữ liệu sự kiện vào hàng đợi
	SDL_PushEvent(&e);
}

SDL_Texture * guiTransparentTextureFromBMP(SDL_Renderer * rend, char * imagePath, Uint8 r, Uint8 g, Uint8 b) {
	//SDL_Surface lưu trữ dữ liệu từ ảnh
	//khởi tạo SDL_Texture
	SDL_Surface * surface = SDL_LoadBMP(imagePath);
	SDL_Texture * texture = NULL;

	//nếu lấy dữ liệu ảnh không thành công
	//in lỗi
	//hàm trả về NULL
	if (surface == NULL) {
		printf("ERROR: loading BMP failed: %s\n", SDL_GetError());
		return NULL;
	}

	//Sử dụng SDL_SetColorKey để điều chỉnh độ trong suốt màu của ảnh
	// set a specific color to transparent
	SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, r, g, b));

	//tạo ảnh từ dữ liệu lưu trữ
	//dữ liệu lưu trữ không cần thiết nữa giải phóng dữ liệu
	texture = SDL_CreateTextureFromSurface(rend, surface);
	SDL_FreeSurface(surface);

	//nếu tạo ảnh không thành công
	//báo lỗi
	if (texture == NULL) printf("ERROR: texture creation failed: %s\n", SDL_GetError());

	return texture;
}

void guiShowMessageBox(const char * title, const char * msg) {
	//hàm kiểm tra nếu xảy ra lỗi sẽ hiện tin nhắn
	// the if statements catches en error
	if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
		title,
		msg,
		NULL) < 0) {
		printf("ERROR: message box creation failed: %s\n", SDL_GetError());
		printf("The message:\n%s: %s\n", title, msg);
	}
}

bool guiDoesFileExist(const char *fname) {
	//kiểm tra mở file thành công hay không
	FILE *file;
	if ((file = fopen(fname, "r")))
	{
		fclose(file);
		return 1;
	}
	return 0;
}