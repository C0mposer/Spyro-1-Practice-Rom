#include <common.h>
#include <prims.h>

#define LINE_SHADED 0x50

//* Custom draw line function, that takes my custom Line data type. I made this so I can change the color of the line.
void CustomDrawLineShaded(ShadedLine customLineData)
{
    ShadedLine* lineData;

    lineData = (ShadedLine*)_ptr_graphicsRelated;
    lineData->tag = 0x4000000;
    lineData->code = LINE_SHADED;

    lineData->point1Pos.x = customLineData.point1Pos.x;
    lineData->point1Pos.y = customLineData.point1Pos.y;
    lineData->point2Pos.x = customLineData.point2Pos.x;
    lineData->point2Pos.y = customLineData.point2Pos.y;

    lineData->colorP1.R = customLineData.colorP1.R;              // First half R color
    lineData->colorP1.G = customLineData.colorP1.G;              // First half G color
    lineData->colorP1.B = customLineData.colorP1.B;              // First half B color

    lineData->colorP2.R = customLineData.colorP2.R;              // Second half R color
    lineData->colorP2.G = customLineData.colorP2.G;              // Second half G color
    lineData->colorP2.B = customLineData.colorP2.B;              // Second half B color

    DrawPrimitive(lineData);
    _ptr_graphicsRelated += 0x14;
}




// HACKY WAY OF DRAWING SHADED LINE
// void CustomDrawLine(int Point1X,int Point1Y,int Point2X,int Point2Y)
// {
//   int lineP1Angle;
//   int lineP2Angle;
//   byte *ptr_toLine;

//   ptr_toLine = _ptr_graphicsRelated;
//   *(int *)_ptr_graphicsRelated = 0x4000000;
//   ptr_toLine[7] = 0x50;

//   *(short *)(ptr_toLine + 8) = (short)Point1X;
//   *(short *)(ptr_toLine + 10) = (short)Point1Y;
//   *(short *)(ptr_toLine + 16) = (short)Point2X;
//   *(short *)(ptr_toLine + 18) = (short)Point2Y;

//   ptr_toLine[4] = 0x50;
//   ptr_toLine[5] = 0x5;
//   ptr_toLine[6] = 0x5;
//   ptr_toLine[0xC] = 0x50;
//   ptr_toLine[0xD] = 0x5;
//   ptr_toLine[0xE] = 0x5;

//   DrawPrimitive(ptr_toLine);
//   _ptr_graphicsRelated = ptr_toLine + 0x14;
//   return;
// }
