#include <common.h>
#include <prims.h>

void CustomDrawPolygon(Poly4F customInfo, PrimitiveCode shapeCode)
{

    Poly4F* shapeInfo = (Poly4F*)_ptr_graphicsRelated;

    shapeInfo->code = shapeCode;

    if (shapeCode == POLY4F_OPAQUE || shapeCode == POLY4F_TRANSPARENT)
        shapeInfo->tag = 0x5000000;
    if (shapeCode == POLY3F_OPAQUE || shapeCode == POLY3F_TRANSPARENT)
        shapeInfo->tag = 0x4000000;

    shapeInfo->color.R = customInfo.color.R;
    shapeInfo->color.G = customInfo.color.G;
    shapeInfo->color.B = customInfo.color.B;

    shapeInfo->point1Pos.x = customInfo.point1Pos.x;
    shapeInfo->point1Pos.y = customInfo.point1Pos.y;
    shapeInfo->point2Pos.x = customInfo.point2Pos.x;
    shapeInfo->point2Pos.y = customInfo.point2Pos.y;
    shapeInfo->point3Pos.x = customInfo.point3Pos.x;
    shapeInfo->point3Pos.y = customInfo.point3Pos.y;

    if (shapeCode == POLY4F_OPAQUE || shapeCode == POLY4F_TRANSPARENT)
    {
        shapeInfo->point4Pos.x = customInfo.point4Pos.x;
        shapeInfo->point4Pos.y = customInfo.point4Pos.y;
    }
    DrawPrimitive(shapeInfo);

    _ptr_graphicsRelated += (sizeof(Poly4F) - 4);
}