# Source Code Overview

## Paradigms
### Update Functions
Any function that ends in the word **Update** is intended to be part of the every-frame game loop, and is ***usually*** called in main.c
### Hook Functions
Any function that ends in the word **Hook** is intended to be called from a function already from inside of the game. 
This can be to hook into some logic to change it, or just to run some code at the same time as other code.
### In Game Variables
Any variable that comes from within the game, and not created by our code, starts with an **_** before the variable name. (ex: _spyro). Any variable that follows this paradigm should be declared in common.h, and it's memory address should exist in symbols.txt

## File Overviews
## main.c
The MainHook function in main.c is being called every frame, at the end of the game's "CreateCameraMatrix" function.   

*Loading CD Code*
Most of our custom code is overwritting code we don't need from the game, so it gets loaded in automatically for us.
   However, some of it we are placing into unused BIOS memory.
  
The first thing main.c does is load some code off the CD into extra BIOS memory that is unused.

*Update Functions*
The next thing main.c does is call a list of all the other functions that need to be called every frame that are part of our mod.