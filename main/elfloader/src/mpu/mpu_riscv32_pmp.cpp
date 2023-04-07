/*
	MIT License

	Copyright (c) 2023 Julian Scheffers

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
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

#include "mpu.hpp"



// PMP region count: either 16 or 64.
#define PMP_REGIONS 16



namespace mpu {

// Whether this platform has an MPU.
bool supported() {
	// Unfortunately, there is currently no way to determine this :(
	return true;
}

// Whether this platform has a privilege-based MPU.
bool hasPrivilege() {
	return false;
}



// Minimum (least trusted) privilege level.
// Not necessarily lower than `MAX_PRIVILEGE`.
const int PRIV_MIN = 0;
// Maximum (most trusted) privilege level.
// Not necessarily higher than `MIN_PRIVILEGE`.
const int PRIV_MAX = 0;



// Determine whether the MPU is currently enabled.
bool isActive() {
	return true;
}

// Try to activate the MPU.
// Returns success status.
bool setActive(bool activity) {
	return false;
}


// Get current number of regions.
int regionCount() {
	return 0;
}

// Get maximum number of regions in current configuration.
int regionMax() {
	return PMP_REGIONS;
}


// Try to append or update an MPU region.
// If this partially overlaps with a preexisting region, the existing region is removed.
bool appendRegion(Region wdata) {
	return false;
}

// Get a list containing all regions present.
// On fail, returns an empty list.
// Entries may be in the list but empty.
std::vector<Region> readRegions() {
	std::vector<Region> list;
	
	// Buffer for pmpcfg* registers.
	size_t pmpcfgRaw[PMP_REGIONS/sizeof(size_t)];
	// Buffer for pmpaddr* registers.
	size_t pmpaddr[PMP_REGIONS];
	
	// Unfortunately, the CSRs are not indexable, only addressable.
	// This requires manually performing CSR reads.
	asm volatile ("csrr %0, pmpcfg0"  : "=r" (pmpcfgRaw[0]));
	asm volatile ("csrr %0, pmpcfg1"  : "=r" (pmpcfgRaw[1]));
	asm volatile ("csrr %0, pmpcfg2"  : "=r" (pmpcfgRaw[2]));
	asm volatile ("csrr %0, pmpcfg3"  : "=r" (pmpcfgRaw[3]));
	#if PMP_REGIONS == 64
	asm volatile ("csrr %0, pmpcfg4"  : "=r" (pmpcfgRaw[4]));
	asm volatile ("csrr %0, pmpcfg5"  : "=r" (pmpcfgRaw[5]));
	asm volatile ("csrr %0, pmpcfg6"  : "=r" (pmpcfgRaw[6]));
	asm volatile ("csrr %0, pmpcfg7"  : "=r" (pmpcfgRaw[7]));
	asm volatile ("csrr %0, pmpcfg8"  : "=r" (pmpcfgRaw[8]));
	asm volatile ("csrr %0, pmpcfg9"  : "=r" (pmpcfgRaw[9]));
	asm volatile ("csrr %0, pmpcfg10" : "=r" (pmpcfgRaw[10]));
	asm volatile ("csrr %0, pmpcfg11" : "=r" (pmpcfgRaw[11]));
	asm volatile ("csrr %0, pmpcfg12" : "=r" (pmpcfgRaw[12]));
	asm volatile ("csrr %0, pmpcfg13" : "=r" (pmpcfgRaw[13]));
	asm volatile ("csrr %0, pmpcfg14" : "=r" (pmpcfgRaw[14]));
	asm volatile ("csrr %0, pmpcfg15" : "=r" (pmpcfgRaw[15]));
	#endif
	
	// Same type of indexing by hand applies to addresses.
	asm volatile ("csrr %0, pmpaddr0"  : "=r" (pmpaddr[0]));
	asm volatile ("csrr %0, pmpaddr1"  : "=r" (pmpaddr[1]));
	asm volatile ("csrr %0, pmpaddr2"  : "=r" (pmpaddr[2]));
	asm volatile ("csrr %0, pmpaddr3"  : "=r" (pmpaddr[3]));
	asm volatile ("csrr %0, pmpaddr4"  : "=r" (pmpaddr[4]));
	asm volatile ("csrr %0, pmpaddr5"  : "=r" (pmpaddr[5]));
	asm volatile ("csrr %0, pmpaddr6"  : "=r" (pmpaddr[6]));
	asm volatile ("csrr %0, pmpaddr7"  : "=r" (pmpaddr[7]));
	asm volatile ("csrr %0, pmpaddr8"  : "=r" (pmpaddr[8]));
	asm volatile ("csrr %0, pmpaddr9"  : "=r" (pmpaddr[9]));
	asm volatile ("csrr %0, pmpaddr10" : "=r" (pmpaddr[10]));
	asm volatile ("csrr %0, pmpaddr11" : "=r" (pmpaddr[11]));
	asm volatile ("csrr %0, pmpaddr12" : "=r" (pmpaddr[12]));
	asm volatile ("csrr %0, pmpaddr13" : "=r" (pmpaddr[13]));
	asm volatile ("csrr %0, pmpaddr14" : "=r" (pmpaddr[14]));
	asm volatile ("csrr %0, pmpaddr15" : "=r" (pmpaddr[15]));
	#if PMP_REGIONS == 64
	asm volatile ("csrr %0, pmpaddr16" : "=r" (pmpaddr[16]));
	asm volatile ("csrr %0, pmpaddr17" : "=r" (pmpaddr[17]));
	asm volatile ("csrr %0, pmpaddr18" : "=r" (pmpaddr[18]));
	asm volatile ("csrr %0, pmpaddr19" : "=r" (pmpaddr[19]));
	asm volatile ("csrr %0, pmpaddr20" : "=r" (pmpaddr[20]));
	asm volatile ("csrr %0, pmpaddr21" : "=r" (pmpaddr[21]));
	asm volatile ("csrr %0, pmpaddr22" : "=r" (pmpaddr[22]));
	asm volatile ("csrr %0, pmpaddr23" : "=r" (pmpaddr[23]));
	asm volatile ("csrr %0, pmpaddr24" : "=r" (pmpaddr[24]));
	asm volatile ("csrr %0, pmpaddr25" : "=r" (pmpaddr[25]));
	asm volatile ("csrr %0, pmpaddr26" : "=r" (pmpaddr[26]));
	asm volatile ("csrr %0, pmpaddr27" : "=r" (pmpaddr[27]));
	asm volatile ("csrr %0, pmpaddr28" : "=r" (pmpaddr[28]));
	asm volatile ("csrr %0, pmpaddr29" : "=r" (pmpaddr[29]));
	asm volatile ("csrr %0, pmpaddr30" : "=r" (pmpaddr[30]));
	asm volatile ("csrr %0, pmpaddr31" : "=r" (pmpaddr[31]));
	asm volatile ("csrr %0, pmpaddr32" : "=r" (pmpaddr[32]));
	asm volatile ("csrr %0, pmpaddr33" : "=r" (pmpaddr[33]));
	asm volatile ("csrr %0, pmpaddr34" : "=r" (pmpaddr[34]));
	asm volatile ("csrr %0, pmpaddr35" : "=r" (pmpaddr[35]));
	asm volatile ("csrr %0, pmpaddr36" : "=r" (pmpaddr[36]));
	asm volatile ("csrr %0, pmpaddr37" : "=r" (pmpaddr[37]));
	asm volatile ("csrr %0, pmpaddr38" : "=r" (pmpaddr[38]));
	asm volatile ("csrr %0, pmpaddr39" : "=r" (pmpaddr[39]));
	asm volatile ("csrr %0, pmpaddr40" : "=r" (pmpaddr[40]));
	asm volatile ("csrr %0, pmpaddr41" : "=r" (pmpaddr[41]));
	asm volatile ("csrr %0, pmpaddr42" : "=r" (pmpaddr[42]));
	asm volatile ("csrr %0, pmpaddr43" : "=r" (pmpaddr[43]));
	asm volatile ("csrr %0, pmpaddr44" : "=r" (pmpaddr[44]));
	asm volatile ("csrr %0, pmpaddr45" : "=r" (pmpaddr[45]));
	asm volatile ("csrr %0, pmpaddr46" : "=r" (pmpaddr[46]));
	asm volatile ("csrr %0, pmpaddr47" : "=r" (pmpaddr[47]));
	asm volatile ("csrr %0, pmpaddr48" : "=r" (pmpaddr[48]));
	asm volatile ("csrr %0, pmpaddr49" : "=r" (pmpaddr[49]));
	asm volatile ("csrr %0, pmpaddr50" : "=r" (pmpaddr[50]));
	asm volatile ("csrr %0, pmpaddr51" : "=r" (pmpaddr[51]));
	asm volatile ("csrr %0, pmpaddr52" : "=r" (pmpaddr[52]));
	asm volatile ("csrr %0, pmpaddr53" : "=r" (pmpaddr[53]));
	asm volatile ("csrr %0, pmpaddr54" : "=r" (pmpaddr[54]));
	asm volatile ("csrr %0, pmpaddr55" : "=r" (pmpaddr[55]));
	asm volatile ("csrr %0, pmpaddr56" : "=r" (pmpaddr[56]));
	asm volatile ("csrr %0, pmpaddr57" : "=r" (pmpaddr[57]));
	asm volatile ("csrr %0, pmpaddr58" : "=r" (pmpaddr[58]));
	asm volatile ("csrr %0, pmpaddr59" : "=r" (pmpaddr[59]));
	asm volatile ("csrr %0, pmpaddr60" : "=r" (pmpaddr[60]));
	asm volatile ("csrr %0, pmpaddr61" : "=r" (pmpaddr[61]));
	asm volatile ("csrr %0, pmpaddr62" : "=r" (pmpaddr[62]));
	asm volatile ("csrr %0, pmpaddr63" : "=r" (pmpaddr[63]));
	#endif
	
	// TODO: Translating this nonsense.
	return {};
}

};
