#include <common.h>
#include <custom_text.h>

void RenderShadedMobyQueue()
{
	memset((void*)0x8006fcf4, 0, 0x900);
	_ptrTextUnk = 0;
	CopyHudToShaded();
	DrawShadedMobys();
}