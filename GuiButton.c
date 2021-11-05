#include "GuiButton.h"
#include <stdio.h>
#include <stdlib.h>

GuiWidget* createButton(
	SDL_Renderer* renderer,
	const char* image,
	SDL_Rect location,
	void(*action)(void))
{
	//cấp phát vùng nhớ động cho Widget
	//trường hợp cấp phát không thành công hàm trả về NULL
	// allocate data
	GuiWidget* res = (GuiWidget*)malloc(sizeof(GuiWidget));
	if (res == NULL)
		return NULL;

	//cấp phát vùng nhớ động cho Button
	//trường hợp cấp phát không thành công
	//giải phóng vùng nhớ Widget đã khởi tạo
	//hàm trả về NULL
	GuiButton* data = (GuiButton*)malloc(sizeof(GuiButton));
	if (data == NULL) {
		free(res);
		return NULL;
	}

	//lấy dữ liệu từ ảnh lưu vào SDL_Surface
	//nếu lấy không thành công
	//giải phóng Widget đã cấp phát
	//giải phóng Button đã cấp phát
	//hàm trả về NULL
	// we use the surface as a temp var
	SDL_Surface* surface = SDL_LoadBMP(image);
	if (surface == NULL) {
		free(res);
		free(data);
		return NULL;
	}

	//tạo ảnh từ dữ liệu ảnh đã lưu vào SDL_Texture
	//nếu lấy không thành công
	//giải phóng Widget đã cấp phát
	//giải phóng Button đã cấp phát
	//giả phóng SDL_Surface
	//hàm trả về NULL
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == NULL) {
		free(res);
		free(data);
		SDL_FreeSurface(surface);
		return NULL;
	}

	//nếu đã tạo thành công ảnh
	//SDL_Surface dữ liệu lưu trữ không cần thiết nữa
	//cần phải giải phóng SDL_Surface
	// surface is not needed after texture is created
	SDL_FreeSurface(surface);

	//lưu trữ dữ liệu ảnh
	//lưu trữ vị trí Button
	//lưu trữ chức năng Button
	//lưu trữ phương thức xóa Button
	//lưu trữ phương thức vẽ Button
	//lưu trữ phương thức kiểm soát vùng tạo chức năng
	//hàm trả về Widget đã lưu trữ đầy đủ dữ liệu
	// store button & widget details
	data->texture = texture;
	data->location = location;
	data->action = action;
	res->destroy = destroyButton;
	res->draw = drawButton;
	res->handleEvent = handleButtonEvent;
	res->data = data;
	return res;
}

void destroyButton(GuiWidget* src)
{
	//cần trỏ đến dữ liệu Widget
	//giải phóng ảnh đã được cấp phát trong dữ liệu
	//giải phóng tất cả dữ liệu của Button
	//giải phóng Widget
	GuiButton* button = (GuiButton*)src->data;
	SDL_DestroyTexture(button->texture);
	free(button);
	free(src);
}

void handleButtonEvent(GuiWidget* src, SDL_Event* e)
{
	//nếu loại sự kiên xảy ra == 1026 có nghĩa click vào Button
	//lấy dữ liệu Button từ Widget
	//lấy vị trí click bằng SDL_Point
	//sử dụng SDL_PointInRect
	//true nếu nếu click nằm trong phạm vi Button
	//nếu thỏa thực hiện chức năng Button
	if (e->type == SDL_MOUSEBUTTONUP) {
		GuiButton* button = (GuiButton*)src->data;
		SDL_Point point = { .x = e->button.x,.y = e->button.y };

		if (SDL_PointInRect(&point, &button->location)) {
			button->action();
		}
	}
}

void drawButton(GuiWidget* src, SDL_Renderer* render)
{
	//lấy dữ liệu Button từ Widget
	//sử dụng SDL_RenderCopy để lấy dữ liệu ra vẽ mà không mất dữ liệu gốc
	//tham số cần truyền : ảnh, vị trí Button
	GuiButton* button = (GuiButton*)src->data;
	SDL_RenderCopy(render, button->texture, NULL, &button->location);
}
