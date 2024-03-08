Key Glitch is a glitch that has plauged S1 runners for a while. It causes a keychest to not open, even though you have seemingly grabbed the key.

Within the past year or so, people have noticed that key glitch only happens after resetting from a level that has a keychest. 
So it never happens on your first run from a fresh console boot.

**The reason Keyglitch happens, is the result of dangling pointer relating to the root gem of a keychest.**

When you open a keychest, a global pointer in memory at **0x80075758** holds the address of the last root gem moby *(object)* that gets spawned from a keychest. 

This gem gets allocated into a heap specifically for the dynamic mobys that get spawned into the level. 

Dynamic mobys are the objects that don't exist until some event spawns them.   
For example: gems that come out of boxes, gems fall out of enemies, lifes that break out of life chests, and of course, the gems that come out of a keychest. 

This pointer is used in the CollectGem function to check if the current gem being collected, is equal to that root-gem that got spawned from the keychest.

Here is a simplified version CollectGem Function with the relavent code commented:

```c

typedef enum KeyStates
{
    KEY_NOT_COLLECTED = 0,
    KEY_COLLECTED = 1,
    KEYCHEST_OPENED = 2,
    ROOTGEM_COLLECTED = 3;
} Keystates;

void CollectGem(Moby* self)

{
  int amount;
  
  amountOfPhysicalGemsCollected = amountOfPhysicalGemsCollected + 1;

  // Checking if the current gem being collected is equal to the root gem pointer.
  if (self == keychest_rootGemPtr) {
    keyState = ROOTGEM_COLLECTED;
  }

  UpdateMobyCollision(self,4);

  CreateParticle(6, 0xc, self, GEM_PARTICLE);

  if (self.type == RED_GEM) {
    amount = 1;
  }
  else if (self.type == GREEN_GEM) {
    amount = 2;
  }
  else if (self.type == BLUE_GEM) {
    amount = 5;
  }
  else if (self.type == YELLOW_GEM) {
    amount = 10;
  }
  else {
    amount = 25;
  }

  CollectCollectable(amount,self);
  return;
}
  ```

If the gem currently being collected is the root gem, it will set your global key state to ROOTGEM_COLLECTED *(3)*. In any normal circumstance, the key state will be at KEYCHEST_OPENED *(2)* when this check succeeds.

However, the problem is that this global pointer that is pointing into dynamically spawned mobys... is never reset back to NULL, ever. Even when you go to the title screen. This global pointer is only changed when you open another keychest, changing it to that new root gem.

So in simple terms, if you are running any%, and reset anytime between opening the keychest in tree tops, and before opening the keychest in metalhead, this pointer will be dangling into dynamically alocated moby data. 

So next time you enter tree tops, and you break a chest, or kill an enemy, that ***spawns a gem that happens to be allocated into the same exact memory location as the root gem was allocated to on your last run***... when you collect that gem, the CollectGem function will think you have collected the root gem! Your keystate will be changed from KEY_NOT_COLLECTED/KEY_COLLECTED (0/1) to ROOTGEM_COLLECTED *(3)*, effectivly removing the key from your inventory. Once you are in either state KEYCHEST_OPENED/ROOTGEM_COLLECTED (2/3), the key is considered "used", as normally you would only be able to access state ROOTGEM_COLLECTED *(3)* once you have used to key!

You may be wondering, isn't it highly unlikely that a gem from a chest/enemy spawns into the ***same exact*** memory location as the root gem on your last run? And why does it only happen in the same level you were in before?

Every level has its own unique statically sized memory location for dynamically allocated mobys. As soon as you enter a level, dynamic mobys will start spawning at the same location, every time. Each moby is also the same in size, 0x58 bytes. Effictively meaning that you will very likely cycle through the same memory locations for these dynamic mobys as they spawn/despawn. 

So since each level has its own location for dynamic mobys, you will never encounter a situation where breaking a keychest in one level, will cause keyglitch in another level.
