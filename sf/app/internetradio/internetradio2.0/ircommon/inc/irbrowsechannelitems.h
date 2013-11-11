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

#ifndef IRBROWSECHANNELITEMS_H
#define IRBROWSECHANNELITEMS_H

#include <e32base.h>

class RReadStream;
class RWriteStream;

/**
 * This class provides the basic structure to hold the browse information 
 * information that come from the isds server.
 * It also provides the basic getter and setter functions to 
 * manage the browse information.
 * @code
 * CIRBrowseChannelItems* browse = CIRBrowseChannelItems::NewL()
 * browse->SetXXX();
 * browse->GetXXX()
 * delete browse;
 * @endcode
 *   
 */
class CIRBrowseChannelItems : public CBase
    {
public :

    /**
     * CIRBrowseChannelItems::NewLC()
     * Static function
     * standard two phased constructor
     * @return CIRBrowseChannelItems*
     */
    IMPORT_C static CIRBrowseChannelItems* NewLC();

    /**
     * CIRBrowseChannelItems::NewL()
     * Static function
     * standard two phased constructor
     * @return CIRBrowseChannelItems*
     */
    IMPORT_C static CIRBrowseChannelItems* NewL();

    /**
     * CIRBrowseChannelItems::ConstructL()
     * standard two phased constructor
     */
    void ConstructL() const;

    /**
     * CIRBrowseChannelItems::~CIRBrowseChannelItems()
     * standard C++ destructor
     */
    ~CIRBrowseChannelItems();

public:

    /**
     * CIRBrowseChannelItems::SetTextL()
     * Set data namely channelname,channelid,description
     * @param TDesC,TDesC,TDesC
     */
    IMPORT_C void SetTextL( 
        const TDesC& aChannelName,const TDesC& aChannelId, const TDesC& aDescription );

    /**
     * CIRBrowseChannelItems::SetGetOperationL()
     * Set data get methord
     * @param TDesC
     */
    IMPORT_C void SetGetOperationL( const TDesC& aChannelGetOperation );

    /**
     * CIRBrowseChannelItems::SetSize()
     * Set data size of data
     * @param TDesC
     */
    IMPORT_C void SetSize( const TDesC& aSize );

    /**
     * CIRBrowseChannelItems::SetImageUrlL()
     * Set data for iImgUrl
     * @param TDesC
     */
    IMPORT_C void SetImageUrlL( const TDesC& aImgUrl );

    /**
     * CIRBrowseChannelItems::SetGetOperation()
     * Set data for the get operation
     * @param TDesC
     */
    IMPORT_C void SetGetOperation( TDesC& aCatGetOperation );

    /**
     * CIRBrowseChannelItems::ExternalizeL()
     * Externalizes data into persistent storage
     * @param RWriteStream
     */
    IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;

    /**
     * CIRBrowseChannelItems::InternalizeL()
     * internalizes data from persistent storage
     * @param RReadStream
     */
    IMPORT_C void InternalizeL( RReadStream& aStream );

    /**
     * CIRBrowseChannelItems::SetLastPlayedUrl()
     * Sets the last played url
     */
    IMPORT_C void SetLastPlayedUrl();

    /**
     * CIRBrowseChannelItems::SetBitrate()
     * Set data for 3bitrates
     * @param TDesC,TDesC,TDesC
     */
    IMPORT_C void SetBitrate( 
        const TDesC& aBitrate1,const TDesC& aBitrate2, const TDesC& aBitrate3 );

    /**
     * CIRBrowseChannelItems::SetBitrate()
     * Set data for 2 bitrates
     * @param TDesC,TDesC
     */
    IMPORT_C void SetBitrate( const TDesC& aBitrate1, const TDesC& aBitrate2 );

    /**
     * CIRBrowseChannelItems::SetBitrate()
     * Set data for 1 bitrate
     * @param TDesC
     */
    IMPORT_C void SetBitrate( const TDesC& aBitrate1 );

    /**
     * CIRBrowseChannelItems::SetBannerUrl()
     * Set data Banner Url, For Advertisements
     * @param TDesC
     */
    IMPORT_C void SetBannerUrl( const TDesC& aBannerUrl );
    
    /**
     * CIRBrowseChannelItems::SetClickThroughUrl()
     * Set data Banner Url
     * @param TDesC
     */
    IMPORT_C  void SetClickThroughUrl( const TDesC& aClickThroughUrl );

public :
    //channel name
    HBufC* iChannelName;
    //short description
    HBufC* iShortDescription;
    //url to fetch the preset from
    HBufC* iChannelGetOperation;
    //url to fetch the image from
    RBuf iImgUrl;
    //supported bitrates(were required for bitrate view)
    //not in use now
    TInt iBitrate1;
    TInt iBitrate2;
    TInt iBitrate3;
    //channel id
    TUint iChannelID;
    TInt iSize;
    
    //For Advertisements
    HBufC* iBannerUrl;
    HBufC* iClickThroughUrl;
    };

#endif // IRBROWSECHANNELITEMS_H

