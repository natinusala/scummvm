/* ScummVM - Scumm Interpreter
 * Copyright (C) 2001-2003 The ScummVM project
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header$
 *
 */

#include "audiostream.h"
#include "mixer.h"

template<int channels, int sampleSize>
class MemoryAudioInputStream : public AudioInputStream {
protected:
	const byte *_ptr;
	const byte *_end;
	void advance() { _ptr += channels * sampleSize; }
public:
	MemoryAudioInputStream(const byte *ptr, uint len) : _ptr(ptr), _end(ptr+len) { }
	virtual int size() { return (_end - _ptr) / (channels * sampleSize); }
};


template<int channels>
class Input8bitSignedStream : public MemoryAudioInputStream<channels, 1> {
protected:
	int16 readIntern() { int8 v = (int8)*_ptr; return v << 8; }
public:
	Input8bitSignedStream(const byte *ptr, int len) : MemoryAudioInputStream<channels, 1>(ptr, len) { }
};

template<int channels>
class Input8bitUnsignedStream : public MemoryAudioInputStream<channels, 1> {
protected:
	int16 readIntern() { int8 v = (int8)(*_ptr ^ 0x80); return v << 8; }
public:
	Input8bitUnsignedStream(const byte *ptr, int len) : MemoryAudioInputStream<channels, 1>(ptr, len) { }
};

template<int channels>
class Input16bitSignedStream : public MemoryAudioInputStream<channels, 2> {
protected:
	int16 readIntern() { return (int16)READ_BE_UINT16(_ptr); }
public:
	Input16bitSignedStream(const byte *ptr, int len) : MemoryAudioInputStream<channels, 2>(ptr, len) { }
};

template<int channels>
class Input16bitUnsignedStream : public MemoryAudioInputStream<channels, 2> {
protected:
	int16 readIntern() { return (int16)(READ_BE_UINT16(_ptr) ^ 0x8000); }
public:
	Input16bitUnsignedStream(const byte *ptr, int len) : MemoryAudioInputStream<channels, 2>(ptr, len) { }
};


template<int channels>
static AudioInputStream *makeInputStream(const byte *ptr, uint32 len, bool isUnsigned, bool is16Bit) {
	if (isUnsigned) {
		if (is16Bit)
			return new Input16bitUnsignedStream<channels>(ptr, len);
		else
			return new Input8bitUnsignedStream<channels>(ptr, len);
	} else {
		if (is16Bit)
			return new Input16bitSignedStream<channels>(ptr, len);
		else
			return new Input8bitSignedStream<channels>(ptr, len);
	}
}


AudioInputStream *makeInputStream(byte _flags, const byte *ptr, uint32 len) {
	if (_flags & SoundMixer::FLAG_STEREO)
		return makeInputStream<2>(ptr, len, _flags & SoundMixer::FLAG_UNSIGNED, _flags & SoundMixer::FLAG_16BITS);
	else
		return makeInputStream<1>(ptr, len, _flags & SoundMixer::FLAG_UNSIGNED, _flags & SoundMixer::FLAG_16BITS);
}
