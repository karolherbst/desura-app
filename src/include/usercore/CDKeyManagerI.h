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


#ifndef DESURA_CDKEYMANAGERI_H
#define DESURA_CDKEYMANAGERI_H
#ifdef _WIN32
#pragma once
#endif

namespace UserCore
{
	namespace Misc
	{
		class CDKeyCallBackI : public gcRefBase
		{
		public:
			//! Call back when complete
			//!
			//! @param id Item id
			//! @param cdkey Item cd key
			//!
			virtual void onCDKeyComplete(DesuraId id, gcString &cdKey) = 0;

			//! Call back when error occures
			//!
			//! @param id Item id
			//! @param e Error that occured
			//!
			virtual void onCDKeyError(DesuraId id, gcException& e) = 0;
		};

	}
	class CDKeyManagerI : public gcRefBase
	{
	public:
		//! Gets the cd key for currently installed branch of an item
		//!
		//! @param id Item id
		//! @param callback Callback to use when complete
		//!
		virtual void getCDKeyForCurrentBranch(DesuraId id, gcRefPtr<UserCore::Misc::CDKeyCallBackI> pCallback) = 0;

		//! Cancels a request to get a cd key
		//!
		//! @param id Item id
		//! @param callback Request callback used in original request
		//!
		virtual void cancelRequest(DesuraId id, gcRefPtr<UserCore::Misc::CDKeyCallBackI> pCallback) = 0;

		//! Does the current branch require a cd key
		//!
		//! @return true of false
		//!
		virtual bool hasCDKeyForCurrentBranch(DesuraId id) = 0;
	};
}

#endif //DESURA_CDKEYMANAGERI_H
