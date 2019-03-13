#include <cassert>
#include <cmath>
#include <iostream>
#include <utility>
#include "misc.h"
#include "cache.h"
#include <random>

SetCache::SetCache(unsigned int num_lines, unsigned int assoc)
{
   this->assoc = assoc;
   this->policy = 'L';

   /** UNCOMMENT BELOW TO GET RANDOM POLICY **/
   // this->policy = 'R';

   assert(num_lines % assoc == 0);
   // The set bits of the address will be used as an index
   // into sets. Each set is a set containing "assoc" items
   sets.resize(num_lines / assoc);
   lruLists.resize(num_lines / assoc);
   lruMaps.resize(num_lines / assoc);
   for(unsigned int i=0; i < sets.size(); i++) {
      for(unsigned int j=0; j < assoc; ++j) {
         cacheLine temp;
         temp.tag = j;
         temp.state = INV;
         sets[i].insert(temp);
         lruLists[i].push_front(j);
         lruMaps[i].insert(make_pair(j, lruLists[i].begin()));
      }
   }
}

/* COMPLETED invalid vs not found */
// Given the set and tag, return the cache lines state
cacheState SetCache::findTag(uint64_t set,
                              uint64_t tag) const
{
    cacheLine temp;
    temp.tag = tag;
    auto it = sets[set].find(temp);
    if (it != sets[set].end()) return it->state;
    return INV;
}


/* COMPLETED invalid vs not found */
// Changes the cache line specificed by "set" and "tag" to "state"
void SetCache::changeState(uint64_t set, uint64_t tag,
                              cacheState state)
{
   cacheLine temp;
   temp.tag = tag;
   std::set<cacheLine>::const_iterator it = sets[set].find(temp);

   if(it != sets[set].end()) {
      cacheLine *target;
      target = (cacheLine*)&*it;
      target->state = state;
   }
}

// COMPLETED
// A complete LRU is mantained for each set, using a separate
// list and map. The front of the list is considered most recently used.
//
// updateLRU called when there is a hit
void SetCache::updateLRU(uint64_t set, uint64_t tag)
{
    // Given a tag, LruMaps can quickly find the corresponding cache line.
    // - Dr. Wu
    // So, use the map to find the iterator
    // to the set, erase, and add the new item

    auto it = lruMaps[set][tag];
    lruLists[set].erase(it);
    lruLists[set].push_front(tag);
    lruMaps[set][tag] = lruLists[set].begin();
}

void SetCache::generateNewNum() {
   randomNum = rand() % assoc;
}

// Called if a new cache line is to be inserted. Checks if
// the least recently used line needs to be written back to
// main memory.
bool SetCache::checkWriteback(uint64_t set,
                                 uint64_t& tag)
{
   generateNewNum();
   cacheLine evict, temp;
   if (policy == 'L') {
       tag = lruLists[set].back();
   } else {
       auto it = lruLists[set].begin();
       std::advance(it, randomNum);
       tag = *it;
   }
   temp.tag = tag;
   evict = *sets[set].find(temp);

   return (evict.state == MOD || evict.state == OWN);
}

// COMPLETED: invalid vs not found
// Insert a new cache line by popping the least recently used line
// and pushing the new line to the front (most recently used)
void SetCache::insertLine(uint64_t set, uint64_t tag,
                           cacheState state)
{
    // Construct the line to be evicted
    cacheLine toEvict;
    uint64_t evictionTag = 0;

    // Get the tag of the cacheline to be evicted
    // If LRU, choose that one in the back
    // If random, randomly select a cacheline to replace
    if (policy == 'L') {
        evictionTag = lruLists[set].back();
        // Then remove it
        lruLists[set].pop_back();
    } else {
        // Remove the random number
        auto it = lruLists[set].begin();
        // From generateNewNum() called in checkWriteBack we have generated a new number
        // Use that to delete
        std::advance(it, randomNum);
        evictionTag = *it;
        lruLists[set].erase(it);
    }

    toEvict.tag = evictionTag;
    // Remove the old cacheline 
    sets[set].erase(toEvict);
    lruMaps[set].erase(evictionTag);

    // Construct the line to be added
    cacheLine insertingLine;
    insertingLine.tag = tag;
    insertingLine.state = state;
    // Add the line
    sets[set].insert(insertingLine);
    lruLists[set].push_front(tag);
    lruMaps[set][tag] = lruLists[set].begin();
}
