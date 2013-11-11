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
* Description:  Declares CUPPrinterContainer class
*
*/


#include "cupprintercontainer.h"
#include "cuplogger.h"
#include "upconsts.h"

_LIT(KUPnPPrinterCache, "printers.txt");

#define KMaxCacheFileSize 512

// Public functions

//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::~CUPPrinterContainer
//
//--------------------------------------------------------------------------------------------
CUPPrinterContainer::~CUPPrinterContainer()
{
	iPrinters.ResetAndDestroy();
	iPrinters.Close();
	iFsSession.Close();
	if(iCacheBuffer)
		delete iCacheBuffer;
}

//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::NewL
//
//--------------------------------------------------------------------------------------------
CUPPrinterContainer* CUPPrinterContainer::NewL()
{
	CUPPrinterContainer* self = new (ELeave) CUPPrinterContainer();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();	// self
	return self;
}

//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::Printer
//
//--------------------------------------------------------------------------------------------
CUPPrinter* CUPPrinterContainer::Printer(CUpnpDevice& aDevice)
{
	TInt index = PrinterIndex(aDevice);
	if(KErrNotFound != index)
	{
		return PrinterByIndex(index);
	}
	else
		return NULL;
}

//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::Printer
//
//--------------------------------------------------------------------------------------------
CUPPrinter* CUPPrinterContainer::Printer(TInt aId)
{
	TInt index = PrinterIndex(aId);
	if(KErrNotFound != index)
	{
		return PrinterByIndex(index);
	}
	else
		return NULL;
}


//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::ToTPrinter
//
//--------------------------------------------------------------------------------------------
TPrinter CUPPrinterContainer::ToTPrinter(CUpnpDevice& aDevice)
{
	TInt index = PrinterIndex(aDevice);
	return ToTPrinter(index);
}

//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::ToTPrinter
//
//--------------------------------------------------------------------------------------------
TPrinter CUPPrinterContainer::ToTPrinter(TInt aIndex)
{
	if (aIndex >= iPrinters.Count() )
	{
		TPrinter nullPrinter;
		nullPrinter.iPrinterID = KErrNotFound;
		nullPrinter.iProtocol = 0;
		nullPrinter.iProperties = TPrinter::Cached;
		return nullPrinter;
	}

	return iPrinters[aIndex]->ToTPrinter();
}

//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::PrinterCount
//
//--------------------------------------------------------------------------------------------
TInt CUPPrinterContainer::PrinterCount()
{
	return iPrinters.Count();
}


//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::AddPrinterL
//
//--------------------------------------------------------------------------------------------
void CUPPrinterContainer::AddPrinterL(CUpnpDevice& aDevice, TInt& aId)
{
	aId = KErrNotFound;
	
	// Check if the discovered printer is cached/ disappeared
	for (TInt i=0; i < iPrinters.Count(); i++)
	{
		if (iPrinters[i]->UId().Compare(aDevice.Uuid()) == 0)
		{
			// Printer already in array - update pointer
			iPrinters[i]->SetDevice(&aDevice);
			if (iPrinters[i]->IsDisappeared())
			{
				// remove disappeared flag
				iPrinters[i]->SetDisappeared(EFalse);
			}
			aId = iPrinters[i]->Id();
			User::Leave(KErrAlreadyExists);
		}
	}

	// New printer - append to list
	TInt id = GenerateId();
	CUPPrinter* newPrinter = CUPPrinter::NewL(aDevice, id);
	CleanupStack::PushL(newPrinter);
	iPrinters.AppendL(newPrinter);
	CleanupStack::Pop(newPrinter);
	aId = id;
}


//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::RemovePrinter
//
//--------------------------------------------------------------------------------------------
TInt CUPPrinterContainer::RemovePrinter(CUpnpDevice& aDevice)
{
	TInt index = PrinterIndex(aDevice);
	if(KErrNotFound != index)
	{
		return RemovePrinter(index);
	}
	else
		return index;
}

//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::RemovePrinter
//
//--------------------------------------------------------------------------------------------
TInt CUPPrinterContainer::RemovePrinter(TInt aIndex)
{
	if (aIndex >= iPrinters.Count() || aIndex < 0)
		return KErrArgument;
	CUPPrinter* temp = iPrinters[aIndex];
	iPrinters.Remove(aIndex);
	if (temp)
	{
		delete temp;
		temp = NULL;	
	}
	return KErrNone;
}
//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::RemovePrinter
//
//--------------------------------------------------------------------------------------------
TInt CUPPrinterContainer::RemoveCachedDeviceL(TInt aDeviceID)
{
	TInt err = KErrNone;
	TBool cached = EFalse;

	for (TInt i=0; i < iPrinters.Count(); i++)
	{
		if (iPrinters[i]->Id() == aDeviceID)
		{
			cached = iPrinters[i]->Cached();
			iPrinters[i]->SetCached(EFalse);
			break;
		}
	}

	if(!cached)
		return KErrNone;

	ReadCacheFileL();

	TInt startPos = 0;
	TInt endPos = 0;

	FindCachedPrinterL(aDeviceID, startPos, endPos);

	LOG1("[CUPPrinterContainer::RemoveCachedDeviceL]\t startPos:\t%d", startPos);
	LOG1("[CUPPrinterContainer::RemoveCachedDeviceL]\t endPos:\t%d", endPos);

	if(0 < endPos)
	{
		//Remove printer form the buffer
		iCacheBuffer->Des().Delete(startPos, endPos-startPos);

		//Write buffer in file
		RFile cacheFile;
		CleanupClosePushL(cacheFile);
		TInt err = cacheFile.Replace( iFsSession, KUPnPPrinterCache, EFileWrite);
		if (err == KErrNone)
		{
			err = cacheFile.Write(iCacheBuffer->Des());
		}
		CleanupStack::PopAndDestroy(&cacheFile);
	}

	delete iCacheBuffer;
	iCacheBuffer = NULL;

	return err;
}

//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::Printer
//
//--------------------------------------------------------------------------------------------
CUPPrinter* CUPPrinterContainer::PrinterByIndex(TInt aIndex)
{
	if (aIndex >= iPrinters.Count() || aIndex < 0)	
		return NULL;
	return iPrinters[aIndex];
}


//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::PrinterIndex
//
//--------------------------------------------------------------------------------------------
TInt CUPPrinterContainer::PrinterIndex(CUpnpDevice& aDevice)
{
	TPtrC8 uuid = aDevice.Uuid();
	TInt count = iPrinters.Count();
	for (TInt i=0; i<count; i++)
	{
		TPtrC8 tempUuid = KNullDesC8();
		if(!iPrinters[i]->IsDisappeared()) 
			tempUuid.Set( (iPrinters[i])->Device()->Uuid() );
			
		if (uuid.Compare(tempUuid) == 0)
			return i;
	}
	return KErrNotFound;
}


//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::PrinterIndex
//
//--------------------------------------------------------------------------------------------
TInt CUPPrinterContainer::PrinterIndex(TInt aId)
{
	TInt count = iPrinters.Count();
	for (TInt i=0; i<count; i++)
	{
		if (aId == iPrinters[i]->Id())
			return i;
	}
	
	return KErrNotFound;
}


//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::PrinterDisappeared
//
//--------------------------------------------------------------------------------------------
void CUPPrinterContainer::PrinterDisappeared(CUpnpDevice& aDevice)
{
  // Check if device is found from iPrinters array. If so add to disappeared array.
  TInt index = PrinterIndex(aDevice);
  if ( index >=0 )
  {
  	iPrinters[index]->SetDisappeared(ETrue);
  }
}


//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::SyncPrinterArray
//
//--------------------------------------------------------------------------------------------
void CUPPrinterContainer::SyncPrinterArray()
{
	for(TInt i=0; i<iPrinters.Count(); i++)
	{
		if (iPrinters[i]->IsDisappeared())
		{
			RemovePrinter(i);
			i--;
		}
	}
}

//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::UpdateCache
//
//--------------------------------------------------------------------------------------------
void CUPPrinterContainer::UpdateCacheL()
{
	// Calculate size for buffer
	TInt bufSize = 0;
	for (TInt i=0; i < iPrinters.Count(); i++)
	{
		bufSize += 3; // Separators and line feed
	
		TBuf8<10> id;
		id.AppendNum(iPrinters[i]->Id());
		TBuf8<10> vendor;
		vendor.AppendNum(iPrinters[i]->Vendor());
		
		bufSize += id.Length();
		bufSize += iPrinters[i]->DisplayName().Length();
		bufSize += vendor.Length();
		bufSize += iPrinters[i]->UId().Length();
	}
	
	// Create buffer
	if(iCacheBuffer)
	{
		delete iCacheBuffer;
		iCacheBuffer = NULL;
	}

	iCacheBuffer = HBufC8::NewL(bufSize);

	// Write buffer
	for (TInt i=0; i < iPrinters.Count(); i++)
	{
		iCacheBuffer->Des().AppendNum(iPrinters[i]->Id());
		iCacheBuffer->Des().Append(KUPnPComma());
		iCacheBuffer->Des().Append(iPrinters[i]->DisplayName());
		iCacheBuffer->Des().Append(KUPnPComma());
		iCacheBuffer->Des().AppendNum(iPrinters[i]->Vendor());
		iCacheBuffer->Des().Append(KUPnPComma());
		iCacheBuffer->Des().Append(iPrinters[i]->UId());
		iCacheBuffer->Des().Append(KUPnPLineFeed8());
	}

	// Write buffer to file
	RFile cacheFile;
	TInt err = cacheFile.Replace( iFsSession, KUPnPPrinterCache, EFileWrite);
	if (err == KErrNone)
	{
		err = cacheFile.Write(iCacheBuffer->Des());
	}
	cacheFile.Close();
	User::LeaveIfError(err);
}

//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::UpdateCache
//
//--------------------------------------------------------------------------------------------
void CUPPrinterContainer::UpdateCacheL(TInt aDeviceId)
{

	// Get correct printer
	CUPPrinter* printer = NULL;
	for (TInt i=0; i < iPrinters.Count(); i++)
	{
		if(iPrinters[i]->Id() == aDeviceId)
		{
			printer = iPrinters[i];
			break;
		}
	}
	User::LeaveIfNull(printer);

	// Calculate size for buffer
	TInt bufSize = 0;
	if(!printer->Cached())
	{
		TBuf8<10> id;
		id.AppendNum(printer->Id());

		TBuf8<10> vendor;
		vendor.AppendNum(printer->Vendor());
		vendor.Append(_L8(" "));

		bufSize += id.Length();
		bufSize += KUPnPComma().Length();
		bufSize += printer->DisplayName().Length();
		bufSize += KUPnPComma().Length();
		bufSize += vendor.Length();
		bufSize += KUPnPComma().Length();
		bufSize += printer->UId().Length();
		bufSize += KUPnPLineFeed().Length();

		// Create buffer
		HBufC8* fileBuffer = HBufC8::NewL(bufSize);
		CleanupStack::PushL(fileBuffer);

		fileBuffer->Des().AppendNum(printer->Id());
		fileBuffer->Des().Append(KUPnPComma());
		fileBuffer->Des().Append(printer->DisplayName());
		fileBuffer->Des().Append(KUPnPComma());
		fileBuffer->Des().Append(vendor);
		fileBuffer->Des().Append(KUPnPComma());
		fileBuffer->Des().Append(printer->UId());
		fileBuffer->Des().Append(KUPnPLineFeed());


		RFile cacheFile;
		CleanupClosePushL(cacheFile);
		TInt fSize = 0;
		// Find end of file to write if exists
		TInt err = cacheFile.Open( iFsSession, KUPnPPrinterCache, EFileWrite);
		if (err == KErrNone)
		{
	    	cacheFile.Size(fSize);
	    	++fSize;
		}
		else
		{
			// Write buffer to file
			User::LeaveIfError(cacheFile.Replace( iFsSession, KUPnPPrinterCache, EFileWrite));
		}		
		User::LeaveIfError(cacheFile.Write(fSize, fileBuffer->Des()));
		printer->SetCached(ETrue);

		CleanupStack::PopAndDestroy(2); // fileBuffer, cacheFile
	}
}

//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::GetDisappearedPrinters
//
//--------------------------------------------------------------------------------------------
void CUPPrinterContainer::GetDisappearedPrinters(RPointerArray<CUPPrinter>& aArray)
{
	for (TInt i=0; i < iPrinters.Count(); i++)
	{
		if (iPrinters[i]->IsDisappeared())
		{
			aArray.Append(iPrinters[i]);
		}
	}
}

///////////////////////////
// Protected functions
///////////////////////////
//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::ConstructL
//
//--------------------------------------------------------------------------------------------
void CUPPrinterContainer::ConstructL()
{
	iCacheBuffer = NULL;

	// Init file server session
	User::LeaveIfError( iFsSession.Connect() );

	TInt err = iFsSession.MkDirAll( KUPnPDataPath );
	if (err != KErrNone && err != KErrAlreadyExists)
	{
	  User::Leave(err);
	}
	
	User::LeaveIfError(iFsSession.SetSessionPath( KUPnPDataPath ));
}

///////////////////////////
// Private functions
///////////////////////////
//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::CUPPrinterContainer
//
//--------------------------------------------------------------------------------------------
CUPPrinterContainer::CUPPrinterContainer()
{
	iIdCounter = 0;
}


//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::ToCDevice
//
//--------------------------------------------------------------------------------------------
CUpnpDevice* CUPPrinterContainer::ToCDevice(TInt aIndex)
{
	if (aIndex >= iPrinters.Count() || aIndex < 0 || iPrinters[aIndex]->IsDisappeared())	
		return NULL;
	return iPrinters[aIndex]->Device();
}

//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::ReadCacheL
//
//--------------------------------------------------------------------------------------------
void CUPPrinterContainer::ReadCacheL()
{
	LOG("[CUPPrinterContainer::ReadCacheL]\t");

	ReadCacheFileL();

	TPtr8 bufferPtr( iCacheBuffer->Des() );

	// Add cached devices.
	while (bufferPtr.Find(KUPnPLineFeed8()) >= 0)
	{
		// Read id
		TInt separPos = bufferPtr.Find(KUPnPComma());
		TInt lfPos = bufferPtr.Find(KUPnPLineFeed8());

		if (lfPos < 1 || lfPos < separPos)
		{
			break; // No id could be read
		}

		TPtrC8 idStr = bufferPtr.Left(separPos);
		TInt id;
		TLex8 lex(idStr);
		TInt error = lex.Val(id);
		User::LeaveIfError( error );
		
		bufferPtr = bufferPtr.Right(bufferPtr.Length()-separPos-1);

		// Read display name
		separPos = bufferPtr.Find(KUPnPComma());
		lfPos = bufferPtr.Find(KUPnPLineFeed8());

		if (lfPos < 1 || lfPos < separPos)
		{
			break; // No name could be read
		}

		TPtrC8 name = bufferPtr.Left(separPos);
		TBuf8<100> dispName;
		dispName.Copy(name);
		bufferPtr = bufferPtr.Right(bufferPtr.Length()-separPos-1);

		// Read vendor
		separPos = bufferPtr.Find(KUPnPComma());
		lfPos = bufferPtr.Find(KUPnPLineFeed8());

		if (lfPos < 1 || lfPos < separPos)
		{
			break; // No vendor could be read
		}

		TPtrC8 vendorStr = bufferPtr.Left(separPos);
		TInt vendor;
		TLex8 lexV(vendorStr);
		TInt err = lexV.Val(vendor);
		
		// FIX: if error reading vendor, use default 0 and return err to none
    if ( err != KErrNone )
    {
      vendor = 0;
      err = KErrNone;
    }
		
		bufferPtr = bufferPtr.Right(bufferPtr.Length()-separPos-1);

		//Read uid
		separPos = bufferPtr.Find(KUPnPComma());
		lfPos = bufferPtr.Find(KUPnPLineFeed8());

		if ( (lfPos < 1) || (lfPos > separPos && separPos >= 0 ) )
		{
			break; // No uid could be read
		}

		TPtrC8 uid = bufferPtr.Left(lfPos);

		// Add cached printer
		if (err == KErrNone)
		{
			AddCachedPrinterL(id, uid, dispName, vendor);
		}

		bufferPtr = bufferPtr.Right(bufferPtr.Length()-lfPos-1);

	}
}

//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::FindCachedPrinterL
//
//--------------------------------------------------------------------------------------------
TPtrC8 CUPPrinterContainer::FindCachedPrinterL(TInt aId, TInt& aStartPos, TInt& aEndPos)
{
	LOG("[CUPPrinterContainer::FindCachedPrinterL]\t");

	User::LeaveIfNull(iCacheBuffer);
	TPtrC8 bufferPtr( iCacheBuffer->Des() );

	TBuf8<10> reqId;
	reqId.AppendNum(aId);

	aStartPos = 0;
	aEndPos = 0;

	// Find cached device.
	while (bufferPtr.Find(KUPnPLineFeed8()) >= 0)
	{
		aStartPos = aEndPos;

		// Read id
		TInt separPos = bufferPtr.Find(KUPnPComma());
		TInt lfPos = bufferPtr.Find(KUPnPLineFeed8());

		if (lfPos < 1 || lfPos < separPos)
		{
			break; // No id could be read
		}

		aEndPos += lfPos+1;

		LOG81("[CUPPrinterContainer::FindCachedPrinterL]\t bufferPtr: %S", &bufferPtr);

		//Find next printer if id doesn't match
		if(0 != reqId.Compare(bufferPtr.Left(separPos)))
		{
			bufferPtr.Set(bufferPtr.Right(bufferPtr.Length()-lfPos-1));
			continue;
		}
		else
		{
			return bufferPtr.Left(aEndPos);
		}
	}

	return NULL;
}

//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::ReadCacheFileLC
//
//--------------------------------------------------------------------------------------------
void CUPPrinterContainer::ReadCacheFileL()
{
	LOG("[CUPPrinterContainer::ReadCacheFileL]\t");

	RFile cacheFile;
	CleanupClosePushL(cacheFile);

	TInt err = cacheFile.Open( iFsSession, KUPnPPrinterCache, EFileRead );

	if (err == KErrNone)
	{
	
		TInt fileSize = 0;
		err = cacheFile.Size(fileSize);
		if(err != KErrNone)
			fileSize = KMaxCacheFileSize;
	
		iCacheBuffer = HBufC8::NewL(fileSize);
	
		TPtr8 bufferPtr( iCacheBuffer->Des() );
		bufferPtr.Zero();
	
		err = cacheFile.Read(bufferPtr);
	}

  if (err != KErrNone)
	{
		// Problem in opening or reading cache file
		if (iCacheBuffer)
		{
				delete iCacheBuffer;
				iCacheBuffer = NULL;
		}
		iCacheBuffer = HBufC8::NewL(0);
		// printer not found from cache, leave!
		User::Leave( KErrNotFound );
	}

	CleanupStack::PopAndDestroy(); //cacheFile
}



//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::GenerateIdL
//
//--------------------------------------------------------------------------------------------
TInt CUPPrinterContainer::GenerateId()
{
	TInt id = -1;
	for (TInt i=1; i < iPrinters.Count()+2; i++)
	{
		TInt j;
		for (j=0; j < iPrinters.Count(); j++)
		{
			if (iPrinters[j]->Id() == i)
			{
				break;
			}
		}

		if (j == iPrinters.Count())
		{
			id = i;
			break;
		}
	}

	return id;
}

//--------------------------------------------------------------------------------------------
//
// CUPPrinterContainer::AddCachedPrinterL
//
//--------------------------------------------------------------------------------------------
void CUPPrinterContainer::AddCachedPrinterL(TInt aId, TDesC8& aUPnPUId, TDesC8& aDisplayName, TInt aVendor)
{
	LOG("[CUPPrinterContainer::AddCachedPrinterL]");
	CUPPrinter* newPrinter = CUPPrinter::NewL(aId, aUPnPUId, aDisplayName, aVendor);
	newPrinter->SetCached(ETrue);
	CleanupStack::PushL(newPrinter);
	iPrinters.AppendL(newPrinter);
	CleanupStack::Pop(newPrinter);
}

//  End of File
