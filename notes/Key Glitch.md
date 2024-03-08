**TLDR: Key Glitch is the result of a dangling pointer to the root gem that spawns out of a keychest, and can be avoided 100% of the time by resetting your console after attempts.** 

**Write Up:**  
Key Glitch has plauged S1 runners for a while. It causes a keychest to not open, even though you have seemingly grabbed the key.

Within the past year or so, people have noticed that key glitch doesn't happen on their first attempt from console boot. It only seems to happen after subsequent attempts. This implied that something wasn't being reset properly when resetting the game through the menu.

**The reason Keyglitch happens, is the result of dangling pointer relating to the root gem of a keychest.**

When you open a keychest, a global pointer in memory at **0x80075758** holds the address of the last root gem moby *(object)* that gets spawned from a keychest. The **root gem** is the first gem that spawns from the keychest.

This gem gets allocated into a heap specifically for the dynamic mobys that get spawned into the level. 

Dynamic mobys are the objects that don't exist until some event spawns them.   
*For example: gems that come out of boxes, gems that fall out of enemies, life's that break out of life chests, and of course, the gems that come out of a keychest.*

This root gem pointer is used in the **CollectGem** function to check if the current gem being collected, is equal to that root-gem that got spawned from the keychest.

Here is a simplified version CollectGem Function with the relevant code commented:

```c

enum KeyStates
{
    KEY_NOT_COLLECTED = 0,
    KEY_COLLECTED = 1,
    KEYCHEST_OPENED = 2,
    ROOTGEM_COLLECTED = 3;
};

void CollectGem(Moby* self)

{
  int amount;
  
  amountOfPhysicalGemsCollected = amountOfPhysicalGemsCollected + 1;

  // Checking if the current gem being collected is equal to the root gem pointer. If it is, set the keystate to 3.
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

However, the problem is that this global pointer that is pointing into dynamically spawned mobys... **is never reset back to NULL, ever**. Even when you go to the title screen. This global pointer is only changed when you open another keychest, changing it to that new root gem.

So in simple terms, if you are running any%, and reset anytime between opening the keychest in tree tops, and before opening another keychest, this pointer will be dangling into dynamically allocated moby data. 

So next time you enter tree tops, and you break a chest, or kill an enemy, that ***spawns a gem that happens to be allocated into the same exact memory location as the root gem was allocated to on your last run***... when you collect that gem, the CollectGem function will think you have collected the root gem!  
Your keystate will be changed from KEY_NOT_COLLECTED/KEY_COLLECTED (0/1) to ROOTGEM_COLLECTED *(3)*, effectivly removing the key from your inventory.  
Once you are in either state KEYCHEST_OPENED/ROOTGEM_COLLECTED (2/3), the key is considered "used", as normally you would only be able to access state ROOTGEM_COLLECTED *(3)* once you have used to key!

You may be wondering, isn't it highly unlikely that a gem from a chest/enemy spawns into the ***same exact*** memory location as the root gem on your last run? And why does it only happen in the same level you were in before?

Every level has its own unique statically sized memory location for dynamically allocated mobys. As soon as you enter a level, dynamic mobys will start spawning at the same location, every time. Each moby is also the same in size, 0x58 bytes. Effectively meaning that you will very likely cycle through the same memory locations for these dynamic mobys as they spawn/despawn. 

And since each level has its own memory location for dynamic mobys, you will never encounter a situation where breaking a keychest in one level, will cause key glitch in another level.

Considering why this works, it makes sense why it happens most in any%, in tree tops. The first key chest we open in any% is in tree tops, and so if we reset to a failed stoneknot, that pointer will be dangling until we get back to that first keychest in tree tops again. 

If we reset after any other key chest, for example jacques, that pointer will dangle up until we break the tree tops key chest.  

This is why it doesn't happen in other levels normally.

This also explains why Saboom suposedly got it in stone hill while practicing ILs. If one were to open a keychest in stone hill, and then reset just to go straight back into stone hill, the pointer will still be dangling from the stone hill keychest root gem prior.  

Key glitch can happen in any level if you don't open any other key chests in between your reset, like with IL practice. It just happens to be any% is one of the only examples where this happens in an actual category, with tree tops.

Thanks for reading :) Write up by: Composer

Shoutouts to OddKara for helping in research
