#include <common.h>
#include <cd.h>
#include <cosmetic.h>

bool should_write_spyro_bmp = false;
bool should_write_flame_bmp = false;

extern SpyroColor spyro_color_index;
extern FlameColor flame_color_index;

//! SPYRO
void LoadSpyroBMPToMainRam(int sector)
{
	byte* spyro_bmp_main_ram_location = (byte*)0x800740B0;

	int spyro_skin_file_index = (spyro_color_index % 4) * 0x200/4; 	// In ints. %4, so that way it resets at the next sector
	int sector_index = spyro_color_index / 4;					// Seek to the next sector, every 4 skins (0x800 bytes)

	ReadFileIntoRam(sector + sector_index, 0x200/4, spyro_bmp_main_ram_location, spyro_skin_file_index);

	should_write_spyro_bmp = true;
	
}

void WriteSpyroBMPToVram()
{
	byte* spyro_bmp_main_ram_location = (byte*)0x800740B0;

	RECT spyro_area_1_rect;
	spyro_area_1_rect.w = 15;
	spyro_area_1_rect.x = 977;
	spyro_area_1_rect.y = 429;
	spyro_area_1_rect.h = 3;

	RECT spyro_area_2_rect;
	spyro_area_2_rect.w = 15;
	spyro_area_2_rect.x = 1009;
	spyro_area_2_rect.y = 481;
	spyro_area_2_rect.h = 14;
	
  	LoadImage(&spyro_area_1_rect, spyro_bmp_main_ram_location);

	LoadImage(&spyro_area_2_rect, spyro_bmp_main_ram_location + 92); //92 instead of 90, because int allignment

	should_write_spyro_bmp = false;
}

//! FLAME
void LoadFlameBMPToMainRam(int sector)
{
	int* flame_bmp_main_ram_location = (int*)0x80074300;

	int flame_skin_file_index = flame_color_index * (0x20/4); // in ints

	ReadFileIntoRam(sector, 0x20/4, flame_bmp_main_ram_location, flame_skin_file_index);

	should_write_flame_bmp = true;
	
}

void WriteFlameBMPToVram()
{
	int* flame_bmp_main_ram_location = (int*)0x80074300;

 	RECT flame_rect;
	flame_rect.w = 15;
	flame_rect.x = 977;
	flame_rect.y = 385;
	flame_rect.h = 1;
	
  	LoadImage(&flame_rect, flame_bmp_main_ram_location);


	should_write_flame_bmp = false;
}





// extern bool should_reload_test_skins;

// void SkinTester(void)
// {
// 	if (should_reload_test_skins)
// 	{
// 		byte* spyro_bmp_main_ram_location = (byte*)0x800740B0;
// 		byte* fire_bmp_main_ram_location = (byte*)0x800742B0;

// 		RECT area_1_rect;
// 		area_1_rect.w = 15;
// 		area_1_rect.x = 977;
// 		area_1_rect.y = 429;
// 		area_1_rect.h = 3;

// 		RECT area_2_rect;
// 		area_2_rect.w = 15;
// 		area_2_rect.x = 1009;
// 		area_2_rect.y = 481;
// 		area_2_rect.h = 14;

// 		RECT flame_rect;
// 		flame_rect.w = 15;
// 		flame_rect.x = 977;
// 		flame_rect.y = 385;
// 		flame_rect.h = 1;
		
// 		if (*spyro_bmp_main_ram_location != 0)
// 		{
// 			LoadImage(&area_1_rect, spyro_bmp_main_ram_location);

// 			LoadImage(&area_2_rect, spyro_bmp_main_ram_location + 92); //92 instead of 90, because int allignment
// 		}
// 		if (*fire_bmp_main_ram_location != 0)
// 		{
// 			LoadImage(&flame_rect, fire_bmp_main_ram_location); //92 instead of 90, because int allignment
// 		}

// 		should_write_bmp = false;

// 		should_reload_test_skins = false;
// 	}
// 	else
// 	{
// 		return;
// 	}
// }