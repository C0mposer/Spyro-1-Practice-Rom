#include <cd.h>
#include <custom_text.h>


bool hasFullyLoaded = FALSE;
int hasStartedLoad = 0;
//CdlCB callback;


#define AMOUNT_OF_SECTORS 2                                                         //? The amount of sectors the mod takes up currently
#define SECTOR_NUMBER 265759                                                       //? The sector number for my mod.bin on the disk
#define FREE_RAM_AREA 0x80008BC0                                                   //? The free area in the header of ram we are loading into
#define GNORC_WALKED_UP_HILL 2      
                                                //? The point in the _mainMenuState where the gnorc has walked up the hill

//* Set the CD-DRIVE back into music playing mode after the read
void read_cb(unsigned char status, unsigned char *result){
    //CdReadCallback(callback);                                                     //? For some reason this keeps the read_cb function from happening a second time
    //CdIntToPos(*(&_musicSectors + 3), &loc);                                      //? Takes the abslute sector numbers of the music from ram, and adds the song offset. Then, converts them into the CdlLOC format
    //CdControlB(CdlReadS, (void *)&loc, 0);                                        //? Starts reading from the disk at the specified location. Must typecase void* onto CdlLOC struct pointer.

    int mode = CDL_MODE_RT | CDL_MODE_SPEED | CDL_MODE_SF;                          //? This mode is Music Playing Mode, Double Speed, Sub-header Filter. const instead of define to avoid error warning lol
    void* modePTR = &mode;                                                          //? Needs to be put in a ptr presumably because this function takes different arguments depending on situation
    CdControlB(CDL_PRIMITIVE_SETMODE, modePTR, NULL);                               //? Puts the Cd subsystem in the right mode to play music
    hasStartedLoad++;                                                          //? Anything in this function will only runs one time after CdRead has finished
}

//* Read a file into ram
void ReadFileIntoRam(int sector_number, int amount_of_sectors, int ram_area){
        CdlLOC location;
        CdIntToPos(sector_number, &location);                                       //? Stores the sector in the correct format for the CdlLOC struct format
        CdControlB(CDL_PRIMITIVE_SEEKL, (void *)&location, NULL);                   //? Seeking to location, which was just set to our sector above to 265759. Platform specific lel just use it

        CdControlB(CDL_PRIMITIVE_SET_LOC, (void *)&location, NULL);                 //? Uses CdSetloc (0x02) to prepare for CdRead. This is required
        CdRead(amount_of_sectors, (void*)ram_area, CDL_MODE_SPEED);                 //? Read 4 Sectors to the ram address 0x8000C000. 0x80 is the mode.
        
        CdReadCallback(read_cb);                                                    //? Defines read_cb as the function to run after CdRead is completed. TODO: REFACTOR WITH CdSync with isFinishedReading bool

        hasStartedLoad++;
}

//* Main CD Hook
void CdLoad()
{
    if(hasStartedLoad == 0)
    {
        ReadFileIntoRam(265759, AMOUNT_OF_SECTORS, 0x80008EB0);      //? Begin the code loading process
    }
    else if(hasStartedLoad == 2){
        ReadFileIntoRam(265761, AMOUNT_OF_SECTORS, 0x80007540);
    }
	else if(hasStartedLoad == 4){
        CustomMenuUpdate();              //? Run the main Loop of the loaded code after its done loading and has returned to music playing mode
        CustomMenuUpdate2();
        RatCodeUpdate();
        CreditsSkipUpdate();
        LootPlaneUpdate();
        MultiTapUpdate();
        MainUpdate();
    }
}



