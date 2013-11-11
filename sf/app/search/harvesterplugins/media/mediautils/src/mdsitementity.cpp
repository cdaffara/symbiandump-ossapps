/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "mdsitementity.h"
_LIT(KColon, ":");

// -----------------------------------------------------------------------------
// CMDSEntity::NewLC
// -----------------------------------------------------------------------------
//
CMDSEntity* CMDSEntity::NewLC()
    {
    CMDSEntity* self = new (ELeave) CMDSEntity();
    CleanupStack::PushL(self);
    return self;
    }
    
// -----------------------------------------------------------------------------
// CMDSEntity::NewL
// -----------------------------------------------------------------------------
//
CMDSEntity* CMDSEntity::NewL()
    {
    CMDSEntity* self = CMDSEntity::NewLC();
    CleanupStack::Pop(self); // self
    return self;
    }   

// -----------------------------------------------------------------------------
// CMDSEntity::NewLC
// -----------------------------------------------------------------------------
//
CMDSEntity* CMDSEntity::NewLC(TInt aKey, const TDesC& aUri, TDriveNumber& aDrive)
    {
    CMDSEntity* self = new (ELeave) CMDSEntity(aKey,aUri,aDrive);
    CleanupStack::PushL(self);  
    return self;
    }

// -----------------------------------------------------------------------------
// CMDSEntity::CMDSEntity()
// -----------------------------------------------------------------------------
//
CMDSEntity::CMDSEntity()
    {
    iKey = 0;
    iUri.Copy(KNullDesC);
    iDriveNumber = TDriveNumber(KErrNotSupported);
    iPath.Copy(KNullDesC);
    }

void CMDSEntity::Reset()
    {
    iKey = 0;
    iUri.Copy(KNullDesC);
    iDriveNumber = TDriveNumber(KErrNotSupported);
    iPath.Copy(KNullDesC);
    }

CMDSEntity::CMDSEntity(TInt aKey, 
                       const TDesC& aUri, 
                       TDriveNumber& aDrive)                                        
    {   
    iKey = aKey;
    iUri.Copy(aUri);
    iDriveNumber = aDrive;
    }   

// -----------------------------------------------------------------------------
// CMDSEntity::~CMDSEntity
// -----------------------------------------------------------------------------
//
CMDSEntity::~CMDSEntity()
    {
    
    }

// -----------------------------------------------------------------------------
// CMDSEntity::Key
// -----------------------------------------------------------------------------
//
TInt CMDSEntity::Key() const
    {
    //unique identifier of the note
    return iKey;
    }


const TDesC& CMDSEntity::Uri() const
    {
    return iUri;
    }

TDriveNumber CMDSEntity::DriveNumber()
    {
    return iDriveNumber;
    }

void CMDSEntity::Set(TInt aKey, const TDesC& aUri, TDriveNumber& aDrive)
    {
    iKey = aKey;
    iUri.Copy(aUri);
    iDriveNumber = aDrive;
    }

void CMDSEntity::Setkey(TInt aKey)
    {
    iKey = aKey;
    }

void CMDSEntity::SetUri(const TDesC& aUri)
    {
    iUri.Copy(aUri);
    }

void CMDSEntity::SetDrive(TDriveNumber& aDrive)
    {
    iDriveNumber = aDrive;
    }

const TDesC& CMDSEntity::GetPath()
    {
    TChar chr;
    const TInt ret = RFs::DriveToChar(iDriveNumber, chr);
    if(KErrNone == ret)
        {
         iPath.Append(chr);
         iPath.Append(KColon);
         iPath.Append(iUri);
        }
    return iPath;
    }
