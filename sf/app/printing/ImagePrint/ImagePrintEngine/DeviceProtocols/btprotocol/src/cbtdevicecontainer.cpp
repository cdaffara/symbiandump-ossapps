/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


//  EXTERNAL INCLUDES
#include <s32file.h>

#include "cbtdevicecontainer.h"
#include "crsbtdevice.h"
#include "rsutils.h"
#include "mprotprintingdevice.h"
#include "clog.h"

//  CONSTANTS
namespace
	{
	/// File name where the BT devices will be stored, for future sessions.
	_LIT(KUsedDevicesFile,"btdevices.dat");
	/// Maximum number of Bluetooth devices to be stored.
	const TInt KUsedDevicesMax = 150;
	/// Version of the file where the BT devices will be stored.
	const TInt KUsedDevicesVersion = 5;
	}




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBtDeviceContainer* CBtDeviceContainer::NewL()
	{
	CBtDeviceContainer* self = new (ELeave) CBtDeviceContainer();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CBtDeviceContainer::ConstructL()
	{
	User::LeaveIfError( iFs.Connect());
	TInt err = RsUtils::CreateDataFolder( iFs, iDataFolder );
	LOG1("[CBtDeviceContainer::ConstructL]\t err: %d", err );
	iDataFolder = RsUtils::PathNotRom( iDataFolder );
	iFs.SetSessionPath( iDataFolder );
	LOG1("CBtDeviceContainer::ConstructL iDataFolder: %S", &iDataFolder );
	}

CBtDeviceContainer::CBtDeviceContainer() : iVersion(KUsedDevicesVersion)
	{
	}

CBtDeviceContainer::~CBtDeviceContainer()
	{
	LOG("CBtDeviceContainer::~CBtDeviceContainer begin");
	iDeviceArray.ResetAndDestroy();
	iDeviceArray.Close();
	iFs.Close();
	LOG("CBtDeviceContainer::~CBtDeviceContainer end");
	}

//--------------------------------------------------------------------------------------------
//
// CBtDeviceContainer::StoreL
//
//--------------------------------------------------------------------------------------------
void CBtDeviceContainer::StoreL()
{
	LOG("[CBtDeviceContainer::StoreL]\t begin");

	TFileName fileName( iDataFolder );
	fileName.Append( KUsedDevicesFile );
	LOG1("CBtDeviceContainer::StoreL fileName 1: %S", &fileName);

	RFileWriteStream outstream;
	TFileName tempFileName;

	User::LeaveIfError( outstream.Temp( iFs, iDataFolder, tempFileName, EFileWrite ) );
	LOG1("[CBtDeviceContainer::StoreL]\t tempFileName: %S", &tempFileName);

	CleanupClosePushL( outstream );
	ExternalizeL( outstream );
	// makes sure all data is written from the buffer to file, before calling close.
	outstream.CommitL();
	CleanupStack::PopAndDestroy(); // outstream

	if( RsUtils::FileExists( fileName, iFs ) )
		User::LeaveIfError( iFs.Delete( fileName ) );
	User::LeaveIfError( iFs.Rename( tempFileName, fileName ) );

	LOG("[CBtDeviceContainer::StoreL]\t end");
}


//--------------------------------------------------------------------------------------------
//
// CBtDeviceContainer::RestoreL
//
//--------------------------------------------------------------------------------------------
void CBtDeviceContainer::RestoreL()
{
	TFileName fileName( iDataFolder );
	fileName.Append( KUsedDevicesFile );
	
	RFileReadStream instream;
	CleanupClosePushL( instream );

	User::LeaveIfError( instream.Open( iFs, fileName, EFileRead ) );
	InternalizeL(instream);

	CleanupStack::PopAndDestroy(); // instream
}

//--------------------------------------------------------------------------------------------
//
// CBtDeviceContainer::ExternalizeL
//
//--------------------------------------------------------------------------------------------
void CBtDeviceContainer::ExternalizeL(RWriteStream& aStream) const
	{
	TInt count = 0;
	for( TInt i = 0; i < iDeviceArray.Count(); i++ )
	{
		if(iDeviceArray[i]->IsUsed())
		{
			++count;
		}
	}
	LOG1("[CBtDeviceContainer::ExternalizeL]\t count: %d", count);
	if(0 == count)
	{
		return;
	}

	aStream.WriteInt32L(iVersion);
	aStream.WriteInt32L(iCurrentId);
	aStream.WriteInt32L(count);
	for( TInt i = 0; i < iDeviceArray.Count(); i++ )
	{
		if(iDeviceArray[i]->IsUsed())
		{
			aStream << *(iDeviceArray[i]);
		}
	}
	LOG("CBtDeviceContainer::ExternalizeL end");
	}

//--------------------------------------------------------------------------------------------
//
// CBtDeviceContainer::InternalizeL
//
//--------------------------------------------------------------------------------------------
void CBtDeviceContainer::InternalizeL(RReadStream& aStream)
{
	TInt version = aStream.ReadInt32L();
	if (version != KUsedDevicesVersion)
	{
		return;
	}
	iVersion = version;

	iCurrentId = aStream.ReadInt32L();
	TInt count = aStream.ReadInt32L();
	for (TInt i = 0; i < count; i++)
	{
		CRsBtDevice* tmpDevice = CRsBtDevice::NewLC();

		tmpDevice->InternalizeL(aStream);

		tmpDevice->SetUsed(ETrue);
		tmpDevice->SetJustFound(EFalse);
		User::LeaveIfError( iDeviceArray.Append( tmpDevice ) );
		CleanupStack::Pop();	// tmpDevice
	}
}

CRsBtDevice* CBtDeviceContainer::At(TInt aPosition) const
	{
	if( aPosition >= 0 && aPosition < iDeviceArray.Count() )
		{
		return iDeviceArray[aPosition];
		}
	else
		{
		return NULL;
		}
	}

TInt CBtDeviceContainer::Count() const
	{
	return iDeviceArray.Count();
	}

//--------------------------------------------------------------------------------------------
//
// CBtDeviceContainer::Find
//
//--------------------------------------------------------------------------------------------
TInt CBtDeviceContainer::Find( const CRsBtDevice &aDevice, TInt aInitIndex) const
{
	TInt nCnt;
	CRsBtDevice *cmpDev;

	for (nCnt = aInitIndex+1; nCnt < iDeviceArray.Count(); nCnt++)
	{
		cmpDev = iDeviceArray[nCnt];
		if (cmpDev->Equals(aDevice))
		{
			return nCnt;
		}
	}

	return KErrNotFound;
}

//--------------------------------------------------------------------------------------------
//
// CBtDeviceContainer::Find
//
//--------------------------------------------------------------------------------------------
TInt CBtDeviceContainer::Find( const TInt aDeviceId, TInt aInitIndex) const
{
	for (TInt i = aInitIndex+1; i < iDeviceArray.Count(); i++)
	{
		if (iDeviceArray[i]->DeviceId() == aDeviceId)
		{
			return i;
		}
	}
	return KErrNotFound;
}

//--------------------------------------------------------------------------------------------
//
// CBtDeviceContainer::Delete
//
//--------------------------------------------------------------------------------------------
TInt CBtDeviceContainer::Delete( const TInt aDeviceId )
{
	TInt pos = Find(aDeviceId);
	if(KErrNotFound != pos)
	{
		iDeviceArray[pos]->SetUsed(EFalse);
	}
	return pos;
}

//--------------------------------------------------------------------------------------------
//
// CBtDeviceContainer::Delete
//
//--------------------------------------------------------------------------------------------
TInt CBtDeviceContainer::Delete( const CRsBtDevice &aDevice )
{
	TInt pos = Find(aDevice);
	if(KErrNotFound != pos)
	{
		iDeviceArray[pos]->SetUsed(EFalse);
	}
	return pos;
}

//--------------------------------------------------------------------------------------------
//
// CBtDeviceContainer::AppendL
//
//--------------------------------------------------------------------------------------------
void CBtDeviceContainer::AppendL( CRsBtDevice* aDevice )
{
	LOG1("[CBtDeviceContainer::AppendL]\t %S", &(aDevice->FriendlyName()));
	
	if( iDeviceArray.Count() == KUsedDevicesMax )
	{
		TInt nOldest = FindOldest(ENotUsed | ETimeLastUsed);
		CRsBtDevice* device = iDeviceArray[nOldest];
		iDeviceArray.Remove(nOldest);
		delete device;
	}

	if( aDevice->DeviceId() <= 0 )
	{
		aDevice->SetDeviceId( GetAvailableId( *aDevice ) );
	}
	iDeviceArray.AppendL( aDevice );
}

TInt CBtDeviceContainer::FindOldest(TUint aFlags) const
	{
	TInt oldest=-1;
	TTime timeOldest, timeCmp;
	CRsBtDevice *cmpDev;
	TBool bSavedCheck;

	timeOldest.UniversalTime();
	for (TInt i=0; i < iDeviceArray.Count(); ++i)
	{
		cmpDev = iDeviceArray[i];
		if ((aFlags & EUsed) && (aFlags & ENotUsed))
		{
			bSavedCheck = ETrue;
		}
		else if (aFlags & ESaved)
		{
			bSavedCheck = cmpDev->IsUsed();
		}
		else if (aFlags & ENotUsed)
		{
			bSavedCheck = !(cmpDev->IsUsed());
		}
		else
		{
			bSavedCheck = ETrue;
		}

		if (bSavedCheck)
		{
			if (aFlags & ETimeLastUsed)
			{
				timeCmp = cmpDev->TimeLastUsed();
			}
			else if (aFlags & ETimeDiscovered)
			{
				timeCmp = cmpDev->TimeDiscovered();
			}
			else
			{
				// Error, but shouldn't get here if program does the right thing
				return -1;
			}

			if (timeCmp < timeOldest)
			{
				oldest = i;
				timeOldest = timeCmp;
			}
		}
	}

	return oldest;
	}

TInt CBtDeviceContainer::GetAvailableId( const CRsBtDevice& aDevice )
{
	LOG("CBtDeviceContainer::GetAvailableId begin");
	const TBTDevAddr& btAddr = aDevice.BDAddr();
	TInt id( 0 );
	TInt intSize = sizeof(TInt);
	LOG1("CBtDeviceContainer::GetAvailableId intSize: %d", intSize);
	for( TInt i = ( KBTDevAddrSize - 1 ), s = 0; i >= 0 && s < intSize; i--, s++ )
	{
		const TUint8& element = btAddr[i];
		TInt temp( element );
		temp <<= ( s * 8 );
		id |= temp;
	}

	id &= DEVICE_ID_FIELD_MASK;
	LOG1("[CBtDeviceContainer::GetAvailableId]\t id after masking: %d", id);

	while( !IsIdAvailable( id ) )
	{
		id++;
	}

	iCurrentId = id;
	LOG1("[CBtDeviceContainer::GetAvailableId]\t return: %d", iCurrentId);
	return iCurrentId;
}


TBool CBtDeviceContainer::IsIdAvailable( TInt aId ) const
{
	TBool result( ETrue );
	for( TInt x = 0; x < iDeviceArray.Count() && result; ++x )
	{
		if ( aId == iDeviceArray[x]->DeviceId() )
		{
			result = EFalse;
		}
	}
	return result;
}

void CBtDeviceContainer::Reset()
	{
	iDeviceArray.ResetAndDestroy();
	}

//  End of File
