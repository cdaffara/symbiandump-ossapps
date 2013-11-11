/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef C_IRPRESETIMPL_H
#define C_IRPRESETIMPL_H

#include "irpreset.h"

class CIRChannelServerUrlImpl;


/**
 * This class provides the basic structure to hold the preset
 *information that is saved in the favourites database
 *It also provides the basic getter and setter functions to
 *manage the preset information.
 *for url selection for a particular bitrate some functionality
 *has been provided
 *copy constructor and assignment operator has been provided to
 *override the default ones
 *
 * @code
 *CIRPresetImpl* preset = CIRPresetImpl::NewL()
 *preset->SetXXX();
 *preset->GetXXX()
 * *preset1 = *preset2
 * @endcode
 *
 */

class CIRPresetImpl : public CIRPreset
    {
public:

    /**
     * CIRPresetImpl::NewL()
     * Static function
     * standard two phased constructor
     * @param void
     * @return *CIRPresetImpl
     */
    static CIRPresetImpl* NewL( TAny* aParams );

    /**
     * CIRPresetImpl::NewLC()
     * Static function
     * standard two phased constructor
     * @param void
     * @return *CIRPresetImpl
     */
    static CIRPresetImpl* NewLC( TAny* aParams );

    /**
     * CIRPresetImpl::~CIRIsdsPreset()
     * standard C++ destructor
     * @param NA
     * @return NA
     */
    ~CIRPresetImpl();

    /**
     * CIRPresetImpl::SetId()
     * sets id
     * @param TDesC,supplied by xml parser
     */
    void SetId( const TDesC &aId );

    /**
     * CIRPresetImpl::SetId()
     * sets id
     * @param TInt,provided to manipulate the id number
     */
    void SetId( TInt aId );

    /**
     * CIRPresetImpl::SetLangInfoL()
     * sets the preset language name and the code
     * @param TdesC,TdesC language id and name to be set
     */
    void SetLangInfoL( const TDesC &aLangId,
                      const TDesC& aLangName );

    /**
     * CIRPresetImpl::SetCountryInfoL()
     * sets the preset country name and the code
     * @param TdesC,TdesC country id and country name to be set
     */
    void SetCountryInfoL( const TDesC &aCountryId,
                         const TDesC& aCountryName );

    /**
     * CIRPresetImpl::SetLangCodeL()
     * sets the language code
     * @param TdesC language code
     */
    void SetLangCodeL( const TDesC &aLangId );

    /**
     * CIRPresetImpl::SetLangL()
     * sets the language
     * @param TdesC language
     */
    void SetLangL( const TDesC& aLangName );

    /**
     * CIRPresetImpl::SetCountryCodeL()
     * sets the language code
     * @param TdesC language code
     */
    void SetCountryCodeL( const TDesC &aCountryId );

    /**
     * CIRPresetImpl::SetCountryNameL()
     * sets the language name
     * @param TdesC language name
     */
    void SetCountryNameL( const TDesC& aCountryName );

    /**
     * CIRPresetImpl::SetLastModifiedTimeL()
     * gets the channel  url at a particular index
     * @param TDesC,time stamp
     */
    void SetLastModifiedTimeL( const TDesC &aLastModified );

    /**
     * CIRPresetImpl::SetGenreInfoL()
     * sets the genreid and name
     * @param aGenreId
     * @param aGenreName
     */
    void SetGenreInfoL( const TDesC& aGenreId,
                       const TDesC& aGenreName );

    /**
     * CIRPresetImpl::SetImgUrlL()
     * sets the image url
     * @param aImgUrl
     */
    void SetImgUrlL( const TDesC& aImgUrl );

    /**
     * CIRPresetImpl::SetLogoDataL()
     * sets the Logo Data
     * @param aData
     * @param aSize
     */
    void SetLogoDataL( const TDesC8& aData );

    /**
     * CIRPresetImpl::SetAdvertisementInUseL()
     * sets the advertisementinuse url
     * @param aAdvertisementInUse
     */
    void SetAdvertisementInUseL( const TDesC& aAdvertisementInUse );

    /**
     * CIRPresetImpl::SetDescriptionL()
     * sets the description for the channel
     * @param aDescription
     */
    void SetDescriptionL( const TDesC& aDescription );

    /**
     * CIRPresetImpl::SetAdvertisementUrlL()
     * sets the advertisement url
     * @param aAdvertisementUrl
     */
    void SetAdvertisementUrlL( const TDesC& aAdvertisementUrl );

    /**
     * CIRPresetImpl::SetUrlCount()
     * sets the count of the number of urls
     */
    void SetUrlCount();

    /**
     * CIRPresetImpl::SetShortDescL()
     * sets the short description
     * @param aShortDesc
     */
    void SetShortDescL( const TDesC& aShortDesc );

    /**
     * CIRPresetImpl::SetUrlCount()
     * sets the count of the number of urls
     * @param aCnt ,count
     */
    void SetUrlCount( TInt aCnt );

    /**
     * CIRPresetImpl::SetUrlL()
     * sets the channel server url
     * @param aServerName ,server name
     * @param aUrl ,server url
     * @param aBitrate .server bitrate
     */
    void SetUrlL( const TDesC &aServerName,
                  const TDesC &aUrl,
                  const TDesC &aBitrate );

    /**
     * CIRPresetImpl::SetUrlL()
     * sets the channel server url
     * @param aServerName ,server name
     * @param aUrl ,server url
     * @param aBitrate ,server bitrate
     */
    void SetUrlL( const TDesC &aServerName,
                  const TDesC &aUrl,
                  TInt aBitrate );

    /**
     * CIRPresetImpl::SetChannelType()
     * sets the channel type
     * @param aChannelType userdefined,isds,adhoc
     */
    void SetChannelType( TInt aChannelType );

    /**
     * CIRPresetImpl::SetMusicStoreStatusL()
     * sets the musicStoreEnabled 
     * @param aMusicStoreStatus
     */
    void SetMusicStoreStatusL( const TDesC& aMusicStoreStatus );

//getter functions
    /**
     * CIRPresetImpl::GetId()
     * gets the channel id
     * @return TInt
     */
    TInt GetId() const;
    
    /**
     * CIRPresetImpl::GetLangId()
     * gets the channel channelangid
     * @return TDesC
     */
    const TDesC& GetLangId() const;

    /**
     * CIRPresetImpl::GetLangName()
     * gets the channel language name
     * @return TDesC
     */
    const TDesC& GetLangName() const;

    /**
     * CIRPresetImpl::GetCountryId()
     * gets the channel countryid
     * @return TDesC
     */
    const TDesC& GetCountryId() const;

    /**
     * CIRPresetImpl::GetCountryName()
     * gets the channel country name
     * @return TDesC
     */
    const TDesC& GetCountryName() const;

    /**
     * CIRPresetImpl::GetLastModifiedTime()
     * gets the channel last modified time stamp
     * @return TDesC
     */
    const TDesC& GetLastModifiedTime() const;

    /**
     * CIRPresetImpl::GetDescription()
     * gets the channel description
     * @return TDesC
     */
    const TDesC& GetDescription() const;

    /**
     * CIRPresetImpl::GetShortDescription()
     * gets the channel  short description
     * @return TDesC
     */
    const TDesC& GetShortDescription() const;

    /**
     * CIRPresetImpl::GetChannelType()
     * gets the channel  Type
     * @return TInt
     */
    TInt GetChannelType() const;

    /**
     * CIRPresetImpl::GetGenreId()
     * gets the channel  genreid
     * @return TDesC
     */
    const TDesC& GetGenreId() const;

    /**
     * CIRPresetImpl::GetGenreName()
     * gets the channel  genre name
     * @return TDesC
     */
    const TDesC& GetGenreName() const;

    /**
     * CIRPresetImpl::GetAdvertisementUrl()
     * gets the channel  advertisement url
     * @return TDesC
     */
    const TDesC& GetAdvertisementUrl() const;

	const TDesC& GetAdvertisementInUse() const;
    /**
     * CIRPresetImpl::GetImgUrl()
     * gets the channel  image url
     * @return TDesC
     */
    const TDesC& GetImgUrl() const;

    /**
     * CIRPresetImpl::GetUrlCount()
     * gets the channel  url count
     * @return TDesC
     */
    TInt GetUrlCount() const;

	CIRChannelServerUrl& GetUrl(TInt aIndex) const;
	/*
	*CIRPresetImpl::GetLogoData()
	*gets the Logo Data
	*@return TDesC8&
	*/
    const TDesC8& GetLogoData() const;
	
	/*
	 * get the channel url at specified position
     *
     */
	const TDesC& GetChannelUrlAt(TInt aIndex) const;

    /**
     * CIRPresetImpl::GetMusicStoreStatus()
     * gets the musicStoreEnabled 
     * @return TDesC&
     */
    const TDesC& GetMusicStoreStatus() const;

    /**
     * CIRPresetImpl::operator=()
     * assignment operator
     * @param CIRIsdsPreset& aPreset
     * @return CIRIsdsPreset
     */
    CIRPresetImpl& operator=( const CIRPresetImpl& aPreset );

    /**
     * CIRPresetImpl::CIRIsdsPresetToCIRPresetImplL()
     * used to convert the isdspresets to plugnized presets.
     * Does a deep copy for all the member variables
     * @param CIRIsdsPreset&,the preset to be copied from
     */
    void CIRIsdsPresetToCIRPresetImplL( const CIRIsdsPreset& aIsdsPreset );

    /**
     * CIRPresetImpl::CopyPresetDataL()
     * copy data to CIRIsdsPreset
     * @param CIRIsdsPreset&,the preset to be copied into
     */
    void CopyPresetDataL( CIRIsdsPreset& aIsdsPreset ) const;

    /**
     * CIRPresetImpl::GetAvailableBitrates()
     * gets the bitrate array
     * @return RArray<TInt>
     */
    RArray<TInt>& GetAvailableBitrates();

    /*
     * Returns the played times of the channel
     */
    TInt GetPlayedTimes() const;

    /**
     * CIRPresetImpl::SetPlayedTimes() 
     * Set the played times of the channel
     */
    void SetPlayedTimes( TInt aPlayedTimes );
    
    /*
     * Get Rename status
     */
    TBool GetRenamed() const;
    
    /*
     * Set Rename status
     */
    void SetRenamed();
    
protected:

    /**
     * CIRPresetImpl::CIRIsdsPreset()
     * standard C++ constructor
     * @param void
     * @return NA
     */
    CIRPresetImpl();

    /**
     * CIRPresetImpl::ConstructL(TAny* aParams)
     * standard second phase constructor(Leaveable)
     * @param void
     * @return void
     */
    void ConstructL( TAny* aParams );

//for serialization
    /**
     * CIRPresetImpl::ExternalizeL()
     * serializes the preset data into a stream
     * @param aWriteStream
     */
    void ExternalizeL( RWriteStream& aWriteStream ) const;

    /**
     * CIRPresetImpl::InternalizeL()
     * serializes the preset data from a stream to a structure
     * @param aReadStream
     */
    void InternalizeL( RReadStream& aReadStream );

private:

	/**
	 * no of channel urls
	 */
	TInt iChannelUrlCount;

	/**
	 * 0 for user defined 1 for isds type.
	 */
	TInt iType;

	/**
	 * isds generated preset id
	 */
	TInt iPresetId;

	/**
	 * preset language code
	 */
	RBuf iLanguageCode;

	/**
	 * preset language
	 */
	RBuf iLanguage;

	/**
	 * preset country of origin code
	 */
	RBuf iCountryCode;

	/**
	 * preset country of origin
	 */
	RBuf iCountry;

	/**
	 * preset last modified
	 */
	RBuf iLastModified;

	/**
	 * not in use
	 */
	RBuf iAdvertisementUrl;

	/**
	 * not in use
	 */
	RBuf iAdvertisementInUse;

	/**
	 * Logo URL
	 */
	RBuf iImgUrl;

	/**
     * Raw image data for the logo.
     * Owned.
     */
    RBuf8 iLogoData;

	/**
	 * channel description
	 */
	RBuf iDescription;

	/**
	 * channel short desription
	 */
	RBuf iShortDesc;

	/**
	 * channel genre name
	 */
	RBuf iGenereName;

	/**
	 * channel genre id
	 */
	RBuf iGenereId;
	
	/**
	 * musicStoreEnabled value
	 */
	RBuf iMusicStoreStatus;

	/**
	 * url array
	 */
	CArrayPtrFlat<CIRChannelServerUrl>* iUrlArray;

    /**
     * bitrate array
     */
    RArray<TInt> iBitrateArray;
    
    /**
     * iPlayedTimes
     */    
    TInt iPlayedTimes;   
    
    /**
     *if the station is renamed after it's added into favorites
     */
    TBool iRenamed;
    
    };

#endif // C_IRPRESETIMPL_H
