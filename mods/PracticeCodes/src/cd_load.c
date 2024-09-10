#include <cd.h>
#include <custom_text.h>

int CdGetSector(void *madr, int size);
int CdReady(int mode, char* result);

bool hasFullyLoaded = FALSE;
int hasStartedLoad = 0;
//CdlCB callback;

//* Set the CD-DRIVE back into music playing mode after the read
void read_cb(unsigned char status, unsigned char *result){
    //CdReadCallback(NULL);                                             // For some reason this keeps the read_cb function from happening a second time
    //CdIntToPos(*(&_musicSectors + 3), &loc);                          // Takes the abslute sector numbers of the music from ram, and adds the song offset. Then, converts them into the CdlLOC format
    //CdControlB(CdlReadS, (void *)&loc, 0);                            // Starts reading from the disk at the specified location. Must typecase void* onto CdlLOC struct pointer.
    
    int mode = CDL_MODE_RT | CDL_MODE_SPEED | CDL_MODE_SF;              // This mode is Music Playing Mode, Double Speed, Sub-header Filter. const instead of define to avoid error warning lol
    void* modePTR = &mode;                                              // Needs to be put in a ptr presumably because this function takes different arguments depending on situation
    CdControlB(CDL_PRIMITIVE_SETMODE, modePTR, NULL);                   // Puts the Cd subsystem in the right mode to play music
}

//* Read a file into ram
void ReadFileIntoRam(int sector_number, int size_to_read_in_ints, int ram_area, int offset_in_ints)
{
        _pauseMusic = 9;
        CdlLOC location;
        CdIntToPos(sector_number, &location);                           // Stores the sector in the correct format for the CdlLOC struct format
        CdControlB(CDL_PRIMITIVE_SEEKL, (void *)&location, NULL);       // Seeking to location, which was just set to our sector above to 265759. Platform specific lel just use it

        //CdControlB(CDL_PRIMITIVE_SET_LOC, (void *)&location, NULL);   // Uses CdSetloc (0x02) to prepare for CdRead
                   
        CdControlB(CDL_PRIMITIVE_READN, (void *)&location, NULL);       //Read into sector buffer 
        CdReady(0, NULL);

        if(!_musicVolume){                                              // The 3 dummy int's only appear when music is off?
            CdGetSector(ram_area, 0x3);                                 // Read the weird 3 ints of dummy data
        }


        unsigned int i = 0;
        if (offset_in_ints > 0x49)                                      // If the offset in ints is > 0x49, do multiple dummy reads into the same buffer.
        {                             
            int amount_of_dummy_reads = (offset_in_ints / 0x50);        // Amount of 0x200 byte (0x50 int) dummy reads to do in the loop
            while (i < amount_of_dummy_reads)                           // Loop 0x200 bytes of dummy data (0x50 ints) at a time down, until < 0x50 bytes of offset remain
            {
                CdGetSector(ram_area, 0x50);
                i++;
            }       
        }

        int remaining_offset = offset_in_ints - (0x50 * i);             // Calculate the remaning ints to remove, with the amount of for loop itterations            
        if (remaining_offset > 0)                                       // If there is any remaining offset bytes remaining, remove them
        {
            CdGetSector(ram_area, remaining_offset);                    // Read remaining offset data < 0x50 ints
        }


        CdGetSector(ram_area, size_to_read_in_ints);                    // Read actual data
}



