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

#include "common/endian.h"
#include "common/str.h"
#include "common/translation.h"

#include "gui/message.h"

#include "gob/gob.h"
#include "gob/inter.h"
#include "gob/dataio.h"
#include "gob/script.h"
#include "gob/resources.h"
#include "gob/game.h"
#include "gob/draw.h"
#include "gob/video.h"
#include "gob/save/saveload.h"
#include "gob/sound/sound.h"
#include "gob/sound/sounddesc.h"

namespace Gob {

#define OPCODEVER Inter_Geisha
#define OPCODEDRAW(i, x)  _opcodesDraw[i]._OPCODEDRAW(OPCODEVER, x)
#define OPCODEFUNC(i, x)  _opcodesFunc[i]._OPCODEFUNC(OPCODEVER, x)
#define OPCODEGOB(i, x)   _opcodesGob[i]._OPCODEGOB(OPCODEVER, x)

Inter_Geisha::Inter_Geisha(GobEngine *vm) : Inter_v1(vm) {
}

void Inter_Geisha::setupOpcodesDraw() {
	Inter_v1::setupOpcodesDraw();
}

void Inter_Geisha::setupOpcodesFunc() {
	Inter_v1::setupOpcodesFunc();

	OPCODEFUNC(0x03, oGeisha_loadCursor);
	OPCODEFUNC(0x25, oGeisha_goblinFunc);
	OPCODEFUNC(0x3A, oGeisha_loadSound);
	OPCODEFUNC(0x3F, oGeisha_checkData);
	OPCODEFUNC(0x4D, oGeisha_readData);
	OPCODEFUNC(0x4E, oGeisha_writeData);

	OPCODEGOB(0, oGeisha_gamePenetration);
	OPCODEGOB(1, oGeisha_gameDiving);
	OPCODEGOB(2, oGeisha_loadTitleMusic);
	OPCODEGOB(3, oGeisha_playMusic);
	OPCODEGOB(4, oGeisha_stopMusic);
	OPCODEGOB(6, oGeisha_caress1);
	OPCODEGOB(7, oGeisha_caress2);
}

void Inter_Geisha::setupOpcodesGob() {
}

void Inter_Geisha::oGeisha_loadCursor(OpFuncParams &params) {
	if (_vm->_game->_script->peekByte(1) & 0x80)
		warning("Geisha Stub: oGeisha_loadCursor: script[1] & 0x80");

	o1_loadCursor(params);
}

void Inter_Geisha::oGeisha_loadSound(OpFuncParams &params) {
	loadSound(-1);
}

void Inter_Geisha::oGeisha_goblinFunc(OpFuncParams &params) {
	OpGobParams gobParams;
	int16 cmd;

	cmd = _vm->_game->_script->readInt16();

	gobParams.paramCount = _vm->_game->_script->readInt16();
	gobParams.extraData = cmd;

	executeOpcodeGob(cmd, gobParams);
}

int16 Inter_Geisha::loadSound(int16 slot) {
	const char *sndFile = _vm->_game->_script->evalString();

	if (slot == -1)
		slot = _vm->_game->_script->readValExpr();

	SoundDesc *sample = _vm->_sound->sampleGetBySlot(slot);
	if (!sample)
		return 0;

	int32 dataSize;
	byte *dataPtr = _vm->_dataIO->getFile(sndFile, dataSize);
	if (!dataPtr)
		return 0;

	if (!sample->load(SOUND_SND, dataPtr, dataSize)) {
		delete[] dataPtr;
		return 0;
	}

	return 0;
}

void Inter_Geisha::oGeisha_checkData(OpFuncParams &params) {
	Common::String file = _vm->_game->_script->evalString();
	int16 varOff = _vm->_game->_script->readVarIndex();

	file.toLowercase();
	if (file.hasSuffix(".0ot"))
		file.setChar('t', file.size() - 3);

	bool exists = false;

	SaveLoad::SaveMode mode = _vm->_saveLoad->getSaveMode(file.c_str());
	if (mode == SaveLoad::kSaveModeNone) {

		exists = _vm->_dataIO->hasFile(file);
		if (!exists)
			warning("File \"%s\" not found", file.c_str());

	} else if (mode == SaveLoad::kSaveModeSave)
		exists = _vm->_saveLoad->getSize(file.c_str()) >= 0;
	else if (mode == SaveLoad::kSaveModeExists)
		exists = true;

	WRITE_VAR_OFFSET(varOff, exists ? 50 : (uint32)-1);
}

void Inter_Geisha::oGeisha_readData(OpFuncParams &params) {
	const char *file = _vm->_game->_script->evalString();

	uint16 dataVar = _vm->_game->_script->readVarIndex();

	debugC(2, kDebugFileIO, "Read from file \"%s\" (%d)", file, dataVar);

	WRITE_VAR(1, 1);

	SaveLoad::SaveMode mode = _vm->_saveLoad->getSaveMode(file);
	if (mode == SaveLoad::kSaveModeSave) {

		if (!_vm->_saveLoad->load(file, dataVar, 0, 0)) {

			GUI::MessageDialog dialog(_("Failed to load game state from file."));
			dialog.runModal();

		} else
			WRITE_VAR(1, 0);

		return;

	} else if (mode == SaveLoad::kSaveModeIgnore) {
		WRITE_VAR(1, 0);
		return;
	}

	warning("Attempted to read from file \"%s\"", file);
}

void Inter_Geisha::oGeisha_writeData(OpFuncParams &params) {
	const char *file = _vm->_game->_script->evalString();

	int16 dataVar = _vm->_game->_script->readVarIndex();
	int32 size    = _vm->_game->_script->readValExpr();

	debugC(2, kDebugFileIO, "Write to file \"%s\" (%d, %d bytes)", file, dataVar, size);

	WRITE_VAR(1, 1);

	SaveLoad::SaveMode mode = _vm->_saveLoad->getSaveMode(file);
	if (mode == SaveLoad::kSaveModeSave) {

		if (!_vm->_saveLoad->save(file, dataVar, size, 0)) {

			GUI::MessageDialog dialog(_("Failed to save game state to file."));
			dialog.runModal();

		} else
			WRITE_VAR(1, 0);

	} else if (mode == SaveLoad::kSaveModeIgnore) {
		WRITE_VAR(1, 0);
		return;
	} else if (mode == SaveLoad::kSaveModeNone)
		warning("Attempted to write to file \"%s\"", file);

	WRITE_VAR(1, 0);
}

void Inter_Geisha::oGeisha_gamePenetration(OpGobParams &params) {
	uint16 var1 = _vm->_game->_script->readUint16();
	uint16 var2 = _vm->_game->_script->readUint16();
	uint16 var3 = _vm->_game->_script->readUint16();
	uint16 var4 = _vm->_game->_script->readUint16();

	WRITE_VAR_UINT32(var4, 0);

	warning("Geisha Stub: Minigame \"Penetration\": %d, %d, %d, %d", var1, var2, var3, var4);

	// Fudge a win for now
	WRITE_VAR_UINT32(var4, 1);
}

void Inter_Geisha::oGeisha_gameDiving(OpGobParams &params) {
	uint16 var1 = _vm->_game->_script->readUint16();
	uint16 var2 = _vm->_game->_script->readUint16();
	uint16 var3 = _vm->_game->_script->readUint16();

	WRITE_VAR_UINT32(var3, 1);

	warning("Geisha Stub: Minigame \"Diving\": %d, %d, %d", var1, var2, var3);

	// Fudge a win for now
	WRITE_VAR_UINT32(var3, 0);
}

void Inter_Geisha::oGeisha_loadTitleMusic(OpGobParams &params) {
	_vm->_sound->adlibLoadTBR("geisha.tbr");
	_vm->_sound->adlibLoadMDY("geisha.mdy");
}

void Inter_Geisha::oGeisha_playMusic(OpGobParams &params) {
	// TODO: The MDYPlayer is still broken!
	warning("Geisha Stub: oGeisha_playMusic");
	// _vm->_sound->adlibPlay();
}

void Inter_Geisha::oGeisha_stopMusic(OpGobParams &params) {
	_vm->_sound->adlibStop();
	_vm->_sound->adlibUnload();
}

void Inter_Geisha::oGeisha_caress1(OpGobParams &params) {
	if (_vm->_draw->_spritesArray[0])
		_vm->_video->drawPackedSprite("hp1.cmp", *_vm->_draw->_spritesArray[0]);
}

void Inter_Geisha::oGeisha_caress2(OpGobParams &params) {
	if (_vm->_draw->_spritesArray[1])
		_vm->_video->drawPackedSprite("hpsc1.cmp", *_vm->_draw->_spritesArray[1]);
}

} // End of namespace Gob