#ifndef CD_H
#define CD_H
#include <common.h>


enum CdControlPrimativeFunctions
{
    CDL_PRIMITIVE_NOP = 0x01,
    CDL_PRIMITIVE_SET_LOC = 0x02,
    CDL_PRIMITIVE_PLAY = 0x03,
    CDL_PRIMITIVE_FORWARD = 0x04,
    CDL_PRIMITIVE_BACKWARDS = 0x05,
    CDL_PRIMITIVE_READN = 0x06,
    CDL_PRIMITIVE_STANDBY= 0x07,
    CDL_PRIMITIVE_STOP = 0x08,
    CDL_PRIMITIVE_PAUSE = 0x09,
    CDL_PRIMITIVE_MUTE = 0x0B,
    CDL_PRIMITIVE_DEMUTE = 0x0C,
    CDL_PRIMITIVE_SETFILTER = 0x0D,
    CDL_PRIMITIVE_SETMODE = 0x0E,
    CDL_PRIMITIVE_GETPARAM = 0x0F,
    CDL_PRIMITIVE_GETlocL = 0x10,
    CDL_PRIMITIVE_GETlocP = 0x11,
    CDL_PRIMITIVE_SEEKL = 0x15,
    CDL_PRIMITIVE_SEEKP = 0x16,
    CDL_PRIMITIVE_READS = 0x1B
};

enum CdModes    //? Bit-flags
{
    CDL_MODE_DA = 0b1,                  
    CDL_MODE_AP = 0b10,
    CDL_MODE_REPT = 0b100,
    CDL_MODE_SF = 0b1000,                   //? 0 or 1 if subheader filter is OFF/ON
    CDL_MODE_SIZE0 = 0b10000,
    CDL_MODE_SIZE1 = 0b100000,
    CDL_MODE_RT = 0b1000000,                //? 0 or 1 if ADPCM is OFF/ON. ADPCM is music streaming mode.
    CDL_MODE_SPEED = 0b10000000,            //? 0 for normal speed, 1 for double speed
    CDL_MODE_STREAM = 0b100000000,          //? 1 for Normal streaming
    CDL_MODE_STREAM2 = 0x120 //fuck off
    
};

typedef enum CdSyncMode
{
    returnWhenDone,
    returnImmediately
}CdSyncMode;

typedef void (*CdlCB)(unsigned char,unsigned char *);

typedef struct {
    unsigned char file;         //? File ID, Always 1
    unsigned char chan;         //? Channel ID, Picks a song from the current music file (goes up to 7)
    unsigned short pad;         //? System reserved
} CdlFILTER; 

typedef struct CdlLOC {
	unsigned char minute;		//? minute (BCD) */
	unsigned char second;		//? second (BCD) */
	unsigned char sector;		//? sector (BCD) */
	unsigned char track;		//? track (void) */
} CdlLOC;

int CdRead(int amountOfSectors, unsigned int * addressInMemory, int mode);
int CdInit();
CdlCB CdReadCallback(CdlCB func);
int CdStatus(void); //!RESEARCH
//* Returns true if the CD has finished its current task (in the main mode).
bool CdSync(CdSyncMode mode, unsigned char* optinalReturnPTR);

//* Returns true if the CD has finished its current task (in the main mode).                                     
bool IsCdFinishedWithTask(CdSyncMode mode, unsigned char* optinalReturnPTR);   

//* Wrapper                    
bool CdSync_(CdSyncMode mode, unsigned char* optinalReturnPTR);       
                 
//* Tells the CD-DRIVE to preform a primitive function, and someimes allows concurrent cd functions
int CdControl(unsigned char command, unsigned char* param, unsigned char * optinalReturnPTR);     

//* Tells the CD-DRIVE to preform a primitive function, and ONLY that one function. 
int CdControlB(unsigned char command, void* param, unsigned char * optinalReturnPTR);      

void *CdDataCallback(void (*func)()); //!RESEARCH

//* Stores the sector in the correct format for the CdlLOC struct format
CdlLOC *CdIntToPos(int sectorNumber, CdlLOC * p);                                               

//* Returns the sector number stored in a CdlLOC struct, as an int
int CdPosToInt(CdlLOC * p);                                                                  

void read_cb(unsigned char status, unsigned char *optinalReturnPTR);

extern int _musicSectors; //0x8006f200


#endif /* CD_H */
