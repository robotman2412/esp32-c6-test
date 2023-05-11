/*
	MIT License

	Copyright    (c) 2023 Julian Scheffers

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files    (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <system.h>

#include "list_malloc.h"

#if 0
#define LOGD(msg, ...) printf("list_malloc: " msg "\n" __VA_OPT__(,) __VA_ARGS__)
#else
#define LOGD(...) do{}while(0)
#endif
#define LOG(msg, ...) printf("list_malloc: " msg "\n" __VA_OPT__(,) __VA_ARGS__)


// Magic value.
#define M_MAGIC	0xcafe0000
// Flag: Magic value.
#define M_FLAG_MAGIC	0xffff0000
// Flag: Memory region in use.
#define M_FLAG_USED		0x00000001


// Mapped block info struct.
typedef struct m_map m_map_t;
// Allocation info struct.
typedef struct m_as m_as_t;

// Mapped block info struct.
struct m_map {
	// Previous block.
	m_map_t *prev;
	// Next block.
	m_map_t *next;
	// Mapped block size excluding `sizeof(m_map_t)`.
	size_t length;
	// Pointer to first region in this mapped block.
	m_as_t *first;
};

// Allocation info struct.
struct m_as {
	// Owning `m_map` block.
	m_map_t *block;
	// Previous region struct pointer.
	m_as_t *prev;
	// Next region struct pointer.
	m_as_t *next;
	// Previous free region struct pointer.
	m_as_t *prev_free;
	// Next free region struct pointer.
	m_as_t *next_free;
	// Region size excluding `sizeof(m_as_t)`.
	size_t length;
	// Status flags.
	size_t flags;
	// Reserved for future use.
	size_t _reserved;
};



// Current free heap space tally.
static size_t free_heap = 0;
// Current allocated memory usage tally.
static size_t used_heap = 0;
// Current raw memory usage tally.
static size_t used_raw = 0;

// Pointer to first mapped block.
static m_map_t *first_map = NULL;
// Pointer to first region.
static m_as_t *first = NULL;
// Pointer to last region.
static m_as_t *last = NULL;
// Pointer to first free region.
static m_as_t *first_free = NULL;

// Round up to nearest power of two multiple.
size_t align_pow2(size_t value, size_t alignment) {
	alignment -= 1;
	return (value + alignment) & ~alignment;
}

// Initialise memory allocator.
void list_malloc_init() {
	LOGD("list_malloc_init()");
}

// Called when heap corruption is detected.
static void heap_corrupt(const m_as_t *subject) {
	puts("list_malloc: fatal error: heap corruption and/or double free detected");
	abort();
}


// Look for a large enough free region.
static m_as_t *find_free(size_t min_size) {
	LOGD("find_free(%zu)", min_size);
	m_as_t *cur = first;
	while (cur) {
		// Check magic as we go.
		if ((cur->flags & M_FLAG_MAGIC) != M_MAGIC) heap_corrupt(cur);
		// Check if region is unused and large enough.
		if (!(cur->flags & M_FLAG_USED) && cur->length >= min_size) {
			return cur;
		}
		// Next one!
		cur = cur->next;
	}
	return NULL;
}

// Insert a new free link into the list.
// Iterates forward and reverse to update `prev_free` and `next_free` of other regions.
static void insert_free(m_as_t *as) {
	LOGD("insert_free(%p)", as);
	
	if (!first_free || !as->next_free->prev_free) {
		first_free = as;
	}
	
	// Forward iteration.
	m_as_t *cur = as->next;
	if (as->next_free) as->next_free->prev_free = as;
	while (cur && (cur->flags & M_FLAG_USED)) {
		cur->prev_free = as;
		cur = cur->next;
	}
	
	// Reverse iteration.
	cur = as->prev;
	if (as->prev_free) as->prev_free->next_free = as;
	while (cur && (cur->flags & M_FLAG_USED)) {
		cur->next_free = as;
		cur = cur->prev;
	}
}

// Remove a free link from the list.
// Iterates forward and reverse to update `prev_free` and `next_free` of other regions.
static void remove_free(m_as_t *as) {
	LOGD("remove_free(%p)", as);
	
	if (!as->prev_free || as->next_free->prev_free == as) {
		first_free = as->next_free;
	}
	
	// Forward iteration.
	m_as_t *cur = as->next;
	if (as->next_free) as->next_free->prev_free = as->prev_free;
	while (cur && (cur->flags & M_FLAG_USED)) {
		cur->prev_free = as->prev_free;
		cur = cur->next;
	}
	
	// Reverse iteration.
	cur = as->prev;
	if (as->prev_free) as->prev_free->next_free = as->next_free;
	while (cur && (cur->flags & M_FLAG_USED)) {
		cur->next_free = as->next_free;
		cur = cur->prev;
	}
}


// Map more memory to expand the heap.
static bool heap_expand(size_t min_size) {
	LOGD("heap_expand(0x%zx)", min_size);
	
	// Make size a multiple of 1KB.
	if (min_size < 1024) {
		min_size = 1024;
	} else if (min_size % 1024) {
		min_size = min_size - min_size % 1024 + 1024;
	}
	
	// Request memory.
	void *raw_mem = __mem_map(min_size, sizeof(size_t), false);
	LOGD("__mem_map(0x%zx, %zu, false) = %p", min_size, sizeof(size_t), raw_mem);
	if (!raw_mem) return false;
	used_raw += min_size;
	
	// Set up mapping information.
	m_map_t *map = raw_mem;
	map->length = min_size - sizeof(m_map_t);
	
	// Add to linked list.
	map->prev = NULL;
	map->next = first_map;
	first_map = map;
	
	// Set up free space information.
	m_as_t *as = (m_as_t *) ((size_t) raw_mem + sizeof(m_map_t));
	map->first = as;
	as->block  = map;
	as->flags  = M_MAGIC;
	as->length = min_size - sizeof(m_map_t) - sizeof(m_as_t);
	free_heap += as->length;
	
	LOGD("map->next = %p", map->next);
	if (map->next) {
		m_as_t *next = map->next->first;
		m_as_t *prev = map->next->first->prev;
		
		// Construct link.
		as->prev = prev;
		as->next = next;
		
		if (next->flags & M_FLAG_USED) {
			as->next_free = next->next_free;
		} else {
			as->next_free = next;
		}
		if (prev && prev->flags & M_FLAG_USED) {
			as->prev_free = prev->prev_free;
		} else {
			as->prev_free = prev;
		}
		
		// Insert in linked list.
		next->prev_free = as;
		next->prev = as;
		if (prev) {
			prev->next = as;
			prev->next_free = as;
		} else {
			first = as;
		}
		
	} else {
		m_as_t *prev = last;
		
		// Construct link.
		as->prev = prev;
		as->next = NULL;
		
		as->next_free = NULL;
		if (prev && prev->flags & M_FLAG_USED) {
			as->prev_free = prev->prev_free;
		} else {
			as->prev_free = prev;
		}
		
		// Concatenate to linked list.
		if (prev) {
			prev->next = as;
			prev->next_free = as;
			last = as;
		} else {
			first = as;
			last = as;
		}
		last = as;
	}
	insert_free(as);
}

// Check for releasing heap memory blocks.
static bool heap_contract(m_as_t *as) {
	LOGD("heap_contract(%p)", as);
	m_map_t *map = as->block;
	if (as != map->first) {
		LOGD("  (not an empty block: %p, %p != %p)", map, as, map->first);
		// Not a free block.
		return false;
	}
	if (as->length != map->length - sizeof(m_as_t)) {
		LOGD("  (not an empty block: %p, %zu != %zu)", map, as->length, map->length - sizeof(m_as_t));
		// Not a free block.
		return false;
	}
	
	// Unlink the AS.
	as->flags = 0;
	if (as->prev) {
		as->prev->next      = as->next;
		as->prev->next_free = as->next_free;
	} else {
		first      = as->next;
		first_free = as->next_free;
	}
	if (as->next) {
		as->next->prev      = as->prev;
		as->next->prev_free = as->prev_free;
	} else {
		last = as->prev;
	}
	
	// Unlink the block.
	if (map->prev) {
		map->prev->next = map->next;
	} else {
		first_map = map->next;
	}
	if (map->next) {
		map->next->prev = map->prev;
	}
	
	// Return the memory to the OS.
	LOGD("__mem_unmap(%p)", map);
	__mem_unmap(map);
	return true;
}


// Try to split the region to free up some memory.
static bool as_split(m_as_t *as, size_t min_size) {
	LOGD("as_split(%p, %zu)", as, min_size);
	// Align `min_size` to be a multiple of `sizeof(size_t)`.
	if (min_size % sizeof(size_t)) {
		min_size += sizeof(size_t) - min_size & sizeof(size_t);
	}
	
	// Determine whether cutting is feasible.
	if (as->length < min_size + 2*sizeof(m_as_t)) return false;
	
	// Compute address of cut.
	m_as_t *cut = (m_as_t *) ((size_t) as + sizeof(m_as_t) + min_size);
	cut->length = as->length - sizeof(m_as_t) - min_size;
	as->length  = min_size;
	
	// Create new entry.
	cut->flags = M_MAGIC;
	cut->block = as->block;
	cut->prev  = as;
	cut->next  = as->next;
	
	if (!(as->flags & M_FLAG_USED)) {
		cut->prev_free = as;
	} else {
		cut->prev_free = as->prev_free;
	}
	cut->next_free = as->next_free;
	
	// Insert it into the list.
	as->next = cut;
	if (cut->next) {
		cut->next->prev = cut;
	} else {
		last = cut;
	}
	insert_free(cut);
	
	return true;
}

// Try to merge the region with the next free one.
static m_as_t *as_merge_next(m_as_t *as) {
	LOGD("as_merge_next(%p)", as);
	m_as_t *next = as->next;
	if (!next) return NULL;
	
	// Check for touching regions.
	if (next != (m_as_t *) ((size_t) as + sizeof(m_as_t) + as->length)) {
		LOGD("  (not touching: %p, %p)", as, next);
		return NULL;
	}
	// Check for free-ness.
	if (next->flags & M_FLAG_USED) return NULL;
	
	// Merge the things.
	remove_free(next);
	as->length   += sizeof(m_as_t) + next->length;
	as->next      = next->next;
	as->next_free = next->next_free;
	if (as->next) {
		as->next->prev = as;
	}
	next->flags   = 0;
	if (!(as->flags & M_FLAG_USED)) {
		insert_free(as);
	}
	
	return as;
}

// Try to merge the region with the previous free one.
static m_as_t *as_merge_prev(m_as_t *as) {
	LOGD("as_merge_prev(%p)", as);
	m_as_t *prev = as->prev;
	if (!prev) return NULL;
	
	// Check for touching regions.
	if (as != (m_as_t *) ((size_t) prev + sizeof(m_as_t) + prev->length)) {
		LOGD("  (not touching: %p, %p)", prev, as);
		return NULL;
	}
	// Check for free-ness.
	if (prev->flags & M_FLAG_USED) return NULL;
	
	// Merge the things.
	if (!(as->flags & M_FLAG_USED)) {
		remove_free(as);
	} else {
		remove_free(prev);
	}
	prev->length   += sizeof(m_as_t) + as->length;
	prev->next      = as->next;
	prev->next_free = as->next_free;
	if (prev->next) {
		prev->next->prev = prev;
	}
	prev->flags     = as->flags;
	as->flags       = 0;
	if (!(prev->flags & M_FLAG_USED)) {
		insert_free(prev);
	}
	
	return prev;
}


// Allocate `size` bytes of memory.
void *list_malloc(size_t size) {
	LOGD("list_malloc(%zu)", size);
	m_as_t *as;
	if (!size) return NULL;
	size = align_pow2(size, sizeof(size_t));
	
	if (!first_map) {
		// Allocate initial memory.
		if (!heap_expand(sizeof(m_map_t) + sizeof(m_as_t) + size)) return NULL;
		
		// Look for a suitably large free region.
		as = find_free(size);
		
	} else {
		// Look for a suitably large free region.
		as = find_free(size);
		
		if (!as) {
			// Try to expand memory to fit this.
			if (!heap_expand(sizeof(m_map_t) + sizeof(m_as_t) + size)) return NULL;
			// Try again.
			as = find_free(size);
		}
	}
	if (!as) return NULL;
	
	// Optional splittening.
	as_split(as, size);
	
	// Mark region as used.
	as->flags |= M_FLAG_USED;
	remove_free(as);
	return (void *) (as + 1);
}

// Reallocate `mem` to become `size` bytes.
void *list_realloc(void *mem, size_t size) {
	LOGD("list_realloc(%p, %zu)", mem, size);
	if (mem && !first_map) heap_corrupt(NULL);
	if (!size) {
		list_free(mem);
		return NULL;
	}
	if (!mem) return list_malloc(size);
	size = align_pow2(size, sizeof(size_t));
	
	// Find region pointer.
	m_as_t *as = (m_as_t *) ((size_t) mem - sizeof(m_as_t));
	if ((as->flags & M_FLAG_MAGIC) != M_MAGIC) heap_corrupt(as);
	
	// Check if change is necessary.
	if (as->length >= size && as->length < size + 2*sizeof(m_as_t)) {
		return mem;
	}
	
	if (as->length < size) {
		m_as_t *new_as;
		
		// Try expanding forward.
		if (new_as = as_merge_next(as)) return new_as+1;
		
		// TODO: Try expanding backward.
		if (new_as = as_merge_prev(as)) return new_as+1;
		
		// Try new allocation.
		void *new_mem = list_malloc(size);
		if (new_mem) {
			memcpy(new_mem, mem, size);
			list_free(mem);
			return new_mem;
		}
		
		// Out of ideas.
		return NULL;
		
	} else /* as->length >= size */ {
		// Optionally split.
		as_split(as, size);
		return mem;
	}
}

// Free `mem`.
void list_free(void *mem) {
	LOGD("list_free(%p)", mem);
	if (!mem) return;
	
	// Find region pointer.
	m_as_t *as = (m_as_t *) ((size_t) mem - sizeof(m_as_t));
	if ((as->flags & M_FLAG_MAGIC) != M_MAGIC) heap_corrupt(as);
	if (!(as->flags & M_FLAG_USED)) heap_corrupt(as);
	
	// Mark as free.
	as->flags &= ~M_FLAG_USED;
	
	// Try to merge the thing.
	as = as_merge_next(as) ?: as;
	as = as_merge_prev(as) ?: as;
	
	// Update free list.
	insert_free(as);
	
	// Check for releasing memory blocks.
	heap_contract(as);
}

// Get real size of `mem`.
size_t list_alloc_size(void *mem) {
	// Find region pointer.
	m_as_t *as = (m_as_t *) ((size_t) mem - sizeof(m_as_t));
	if ((as->flags & M_FLAG_MAGIC) != M_MAGIC) heap_corrupt(as);
	// Return the length verbatim.
	if (!(as->flags & M_FLAG_USED)) heap_corrupt(as);
	return as->length;
}


// Print debug info.
void list_alloc_debug() {
	m_as_t *cur = first;
	LOG("AS list:");
	while (cur) {
		LOG("  %p %s %s %zu", cur, (cur->flags & M_FLAG_MAGIC) == M_MAGIC ? "valid" : "invalid", (cur->flags & M_FLAG_USED) ? "used" : "free", cur->length);
		cur = cur->next;
	}
	cur = first_free;
	LOG("Free AS list:");
	while (cur) {
		LOG("  %p %s %s %zu", cur, (cur->flags & M_FLAG_MAGIC) == M_MAGIC ? "valid" : "invalid", (cur->flags & M_FLAG_USED) ? "used" : "free", cur->length);
		cur = cur->next_free;
	}
	m_map_t *map = first_map;
	LOG("Map list:");
	while (map) {
		LOG("  %p %zu", map, map->length);
		map = map->next;
	}
}
