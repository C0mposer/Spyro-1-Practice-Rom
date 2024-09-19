#include <common.h>
#include <cosmetic.h>

extern SparxColor sparx_color_index;

int* end_of_rand_switch = 0; // Global because we can't have an argument in the function below, as we are injecting a jump to it

// Change Sparx's Particle Color. This function get's jumped to by the particle overlay code
__attribute__((noreturn)) // dont return
void ChangeFullSparxParticleColor()
{
    register byte* s2 asm("s2"); // Particle register

    switch (sparx_color_index)
    {
        case(SPARX_SKIN_DEFAULT):
        {
            s2[0xc] = 0xFF; // Red
            s2[0xd] = 0xFF; // Green
            s2[0xe] = 0x00; // Blue
            break;
        }
        case(SPARX_SKIN_RED):
        {
            s2[0xc] = 0xFF; // Red
            s2[0xd] = 0x00; // Green
            s2[0xe] = 0x00; // Blue
            break;
        }
        case(SPARX_SKIN_ICY):
        {
            s2[0xc] = 0x0; // Red
            s2[0xd] = 0xB0; // Green
            s2[0xe] = 0xFF; // Blue
            break;
        }
        case(SPARX_SKIN_JADE):
        {
            s2[0xc] = 0x0; // Red
            s2[0xd] = 0xFF; // Green
            s2[0xe] = 0x90; // Blue
            break;
        }
        case(SPARX_SKIN_DARK_BLUE):
        {
            s2[0xc] = 0x00; // Red
            s2[0xd] = 0x00; // Green
            s2[0xe] = 0xFF; // Blue
            break;
        }
        case(SPARX_SKIN_PURPLE):
        {
            s2[0xc] = 0x70; // Red
            s2[0xd] = 0x00; // Green
            s2[0xe] = 0xFF; // Blue
            break;
        }
        case(SPARX_SKIN_ORANGE):
        {
            s2[0xc] = 0xFF; // Red
            s2[0xd] = 0x8B; // Green
            s2[0xe] = 0x00; // Blue
            break;
        }
        case(SPARX_SKIN_PINK):
        {
            s2[0xc] = 0xFA; // Red
            s2[0xd] = 0x14; // Green
            s2[0xe] = 0xFF; // Blue
            break;
        }
        case(SPARX_SKIN_TURQUOISE):
        {
            s2[0xc] = 0x00; // Red
            s2[0xd] = 0xF6; // Green
            s2[0xe] = 0xBC; // Blue
            break;
        }
        case(SPARX_SKIN_SILVER):
        {
            s2[0xc] = 0xB0; // Red
            s2[0xd] = 0xB0; // Green
            s2[0xe] = 0xB0; // Blue
            break;
        }
        case(SPARX_SKIN_BLACK):
        {
            s2[0xc] = 0x20; // Red
            s2[0xd] = 0x20; // Green
            s2[0xe] = 0x20; // Blue
            break;
        }
    }

    // jump to end_of_rand_switch (where normal control flow in the original function should resume)
    asm(
        "j %0"
        :
        : "r" (end_of_rand_switch)
    );
}

// Finds the specific opcodes in the particle overlay function for sparx's particles
int* FindSparxParticleCode(void)
{
    int* funcptr_create_particle = (int*)0x800758e4; // Global function pointer to CreateParticle

    int* create_particle_current_addr = *funcptr_create_particle; // Address of CreateParticle in current level overlay

    // Find the sparx particle code
    while (*create_particle_current_addr != (int*)0x8C428BBC)
    {
        create_particle_current_addr++;
    }
    //printf("Found! %X\n", create_particle_current_addr);

    int* start_of_sparx_particle_code = create_particle_current_addr;  // Address of the sparx particle code
    end_of_rand_switch = start_of_sparx_particle_code + 45;            // Address to our the end of the switch, which will be where we return normal control flow

    return start_of_sparx_particle_code;

}

// Injects the "ChangeSparxParticleColor" function into the relavent particle overlay function
void InjectChangeSparxParticleColorJump()
{
    int* start_of_sparx_particle_code = FindSparxParticleCode();

    int* hook_into_color = start_of_sparx_particle_code + 5;           // Address to opcode for jump we will replace

    int* funcptr_ChangeFullSparxParticleColor = ChangeFullSparxParticleColor; // Save pointer to our function
    short opcode_ChangeFulLSparxParticleColor = ((int)funcptr_ChangeFullSparxParticleColor / 4 - 0x20000000); // Get the address of our function to be used in a j opcode. (addr/4 without 0x80)
    *(funcptr_ChangeFullSparxParticleColor + 2) = 0x0; // NOP the stack pointer decrement of our compiled ChangeFullSparxParticleColor function. (Can't remove prolouge?)

    // Dynamic Hook
    *((short*)start_of_sparx_particle_code + 10) = opcode_ChangeFulLSparxParticleColor; // j ChangeFullSparxParticleColor
    *((short*)start_of_sparx_particle_code + 11) = 0x0800; // j opcode
    *(start_of_sparx_particle_code + 6) = 0x00000000; // nop the branch delay
}

// // Set sparx's particles to always be random, or not
// void SetParticlesAlwaysRandom(bool b)
// {
//     int* start_of_sparx_particle_code = FindSparxParticleCode();

//     int* spyro_health_if_statement = start_of_sparx_particle_code + 2; // Address to opcode for if(spyro.health < 3)

//     if (b == true)
//         spyro_health_if_statement = 0x28420004; // if (spyro.health < 4)
//     else
//         spyro_health_if_statement = 0x28420003; // if (spyro.health < 3)
// }


//return to 0x80087f54
