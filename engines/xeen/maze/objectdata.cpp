/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */
#define XEEN_MAZE_SOURCE

#include "xeen/game.h"
#include "xeen/archive/file.h"
#include "xeen/maze/objectdata_.h"

//
////

static const uint8 cloudsDAT[] =
{
     0,  4,  5,  4,  0,  0,  0,  1,  3,  4,  5,  4, 
     0,  1,  0,  1,  0,  0,  1,  1,  0,  1,  0,  1, 
     0,  1,  2,  1,  0,  0,  0,  1,  0,  1,  2,  1, 
     0,  1,  2,  1,  0,  0,  0,  1,  0,  1,  2,  1, 
     0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0, 
     0,  3,  2,  1,  0,  0,  0,  0,  0,  3,  2,  1, 
     0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0, 
     0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0, 
     0,  8,  9,  8,  0,  1,  0,  0,  7,  8,  9,  8, 
     0,  4,  4,  4,  0,  0,  1,  0,  3,  7,  7,  7, 
     0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0, 
     0,  1,  0,  1,  0,  0,  1,  1,  0,  1,  0,  1, 
     0,  1,  2,  1,  0,  1,  0,  0,  0,  1,  2,  1, 
     0,  1,  2,  3,  0,  0,  0,  0,  0,  1,  2,  3, 
     0,  1,  0,  1,  0,  0,  0,  1,  0,  1,  0,  1, 
     0,  1,  2,  3,  0,  0,  0,  0,  0,  1,  2,  3, 
     0,  5,  6,  5,  0,  0,  0,  1,  0,  5,  6,  5, 
     0,  1,  0,  1,  0,  0,  1,  1,  0,  1,  0,  1, 
     0,  4,  0,  4,  0,  0,  1,  1,  3,  7,  3,  7, 
     0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0, 
     0,  8,  9, 10,  0,  0,  0,  0,  7,  8,  9, 10, 
     0,  8,  0,  8,  0,  0,  1,  1,  7, 15,  7, 15, 
     0,  8,  0,  8,  0,  0,  1,  1,  7, 15,  7, 15, 
     0,  1,  2,  1,  0,  1,  0,  0,  0,  1,  2,  1, 
     0,  8, 16,  8,  0,  0,  0,  1,  7, 15, 23, 15, 
     0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0, 
     0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0, 
     0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0, 
     0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0, 
     0,  0,  0,  0,  0,  1,  0,  1,  7,  7,  7,  7, 
     0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0, 
     0,  4,  5,  4,  0,  0,  0,  1,  3,  4,  5,  4, 
     0,  0,  0,  0,  0,  1,  0,  1,  7,  7,  7,  7, 
     0,  1,  0,  1,  0,  0,  1,  1,  0,  1,  0,  1, 
     0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0, 
     0,  0,  0,  0,  0,  1,  0,  1,  3,  3,  3,  3, 
     0,  0,  0,  0,  0,  1,  0,  1,  3,  3,  3,  3, 
     0,  3,  1,  2,  0,  0,  0,  0,  0,  3,  1,  2, 
     0,  4,  5,  6,  0,  0,  0,  0,  3,  4,  5,  6, 
     0,  0,  0,  0,  0,  1,  0,  1,  3,  3,  3,  3, 
     0,  4,  0,  4,  0,  0,  1,  1,  3,  7,  3,  7, 
     0,  0,  0,  0,  0,  1,  0,  1,  3,  3,  3,  3, 
     0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0, 
     0,  1,  2,  1,  0,  0,  0,  1,  0,  1,  2,  1, 
     0,  1,  2,  1,  0,  0,  0,  1,  0,  1,  2,  1, 
     0,  1,  2,  1,  0,  0,  0,  1,  0,  1,  2,  1, 
     0,  4,  0,  5,  0,  0,  1,  0,  3,  4,  3,  5, 
     0,  1,  2,  1,  0,  0,  0,  1,  0,  1,  2,  1, 
     0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0, 
     0,  1,  2,  1,  0,  1,  0,  0,  0,  1,  2,  1, 
     0,  1,  2,  3,  0,  0,  0,  0,  0,  1,  2,  3, 
     0,  1,  2,  3,  0,  0,  0,  0,  0,  1,  2,  3, 
     0,  1,  1,  1,  0,  1,  0,  1,  0,  1,  1,  1, 
     0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0, 
     0,  1,  2,  1,  0,  0,  0,  1,  0,  1,  2,  1, 
     0,  0,  0,  0,  0,  1,  0,  1,  3,  3,  3,  3, 
     0,  0,  0,  0,  0,  1,  0,  1,  3,  3,  3,  3, 
     0,  1,  0,  1,  0,  0,  1,  1,  0,  1,  0,  1, 
     0,  2,  3,  2,  0,  1,  0,  0,  0,  2,  3,  2, 
     0,  0,  0,  0,  0,  1,  0,  1,  3,  3,  3,  3, 
     0,  0,  0,  0,  0,  1,  0,  1,  3,  3,  3,  3, 
     0,  1,  2,  1,  0,  0,  0,  1,  0,  1,  2,  1, 
     0,  1,  2,  1,  0,  1,  0,  0,  0,  1,  2,  1, 
     0,  8,  0,  8,  0,  1,  0,  1,  7, 15,  7, 15, 
     0,  8,  0,  8,  0,  0,  1,  1,  7, 15,  7, 15, 
     0,  1,  2,  3,  0,  0,  0,  0,  0,  1,  2,  3, 
     0,  0,  0,  0,  0,  1,  0,  1,  3,  3,  3,  3, 
     0,  0,  0,  0,  0,  1,  0,  1,  7,  7,  7,  7, 
     0,  8,  9,  8,  0,  0,  0,  1,  7,  8,  9,  8, 
     0,  0,  0,  0,  0,  1,  0,  1,  7,  7,  7,  7, 
     0,  0,  0,  0,  0,  0,  0,  1,  7,  7,  7,  7, 
     0,  0,  0,  0,  0,  1,  0,  1, 13, 13, 13, 13, 
     0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0, 
     0,  5,  6,  7,  0,  0,  0,  0,  0,  5,  6,  7, 
     0,  0,  0,  0,  0,  1,  0,  1,  7,  7,  7,  7, 
     0,  0,  0,  0,  0,  1,  0,  1,  7,  7,  7,  7, 
     0,  4,  5,  6,  0,  0,  0,  0,  3,  4,  5,  6, 
     0,  0,  0,  0,  0,  1,  0,  1,  7,  7,  7,  7, 
     0,  4,  5,  6,  0,  0,  0,  0,  3,  4,  5,  6, 
     0,  1,  3,  3,  0,  0,  1,  0,  0,  1,  3,  3, 
     0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0, 
     0,  8,  0,  8,  0,  0,  1,  1,  7, 15,  7, 15, 
     0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0, 
     0,  1,  2,  1,  0,  0,  0,  1,  0,  1,  2,  1, 
     0,  1,  0,  1,  0,  0,  1,  1,  0,  1,  0,  1, 
     0, 12,  8,  4,  0,  0,  0,  0,  3, 15, 11,  7, 
     0,  0,  0,  0,  0,  1,  0,  1,  7,  7,  7,  7, 
     0,  0,  0,  0,  0,  1,  0,  1,  3,  3,  3,  3, 
     0,  0,  0,  0,  0,  1,  0,  1,  3,  3,  3,  3, 
     0,  0,  0,  0,  0,  1,  0,  1,  7,  7,  7,  7, 
     0,  0,  0,  0,  0,  1,  0,  1,  3,  3,  3,  3, 
     0,  0,  0,  0,  0,  1,  0,  1,  3,  3,  3,  3, 
     0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0, 
     0,  4,  5,  4,  0,  1,  0,  0,  3,  4,  5,  4, 
     0,  4,  5,  6,  0,  0,  0,  0,  3,  4,  5,  6, 
     0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0, 
     0,  0,  0,  0,  0,  1,  0,  1,  6,  6,  6,  6, 
     0,  4,  4,  4,  0,  0,  1,  0,  3,  7,  7,  7, 
     0,  4,  4,  4,  0,  0,  1,  0,  3,  7,  7,  7, 
     0,  4,  4,  4,  0,  0,  1,  0,  3,  7,  7,  7, 
     0,  1,  1,  1,  0,  0,  1,  0,  0,  1,  1,  1, 
     0,  0,  0,  0,  0,  1,  0,  1,  7,  7,  7,  7, 
     0,  8,  4,  8,  0,  0,  0,  1,  3, 11,  7, 11, 
     0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0, 
     0,  0,  0,  0,  0,  1,  0,  1,  3,  3,  3,  3, 
     0,  8,  8,  8,  0,  0,  1,  0,  7,  8,  8,  8, 
     0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0, 
     0,  1,  2,  1,  0,  1,  0,  0,  0,  1,  2,  1, 
     0,  4,  0,  4,  0,  0,  1,  1,  3,  7,  3,  7, 
     0,  0,  0,  0,  0,  1,  0,  1,  7,  7,  7,  7, 
     0,  4,  5,  6,  0,  0,  0,  0,  3,  4,  5,  6, 
     0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0, 
     0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0, 
     0,  1,  1,  1,  0,  0,  1,  0,  0,  1,  1,  1, 
     0,  1,  2,  1,  0,  1,  0,  0,  0,  1,  2,  1, 
     0,  1,  2,  3,  0,  0,  0,  0,  0,  1,  2,  3, 
     0,  1,  2,  3,  0,  0,  0,  0,  0,  1,  2,  3, 
     0,  3,  2,  1,  0,  0,  0,  0,  0,  3,  2,  1, 
     0,  1,  2,  3,  0,  0,  0,  0,  0,  1,  2,  3, 
     0,  1,  2,  3,  0,  0,  0,  0,  0,  1,  2,  3, 
     0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0, 
};
//

XEEN::Maze::ObjectData::ObjectData() : _cloudsDAT(XEENgame.getFile("CLOUDS.DAT")), _darkDAT(XEENgame.getFile("DARK.DAT"))
{
    const Game::Type gt = XEENgame.getGameType();

    if((gt == Game::DARKSIDE || gt == Game::WORLD) && (!valid(_cloudsDAT) || !valid(_darkDAT)))
    {
        markInvalid("GAME.DAT file not found.");
    }

    // TODO: Other games
}

const uint8* XEEN::Maze::ObjectData::getDataForObject(uint32 id) const
{
    XEEN_VALID();

    const Game::Type gt = XEENgame.getGameType();

    if(enforce(id < MAX_OBJECTS) && gt == Game::CLOUDS)
    {
        return &cloudsDAT[id * 12];
    }

    // TODO: Other games
    assert(false);
}

