# Cache Simulator
## Author: Clayton Kramp

## SWITCH BETWEEN LRU and Random Policy through the constructor in cache.cpp, line 13

## LRU Policy
The functions `findTag`, `updateLRU`, and `insertLine` have been completed to reflect the LRU policy.  To use LRU, set `bool policy = 'L'` in the constructor of `cache.cpp`
### findTag
In `findTag` we simply look through the `sets[set]` to see if that tag is included.  If it is, we return the cacheline's state.  Otherwise, we return INV.
### updateLRU
In `updateLRU`, we have as input a cacheline to be updated. We find the iterator to that cacheline through `lruMaps`, delete it within `lruLists`, re-add the cacheline to the front, and finally reconfigure the `lruMaps` to point to the beginning of the list.
### insertLine
In `insertLine`, we first find the cacheline to be evicted.  This is the cachline that is in the back of `lruLists`.  Then, we remove that cacheline from `lruLists[set]`, `lruMaps[set]`, and `sets[set]`.  Then, we add our new line that we get through the arguments, and add the new line to `lruLists[set]`, `lruMaps[set]`, and `sets[set]`.
### Output
Accesses: 496611

Hits: 494161

Misses: 2450

Local reads: 2446

Local writes: 245

Remote reads: 0

Remote writes: 0

Other-cache reads: 0

## Random Policy
The functions `checkWriteBack` and `insertLine` have been edited to accommadate the Random replacement policy.  In addition, a helper `generateNewNum` was created.  To use RR, set `bool policy = 'R'` in the constructor of `cache.cpp`
### generateNewNum
This is a helper to create a new random number and to fill in the variable `randomNum` to be used by both `checkWriteBack` and `insertLine`.
### checkWriteBack
This has been edited to call `generateNewNum()`, and use that to find the index that is to be evicted from `lruLists[set]`
### insertLine
If RR is being used, then we use `randomNum` to evict that index.  The rest remains the same from LRU
### Output
Accesses: 496611

Hits: 493566

Misses: 3045

Local reads: 3041

Local writes: 529

Remote reads: 0

Remote writes: 0

Other-cache reads: 0

## Ratio.txt
The following were the hit ratios I got:

494161/496611 = 99.5\% for LRU

493566/496611 = 99.38\% for Random Replacement

The results show that LRU is a better policy than random replacement.  This is an expected result
