#include "GuiWidget.h"

void destroyWidget(GuiWidget* src)
{
	//nếu dữ liệu Widget khác NULL cần phải giải phóng
	if (src != NULL) src->destroy(src);
}