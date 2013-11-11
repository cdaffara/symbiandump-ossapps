/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:
* This class for DirectPrint UI_Plugin managemrnt
*
*/

// User includes
#include "DirectPrintUiPluginMgr.h"
#include "DirectPrintModel.h"
#include "DirectPrintUiPlugin.h"
#include "directprintprinterdata.h"

CDirectPrintUiPluginMgr::CDirectPrintUiPluginMgr(CDirectPrintModel& aModel)
	: iModel(aModel)
	, iPlugin(NULL)
	{
	}

CDirectPrintUiPluginMgr::~CDirectPrintUiPluginMgr()
	{
	delete iPlugin;
	}

CDirectPrintUiPluginMgr* CDirectPrintUiPluginMgr::NewL(CDirectPrintModel& aModel)
	{
	CDirectPrintUiPluginMgr* self = CDirectPrintUiPluginMgr::NewLC(aModel);
	CleanupStack::Pop(self);
	return self;
	}

CDirectPrintUiPluginMgr* CDirectPrintUiPluginMgr::NewLC(CDirectPrintModel& aModel)
	{
	CDirectPrintUiPluginMgr* self = new(ELeave) CDirectPrintUiPluginMgr(aModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CDirectPrintUiPluginMgr::ConstructL()
	{
	iCurrentPluginUid = TUid::Uid(0);
	}

void CDirectPrintUiPluginMgr::LoadPluginL(TUid aUid)
	{
	delete iPlugin;
	iPlugin = NULL;
	if (aUid.iUid > 0)
		{
		iPlugin = CDirectPrintUiPlugin::NewL(aUid);
		}
	}

void CDirectPrintUiPluginMgr::LoadPluginL(const TDesC8& aKey)
	{
	delete iPlugin;
	iPlugin = NULL;
	if (aKey.Length() > 0)
		{
		iPlugin = CDirectPrintUiPlugin::NewL(aKey);
		}
	}

CDirectPrintUiPlugin* CDirectPrintUiPluginMgr::CurrentPluginL()
	{
	TUid uid = iModel.SettingUIPluginUidL();
	if (iCurrentPluginUid != uid)
		{
		LoadPluginL(uid);
		iCurrentPluginUid = uid;
		}

	return iPlugin;
	}

