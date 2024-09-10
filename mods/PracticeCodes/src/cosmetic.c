#include <common.h>
#include <cd.h>
#include <cosmetic.h>
#include <flame_triangle_colors.h>
#include <sparx_glow_colors.h>

const RedGreen bg_colors[7] = {{0x0, 0x25}, {0x40, 0x18}, {0x00, 0x50}, {0x50, 0x50}, {0x70, 0}, {0xCD, 0x80}, {0xA0, 0xA0}};

bool should_write_spyro_bmp = false;
bool should_write_flame_bmp = false;
bool should_write_sparx_bmp = false;

extern SpyroColor spyro_color_index;
extern FlameColor flame_color_index;
extern SparxColor sparx_color_index;
extern BackgroundColor bg_color_index;

extern bool should_update_bg_color;
extern bool should_load_spyro_color;
extern bool should_load_flame_color;
extern bool should_load_sparx_color;
void* previous_sparx_ptr = NULL;

//! BG COLOR
//Changing asm instructions for pause menu RGB. Cannot change B value, as the value is in a shared register with other crucial parts of the struct.
inline void SetTitleScreenColor(byte r, byte g)
{
    *(short*)(0x8001A674) = r;
    *(short*)(0x8001A67C) = g;
}

//! FLAME TRIANGLE
void ChangeFlameTriangleColor(FlameTriangleColorGradient new_color_gradient)
{
	_flame_triangle_color_gradient = new_color_gradient;
}


//! SPYRO SKIN
void LoadSpyroBMPToMainRam(int sector)
{
	const int SPYRO_BMP_FILE_SIZE = 0x200;

	byte* spyro_bmp_main_ram_location = (byte*)0x800740B0;

	int spyro_skin_file_index = (spyro_color_index % 4) * SPYRO_BMP_FILE_SIZE/4; 	// In ints. %4, so that way it resets at the next sector
	int sector_index = spyro_color_index / 4;					// Seek to the next sector, every 4 skins (0x800 bytes)

	ReadFileIntoRam(sector + sector_index, SPYRO_BMP_FILE_SIZE/4, spyro_bmp_main_ram_location, spyro_skin_file_index);

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
	const int FLAME_BMP_FILE_SIZE = 0x20;

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


//! SPARX SKIN
//Load the sparx skin to a buffer in main ram
void LoadSparxBMPToMainRam(int sector)
{
	const int SPARX_BMP_FILE_SIZE = 0xD8;

	byte* sparx_bmp_main_ram_location = (int*)0x80074330;

	int sparx_skin_file_index = (sparx_color_index % 4) * SPARX_BMP_FILE_SIZE/4; 	// In ints. %4, so that way it resets at the next sector
	int sector_index = sparx_color_index / 4;					// Seek to the next sector, every 4 skins (0x800 bytes)

	ReadFileIntoRam(sector + sector_index, SPARX_BMP_FILE_SIZE/4, sparx_bmp_main_ram_location, sparx_skin_file_index);
	should_write_sparx_bmp = true;
}

// Write the sparx skin from the buffer, into the correct area for sparx's vertex coloring. 
//Keeping it in a buffer initially to avoid loading from the disk everytime we need it again.
void WriteSparxBMPToSparxRam()
{
	const int SPARX_BMP_FILE_SIZE = 0xD8;

	// Get the pointer to sparx's vertex colors
	const int MOBY_TYPE_OFFSET = 0x36;
	const int MODELS_HEADER_OFFSET = 0x38;
    const int SPARX_ANIM = 0x4;
    const int SPARX_TYPE = 0x78;
	
	int* sparx_base_ptr = 0x80075898;
	int* current_sparx_ptr = *sparx_base_ptr;

	int* models_base_ptr = 0x80076378;
	int* current_models_ptr = *models_base_ptr;

	int* start_of_current_sparx_models = *(int*)((int)models_base_ptr + (SPARX_TYPE * 4));
	int* start_of_sparx_vertex_colors_ptr = *(int*)((int)start_of_current_sparx_models + 0x2D8);

	// Logic				
	byte* sparx_bmp_main_ram_location = (int*)0x80074330;
	memcpy(start_of_sparx_vertex_colors_ptr, sparx_bmp_main_ram_location, 0xD8); // Copy the skin from the buffer to the correct area
	
	should_write_sparx_bmp = false;
}

void ChangeSparxGlowColor(u8RGBA new_color)
{
	_sparxGlowColor = new_color;
	_sparxGlowColor2 = new_color;
}


//! Every Frame Update
// Check for Loading Skins
void CosmeticsUpdate(void)
{
	//Change background color when menu gets updated
	if(should_update_bg_color)
	{
		SetTitleScreenColor(bg_colors[bg_color_index].r, bg_colors[bg_color_index].g);
		should_update_bg_color = false;
	}
	//Change Spyro Skin
	if(should_load_spyro_color)
	{
		LoadSpyroBMPToMainRam(SKIN_SECTOR);
		should_load_spyro_color = false;
	}
	if(should_write_spyro_bmp && _isLoading == false)
	{
		WriteSpyroBMPToVram();
	}
	//Change Flame Skin
	if(should_load_flame_color)
	{
		LoadFlameBMPToMainRam(FLAME_SECTOR);
		should_load_flame_color = false;
	}
	if(should_write_flame_bmp && _isLoading == false)
	{
		WriteFlameBMPToVram();
		ChangeFlameTriangleColor(CUSTOM_FLAME_TRIANGLE_GRADIENTS[flame_color_index]); // Change Triangle Gradient
	}
	//Change Sparx Skin
	if(should_load_sparx_color)
	{
		LoadSparxBMPToMainRam(SPARX_SECTOR); // Change Sparx Vertex Color
		should_load_sparx_color = false;
	}
	if(should_write_sparx_bmp)
	{
		WriteSparxBMPToSparxRam();
		ChangeSparxGlowColor(CUSTOM_SPARX_GLOW_COLORS[sparx_color_index]); // Change Sparx Glow
		InjectChangeSparxParticleColorJump(); // Inject Change Sparx Particles Function into Overlay
	}

	// Check for re-loading skin's in loads
	if(_levelLoadState == 0x7 && _gameState == GAMESTATE_LOADING)
	{
		if (spyro_color_index > 0) 
		{
			should_write_spyro_bmp = true;    
		}   
		//Reload Flame Skin in Load
		if(flame_color_index > 0) 
		{
			should_write_flame_bmp = true;  
		}   
	}
	
	// Check for reloading sparx skin when sparx is loaded again (from load, sparx death, or health up)
	if(sparx_color_index > 0)
	{
		// If our local sparx pointer is different than the in game sparx pointer, that means he has moved in RAM
		if(_sparx_ptr != previous_sparx_ptr)
		{
			previous_sparx_ptr = _sparx_ptr; 	// Set previous sparx pointer as the current one
			if(_sparx_ptr != NULL)
			{
				should_write_sparx_bmp = true; 	// Reload sparx since he has moved.
			}
		}
		// If the glow strength is between this range, that means it's growing from regetting max sparx
		if(_sparxGlowStrength > 10 || _sparxGlowStrength < 60)
		{
			ChangeSparxGlowColor(CUSTOM_SPARX_GLOW_COLORS[sparx_color_index]); // Change Sparx Glow if we are back to 3 health
		}
	}

}

// void VramTester()
// {
// 	int* flame_bmp_main_ram_location = (int*)0x80074400;

// 	// VORTEX CLUTS
//  	RECT test_rect;
// 	test_rect.w = 15;
// 	test_rect.x = 545;
// 	test_rect.y = 432;
// 	test_rect.h = 7;
	
//   	LoadImage(&test_rect, flame_bmp_main_ram_location);

// 	test_rect.w = 15;
// 	test_rect.x = 545;
// 	test_rect.y = 424;
// 	test_rect.h = 7;
	
//   	LoadImage(&test_rect, flame_bmp_main_ram_location);

// 	test_rect.w = 15;
// 	test_rect.x = 545;
// 	test_rect.y = 440;
// 	test_rect.h = 7;
	
//   	LoadImage(&test_rect, flame_bmp_main_ram_location);

// 	test_rect.w = 15;
// 	test_rect.x = 561;
// 	test_rect.y = 424;
// 	test_rect.h = 7;
	
//   	LoadImage(&test_rect, flame_bmp_main_ram_location);
// }

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