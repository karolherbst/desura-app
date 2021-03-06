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

#include "Common.h"
#include "BaseItemTask.h"

#include "usercore/ItemHandleI.h"

#include "ItemInfo.h"
#include "ItemHandle.h"
#include "User.h"
#include "webcore/WebCoreI.h"


using namespace UserCore::ItemTask;


BaseItemTask::BaseItemTask(UserCore::Item::ITEM_STAGE type, const char* name, gcRefPtr<UserCore::Item::ItemHandleI> &handle, MCFBranch branch, MCFBuild build)
	: m_pHandle(handle)
	, m_uiType(type)
	, m_uiMcfBranch(branch)
	, m_uiMcfBuild(build)
	, m_szName(name)
{
	gcAssert(handle);
}

BaseItemTask::~BaseItemTask()
{
}

const char* BaseItemTask::getTaskName()
{
	return m_szName.c_str();
}

UserCore::Item::ITEM_STAGE BaseItemTask::getTaskType()
{
	return m_uiType;
}

void BaseItemTask::setWebCore(gcRefPtr<WebCore::WebCoreI> &wc)
{
	m_pWebCore = wc;
}

void BaseItemTask::setUserCore(gcRefPtr<UserCore::UserI> &uc)
{
	m_pUserCore = uc;
}

void BaseItemTask::doTask()
{
	if (!m_pWebCore || !m_pUserCore)
	{
		gcException e(ERR_BADCLASS);
		onErrorEvent(e);
		return;
	}

	try
	{
		doRun();
	}
	catch (gcException& e)
	{
		onErrorEvent(e);
	}
}

void BaseItemTask::onStop()
{
	m_bIsStopped = true;

	if (m_hMCFile.handle())
		m_hMCFile->stop();
}

void BaseItemTask::onPause()
{
	m_bIsPaused = true;
}

void BaseItemTask::onUnpause()
{
	m_bIsPaused = false;
}

void BaseItemTask::cancel()
{

}

gcRefPtr<UserCore::Item::ItemHandleI> BaseItemTask::getItemHandle()
{
	return m_pHandle;
}

gcRefPtr<UserCore::Item::ItemInfoI> BaseItemTask::getItemInfo()
{
	if (!m_pHandle)
		return nullptr;

	return m_pHandle->getItemInfo();
}

gcRefPtr<UserCore::Item::ItemInfoI> BaseItemTask::getParentItemInfo()
{
	auto item = getItemInfo();

	if (!m_pUserCore || !item)
		return nullptr;

	return m_pUserCore->getItemManager()->findItemInfo(item->getParentId());
}

DesuraId BaseItemTask::getItemId()
{
	return getItemInfo()->getId();
}

gcRefPtr<WebCore::WebCoreI> BaseItemTask::getWebCore()
{
	return m_pWebCore;
}

gcRefPtr<UserCore::UserI> BaseItemTask::getUserCore()
{
	return m_pUserCore;
}

MCFBuild BaseItemTask::getMcfBuild()
{
	return m_uiMcfBuild;
}

MCFBranch BaseItemTask::getMcfBranch()
{
	return m_uiMcfBranch;
}

bool BaseItemTask::isStopped()
{
	return m_bIsStopped;
}

bool BaseItemTask::isPaused()
{
	return m_bIsPaused;
}


