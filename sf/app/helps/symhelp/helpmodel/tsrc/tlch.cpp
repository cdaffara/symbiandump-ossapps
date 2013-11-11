// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Element within the Help Model module
// 
//

#include "tlch.hrh"
#include "tlch.h"
#include "HLPLCH.H"
#include <eikstart.h>

LOCAL_C CApaApplication* NewApplication()
	{
	return new CTLchApplication;
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}



TUid CTLchApplication::AppDllUid() const
	{
	return KUidTLchApp;
	}

CApaDocument* CTLchApplication::CreateDocumentL()
	{
	return new (ELeave) CTLchDocument(*this);
	}

CTLchDocument::CTLchDocument(CEikApplication& aApp)
		: CEikDocument(aApp)
	{
	}

CEikAppUi* CTLchDocument::CreateAppUiL()
	{
    return new(ELeave) CTLchAppUi;
	}

void CTLchAppUi::ConstructL()
    {
    BaseConstructL();
    iAppView=new(ELeave) CTLchAppView;
    iAppView->ConstructL(ClientRect());
	AddToStackL(iAppView);
    }


CTLchAppUi::~CTLchAppUi()
	{
	RemoveFromStack(iAppView);
    delete iAppView;
	}

void CTLchAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case ETlchMenuHelpC:
			{
			CArrayFix<TCoeHelpContext>* buf=AppHelpContextL();
			HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(), buf);
			break;
			}
		case ETlchMenuHelp:
			{
			HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession());
			break;
			}
		case EEikCmdExit: 
			Exit();
			break;
		}
	}

CArrayFix<TCoeHelpContext>* CTLchAppUi::HelpContextL() const
	{
	CArrayFixFlat<TCoeHelpContext>* array=new(ELeave)CArrayFixFlat<TCoeHelpContext>(1);
	array->AppendL(TCoeHelpContext(KUidTLchApp, _L("App Ui Context 1")));
	array->AppendL(TCoeHelpContext(KUidTLchApp, _L("App Ui Context 2")));
	array->AppendL(TCoeHelpContext(KUidTLchApp, _L("App Ui Context 3")));
	return array;
	}

void CTLchAppView::ConstructL(TRect aRect)
	{
	CreateWindowL();
    SetRect(aRect);
    ActivateL();
	}

void CTLchAppView::GetHelpContext(TCoeHelpContext& aContext) const
	{
	aContext.iMajor=KUidTLchApp;
	aContext.iContext=_L("Context Id 12");
	}
