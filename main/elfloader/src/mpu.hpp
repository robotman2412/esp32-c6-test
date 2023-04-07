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

#pragma once

#include <stdint.h>
#include <stddef.h>

#include <vector>

namespace mpu {

// Whether this platform has an MPU.
bool supported();
// Whether this platform has a privilege-based MPU.
bool hasPrivilege();

// Minimum (least trusted) privilege level.
// Not necessarily lower than `MAX_PRIVILEGE`.
extern const int PRIV_MIN;
// Maximum (most trusted) privilege level.
// Not necessarily higher than `MIN_PRIVILEGE`.
extern const int PRIV_MAX;

// Representation of an MPU region.
// Region addresses may be either constant or dynamic based on the type of MPU.
struct Region {
	// Region starting address.
	size_t base;
	// Region length.
	size_t size;
	
	// The memory's privilege requirement.
	int privilege;
	
	// Allow read.
	// May become implicitly true if either `write` or `exec` are true.
	bool read;
	// Allow write.
	bool write;
	// Allow execute.
	bool exec;
	
	// Whether this region is active (i.e. being enforced).
	bool active;
};

// Determine whether the MPU is currently enabled.
bool isActive();
// Try to activate the MPU.
// Returns success status.
bool setActive(bool activity);

// Get current number of regions.
int regionCount();
// Get maximum number of regions in current configuration.
int regionMax();

// Try to append or update an MPU region.
// If this partially overlaps with a preexisting region, the existing region is removed.
bool appendRegion(Region wdata);
// Get a list containing all regions present.
// On fail, returns an empty list.
// Entries may be in the list but empty.
std::vector<Region> readRegions();

};