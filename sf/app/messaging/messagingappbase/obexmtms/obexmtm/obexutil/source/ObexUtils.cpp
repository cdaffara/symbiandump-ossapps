// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

//class include
#include <obexutils.h>


#include <e32base.h>



// Used as panic category and number when a removed export is called
_LIT(KLDRIMPORT, "LDR-IMPORT");
const TInt KLdrImportedOrdinalDoesNotExist = 2;

EXPORT_C void CObexUtils::DummySaveObjToInboxL(CMsvSession& /*aSession*/,CBaseContainer& /*aContainer*/,
                                               TUid /*aMsgType*/,TMsvId& /*aParentMsvId*/,TDesC& /*aDeviceName*/)
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}                



EXPORT_C void CObexUtils::DummyRemoveFromInboxL(CMsvSession& /*aSession*/, TMsvId /*aMsvId*/)
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}                




EXPORT_C void CObexUtils::DummyFinaliseObexMessageL(CMsvSession& /*aSession*/, TMsvId /*aParentTMsvId*/)
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}                

	



