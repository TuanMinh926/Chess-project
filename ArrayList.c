#include "ArrayList.h"

ArrayList* arrayListCreate(int maxSize) {
	//nếu giá trị truyền vào maxSize <=0 thì sẽ khởi tạo không thành trả về NULL
	if (maxSize <= 0) return NULL;

    //cấp phát vùng nhớ động dựa vào size ArrayList
    //trường hợp cấp phát không thành công có nghĩa sẽ trỏ về NULL hàm sẽ trả về NULL
	ArrayList *al = (ArrayList*)(malloc(sizeof(ArrayList)));
	if ((void*)al == NULL) return NULL;

    //cấp phát vùng nhớ động có kích thước của HistoryElement với số lượng maxSize
    //trường hợp cấp phát không thành công cần giải phóng ArrayList đã khởi tạo rồi hàm sẽ trả về NULL
	al->elements = (HistoryElement*)(malloc(sizeof(HistoryElement) * maxSize));
	if ((void*)(al->elements) == NULL) {
		free(al);
		return NULL;
	}

	al->maxSize = maxSize;
	al->actualSize = 0;

	return al;
}

void arrayListDestroy(ArrayList* src) {
    //trường hợp ArrayList đã khởi không thành công không cần giải phóng vùng nhớ
	if ((void*)src == NULL) return;

	//giải phóng vùng nhớ của HistoryElement đến ArrayList
	free(src->elements);
	free(src);
}

ARRAY_LIST_MESSAGE arrayListAddAt(ArrayList* src, HistoryElement elem, int index) {
    //các trường hợp tham số truyền vào không đúng
    //trả về trạng thái tham số sai = 1
    //trường hợp ArrayList không được cấp vùng nhớ
    //trường hợp vị trí cần thêm vào <0
    //trường hợp vị trí thêm vào lớn hơn kích thước ArrayList
	if ((void*)src == NULL || index < 0 || index > src->actualSize) {
		return ARRAY_LIST_INVALID_ARGUMENT;
	}

    //trường hợp ArrayList đã đầy không thể thêm vào
    //trả về trạng thái ArrayList đã đầy = 2
	if (src->actualSize == src->maxSize) return ARRAY_LIST_FULL;

    //đôn phần tử thứ i lên i+1 từ vị trí chèn đến cuối mảng
    //gán phần tử chèn vào vị trí index
    //tăng kích thước ArrayList thêm 1
	for (int i = src->actualSize - 1; i >= index; i--) {
		(src->elements)[i + 1] = (src->elements)[i];
	}

	(src->elements)[index] = elem;
	(src->actualSize)++;

    //trả về trạng thái thao tác thành công
	return ARRAY_LIST_SUCCESS;
}

ARRAY_LIST_MESSAGE arrayListAddFirst(ArrayList* src, HistoryElement elem) {
	//sử dụng lại hàm thêm theo vị trí nhưng với vị trí đầu tiên
	return arrayListAddAt(src, elem, 0);
}

ARRAY_LIST_MESSAGE arrayListAddLast(ArrayList* src, HistoryElement elem) {
    //trường hợp ArrayList không được cấp phát
    //trả về trạng thái tham số sai = 1
	if ((void*)src == NULL) return ARRAY_LIST_INVALID_ARGUMENT;

    //trường hợp ArrayList đầy không thể thêm
    //trả về trạng thái ArrayList đã đầy = 2
	if (src->actualSize == src->maxSize) return ARRAY_LIST_FULL;

    //sử dụng lại hàm thêm theo vị trí với vị trí là kích thước ArrayList
	return arrayListAddAt(src, elem, src->actualSize);
}

ARRAY_LIST_MESSAGE arrayListRemoveAt(ArrayList* src, int index) {
	//các trường hợp tham số truyền vào không đúng
    //trả về trạng thái tham số sai = 1
    //trường hợp ArrayList không được cấp vùng nhớ
    //trường hợp vị trí cần xóa vào <0
    //trường hợp vị trí xóa lớn hơn hoặc bằng kích thước ArrayList
	if ((void*)src == NULL || index < 0 || index >= src->actualSize) {
		return ARRAY_LIST_INVALID_ARGUMENT;
	}

	//trường hợp kích thước = 0 không thể xóa
    //trả về trạng thái ArrayList trống = 3
	if (src->actualSize == 0) return ARRAY_LIST_EMPTY;

	//đôn phần tử i+1 về i đến cuối mảng
    //giảm kích thước ArrayList đi 1
	for (int i = index + 1; i < src->actualSize; i++) {
		(src->elements)[i - 1] = (src->elements)[i];
	}

	(src->actualSize)--;

    //trả về trạng thái thao tác thành công
	return ARRAY_LIST_SUCCESS;
}

ARRAY_LIST_MESSAGE arrayListRemoveFirst(ArrayList* src) {
    //trường hợp ArrayList không được cấp phát
    //trả về trạng thái tham số sai = 1
	if ((void*)src == NULL) return ARRAY_LIST_INVALID_ARGUMENT;

    //trường hợp ArrayList trống không thể xóa
    //trả về trạng thái ArrayList trống = 3
	if (src->actualSize == 0) return ARRAY_LIST_EMPTY;

    //sử dụng lại hàm xóa theo vị trí với vị trí đầu tiên
	return arrayListRemoveAt(src, 0);
}

ARRAY_LIST_MESSAGE arrayListRemoveLast(ArrayList* src) {
	if ((void*)src == NULL) return ARRAY_LIST_INVALID_ARGUMENT;

	if (src->actualSize == 0) return ARRAY_LIST_EMPTY;

	return arrayListRemoveAt(src, src->actualSize - 1);
}

HistoryElement arrayListGetAt(ArrayList* src, int index) {
	//các trường hợp không thể lấy HistoryElement trong ArrayList
    //trả về vị trí trên bàn cờ dòng -1, cột -1
    //trường hợp ArrayList không được cấp vùng nhớ
    //trường hợp vị trí cần lấy <0
    //trường hợp vị trí lấy lớn hơn hoặc bằng kích thước ArrayList
	if ((void*)src == NULL || index < 0 || index >= src->actualSize) {
		return (HistoryElement) { .oldSquare = { -1, -1 } };
	}

    //lấy theo vị trí index
	return src->elements[index];
}

HistoryElement arrayListGetFirst(ArrayList* src) {
    //sử dụng lại hàm lấy theo vị trí với vị trí đầu tiên
	return arrayListGetAt(src, 0);
}

HistoryElement arrayListGetLast(ArrayList* src) {
    //trường hợp ArrayList không được cấp phát
    //trả về vị trí trên bàn cờ dòng -1, cột -1
	if ((void*)src == NULL) return (HistoryElement) { .oldSquare = { -1, -1 } };

    //sử dụng lại hàm lấy theo vị trí với vị trí cuối ArrayList
	return arrayListGetAt(src, src->actualSize - 1);
}

int arrayListMaxCapacity(ArrayList* src) {
    //trường hợp ArrayList không được cấp phát kích thước max = -1
	if ((void*)src == NULL) return -1;

    //trả về kích thước max của Arraylist
	return src->maxSize;
}

int arrayListSize(ArrayList* src) {
    //trường hợp ArrayList không được cấp phát kích thước thật = -1
	if ((void*)src == NULL) return -1;

    //trả về kích thước thật của ArrayList
	return src->actualSize;
}

bool arrayListIsFull(ArrayList* src) {
    //trường hợp ArrayList không được cấp phát
    //trả về false
	if ((void*)src == NULL) return false;

    //trả về true nếu kích thước thật = kích thước max
	return src->actualSize == src->maxSize;
}

bool arrayListIsEmpty(ArrayList* src) {
    //trường hợp ArrayList không được cấp phát
    //trả về false
	if ((void*)src == NULL) return false;

    //trả về true nếu kích thước thật = 0
	return src->actualSize == 0;
}