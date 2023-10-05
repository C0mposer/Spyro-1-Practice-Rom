#include <common.h>
#include <custom_text.h>

void MobyRender()
{
    memset((void*)0x8006fcf4, 0, 0x900);
	_ptrTextUnk = 0;
	CopyHudToShaded();
	DrawShadedMobys();
}