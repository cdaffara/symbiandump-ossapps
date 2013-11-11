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

#ifndef IRBROWSECATAGORYITEMS_H
#define IRBROWSECATAGORYITEMS_H

#include <e32base.h>

class RReadStream;
class RWriteStream;

/**
 * This class provides the basic structure to hold the browse information 
 * information that come from the isds server.
 * It also provides the basic getter and setter functions to 
 * manage the browse information.
 * @code
 * CIRBrowseCatagoryItems* browse = CIRBrowseCatagoryItems::NewL()
 * browse->SetXXX();
 * browse->GetXXX()
 * *browse = *browse2
 * delete browse;
 * @endcode
 *   
 */
class CIRBrowseCatagoryItems : public CBase
    {
public :

    /**
     * CIRBrowseCatagoryItems::NewL()
     * Static function
     * standard two phased constructor
     * @return CIRBrowseCatagoryItems*
     */
    IMPORT_C static CIRBrowseCatagoryItems* NewL();
    
    /**
     * CIRBrowseCatagoryItems::NewL()
     * Static function
     * standard two phased constructor
     * @return CIRBrowseCatagoryItems*
     */
    IMPORT_C static CIRBrowseCatagoryItems* NewLC();

    /**
     * CIRBrowseCatagoryItems::ConstructL()
     * standard second phase constructor
     */
    void ConstructL() const;

    /**
     * CIRBrowseCatagoryItems::~CIRBrowseCatagoryItems()
     * standard C++ destructor
     */
    ~CIRBrowseCatagoryItems();

    /**
     * CIRBrowseCatagoryItems::SetTextL()
     * sets the category name and category id
     */
    IMPORT_C void SetTextL( const TDesC& aCatName, const TDesC& aCatId );

    /**
     * CIRBrowseCatagoryItems::SetSize()
     * sets the count of the no of items present under that type
     */
    IMPORT_C void SetSize( const TDesC& aCatSize );
    
    /**
     * CIRBrowseCatagoryItems::SetGetOperation()
     * sets the url that is used to get the next level of data
     */
    IMPORT_C void SetGetOperation( const TDesC& aCatGetOperation );
    
    /**
     * CIRBrowseCatagoryItems::SetBannerUrl()
     * Set data Banner Url
     * @param TDesC
     */
    IMPORT_C void SetBannerUrl( const TDesC& aBannerUrl );
    
    /**
     * CIRBrowseCatagoryItems::SetClickThroughUrl( )
     * Set data Banner Url
     * @param TDesC
     */
    IMPORT_C void SetClickThroughUrl( const TDesC& aClickThroughUrl );

    /**
     * CIRBrowseCatagoryItems::ExternalizeL()
     * Externalizes the data from file
     * @param RWriteStream
     */
    IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;

    /**
     *CIRBrowseCatagoryItems::InternalizeL()
     *Internalizes the data from file
     *@param RWriteStream&
     */
    IMPORT_C void InternalizeL( RReadStream& aStream );

public:
    //category name
    HBufC* iCatName;
    //category id
    HBufC* iCatId;
    //get operation
    HBufC* iCatGetOperation;
    //no of entities
    TInt iSize;
    //For Advertisements
    HBufC* iCatBannerUrl;
    HBufC* iCatClickThroughUrl;
    };
    
#endif// IRBROWSECATAGORYITEMS_H
