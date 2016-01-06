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
 * MIT License:
 *
 * Copyright (c) 2009 Alexei Svitkine, Eugene Sandulenko
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "wage/wage.h"
#include "wage/entities.h"
#include "wage/design.h"
#include "wage/world.h"

#include "common/memstream.h"

namespace Wage {

void Designed::setDesignBounds(Common::Rect *bounds) {
	_designBounds = new Common::Rect(*bounds);
	_design->setBounds(bounds);
}

Designed::~Designed() {
	delete _design;
	delete _designBounds;
}

Context::Context() {
	_visits = 0;
	_kills = 0;
	_experience = 0;
	_frozen = false;

	for (int i = 0; i < 26 * 9; i++)
		 _userVariables[i] = 0;

	for (int i = 0; i < 18; i++)
		_statVariables[i] = 0;
}

Scene::Scene() {
	_script = NULL;
	_design = NULL;
	_textBounds = NULL;
	_fontSize = 0;
	_fontType = 0;

	for (int i = 0; i < 4; i++)
		_blocked[i] = false;

	_soundFrequency = 0;
	_soundType = 0;
	_worldX = 0;
	_worldY = 0;

	_visited = false;
}

Scene::Scene(String name, Common::SeekableReadStream *data) {
	_name = name;
	_classType = SCENE;
	_design = new Design(data);

	_script = NULL;
	_textBounds = NULL;
	_fontSize = 0;
	_fontType = 0;

	setDesignBounds(readRect(data));
	_worldY = data->readSint16BE();
	_worldX = data->readSint16BE();
	_blocked[Scene::NORTH] = (data->readByte() != 0);
	_blocked[Scene::SOUTH] = (data->readByte() != 0);
	_blocked[Scene::EAST] = (data->readByte() != 0);
	_blocked[Scene::WEST] = (data->readByte() != 0);
	_soundFrequency = data->readSint16BE();
	_soundType = data->readByte();
	data->readByte(); // unknown
	_messages[Scene::NORTH] = readPascalString(data);
	_messages[Scene::SOUTH] = readPascalString(data);
	_messages[Scene::EAST] = readPascalString(data);
	_messages[Scene::WEST] = readPascalString(data);
	_soundName = readPascalString(data);

	_visited = false;

	delete data;
}

Scene::~Scene() {
}

void Scene::paint(Graphics::Surface *surface, int x, int y) {
	Common::Rect r(x + 5, y + 5, _design->getBounds()->width() + x - 10, _design->getBounds()->height() + y - 10);
	surface->fillRect(r, kColorWhite);

	_design->paint(surface, ((WageEngine *)g_engine)->_world->_patterns, x, y);

	for (Common::List<Obj *>::const_iterator it = _objs.begin(); it != _objs.end(); ++it) {
		debug(2, "paining Obj: %s", (*it)->_name.c_str());
		(*it)->_design->paint(surface, ((WageEngine *)g_engine)->_world->_patterns, x, y);
	}

	for (Common::List<Chr *>::const_iterator it = _chrs.begin(); it != _chrs.end(); ++it) {
		debug(2, "paining Chr: %s", (*it)->_name.c_str());
		(*it)->_design->paint(surface, ((WageEngine *)g_engine)->_world->_patterns, x, y);
	}
}

// Source: Apple IIGS Technical Note #41, "Font Family Numbers"
// http://apple2.boldt.ca/?page=til/tn.iigs.041
const char *fontNames[] = {
	"Chicago",	// system font
	"Geneva",	// application font
	"New York",
	"Geneva",

	"Monaco",
	"Venice",
	"London",
	"Athens",

	"San Francisco",
	"Toronto",
	"Cairo",
	"Los Angeles", // 12

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, // not in Inside Macintosh

	"Times", // 20
	"Helvetica",
	"Courier",
	"Symbol",
	"Taliesin" // mobile?
};

const char *Scene::getFontName() {
	if (_fontType >= 0 && _fontType < ARRAYSIZE(fontNames) && fontNames[_fontType] != NULL) {
		return fontNames[_fontType];
	}
	return "Unknown";
}

Obj::Obj(String name, Common::SeekableReadStream *data) {
	_name = name;
	_classType = OBJ;
	_currentOwner = NULL;
	_currentScene = NULL;

	_index = 0;

	_design = new Design(data);

	setDesignBounds(readRect(data));

	int16 namePlural = data->readSint16BE();

	if (namePlural == 256)
		_namePlural = true; // TODO: other flags?
	else if (namePlural == 0)
		_namePlural = false;
	else
		error("Obj <%s> had weird namePlural set (%d)", name.c_str(), namePlural);

	if (data->readSint16BE() != 0)
		error("Obj <%s> had short set", name.c_str());

	if (data->readByte() != 0)
		error("Obj <%s> had byte set", name.c_str());

	_accuracy = data->readByte();
	_value = data->readByte();
	_type = data->readSByte();
	_damage = data->readByte();
	_attackType = data->readSByte();
	_numberOfUses = data->readSint16BE();
	int16 returnTo = data->readSint16BE();
	if (returnTo == 256) // TODO any other possibilities?
		_returnToRandomScene = true;
	else if (returnTo == 0)
		_returnToRandomScene = false;
	else
		error("Obj <%s> had weird returnTo set", name.c_str());

	_sceneOrOwner = readPascalString(data);
	_clickMessage = readPascalString(data);
	_operativeVerb = readPascalString(data);
	_failureMessage = readPascalString(data);
	_useMessage = readPascalString(data);
	_sound = readPascalString(data);

	delete data;
}

Chr *Obj::removeFromChr() {
	if (_currentOwner != NULL) {
	  for (int i = (int)_currentOwner->_inventory.size() - 1; i >= 0; i--)
			if (_currentOwner->_inventory[i] == this)
				_currentOwner->_inventory.remove_at(i);

		for (int i = 0; i < Chr::NUMBER_OF_ARMOR_TYPES; i++) {
			if (_currentOwner->_armor[i] == this) {
				_currentOwner->_armor[i] = NULL;
			}
		}
	}

	return _currentOwner;
}

Designed *Obj::removeFromCharOrScene() {
	Designed *from = removeFromChr();

	if (_currentScene != NULL) {
		_currentScene->_objs.remove(this);
		from = _currentScene;
	}

	return from;
}

Chr::Chr(String name, Common::SeekableReadStream *data) {
	_name = name;
	_classType = CHR;
	_design = new Design(data);

	_index = 0;
	_currentScene = NULL;

	setDesignBounds(readRect(data));

	_physicalStrength = data->readByte();
	_physicalHp = data->readByte();
	_naturalArmor = data->readByte();
	_physicalAccuracy = data->readByte();

	_spiritualStength = data->readByte();
	_spiritialHp = data->readByte();
	_resistanceToMagic = data->readByte();
	_spiritualAccuracy = data->readByte();

	_runningSpeed = data->readByte();
	_rejectsOffers = data->readByte();
	_followsOpponent = data->readByte();

	data->readSByte(); // TODO: ???
	data->readSint32BE(); // TODO: ???

	_weaponDamage1 = data->readByte();
	_weaponDamage2 = data->readByte();

	data->readSByte(); // TODO: ???

	if (data->readSByte() == 1)
		_playerCharacter = true;
	else
		_playerCharacter = false;

	_maximumCarriedObjects = data->readByte();
	_returnTo = data->readSByte();

	_winningWeapons = data->readByte();
	_winningMagic = data->readByte();
	_winningRun = data->readByte();
	_winningOffer = data->readByte();
	_losingWeapons = data->readByte();
	_losingMagic = data->readByte();
	_losingRun = data->readByte();
	_losingOffer = data->readByte();

	_gender = data->readSByte();
	if (data->readSByte() == 1)
		_nameProperNoun = true;
	else
		_nameProperNoun = false;

	_initialScene = readPascalString(data);
	_nativeWeapon1 = readPascalString(data);
	_operativeVerb1 = readPascalString(data);
	_nativeWeapon2 = readPascalString(data);
	_operativeVerb2 = readPascalString(data);

	_initialComment = readPascalString(data);
	_scoresHitComment = readPascalString(data);
	_receivesHitComment = readPascalString(data);
	_makesOfferComment = readPascalString(data);
	_rejectsOfferComment = readPascalString(data);
	_acceptsOfferComment = readPascalString(data);
	_dyingWords = readPascalString(data);

	_initialSound = readPascalString(data);
	_scoresHitSound = readPascalString(data);
	_receivesHitSound = readPascalString(data);
	_dyingSound = readPascalString(data);

	_weaponSound1 = readPascalString(data);
	_weaponSound2 = readPascalString(data);

	for (int i = 0; i < NUMBER_OF_ARMOR_TYPES; i++)
		_armor[i] = NULL;

	delete data;
}

void Chr::resetState() {
	_context._statVariables[PHYS_STR_BAS] = _context._statVariables[PHYS_STR_CUR] = _physicalStrength;
	_context._statVariables[PHYS_HIT_BAS] = _context._statVariables[PHYS_HIT_CUR] = _physicalHp;
	_context._statVariables[PHYS_ARM_BAS] = _context._statVariables[PHYS_ARM_CUR] = _naturalArmor;
	_context._statVariables[PHYS_ACC_BAS] = _context._statVariables[PHYS_ACC_CUR] = _physicalAccuracy;

	_context._statVariables[SPIR_STR_BAS] = _context._statVariables[SPIR_STR_CUR] = _spiritualStength;
	_context._statVariables[SPIR_HIT_BAS] = _context._statVariables[SPIR_HIT_CUR] = _spiritialHp;
	_context._statVariables[SPIR_ARM_BAS] = _context._statVariables[SPIR_ARM_CUR] = _naturalArmor;
	_context._statVariables[SPIR_ACC_BAS] = _context._statVariables[SPIR_ACC_CUR] = _physicalAccuracy;

	_context._statVariables[PHYS_SPE_BAS] = _context._statVariables[PHYS_SPE_CUR] = _runningSpeed;
}

WeaponArray *Chr::getWeapons() {
	WeaponArray *list = new WeaponArray;

	warning("STUB: getWeapons");

	return list;
}

void Chr::wearObjs() {
	for (uint i = 0; i < _inventory.size(); i++)
		wearObjIfPossible(_inventory[i]);
}

int Chr::wearObjIfPossible(Obj *obj) {
	switch (obj->_type) {
	case Obj::HELMET:
		if (_armor[HEAD_ARMOR] == NULL) {
			_armor[HEAD_ARMOR] = obj;
			return Chr::HEAD_ARMOR;
		}
		break;
	case Obj::CHEST_ARMOR:
		if (_armor[BODY_ARMOR] == NULL) {
			_armor[BODY_ARMOR] = obj;
			return Chr::BODY_ARMOR;
		}
		break;
	case Obj::SHIELD:
		if (_armor[SHIELD_ARMOR] == NULL) {
			_armor[SHIELD_ARMOR] = obj;
			return Chr::SHIELD_ARMOR;
		}
		break;
	case Obj::SPIRITUAL_ARMOR:
		if (_armor[MAGIC_ARMOR] == NULL) {
			_armor[MAGIC_ARMOR] = obj;
			return Chr::MAGIC_ARMOR;
		}
		break;
	default:
		return -1;
	}

	return -1;
}

String &Chr::getNameWithDefiniteArticle(bool capitalize) {
	Common::String *res = new Common::String;

	if (!_nameProperNoun)
		*res += capitalize ? "The " : "the ";
	*res += _name;

	return *res;
}


} // End of namespace Wage
