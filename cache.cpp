#include <cassert>
#include <cmath>
#include <iostream>
#include <utility>
#include "misc.h"
#include "cache.h"

SetCache::SetCache(unsigned int num_lines, unsigned int assoc)
{
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
    auto setIt = lruMaps[set][tag];
    lruLists[set].erase(setIt);
    lruLists[set].push_front(tag);
    lruMaps[set][tag] = lruLists[set].begin();
}

// Called if a new cache line is to be inserted. Checks if
// the least recently used line needs to be written back to
// main memory.
bool SetCache::checkWriteback(uint64_t set,
                                 uint64_t& tag) const
{
   cacheLine evict, temp;
   tag = lruLists[set].back();
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
    // Construct the line to be added
    cacheLine newLine;
    newLine.tag = tag;
    newLine.state = state;

    // Construct the line to be evicted
    cacheLine toEvict;
    uint64_t evictionTag = lruLists[set].back();
    toEvict.tag = evictionTag;

    // Remove the old line and add the new line
    sets[set].erase(toEvict);
    sets[set].insert(newLine);

    lruLists[set].pop_back();
    lruLists[set].push_front(tag);

    lruMaps[set].erase(evictionTag);
    lruMaps[set][tag] = lruLists[set].begin();
}

