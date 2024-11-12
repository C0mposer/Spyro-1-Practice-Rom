#include <common.h>
#include <vector.h>
#include <balloon_data.h>

BalloonLevelSelectData balloon_level_select_data[6] =
{
    // Artisans
    {
        .balloonPos.x = 0x00014C00,
        .balloonPos.y = 0x0000B445,
        .balloonPos.z = 0x0000275B,
        .balloonRotation = 0x04C00000,

        .cameraPos.x = 0x00013E30,
        .cameraPos.y = 0x0000B64B,
        .cameraPos.z = 0x00002ADF,

        .spyroPos.x = 0x00014C00,
        .spyroPos.y = 0x0000B445,
        .spyroPos.z = 0x00002A2F,

        .balloonTimer = 110,
        .dropOffLocation = 0,

        .unk = 0x01A001A2,

        .unk2 = 0x000046AA,
        .unk3 = 0x00003235,
        .unk4 = 0x000007A0,
        .unk5 = 0x00000DF6,
        .unk6 = 0x000036DC

    },
    // Peace Keepers
    {
        .balloonPos.x = 0x0001F5A0,
        .balloonPos.y = 0x00018E00,
        .balloonPos.z = 0x0000D8F1,
        .balloonRotation = 0x04000000,

        .cameraPos.x = 0x0001F398,
        .cameraPos.y = 0x00018030,
        .cameraPos.z = 0x0000DC75,

        .spyroPos.x = 0x0001F5A0,
        .spyroPos.y = 0x00018E00,
        .spyroPos.z = 0x0000DBC5,

        .balloonTimer = 110,
        .dropOffLocation = 1,

        .unk = 0x01A001A8,

        .unk2 = 0x0000F840,
        .unk3 = 0x0000E3CB,
        .unk4 = 0x00000BA0,
        .unk5 = 0x00000DF6,
        .unk6 = 0x0000E872

    },
    // Magic Crafters
    {
        .balloonPos.x = 67846,
        .balloonPos.y = 60665,
        .balloonPos.z = 26638,
        .balloonRotation = 0x048D0000,

        .cameraPos.x = 67229,
        .cameraPos.y = 64183,
        .cameraPos.z = 27538,

        .spyroPos.x = 67846,
        .spyroPos.y = 60665,
        .spyroPos.z = 27362,

        .balloonTimer = 110,
        .dropOffLocation = 2,

        .unk = 27263535,

        .unk2 = 0x0000875D,
        .unk3 = 0x000072E8,
        .unk4 = 0x00000470,
        .unk5 = 0x00000DF6,
        .unk6 = 0x0000778F

    },
    // Beast Makers
    {
        .balloonPos.x = 96832,
        .balloonPos.y = 95340,
        .balloonPos.z = 17076,
        .balloonRotation = 0x04950000,

        .cameraPos.x = 95540,
        .cameraPos.y = 98671,
        .cameraPos.z = 18015,

        .spyroPos.x = 96832,
        .spyroPos.y = 95340,
        .spyroPos.z = 17800,

        .balloonTimer = 110,
        .dropOffLocation = 3,

        .unk = 27263400,

        .unk2 = 0x0000622D,
        .unk3 = 0x00004D6D,
        .unk4 = 0x000004F0,
        .unk5 = 0x00000DF6,
        .unk6 = 0x0000525F

    },
    //Dream Weavers
    {
        .balloonPos.x = 100425,
        .balloonPos.y = 82415,
        .balloonPos.z = 14188,
        .balloonRotation = 0x04400000,

        .cameraPos.x = 103959,
        .cameraPos.y = 81895,
        .cameraPos.z = 15088,

        .spyroPos.x = 100425,
        .spyroPos.y = 82415,
        .spyroPos.z = 14912,

        .balloonTimer = 110,
        .dropOffLocation = 4,

        .unk = 27263144,

        .unk2 = 0x000056BB,
        .unk3 = 0x00004246,
        .unk4 = 0x00000FA0,
        .unk5 = 0x00000DF6,
        .unk6 = 0x000046ED

    },
    //Gnastys World
    {
        .balloonPos.x = 87096,
        .balloonPos.y = 87351,
        .balloonPos.z = 8268,
        .balloonRotation = 0x04240000,

        .cameraPos.x = 89499,
        .cameraPos.y = 84707,
        .cameraPos.z = 9168,

        .spyroPos.x = 87096,
        .spyroPos.y = 87351,
        .spyroPos.z = 8992,

        .balloonTimer = 110,
        .dropOffLocation = 5,

        .unk = 27263043,

        .unk2 = 0x00003F9B,
        .unk3 = 0x00002B26,
        .unk4 = 0x00000DE0,
        .unk5 = 0x00000DF6,
        .unk6 = 0x00002FCD

    },
};

void LoadBalloonData(int i)
{
    _balloon_ptr = 0x80071e9c;

    // Copy balloon moby to global balloon data depending on HW
    if (i == 0)
    {
        memcpy(0x80071e9c, 0x8016E3B8, 0x58);
    }
    if (i == 1)
    {
        memcpy(0x80071e9c, 0x8017D3D0, 0x58);
    }
    if (i == 2)
    {
        memcpy(0x80071e9c, 0x8017C860, 0x58);
    }
    if (i == 3)
    {
        memcpy(0x80071e9c, 0x8017D8E8, 0x58);
    }
    if (i == 4)
    {
        memcpy(0x80071e9c, 0x80179494, 0x58);
    }
    if (i == 5)
    {
        memcpy(0x80071e9c, 0x8015870C, 0x58);
    }


    _balloon_position = balloon_level_select_data[i].balloonPos;
    _balloon_rotation = balloon_level_select_data[i].balloonRotation;
    _cameraPosition = balloon_level_select_data[i].cameraPos;
    _spyro.position = balloon_level_select_data[i].spyroPos;

    _balloon_timer = balloon_level_select_data[i].balloonTimer;
    _balloon_drop_off_location = balloon_level_select_data[i].dropOffLocation;

    _balloon_unk1 = balloon_level_select_data[i].unk;

    _balloon_unk2 = balloon_level_select_data[i].unk2;
    _balloon_unk3 = balloon_level_select_data[i].unk3;
    _balloon_unk4 = balloon_level_select_data[i].unk4;
    _balloon_unk5 = balloon_level_select_data[i].unk5;
    _balloon_unk6 = balloon_level_select_data[i].unk6;
}