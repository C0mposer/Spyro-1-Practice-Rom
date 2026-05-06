#include <common.h>

void StoreMatrixRegisters(Mat33* dest)
{
    Vec3 col1;
    Vec3 col2;
    Vec3 col3;

    col1.x = 1;
    col2.y = 1;
    col3.z = 1;

    Vec3ApplyLastLoadedRotationMatrix(&col1, &col1);
    Vec3ApplyLastLoadedRotationMatrix(&col2, &col2);
    Vec3ApplyLastLoadedRotationMatrix(&col3, &col3);

    dest->R11 = col1.x;
    dest->R21 = col1.y;
    dest->R31 = col1.z;
    dest->R12 = col2.x;
    dest->R22 = col2.y;
    dest->R32 = col2.z;
    dest->R13 = col3.x;
    dest->R23 = col3.y;
    dest->R33 = col3.z;
}

void RestoreMatrixRegisters(Mat33* src)
{
    Vec3 tempVec = { 1,1,1 };
    Vec3ApplyRotationMatrix(src, &tempVec, &tempVec);

}