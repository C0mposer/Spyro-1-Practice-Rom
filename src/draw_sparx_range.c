#include <common.h>
#include <prims.h>

// Author: ThirstyWraith
const short OFFSETS[8][2] = {
        {0, 1434},
        {1043, 1043},
        {1434, 0},
        {1043, -1043},
        {0, -1434},
        {-1043, -1043},
        {-1434, 0},
        {-1043, 1043}
};

void DrawSparxRange(void) {
    if (_gameState == GAMESTATE_GAMEPLAY)
    {
        ShadedLine line;

        Vec3 octagon_vertices[8];
        Vec3 vertex_1;
        Vec3 vertex_2;

        // Calculate octagon vertices in 3D space
        int i;
        for (i = 0; i < 8; i++)
        {
            octagon_vertices[i].x = _spyro.position.x + OFFSETS[i][0];
            octagon_vertices[i].y = _spyro.position.y + OFFSETS[i][1];
            octagon_vertices[i].z = _spyro.position.z - 350;
        }
        // Draw octagon lines
        for (i = 0; i < 8; i++)
        {
            WorldToScreen(&vertex_1, &octagon_vertices[i]);
            WorldToScreen(&vertex_2, &octagon_vertices[(i + 1) % 8]);

            line.point1Pos.x = vertex_1.x;
            line.point1Pos.y = vertex_1.y;
            line.point2Pos.x = vertex_2.x;
            line.point2Pos.y = vertex_2.y;

            line.colorP1.R = 0x0;
            line.colorP1.G = 0xFF;
            line.colorP1.B = 0xFF;

            line.colorP2.R = 0x0;
            line.colorP2.G = 0xFF;
            line.colorP2.B = 0xFF;

            CustomDrawLineShaded(line);
        }
    }
    return;
}