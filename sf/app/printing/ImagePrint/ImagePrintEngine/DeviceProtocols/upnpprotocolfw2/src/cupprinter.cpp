/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Declares CUPPrinter class
*
*/


#include <upnpmediaserversettings.h>

#include "cupprinter.h"
#include "cuplogger.h"
#include "tprintcapability.h"

// Public functions
//--------------------------------------------------------------------------------------------
//
// CUPPrinter::~CUPPrinter
//
//--------------------------------------------------------------------------------------------
CUPPrinter::~CUPPrinter()
{	
    if ( iUId )
    {
        delete iUId;
    }

    if ( iDisplayName )
    {
        delete iDisplayName;
    }
}


//--------------------------------------------------------------------------------------------
//
// CUPPrinter::NewL
//
//--------------------------------------------------------------------------------------------
CUPPrinter* CUPPrinter::NewL(CUpnpDevice& aDevice, TInt aId)
{
	CUPPrinter* self = new (ELeave) CUPPrinter();
	CleanupStack::PushL(self);
	self->ConstructL(&aDevice, aId, aDevice.Uuid(), aDevice.DescriptionProperty(UpnpMediaServerSettings::KModelName()), TPrinter::EVendorNone);
	CleanupStack::Pop();	// self
	return self;
}

//--------------------------------------------------------------------------------------------
//
// CUPPrinter::NewL
//
//--------------------------------------------------------------------------------------------
CUPPrinter* CUPPrinter::NewL(const TInt aId, const TDesC8& aUId, const TDesC8& aDisplayName, const TInt aVendor)
{
	CUPPrinter* self = new (ELeave) CUPPrinter();
	CleanupStack::PushL(self);
	self->ConstructL(NULL, aId, aUId, aDisplayName, aVendor);
	CleanupStack::Pop();	// self
	return self;
}


//--------------------------------------------------------------------------------------------
//
// CUPPrinter::Device
//
//--------------------------------------------------------------------------------------------
CUpnpDevice* CUPPrinter::Device()
{
	return iDevice;
}

//--------------------------------------------------------------------------------------------
//
// CUPPrinter::SetCached
//
//--------------------------------------------------------------------------------------------
void CUPPrinter::SetCached(TBool aCached)
{
	iCached = aCached;
}

//--------------------------------------------------------------------------------------------
//
// CUPPrinter::Cached
//
//--------------------------------------------------------------------------------------------
TBool CUPPrinter::Cached()
{
	return iCached;
}

//--------------------------------------------------------------------------------------------
//
// CUPPrinter::Vendor
//
//--------------------------------------------------------------------------------------------
TPrinter::TPrinterVendor CUPPrinter::Vendor()
	{
	return iVendor;
	}

//--------------------------------------------------------------------------------------------
//
// CUPPrinter::SetVendor
//
//--------------------------------------------------------------------------------------------
void CUPPrinter::SetVendor(const TDesC8& aManufacturer)
	{
	LOG("[CUPPrinter::SetVendor]\t Begin");
	iVendor = TPrinter::EVendorNone;
	// At the moment this functionality is not used, but no Vendor is set.
 	LOG("[CUPPrinter::SetVendor]\t End");
	}

//--------------------------------------------------------------------------------------------
//
// CUPPrinter::ToTPrinter
//
//--------------------------------------------------------------------------------------------
TPrinter CUPPrinter::ToTPrinter()
{
	TInt cached(0);
	TPrinter printer;
	printer.iPrinterID = iId;
	printer.iDisplayName.Copy(iDisplayName->Des());
	printer.iVendor = Vendor();
	// cast boolean value to integer
	cached = Cached();
	printer.iProperties = cached | TPrinter::SupportsPreview;
	printer.iProtocol = KImagePrint_PrinterProtocol_UPnP;

	LOG1("[CUPPrinter::ToTPrinter]\t printer.iVendor = %d", printer.iVendor)
	
	return printer;
}

//--------------------------------------------------------------------------------------------
//
// CUPPrinter::Id
//
//--------------------------------------------------------------------------------------------
TInt CUPPrinter::Id()
	{
	return iId;
	}

//--------------------------------------------------------------------------------------------
//
// CUPPrinter::UId
//
//--------------------------------------------------------------------------------------------
TPtrC8 CUPPrinter::UId()
{
    if ( iUId )
	{
    	return TPtrC8( iUId->Des() );
    }
    else 
    {
    	return TPtrC8( KNullDesC8 );
    }
	
}

//--------------------------------------------------------------------------------------------
//
// CUPPrinter::DisplayName
//
//--------------------------------------------------------------------------------------------
TPtrC8 CUPPrinter::DisplayName()
{
    if ( iDisplayName )
	{
    	return TPtrC8( iDisplayName->Des() );
    }
    else 
    {
    	return TPtrC8( KNullDesC8 );
    }
	
}


//--------------------------------------------------------------------------------------------
//
// CUPPrinter::IsDisappeared
//
//--------------------------------------------------------------------------------------------
TBool CUPPrinter::IsDisappeared()
{
	return iDisappeared;
}


//--------------------------------------------------------------------------------------------
//
// CUPPrinter::SetDisappeared
//
//--------------------------------------------------------------------------------------------
void CUPPrinter::SetDisappeared(TBool aDisappeared)
{
	iDisappeared = aDisappeared;
	
	if (iDisappeared)
	{
		iDevice = NULL;
	}
}


//--------------------------------------------------------------------------------------------
//
// CUPPrinter::SetDevice
//
//--------------------------------------------------------------------------------------------
void CUPPrinter::SetDevice(CUpnpDevice* aDevice)
{
	iDevice = aDevice;

	if (!aDevice)
		return;
	
	// Update display name
	delete iDisplayName;
	iDisplayName = NULL;
	
	iDisplayName = aDevice->DescriptionProperty(UpnpMediaServerSettings::KModelName()).Alloc();
}

// Protected functions

//--------------------------------------------------------------------------------------------
//
// CUPPrinter::ConstructL(
//
//--------------------------------------------------------------------------------------------
void CUPPrinter::ConstructL(CUpnpDevice* aDevice, const TInt aId, const TDesC8& aUId, const TDesC8& aDisplayName, const TInt aVendor)
{
	iId = aId;
	iUId = aUId.AllocL();
	iDisplayName = aDisplayName.AllocL();
	iVendor = (TPrinter::TPrinterVendor)aVendor;
	
	if (aDevice)
	{
		// Discovered device
		iDevice = aDevice;
		iDisappeared = EFalse;

		SetVendor(iDevice->DescriptionProperty(UpnpMediaServerSettings::KManufacturer()));
	}
	else
	{
		// Cached device
		iDisappeared = ETrue;
	}
	
	iCached = EFalse;
}

// Private functions
//--------------------------------------------------------------------------------------------
//
// CUPPrinter::CUPPrinter
//
//--------------------------------------------------------------------------------------------
CUPPrinter::CUPPrinter()
{
}

//  End of File
