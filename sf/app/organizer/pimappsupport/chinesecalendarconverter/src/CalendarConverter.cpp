// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <calendarconverterplugin.h>

/**
Creates a new CChineseCalendarConverter object.
@return Pointer to the new object.
*/
EXPORT_C CChineseCalendarConverter* CChineseCalendarConverter::NewL()
	{
	return CChineseCalendarConverterEcomPlugin::NewL();
	}

static void ResetAndDestroy(TAny* aPtr)
	{
	RImplInfoPtrArray* self = static_cast<RImplInfoPtrArray*>(aPtr);
	if (self)
		{
		self->ResetAndDestroy();
		}
	} 

CChineseCalendarConverter* CChineseCalendarConverterEcomPlugin::NewL()
	{
	RImplInfoPtrArray implInfoArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroy, &implInfoArray));

	// Use default resolver parameters.
	TEComResolverParams resolverParams;

	// List implementations, searching only in ROM for the plugin.
	REComSession::ListImplementationsL(KUidEcomChineseCalendarConverterInterface, resolverParams, 
		KRomOnlyResolverUid, implInfoArray); 

	const TInt count = implInfoArray.Count();
	if (count == 0)
		{
		User::Leave(KErrNotFound);
		}
	// Create the first implementation found for KUidEcomChineseCalendarConverterInterface
	const TUid firstImplementationFound = implInfoArray[0]->ImplementationUid();
	TAny* ptr = REComSession::CreateImplementationL(firstImplementationFound, 
		_FOFF(CChineseCalendarConverterEcomPlugin, iDtor_ID_Key));
	CChineseCalendarConverter* converter = reinterpret_cast<CChineseCalendarConverter*>(ptr);
	CleanupStack::PopAndDestroy(&implInfoArray);
	return converter;
	}
