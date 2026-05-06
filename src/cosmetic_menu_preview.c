/* Cosmetic practice-menu preview: Spyro/DrawSpyro + pause-screen grey.
 * Linked only in REDUX / DECKARD (extra or PS2M) — see cosmetic.c for retail stubs. */
#include <common.h>
#include <cosmetic.h>
#include <deckard.h>

#define COSMETIC_PREVIEW_BASE_DISTANCE 1900
#define COSMETIC_PREVIEW_BASE_HEIGHT -450
#define COSMETIC_MOBY_INPUT_LIST_ADDR 0x8006FCF4
#define COSMETIC_MOBY_INPUT_LIST_SIZE 0x900

#if BUILD == REDUX || BUILD == DUCKSTATION
#define COSMETIC_TEMP_SPYRO_REGION 0x80500000
#elif BUILD == PS2_DECKARD
#define COSMETIC_TEMP_SPYRO_REGION 0x80A28000
#endif

int g_cosmetic_preview_ofs_x = 0;
int g_cosmetic_preview_ofs_y = 0;
int g_cosmetic_preview_ofs_z = 0;
int g_cosmetic_preview_spin_step = 10;
int g_cosmetic_preview_mode = COSMETIC_PREVIEW_SPYRO;

static Angle s_cosmetic_saved_spyro_angle;
static Vec3 s_cosmetic_saved_spyro_position;
static CameraAngle s_cosmetic_saved_camera_angle;
static Vec3 s_cosmetic_saved_camera_position;
static Vec3 s_cosmetic_preview_position;
static Angle s_cosmetic_preview_angle;
static int s_cosmetic_spyro_angle_armed;
static unsigned int s_cosmetic_spin_accum;


static int CosmeticPreviewSin(int angle)
{
	return SinScaled((unsigned int)angle);
}

static int CosmeticPreviewCos(int angle)
{
	return SinScaled((unsigned int)(angle + 0x400));
}

static void CosmeticPreviewPatchDrawSpyro(void)
{
	#if BUILD == REDUX || BUILD == DUCKSTATION
	* (int*)0x80023afc = 0x3C1F8050;
	*(int*)0x80023b00 = 0x27FF0000;
	*(int*)0x80024424 = 0x3C198050;
	*(int*)0x80024428 = 0x27390000;
	*(int*)0x80024a5c = 0x3C018050;
	*(int*)0x80024a60 = 0x24210000;
	*(int*)0x80024b60 = 0x3C048050;
	*(int*)0x80024b64 = 0x24840000;
	#elif BUILD == PS2_DECKARD
	* (int*)0x80023afc = 0x3C1F80A3;
	*(int*)0x80023b00 = 0x27FF8000;
	*(int*)0x80024424 = 0x3C1980A3;
	*(int*)0x80024428 = 0x27398000;
	*(int*)0x80024a5c = 0x3C0180A3;
	*(int*)0x80024a60 = 0x24218000;
	*(int*)0x80024b60 = 0x3C0480A3;
	*(int*)0x80024b64 = 0x24848000;
	#endif
}

static void CosmeticPreviewRestoreDrawSpyro(void)
{
	*(int*)0x80023afc = 0x3C1F8008;
	*(int*)0x80023b00 = 0x27FF8A58;
	*(int*)0x80024424 = 0x3C198008;
	*(int*)0x80024428 = 0x27398A58;
	*(int*)0x80024a5c = 0x3C018008;
	*(int*)0x80024a60 = 0x24218A58;
	*(int*)0x80024b60 = 0x3C048008;
	*(int*)0x80024b64 = 0x24848A58;

	*(int*)0x80024110 = 0xAC2700B8;
	*(int*)0x80024114 = 0xAC2800BC;
	*(int*)0x80024118 = 0xAC2900C0;
	*(int*)0x8002411C = 0xAC2A00C4;
	*(int*)0x80024120 = 0xAC2B00C8;
}

void CosmeticMenuSpyroAngleRestoreIfNeeded(void)
{
	if (!s_cosmetic_spyro_angle_armed)
		return;
	s_cosmetic_spyro_angle_armed = 0;
	s_cosmetic_spin_accum = 0;
}

void CosmeticMenuSetPreviewMode(int preview_mode)
{
	(void)preview_mode;
	g_cosmetic_preview_mode = COSMETIC_PREVIEW_SPYRO;
}

void CosmeticMenuSpyroLockForPreviewTick(void)
{
	if (!s_cosmetic_spyro_angle_armed) {
		s_cosmetic_saved_spyro_angle = _spyro.angle;
		s_cosmetic_saved_spyro_position = _spyro.position;
		deckard_write_u16(&s_cosmetic_saved_camera_angle.yaw, _cameraAngle.yaw);
		deckard_write_u16(&s_cosmetic_saved_camera_angle.pitch, _cameraAngle.pitch);
		deckard_write_u16(&s_cosmetic_saved_camera_angle.roll, _cameraAngle.roll);
		s_cosmetic_saved_camera_position = _cameraPosition;
		s_cosmetic_preview_position = _spyro.position;
		s_cosmetic_preview_angle = _spyro.angle;
		s_cosmetic_spyro_angle_armed = 1;
		s_cosmetic_spin_accum = 0;
	}
	{
		int yaw = (int)deckard_read_u16(&s_cosmetic_saved_camera_angle.yaw);
		int pitch = (int)deckard_read_u16(&s_cosmetic_saved_camera_angle.pitch);
		int forward_distance = COSMETIC_PREVIEW_BASE_DISTANCE + g_cosmetic_preview_ofs_y;
		int fwd_x = CosmeticPreviewCos(yaw);
		int fwd_y = CosmeticPreviewSin(yaw);
		int fwd_z = -CosmeticPreviewSin(pitch);
		int pitch_cos = CosmeticPreviewCos(pitch);
		int right_x;
		int right_y;

		fwd_x = (fwd_x * pitch_cos) / 4096;
		fwd_y = (fwd_y * pitch_cos) / 4096;
		right_x = CosmeticPreviewCos(yaw - 0x400);
		right_y = CosmeticPreviewSin(yaw - 0x400);

		s_cosmetic_preview_position.x = s_cosmetic_saved_camera_position.x
			+ ((fwd_x * forward_distance) / 4096)
			+ ((right_x * g_cosmetic_preview_ofs_x) / 4096);
		s_cosmetic_preview_position.y = s_cosmetic_saved_camera_position.y
			+ ((fwd_y * forward_distance) / 4096)
			+ ((right_y * g_cosmetic_preview_ofs_x) / 4096);
		s_cosmetic_preview_position.z = s_cosmetic_saved_camera_position.z
			+ ((fwd_z * forward_distance) / 4096)
			+ COSMETIC_PREVIEW_BASE_HEIGHT
			+ g_cosmetic_preview_ofs_z;
	}
	if (g_cosmetic_preview_spin_step != 0)
		s_cosmetic_spin_accum += (unsigned int)g_cosmetic_preview_spin_step;
	s_cosmetic_preview_angle = s_cosmetic_saved_spyro_angle;
	s_cosmetic_preview_angle.yaw = s_cosmetic_saved_spyro_angle.yaw + (int)s_cosmetic_spin_accum;
}

void CosmeticMenuDrawSpyroPreview(void)
{
	Spyro* temp_spyro;

	if (_spyroInvisible)
		return;

	temp_spyro = (Spyro*)COSMETIC_TEMP_SPYRO_REGION;
	memcpy(temp_spyro, &_spyro, sizeof(Spyro));
	deckard_memcpy(&temp_spyro->position, &s_cosmetic_preview_position, sizeof(Vec3));
	deckard_memcpy(&temp_spyro->angle, &s_cosmetic_preview_angle, sizeof(Angle));
	deckard_write_u8(&temp_spyro->visualAngle.yaw,
		(unsigned char)(_spyro.visualAngle.yaw + ((s_cosmetic_spin_accum >> 4) & 0xFF)));

	// Keep the preview spyro "on flat ground", regardless of his actual pitch angle
	deckard_write_u8(&temp_spyro->visualAngle.pitch, 0);
	deckard_write_u8(&temp_spyro->visualAngle.roll, 0);

	CosmeticPreviewPatchDrawSpyro();
	memset((void*)COSMETIC_MOBY_INPUT_LIST_ADDR, 0, COSMETIC_MOBY_INPUT_LIST_SIZE);
	DrawSpyro();

	CosmeticPreviewRestoreDrawSpyro();
}
