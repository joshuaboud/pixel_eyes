/*    frames.h
 *
 *    Copyright (C) 2021 Joshua Boudreau <joshuaboud@hotmail.com>
 *
 *    This file is part of pixel_eyes.
 *
 *    pixel_eyes is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    pixel_eyes is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with pixel_eyes.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "frame.h"

/**
 * @brief Global array of Frame structs
 * 
 */
extern const Frame frames[];

/**
 * @brief LUT of initial frame addresses for each animation
 * 
 */
extern const Frame *frame_lut[];
