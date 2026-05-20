# Nestor Skip Frame Data
Get feedback on your Nestor Skip attempts to quickly diagnose what inputs you're early/late on.

# How To Use
When you load into the Practice Rom, it will automatically save a state the frame you load into artisans.  
  
You can attempt nestor skip over and over by reloading this savestate.  
  
After you do a Nestor Skip attempt, you will be presented with 5 numbers.  
  
These numbers represent frame data, for how early/late you are, for each input after the initial jump.

| Number | Input |
| --- | --- |
 1 | Flame |
 2 | Charge |
 3 | Jump |
 4 | Charge |
 5 | Glide |

# Examples
A period (**.**) before a number, represents a **(-)**, for being early. 

For example:
```
0 0 0 0 0
```
Would be a perfect attempt.

---
```
0 1 1 1 1
```
Would be every input after the flame, was 1 frame late.
  
---
```
0 .1 1 0 2
```
would be the charge is one frame early, the jump is one frame late, and the glide is 2 frames late.

---

# Sequences
There are many known sequences that work. The reference point for this tool is the original Daze Bagabones setup.  
  
Here is a table of all currently known sequences that work:
| Sequence | Used by |
| --- | --- |
| 0 0 0 0 0 | Composer, Daze, Jordy, OddKara|
| 0 1 2 3 3 | Dayoman, Lumilaura |
| 0 0 4 5 5 | AdamTheDragon |

It is recommended to aim for 0 0 0 0 0.

---
If you decide to use another sequence, keep in mind you'll need to compare your timings relative to the sequence you chose.

