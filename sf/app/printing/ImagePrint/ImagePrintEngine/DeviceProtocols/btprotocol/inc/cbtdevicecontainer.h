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


#ifndef CBTDEVICECONTAINER_H
#define CBTDEVICECONTAINER_H

#include <e32base.h>
#include <s32strm.h>
#include <f32file.h>

class CRsBtDevice;

/**
  * \brief Brief Description
  *
  * Detailed Description
  */
NONSHARABLE_CLASS(CBtDeviceContainer) : public CBase
	{
	public:
		void StoreL();
		void RestoreL();
		void AppendL( CRsBtDevice* aDevice );
		TInt Delete( const TInt aDeviceId ); // returns the position in array or KErrNotFound
		TInt Delete( const CRsBtDevice &aDevice ); // returns the position in array or KErrNotFound
		TInt Find( const CRsBtDevice &aDevice, TInt aInitIndex = -1 ) const;
		TInt Find( const TInt aDeviceId, TInt aInitIndex = -1 ) const;
		TInt Count() const;
		CRsBtDevice* At(TInt aPosition) const;
		static CBtDeviceContainer* NewL();
		~CBtDeviceContainer();
		void Reset();

	private:

		enum
			{
			ETimeLastUsed = 1,
			ETimeDiscovered = 2,
			EUsed = 4,
			ENotUsed = 8
			};

		TInt FindOldest(TUint aFlags) const;
		void ExternalizeL(RWriteStream& aStream) const;
		void InternalizeL(RReadStream& aStream);
		void ConstructL();
		CBtDeviceContainer();
		TInt GetAvailableId( const CRsBtDevice& aDevice );
		TBool IsIdAvailable( TInt aId ) const;

	private:

		TInt iVersion;
		RPointerArray<CRsBtDevice> iDeviceArray;
		TFileName iDataFolder;
		TInt iCurrentId;
		RFs iFs;
	};

#endif // CBTDEVICECONTAINER_H

//  End of File
