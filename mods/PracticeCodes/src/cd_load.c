#include <cd.h>
#include <custom_text.h>

int CdGetSector(void *madr, int size);
int CdReady(int mode, char* result);

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
    //CdReadCallback(NULL);                                                     //? For some reason this keeps the read_cb function from happening a second time
    //CdIntToPos(*(&_musicSectors + 3), &loc);                                      //? Takes the abslute sector numbers of the music from ram, and adds the song offset. Then, converts them into the CdlLOC format
    //CdControlB(CdlReadS, (void *)&loc, 0);                                        //? Starts reading from the disk at the specified location. Must typecase void* onto CdlLOC struct pointer.
    
    int mode = CDL_MODE_RT | CDL_MODE_SPEED | CDL_MODE_SF;                          //? This mode is Music Playing Mode, Double Speed, Sub-header Filter. const instead of define to avoid error warning lol
    void* modePTR = &mode;                                                          //? Needs to be put in a ptr presumably because this function takes different arguments depending on situation
    CdControlB(CDL_PRIMITIVE_SETMODE, modePTR, NULL);                               //? Puts the Cd subsystem in the right mode to play music
    hasStartedLoad++;                                                          //? Anything in this function will only runs one time after CdRead has finished
}

//* Read a file into ram
void ReadFileIntoRam(int sector_number, int size_to_read_in_ints, int ram_area, int offset_in_ints)
{
        _pauseMusic = 9;
        CdlLOC location;
        CdIntToPos(sector_number, &location);                                       //? Stores the sector in the correct format for the CdlLOC struct format
        //CdControlB(CDL_PRIMITIVE_SEEKL, (void *)&location, NULL);                   //? Seeking to location, which was just set to our sector above to 265759. Platform specific lel just use it

        //CdControlB(CDL_PRIMITIVE_SET_LOC, (void *)&location, NULL);                 //? Uses CdSetloc (0x02) to prepare for CdRead. This is required
                   
        CdControlB(CDL_PRIMITIVE_READN, (void *)&location, NULL);       //Read into sector buffer 
        CdReady(0, NULL);

        if(!_musicVolume){
            CdGetSector(ram_area, 0x3);                                     // Read the weird 3 ints of dummy data
        }

        int sector_int_amount_left = 0x200;                              // We need to read the full 0x800 bytes because of the old PSYQ

        unsigned int i = 0;
        if (offset_in_ints > 0x49)                                      // If the offset in ints is > 0x49, we need to do multiple reads to avoid overwriting data.
        {                             
            int amount_of_dummy_reads = (offset_in_ints / 0x50);        // Amount of 0x200 byte (0x50 int) dummy reads to do in the loop
            while (i < amount_of_dummy_reads)                           // Loop 0x200 bytes of dummy data (0x50 ints) at a time down, until < 0x50 bytes of offset remain
            {
                CdGetSector(ram_area, 0x50);
                sector_int_amount_left -= 0x50;
                i++;
            }       
        }

        int remaining_offset = offset_in_ints - (0x50 * i);             // Calculate the remaning ints to remove, with the amount of for loop itterations            
        if (remaining_offset > 0)                                       // If there is any remaining offset bytes remaining, remove them
        {
            CdGetSector(ram_area, remaining_offset);                    // Read remaining offset data < 0x50 ints
            sector_int_amount_left -= remaining_offset;
        }


        CdGetSector(ram_area, size_to_read_in_ints);                    // Read actual data
        sector_int_amount_left -= size_to_read_in_ints;

        //
        //CdGetSector(0x80010084, 0x150);
        // i = 0;
        // if (sector_int_amount_left > 0x49)                              // If the amount of ints left at the end of the sector is > 0x49, we need to do multiple reads to avoid overwriting data.
        // {                             
        //     int amount_of_dummy_reads = (sector_int_amount_left / 0x50);        // Amount of 0x200 byte (0x50 int) dummy end of sector reads to do in the loop
        //     while (i < amount_of_dummy_reads)                           // Loop 0x200 bytes of dummy end of sector data (0x50 ints) at a time down, until < 0x50 ints remain
        //     {
        //         CdGetSector(0x80010084, 0x50);                          // Read 0x50 ints of end of sector dummy data into the dummy ram address (fixed for now)
        //         sector_int_amount_left -= 0x50;
        //         i++;
        //     }       
        // }

        // if (sector_int_amount_left > 0)                                 // If there is any remaining offset bytes remaining, remove them
        // {
        //     CdGetSector(0x80010084, sector_int_amount_left);            // Read remaining end of sector data that is < 0x50 ints
        //     sector_int_amount_left -= remaining_offset;
        // }
        
        //CdReadCallback(read_cb);                                        //? Defines read_cb as the function to run after CdRead is completed. TODO: REFACTOR WITH CdSync with isFinishedReading bool
        // if(_musicVolume){
        //     _musicState = 0x10;
        // }
        
        PlayMusic(_currentMusicTrack, 8);
        hasStartedLoad++;
}

//* Main CD Hook
// void CdLoad()
// {
//     if(hasStartedLoad == 0)
//     {
//         ReadFileIntoRam(265759, AMOUNT_OF_SECTORS, 0x8000C000);      //? Begin the code loading process
//     }
//     else if(hasStartedLoad == 2){
//         ReadFileIntoRam(265761, AMOUNT_OF_SECTORS, 0x8000D000);
//     }
// 	else if(hasStartedLoad == 4){
//         CustomMenuUpdate();              //? Run the main Loop of the loaded code after its done loading and has returned to music playing mode
//         CustomMenuUpdate2();
//         RatCodeUpdate();
//         CreditsSkipUpdate();
//         LootPlaneUpdate();
//         MultiTapUpdate();
//         CheckLandingUpdate();
//         MainUpdate();
//     }
// }



