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
*
*/

#include <aknviewappui.h>
#include <barsread.h>
#include <akntitle.h>

#include "directprintviewbase.h"
#include "directprintappappui.h"
#include "directprintappdocument.h"
#include "directprintmodel.h"
#include "directprintuipluginmgr.h"
#include "directprintengine.h"

CDirectPrintViewBase::CDirectPrintViewBase()
	{
	}

CDirectPrintViewBase::~CDirectPrintViewBase()
	{
	}

void CDirectPrintViewBase::ConstructL()
	{
	}

TUid CDirectPrintViewBase::Id() const
	{
	return iViewId;
	}

void CDirectPrintViewBase::HandleListIndexL(TInt /*aIndex*/)
	{
	}

void CDirectPrintViewBase::HandleEditItemL(TInt /*aIndex*/, TBool /*aChanged*/)
	{
	}

void CDirectPrintViewBase::DoActivateL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
	{
	}

void CDirectPrintViewBase::DoDeactivate()
	{
	}

void CDirectPrintViewBase::SetActiveTitleL(TInt aTitleResId)
	{
	TUid titlePaneUid = TUid::Uid( EEikStatusPaneUidTitle );
	CEikStatusPaneBase::TPaneCapabilities subPaneTitle = 
		StatusPane()->PaneCapabilities( titlePaneUid );
	if ( subPaneTitle.IsPresent() && subPaneTitle.IsAppOwned() )
		{
		CAknTitlePane* title = static_cast< CAknTitlePane* >( 
			StatusPane()->ControlL( titlePaneUid ) );
		TResourceReader reader;
		iEikonEnv->CreateResourceReaderLC( reader, aTitleResId );
		title->SetFromResourceL( reader );
		CleanupStack::PopAndDestroy(); // reader internal state
		}
	}

CDirectPrintModel& CDirectPrintViewBase::GetModel()
	{
	return (static_cast<CDirectPrintAppAppUi*>(AppUi()))->Doc()->Model();
	}

CDirectPrintUiPluginMgr& CDirectPrintViewBase::GetPluginMgr()
	{
	return (static_cast<CDirectPrintAppAppUi*>(AppUi()))->PluginMgr();
	}

CDirectPrintEngine& CDirectPrintViewBase::GetEngine()
	{
	return *((static_cast<CDirectPrintAppAppUi*>(AppUi()))->Engine());
	}

