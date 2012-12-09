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

#ifndef XEEN_ARCHIVE_FILE_H
#define XEEN_ARCHIVE_FILE_H

#include "xeen/utility.h"

namespace XEEN
{
    struct File : public Common::MemoryReadStream, public Validateable
    {
        public:
            File(CCFileId id, byte* data, uint32 datasize) : Common::MemoryReadStream(data, datasize), _id(id), _size(datasize), _data(data) { }
            ~File() { delete[] _data; }
            
            uint32 getSize()               { return _size; }
            byte* getData()                { return _data; }
            
            byte getByteAt(uint16 loc)     { return (enforce(loc < _size)) ? _data[loc] : 0; }
            byte* getBytePtrAt(uint16 loc) { return (enforce(loc < _size)) ? &_data[loc] : 0; }
            uint16 getU16At(uint16 loc)    { return getByteAt(loc) | (getByteAt(loc + 1) << 8); }
            uint32 getU32At(uint16 loc)    { return getByteAt(loc) | (getByteAt(loc + 1) << 8) | (getByteAt(loc + 2) << 16) | (getByteAt(loc + 3) << 24); }
    
        private:
            uint16 _id;
    
            uint32 _size;
            byte* _data;
    };

    typedef Common::SharedPtr<File> FilePtr;
}

#endif // XEEN_ARCHIVE_FILE_H