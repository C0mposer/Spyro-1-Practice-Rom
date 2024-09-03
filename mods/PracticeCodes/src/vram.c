#include <common.h>
#include <cd.h>
#include <bg_colors.h>

bool should_write_bmp = false;

extern SpyroColor spyro_color_index;

void LoadBMPToMainRam(int sector)
{
	byte* bmp_main_ram_location = (byte*)0x800740B0;

	int skin_file_index = spyro_color_index * 0x1000;

	LoadCdData(sector, bmp_main_ram_location, 0x200, skin_file_index, 600);

	should_write_bmp = true;
	
}

void WriteBMPToSpyroVram()
{
	byte* bmp_main_ram_location = (byte*)0x800740B0;

	RECT area_1_rect;
	area_1_rect.w = 15;
	area_1_rect.x = 977;
	area_1_rect.y = 429;
	area_1_rect.h = 3;

	RECT area_2_rect;
	area_2_rect.w = 15;
	area_2_rect.x = 1009;
	area_2_rect.y = 481;
	area_2_rect.h = 14;
	
  	LoadImage(&area_1_rect, bmp_main_ram_location);

	LoadImage(&area_2_rect, bmp_main_ram_location + 92); //92 instead of 90, because int allignment

	should_write_bmp = false;
}


extern bool should_reload_test_skins;

void SkinTester(void)
{
	if (should_reload_test_skins)
	{
		byte* spyro_bmp_main_ram_location = (byte*)0x800740B0;
		byte* fire_bmp_main_ram_location = (byte*)0x800742B0;

		RECT area_1_rect;
		area_1_rect.w = 15;
		area_1_rect.x = 977;
		area_1_rect.y = 429;
		area_1_rect.h = 3;

		RECT area_2_rect;
		area_2_rect.w = 15;
		area_2_rect.x = 1009;
		area_2_rect.y = 481;
		area_2_rect.h = 14;

		RECT flame_rect;
		flame_rect.w = 15;
		flame_rect.x = 977;
		flame_rect.y = 385;
		flame_rect.h = 1;
		
		if (*spyro_bmp_main_ram_location != 0)
		{
			LoadImage(&area_1_rect, spyro_bmp_main_ram_location);

			LoadImage(&area_2_rect, spyro_bmp_main_ram_location + 92); //92 instead of 90, because int allignment
		}
		if (*fire_bmp_main_ram_location != 0)
		{
			LoadImage(&flame_rect, fire_bmp_main_ram_location); //92 instead of 90, because int allignment
		}

		should_write_bmp = false;

		should_reload_test_skins = false;
	}
	else
	{
		return;
	}
}