#include <common.h>
#include <cd.h>
#include <cosmetic.h>

bool should_write_spyro_bmp = false;
bool should_write_flame_bmp = false;

extern SpyroColor spyro_color_index;
extern FlameColor flame_color_index;

//! FLAME TRIANGLE

// Custom Gradients Array
FlameTriangleColorGradient CUSTOM_FLAME_TRIANGLE_GRADIENTS[5] =
{
	//0 Original
	{
		.tip.r = 0xF0,
		.tip.g = 0xF0,
		.tip.b = 0x60,
		.tip.shape_code = 0x30,

		.center.r = 0xD0,
		.center.g = 0x90,
		.center.b = 0x40,
		.center.shape_code = 0x30,
		
		.right.r = 0x90,
		.right.g = 0x60,
		.right.b = 0x20,
		.right.shape_code = 0x30,

		.left.r = 0xA0,
		.left.g = 0x30,
		.left.b = 0x10,
		.left.shape_code = 0x30,
	},
	//1 Blood Red
	{
        .tip.r = 0xb1,
		.tip.g = 0x3a,
		.tip.b = 0x3a,
		.tip.shape_code = 0x30,

		.center.r = 0x7b,
		.center.g = 0x2b,
		.center.b = 0x2b,
		.center.shape_code = 0x30,
		
		.right.r = 0xb7,
		.right.g = 0x28,
		.right.b = 0x28,
		.right.shape_code = 0x30,

		.left.r = 0xab,
		.left.g = 0x22,
		.left.b = 0x22,
		.left.shape_code = 0x30
	},
	//2 Icy
	{
		.tip.r = 0x46,
		.tip.g = 0xBF,
		.tip.b = 0xBF,
		.tip.shape_code = 0x30,

		.center.r = 0xDE,
		.center.g = 0xF9,
		.center.b = 0xFE,
		.center.shape_code = 0x30,
		
		.right.r = 0x4B,
		.right.g = 0xFF,
		.right.b = 0xF9,
		.right.shape_code = 0x30,

		.left.r = 0x56,
		.left.g = 0xE3,
		.left.b = 0xEE,
		.left.shape_code = 0x30,
	},
	//3 Jade
	{
        .tip.r = 0x87,
		.tip.g = 0xff,
		.tip.b = 0xda,
		.tip.shape_code = 0x30,

		.center.r = 0x28,
		.center.g = 0x74,
		.center.b = 0x5c,
		.center.shape_code = 0x30,
		
		.right.r = 0x2,
		.right.g = 0xa2,
		.right.b = 0x6c,
		.right.shape_code = 0x30,

		.left.r = 0x0,
		.left.g = 0x98,
		.left.b = 0x61,
		.left.shape_code = 0x30
	},
	//4 Ghost
	{
        .tip.r = 0xd2,
		.tip.g = 0xd2,
		.tip.b = 0xd2,
		.tip.shape_code = 0x30,

		.center.r = 0x8d,
		.center.g = 0x8d,
		.center.b = 0x8d,
		.center.shape_code = 0x30,
		
		.right.r = 0xcf,
		.right.g = 0xcf,
		.right.b = 0xcf,
		.right.shape_code = 0x30,

		.left.r = 0xbd,
		.left.g = 0xbd,
		.left.b = 0xbd,
		.left.shape_code = 0x30
	}
};

void ChangeFlameTriangleColor(FlameTriangleColorGradient new_color_gradient)
{
	_flame_triangle_color_gradient = new_color_gradient;
}


//! SPYRO SKIN
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

//! FLAME SKIN
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

	ChangeFlameTriangleColor(CUSTOM_FLAME_TRIANGLE_GRADIENTS[flame_color_index]); //Change Triangle Gradient

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