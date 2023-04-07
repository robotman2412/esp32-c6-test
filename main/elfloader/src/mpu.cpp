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

namespace mpu {

// Whether this platform has an MPU.
bool supported() __attribute__((weak));
bool supported() { return false; }
// Whether this platform has a privilege-based MPU.
bool hasPrivilege() __attribute__((weak));
bool hasPrivilege() { return false; }

// Minimum (least trusted) privilege level.
// Not necessarily lower than `MAX_PRIVILEGE`.
extern const int PRIV_MIN __attribute__((weak));
const int PRIV_MIN = 0;
// Maximum (most trusted) privilege level.
// Not necessarily higher than `MIN_PRIVILEGE`.
extern const int PRIV_MAX __attribute__((weak));
const int PRIV_MAX = 0;

// Determine whether the MPU is currently enabled.
bool isActive() __attribute__((weak));
bool isActive() { return false; }
// Try to activate the MPU.
// Returns success status.
bool setActive(bool activity) __attribute__((weak));
bool setActive(bool activity) { return false; }

// Get current number of regions.
int regionCount() __attribute__((weak));
int regionCount() { return 0; }
// Get maximum number of regions in current configuration.
int regionMax() __attribute__((weak));
int regionMax() { return 0; }


// Try to append or update an MPU region.
// If this partially overlaps with a preexisting region, the existing region is removed.
bool appendRegion(Region wdata) __attribute__((weak));
bool appendRegion(Region wdata) { return false; }
// Get a list containing all regions present.
// On fail, returns an empty list.
// Entries may be in the list but empty.
std::vector<Region> readRegions() __attribute__((weak));
std::vector<Region> readRegions() { return {}; }

};