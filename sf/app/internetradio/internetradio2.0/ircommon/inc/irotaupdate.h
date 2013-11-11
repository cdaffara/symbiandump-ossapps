/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef IROTAUPDATE_H
#define IROTAUPDATE_H

#include <e32base.h>

class RReadStream;
class RWriteStream;

/**
 * This class provides the basic structure to hold the OTA information.
 * information that come from the isds server is parsed and put into this structure
 * It also provides the basic setter functions to 
 * manage the OTA information.
 * @code
 * CIROTAUpdate* ota = CIROTAUpdate::NewL()
 * ota->SetXXX();
 * delete ota;
 * @endcode
 *   
 */

class CIROTAUpdate : public CBase
    {
    
    public:
    
    /**
     * Function : NewL
     * Function returns an instance of CIROTAUpdate
     * Two phase constructor
     * @return instance of CIROTAUpdate
     */
    IMPORT_C static CIROTAUpdate* NewLC();
    
    /**
     * CIROTAUpdate::NewLC()
     * Static function
     * standard two phased constructor
     * @return *CIROTAUpdate
     */
    IMPORT_C static CIROTAUpdate* NewL();
    
    /**
     * CIROTAUpdate::ConstructL()
     * standard second phase constructor
     */
    IMPORT_C void ConstructL();
    
    /**
     * CIROTAUpdate::~CIROTAUpdate()
     * standard C++ destructor
     */
    ~CIROTAUpdate();
    
    public:
    
    /**
     * CIROTAUpdate::SetUpdateMethod()
     * Function to set update method
     * @param TDesC
     */
    IMPORT_C void SetUpdateMethod( const TDesC& aUpdateMethod );
    
    /**
     * CIROTAUpdate::SetVersion()
     * Function to set version
     * @param TDesC
     */
    IMPORT_C void SetVersion( const TDesC& aVersion );
    
    /**
     * CIROTAUpdate::SetUpdateInterval()
     * Function to set update interval
     * @param TDesC
     */
    IMPORT_C void SetUpdateInterval( const TDesC& aUpdateInterval );
    
    /**
     * CIROTAUpdate::SetOTAUrl()
     * Function to set OTA Url
     * @param TDesC
     */
    IMPORT_C void SetOTAUrl( const TDesC& aOTAUrl );
    
    /**
     * CIROTAUpdate::ExternalizeL()
     * externalizes the data
     * @param RWriteStream
     */
    IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
    
    /**
     * CIROTAUpdate::InternalizeL()
     * internalizes the data
     * @param RWriteStream
     */
    IMPORT_C void InternalizeL( RReadStream& aStream );
    
    public:
    // native or browser    
    HBufC* iUpdateMethod;
    // version no
    HBufC* iVersion; 
    // url where the newer version can be found
    HBufC* iOTAUrl;
    // silence period
    TInt iUpdateInterval;
   };
   
#endif// IROTAUPDATE_H

