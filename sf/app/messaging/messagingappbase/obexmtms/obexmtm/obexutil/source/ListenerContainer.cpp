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

#include <listenercontainer.h>



// Used as panic category and number when a removed export is called
_LIT(KLDRIMPORT, "LDR-IMPORT");
const TInt KLdrImportedOrdinalDoesNotExist = 2;

//Class CBaseContainer
EXPORT_C void CBaseContainer::DummyGetHeaderDetailsL(CObexBaseObject& /*aTarget*/) const
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}                

EXPORT_C void CBaseContainer::DummySetHeaderDetailsL(CObexBaseObject& /*aSource*/)
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}                




EXPORT_C void CContainer::DummyDtorCContainer()
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}       



EXPORT_C void CContainer::DummyNewL()
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}  

	
	
EXPORT_C void CContainer::DummyNewL(CBufBase* /*aBuf*/)
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}  


	
	
EXPORT_C void CContainer::DummyCopyOfDataLC(TInt /*aSize*/)
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}  



EXPORT_C void CContainer::DummyCopyOfDataLC()
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}  



EXPORT_C void CContainer::DummyMoveToFile(RFs& /*aFs*/, const TDesC& /*aFileName*/)
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}  


EXPORT_C void CContainer::DummyObexBaseObject() const
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}  



EXPORT_C void CContainer::DummyCopyToFileContainerLC(RFs& /*aFs*/, const TDesC& /*aFileName*/)
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}  


EXPORT_C void CObexFileContainer::DummyDtorCObexFileContainer()
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}  


EXPORT_C void CObexFileContainer::DummyNewL(const TDesC& /*aFileName*/)
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}  



EXPORT_C void CObexFileContainer::DummyCopyOfDataLC(TInt /*aSize*/)
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}  


EXPORT_C void CObexFileContainer::DummyCopyOfDataLC()
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}  


EXPORT_C void CObexFileContainer::DummyMoveToFile(RFs& /*aFs*/, const TDesC& /*aFileName*/)
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}  

	
EXPORT_C void CObexFileContainer::DummyObexBaseObject() const
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}  
