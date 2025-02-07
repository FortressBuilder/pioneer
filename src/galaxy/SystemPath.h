// Copyright © 2008-2016 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef _SYSTEMPATH_H
#define _SYSTEMPATH_H

#include "Serializer.h"
#include "LuaWrappable.h"
#include "json/json.h"
#include <stdexcept>

class SystemPath : public LuaWrappable {
public:
	struct ParseFailure : public std::invalid_argument {
		ParseFailure(): std::invalid_argument("invalid SystemPath format") {}
	};
	static SystemPath Parse(const char * const str);

	SystemPath() :
		sectorX(0), sectorY(0), sectorZ(0), systemIndex(Uint32(-1)), bodyIndex(Uint32(-1)) {}

	SystemPath(Sint32 x, Sint32 y, Sint32 z) :
		sectorX(x), sectorY(y), sectorZ(z), systemIndex(Uint32(-1)), bodyIndex(Uint32(-1)) {}
	SystemPath(Sint32 x, Sint32 y, Sint32 z, Uint32 si) :
		sectorX(x), sectorY(y), sectorZ(z), systemIndex(si), bodyIndex(Uint32(-1)) {}
	SystemPath(Sint32 x, Sint32 y, Sint32 z, Uint32 si, Uint32 bi) :
		sectorX(x), sectorY(y), sectorZ(z), systemIndex(si), bodyIndex(bi) {}

	SystemPath(const SystemPath &path)
		: sectorX(path.sectorX), sectorY(path.sectorY), sectorZ(path.sectorZ), systemIndex(path.systemIndex), bodyIndex(path.bodyIndex) {}
	SystemPath(const SystemPath *path)
		: sectorX(path->sectorX), sectorY(path->sectorY), sectorZ(path->sectorZ), systemIndex(path->systemIndex), bodyIndex(path->bodyIndex) {}

	Sint32 sectorX;
	Sint32 sectorY;
	Sint32 sectorZ;
	Uint32 systemIndex;
	Uint32 bodyIndex;

	friend bool operator==(const SystemPath &a, const SystemPath &b) {
		if (a.sectorX != b.sectorX) return false;
		if (a.sectorY != b.sectorY) return false;
		if (a.sectorZ != b.sectorZ) return false;
		if (a.systemIndex != b.systemIndex) return false;
		if (a.bodyIndex != b.bodyIndex) return false;
		return true;
	}

	friend bool operator!=(const SystemPath &a, const SystemPath &b) {
		return !(a==b);
	}

	friend bool operator<(const SystemPath &a, const SystemPath &b) {
		if (a.sectorX != b.sectorX) return (a.sectorX < b.sectorX);
		if (a.sectorY != b.sectorY) return (a.sectorY < b.sectorY);
		if (a.sectorZ != b.sectorZ) return (a.sectorZ < b.sectorZ);
		if (a.systemIndex != b.systemIndex) return (a.systemIndex < b.systemIndex);
		return (a.bodyIndex < b.bodyIndex);
	}

	class LessSectorOnly {
	public:
		bool operator()(const SystemPath& a, const SystemPath& b) const {
			if (a.sectorX != b.sectorX) return (a.sectorX < b.sectorX);
			if (a.sectorY != b.sectorY) return (a.sectorY < b.sectorY);
			return (a.sectorZ < b.sectorZ);
		}
	};

	class LessSystemOnly {
	public:
		bool operator()(const SystemPath& a, const SystemPath& b) const {
			if (a.sectorX != b.sectorX) return (a.sectorX < b.sectorX);
			if (a.sectorY != b.sectorY) return (a.sectorY < b.sectorY);
			if (a.sectorZ != b.sectorZ) return (a.sectorZ < b.sectorZ);
			return (a.systemIndex < b.systemIndex);
		}
	};

	bool IsSectorPath() const {
		return (systemIndex == Uint32(-1) && bodyIndex == Uint32(-1));
	}

	bool IsSystemPath() const {
		return (systemIndex != Uint32(-1) && bodyIndex == Uint32(-1));
	}
	bool HasValidSystem() const {
		return (systemIndex != Uint32(-1));
	}

	bool IsBodyPath() const {
		return (systemIndex != Uint32(-1) && bodyIndex != Uint32(-1));
	}
	bool HasValidBody() const {
		assert((bodyIndex == Uint32(-1)) || (systemIndex != Uint32(-1)));
		return (bodyIndex != Uint32(-1));
	}

	bool IsSameSector(const SystemPath &b) const {
		if (sectorX != b.sectorX) return false;
		if (sectorY != b.sectorY) return false;
		if (sectorZ != b.sectorZ) return false;
		return true;
	}

	bool IsSameSystem(const SystemPath &b) const {
		assert(HasValidSystem());
		assert(b.HasValidSystem());
		if (sectorX != b.sectorX) return false;
		if (sectorY != b.sectorY) return false;
		if (sectorZ != b.sectorZ) return false;
		if (systemIndex != b.systemIndex) return false;
		return true;
	}

	SystemPath SectorOnly() const {
		return SystemPath(sectorX, sectorY, sectorZ);
	}

	SystemPath SystemOnly() const {
		assert(systemIndex != Uint32(-1));
		return SystemPath(sectorX, sectorY, sectorZ, systemIndex);
	}

	void ToJson(Json::Value &jsonObj) const {
		Json::Value systemPathObj(Json::objectValue); // Create JSON object to contain system path data.
		systemPathObj["sector_x"] = sectorX;
		systemPathObj["sector_y"] = sectorY;
		systemPathObj["sector_z"] = sectorZ;
		systemPathObj["system_index"] = systemIndex;
		systemPathObj["body_index"] = bodyIndex;
		jsonObj["system_path"] = systemPathObj; // Add system path object to supplied object.
	}
	static SystemPath FromJson(const Json::Value &jsonObj) {
		if (!jsonObj.isMember("system_path")) throw SavedGameCorruptException();
		Json::Value systemPathObj = jsonObj["system_path"];
		if (!systemPathObj.isMember("sector_x")) throw SavedGameCorruptException();
		if (!systemPathObj.isMember("sector_y")) throw SavedGameCorruptException();
		if (!systemPathObj.isMember("sector_z")) throw SavedGameCorruptException();
		if (!systemPathObj.isMember("system_index")) throw SavedGameCorruptException();
		if (!systemPathObj.isMember("body_index")) throw SavedGameCorruptException();
		Sint32 x = systemPathObj["sector_x"].asInt();
		Sint32 y = systemPathObj["sector_y"].asInt();
		Sint32 z = systemPathObj["sector_z"].asInt();
		Uint32 si = systemPathObj["system_index"].asUInt();
		Uint32 bi = systemPathObj["body_index"].asUInt();
		return SystemPath(x, y, z, si, bi);
	}

	// sometimes it's useful to be able to get the SystemPath data as a blob
	// (for example, to be used for hashing)
	// see, LuaObject<SystemPath>::PushToLua in LuaSystemPath.cpp
	static_assert(sizeof(Sint32) == sizeof(Uint32), "something crazy is going on!");
	static const size_t SizeAsBlob = 5*sizeof(Uint32);
	void SerializeToBlob(char *blob) const {
		// could just memcpy(blob, this, sizeof(SystemPath))
		// but that might include packing and/or vtable pointer
		memcpy(blob + 0*sizeof(Uint32), &sectorX, sizeof(Uint32));
		memcpy(blob + 1*sizeof(Uint32), &sectorY, sizeof(Uint32));
		memcpy(blob + 2*sizeof(Uint32), &sectorZ, sizeof(Uint32));
		memcpy(blob + 3*sizeof(Uint32), &systemIndex, sizeof(Uint32));
		memcpy(blob + 4*sizeof(Uint32), &bodyIndex, sizeof(Uint32));
	}
};

#endif
