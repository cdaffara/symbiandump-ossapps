// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

// Including the SIM1 folders is optional as we don't support SIM Access
#ifdef __INCLUDE_SIM1_FOLDERS__

#include "pbapfoldersim.h"
#include "pbapappheader.h"

#include "btaccesshostlog.h"


_LIT(KFolderSIM, 		"SIM1");
_LIT(KFolderSIMTelecom, "telecom");
_LIT(KFolderSIMPb, 		"pb");
_LIT(KFolderSIMIch, 	"ich");
_LIT(KFolderSIMOch, 	"och");
_LIT(KFolderSIMMch, 	"mch");
_LIT(KFolderSIMCch, 	"cch");

//
// CFolderSIM1Base
//
CFolderSIM1Base::CFolderSIM1Base(MVirtualFolderClient& aClient, const TDesC& aFolderName)
: CFolderBase(aClient, aFolderName)
	{
	LOG_FUNC
	}
		
TInt CFolderSIM1Base::Get(const TDesC& /*aObjectName*/, CPbapAppHeader* aGetData)
	{
	LOG_FUNC

	// we need to take ownership of aGetData but don't require it
	delete aGetData;
	
	// SIM repository support not implemented
	return KErrNotSupported;
	}

void CFolderSIM1Base::CancelGet()
	{
	LOG_FUNC
	}

	
//
// CFolderSIM1
//
CFolderSIM1* CFolderSIM1::NewL(MVirtualFolderClient& aClient)
	{
	LOG_STATIC_FUNC
	
	CFolderSIM1* self = new(ELeave) CFolderSIM1(aClient);
	return self;
	}

CFolderSIM1::CFolderSIM1(MVirtualFolderClient& aClient)
	: CFolderSIM1Base(aClient, KFolderSIM())
	{
	LOG_FUNC
	}

//
// CFolderSIM1Telecom
//	
CFolderSIM1Telecom* CFolderSIM1Telecom::NewL(MVirtualFolderClient& aClient)
	{
	LOG_STATIC_FUNC
	
	CFolderSIM1Telecom* self = new(ELeave) CFolderSIM1Telecom(aClient);
	return self;
	}

CFolderSIM1Telecom::CFolderSIM1Telecom(MVirtualFolderClient& aClient)
	: CFolderSIM1Base(aClient, KFolderSIMTelecom())
	{
	LOG_FUNC
	}
		
//
// CFolderSIM1NodePb
//		
CFolderSIM1NodePb* CFolderSIM1NodePb::NewL(MVirtualFolderClient& aClient)
	{
	LOG_STATIC_FUNC
	
	CFolderSIM1NodePb* self = new(ELeave) CFolderSIM1NodePb(aClient);
	return self;
	}

CFolderSIM1NodePb::CFolderSIM1NodePb(MVirtualFolderClient& aClient)
	: CFolderSIM1Base(aClient, KFolderSIMPb())
	{
	LOG_FUNC
	}
		
//
// CFolderSIM1NodeIch
//		
CFolderSIM1NodeIch* CFolderSIM1NodeIch::NewL(MVirtualFolderClient& aClient)
	{
	LOG_STATIC_FUNC
	
	CFolderSIM1NodeIch* self = new(ELeave) CFolderSIM1NodeIch(aClient);
	return self;	
	}

CFolderSIM1NodeIch::CFolderSIM1NodeIch(MVirtualFolderClient& aClient)
	: CFolderSIM1Base(aClient, KFolderSIMIch())
	{
	LOG_FUNC
	}
		
//
// CFolderSIM1NodeOch
//		
CFolderSIM1NodeOch* CFolderSIM1NodeOch::NewL(MVirtualFolderClient& aClient)
	{
	LOG_STATIC_FUNC
	
	CFolderSIM1NodeOch* self = new(ELeave) CFolderSIM1NodeOch(aClient);
	return self;
	}

CFolderSIM1NodeOch::CFolderSIM1NodeOch(MVirtualFolderClient& aClient)
	: CFolderSIM1Base(aClient, KFolderSIMOch())
	{
	LOG_FUNC
	}
	
//
// CFolderSIM1NodeMch
//	
CFolderSIM1NodeMch* CFolderSIM1NodeMch::NewL(MVirtualFolderClient& aClient)
	{
	LOG_STATIC_FUNC
	
	CFolderSIM1NodeMch* self = new(ELeave) CFolderSIM1NodeMch(aClient);
	return self;
	}

CFolderSIM1NodeMch::CFolderSIM1NodeMch(MVirtualFolderClient& aClient)
	: CFolderSIM1Base(aClient, KFolderSIMMch())
	{
	LOG_FUNC
	}
			
//
// CFolderSIM1NodeCch
//
CFolderSIM1NodeCch* CFolderSIM1NodeCch::NewL(MVirtualFolderClient& aClient)
	{
	LOG_STATIC_FUNC
	
	CFolderSIM1NodeCch* self = new(ELeave) CFolderSIM1NodeCch(aClient);
	return self;
	}

CFolderSIM1NodeCch::CFolderSIM1NodeCch(MVirtualFolderClient& aClient)
	: CFolderSIM1Base(aClient, KFolderSIMCch())
	{
	LOG_FUNC
	}

#endif // __INCLUDE_SIM1_FOLDERS__
