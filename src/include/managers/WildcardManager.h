/*
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)
Copyright (C) 2014 Bad Juju Games, Inc.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA.

Contact us at legal@badjuju.com.
*/

#ifndef DESURA_WILDCARD_MANAGER_H
#define DESURA_WILDCARD_MANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "BaseManager.h"
#include "Event.h"

enum
{
	WCM_OK = 0,
	WCM_ERR_BADXML,
	WCM_ERR_MAXDEPTH,
	WCM_ERR_FAILEDRESOVELD,
	WCM_ERR_FAILEDFIND,
	WCM_ERR_NULLPATH,
};

class WCSpecialInfo
{
public:
	gcString name;
	gcString result;
	volatile bool handled = false;	//true = special wildcard resolved
};

class WildcardInfo : public BaseItem
{
public:
	WildcardInfo(gcString name, gcString path, gcString type, bool resolved = false)
		: BaseItem(name.c_str())
		, m_szName(name)
		, m_szPath(path)
		, m_szType(type)
		, m_bResolved(resolved)
	{
	}

	gcString m_szName;
	gcString m_szPath;
	gcString m_szType;
	bool m_bResolved;
};

namespace XML
{
	class gcXMLElement;
}

//! Stores wild cards (path and special locations) and allows them to be resolved into full paths on the file system
class WildcardManager : public BaseManager<WildcardInfo>, public gcRefBase
{
public:
	WildcardManager();
	WildcardManager(gcRefPtr<WildcardManager> &mng);
	virtual ~WildcardManager();

	void load();
	void unload();

	//! used to update INSTALL_PATH and PARENT_INSTALL_PATH wildcards
	void updateInstallWildcard(const char* name, const char* value);

	//! converts wildcards into there properpath. Res needs to be a null char* pointer
	//!
	//! @param path Path to search for wild cards
	//! @param res Out path
	//! @param fixPath fix slashes to be os
	//!
	void constructPath(const char* path, char **res, bool fixPath = true);
	gcString constructPath(const char* path, bool fixPath = true);

	//! this parses an xml feed.
	uint8 parseXML(const XML::gcXMLElement &xmlElement);

	uint32 getDepth(){return m_uiDepth;}

	Event<WCSpecialInfo> onNeedSpecialEvent;
	Event<WCSpecialInfo> onNeedInstallSpecialEvent;

	void compactWildCards();

protected:

	//! Triggers the event to resolve the wild card
	void needSpecial(WCSpecialInfo *info);

	//! Make sure its a valid wildcard
	bool wildcardCheck(const char* string);

	//! Recursive version of above
	void constructPath(const char* path, char **res, uint8* depth);

	//!
	void resolveWildCard(gcRefPtr<WildcardInfo> temp);

private:
	uint32 m_uiDepth;

	gc_IMPLEMENT_REFCOUNTING(WildcardManager)
};

#endif //DESURA_WILDCARD_MANAGER_H
