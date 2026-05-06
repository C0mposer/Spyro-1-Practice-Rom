#include <common.h>
#include <prims.h>
#include <moby.h>

static const short OFFSETS[8][2] = {
    {0, 4096},
    {2979, 2979},
    {4096, 0},
    {2979, -2979},
    {0, -4096},
    {-2979, -2979},
    {-4096, 0},
    {-2979, 2979}
};

static const short SPYRO_OFFSETS[8][2] = {
    {0, 30},
    {22, 22},
    {30, 0},
    {22, -22},
    {0, -30},
    {-22, -22},
    {-30, 0},
    {-22, 22}
};

const int MOBY_SIZE = 0x58;

void DrawFoolRange(void) {
    if (_gameState == GAMESTATE_GAMEPLAY)
    {
        ShadedLine line;

        Vec3 octagon_vertices[8];
        Vec3 screen_vertices[8];
        Vec3 spyro_vertices[8];
        Vec3 spyro_screen[8];

        int amountOfMobysInLevel = (*(_ptr_levelMobys - 2) / MOBY_SIZE);

        // Draw Spyro's center octagon
        for (int i = 0; i < 8; i++)
        {
            spyro_vertices[i].x = _spyro.position.x + SPYRO_OFFSETS[i][0];
            spyro_vertices[i].y = _spyro.position.y + SPYRO_OFFSETS[i][1];
            spyro_vertices[i].z = _spyro.lastGroundedPosition.z;

            WorldToScreen(&spyro_screen[i], &spyro_vertices[i]);
        }

        for (int i = 0; i < 8; i++)
        {
            if (spyro_screen[i].z <= 0 || spyro_screen[(i + 1) % 8].z <= 0) continue;

            line.point1Pos.x = spyro_screen[i].x;
            line.point1Pos.y = spyro_screen[i].y;
            line.point2Pos.x = spyro_screen[(i + 1) % 8].x;
            line.point2Pos.y = spyro_screen[(i + 1) % 8].y;

            line.colorP1.R = 0xFF;
            line.colorP1.G = 0xFF;
            line.colorP1.B = 0x0;
            line.colorP2.R = 0xFF;
            line.colorP2.G = 0xFF;
            line.colorP2.B = 0x0;

            CustomDrawLineShaded(line);
        }

        // Draw fool ranges
        for (int m = 1; m <= amountOfMobysInLevel; m++)
        {
            Moby* currentStruct = (Moby*)_ptr_levelMobys + 1 * m;

            if (currentStruct->type == -1) continue;
            if (currentStruct->state == -1) continue;

            if (currentStruct->type == 0x7E)
            {
                if (currentStruct == 0x8016DFB8 || currentStruct == 0x8016E010 || currentStruct == 0x8016E068)
                {                // Calculate all vertices and convert to screen space
                    for (int i = 0; i < 8; i++)
                    {
                        octagon_vertices[i].x = currentStruct->position.x + OFFSETS[i][0];
                        octagon_vertices[i].y = currentStruct->position.y + OFFSETS[i][1];
                        octagon_vertices[i].z = currentStruct->position.z;

                        WorldToScreen(&screen_vertices[i], &octagon_vertices[i]);

                    }

                    // Draw shading lines across the octagon
                    for (int i = 0; i < 4; i++)
                    {
                        line.point1Pos.x = screen_vertices[i].x;
                        line.point1Pos.y = screen_vertices[i].y;
                        line.point2Pos.x = screen_vertices[i + 4].x;
                        line.point2Pos.y = screen_vertices[i + 4].y;

                        line.colorP1.R = 0x0;
                        line.colorP1.G = 0x40;
                        line.colorP1.B = 0x40;
                        line.colorP2.R = 0x0;
                        line.colorP2.G = 0x40;
                        line.colorP2.B = 0x40;

                        CustomDrawLineShaded(line);
                    }

                    // Draw octagon outline
                    for (int i = 0; i < 8; i++)
                    {
                        line.point1Pos.x = screen_vertices[i].x;
                        line.point1Pos.y = screen_vertices[i].y;
                        line.point2Pos.x = screen_vertices[(i + 1) % 8].x;
                        line.point2Pos.y = screen_vertices[(i + 1) % 8].y;

                        line.colorP1.R = 0x0;
                        line.colorP1.G = 0xFF;
                        line.colorP1.B = 0xFF;
                        line.colorP2.R = 0x0;
                        line.colorP2.G = 0xFF;
                        line.colorP2.B = 0xFF;

                        CustomDrawLineShaded(line);
                    }
                }
            }
        }
    }
    return;
}