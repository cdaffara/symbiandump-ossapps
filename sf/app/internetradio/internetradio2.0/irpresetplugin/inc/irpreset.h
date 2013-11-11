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


#ifndef C_IRPRESET_H
#define C_IRPRESET_H

#include <badesca.h>
#include "pspresetinterface.h"

class CIRIsdsPreset;
class CIRChannelServerUrl;

/**
 * preset abstract class to expose general apis to the app
 * it is the base class for IRPresetImpl,which is a plugin
 * the plugin implements all the pure virtual functions
 */
class CIRPreset : public CPSPresetInterface
    {

public:

    /**
     * CIRPreset::SetId()
     * sets id
     * @param TDesC,supplied by xml parser
     */
    virtual void SetId( const TDesC &aId ) = 0;

    /**
     * CIRPreset::SetId()
     * sets id
     * @param TInt,provided to manipulate the id number
     */
    virtual void SetId( TInt aId ) = 0;

    /**
     * CIRPreset::SetLangInfoL()
     * sets the preset language name and the code
     * @param TdesC,TdesC language id and name to be set
     */
    virtual void SetLangInfoL( const TDesC &aLangId,
                              const TDesC& aLangName ) = 0;

    /**
     * CIRPreset::SetCountryInfoL()
     * sets the preset country name and the code
     * @param TdesC,TdesC country id and country name to be set
     */
    virtual void SetCountryInfoL( const TDesC &aCountryId,
                                 const TDesC& aCountryName ) = 0;

    /**
     * CIRPreset::SetLangCodeL()
     * sets the language code
     * @param TdesC language code
     */
    virtual void SetLangCodeL( const TDesC &aLangId )= 0;

    /**
     * CIRPreset::SetLangL()
     * sets the language
     * @param TdesC language
     */
    virtual void SetLangL( const TDesC& aLangName ) = 0;

    /**
     * CIRPreset::SetCountryCodeL()
     * sets the language code
     * @param TdesC language code
     */
    virtual void SetCountryCodeL( const TDesC &aCountryId ) = 0;

    /**
     * CIRPreset::SetCountryNameL()
     * sets the language name
     * @param TdesC language name
     */
    virtual void SetCountryNameL( const TDesC& aCountryName ) = 0;

    /**
     * CIRPreset::SetLastModifiedTimeL()
     * gets the channel  url at a particular index
     * @param TDesC,time stamp
     */
    virtual void SetLastModifiedTimeL( const TDesC &aLastModified ) = 0;

    /**
     * CIRPreset::SetGenreInfoL()
     * sets the genreid and name
     * @param aGenreId
     * @param aGenreName
     */
    virtual void SetGenreInfoL( const TDesC& aGenreId,
                               const TDesC& aGenreName ) = 0;

    /**
     * CIRPreset::SetImgUrlL()
     * sets the image url
     * @param aImgUrl
     */
    virtual void SetImgUrlL( const TDesC& aImgUrl ) = 0;

    /**
     * CIRPreset::SetAdvertisementInUseL()
     * sets the advertisementinuse url
     * @param aAdvertisementInUse
     */
    virtual void SetAdvertisementInUseL( const TDesC& aAdvertisementInUse ) = 0;

    /**
     * CIRPreset::SetDescriptionL()
     * sets the description for the channel
     * @param aDescription
     */
    virtual void SetDescriptionL( const TDesC& aDescription ) = 0;

    /**
     * CIRPreset::SetAdvertisementUrlL()
     * sets the advertisement url
     * @param aAdvertisementUrl
     */
    virtual void SetAdvertisementUrlL( const TDesC& aAdvertisementUrl ) = 0;

    /**
     * CIRPreset::SetUrlCount()
     * sets the count of the number of urls
     */
    virtual void SetUrlCount() = 0;

    /**
     * CIRPreset::SetShortDescL()
     * sets the short description
     * @param aShortDesc
     */
    virtual void SetShortDescL( const TDesC& aShortDesc ) = 0;

    /**
     * CIRPreset::SetMusicStoreStatusL()
     * sets the musicStoreEnabled flag
     * @param aMusicStoreStatus
     */
    virtual void SetMusicStoreStatusL( const TDesC& aMusicStoreStatus ) = 0;

    /**
     * CIRPreset::SetUrlCount()
     * sets the count of the number of urls
     * @param aCnt ,count
     */
    virtual void SetUrlCount( TInt aCnt ) = 0;

    /**
     * CIRPreset::SetIndex()
     * sets the index for the preset.used only for presets in the fav list
     *  @param aInt
     */
    //not required
    //virtual void SetIndex( TInt aInt ) = 0

    /**
     * CIRPreset::SetUrlL()
     * sets the channel server url
     * @param aServerName ,server name
     * @param aUrl ,server url
     * @param aBitrate .server bitrate
     */
    virtual void SetUrlL( const TDesC &aServerName,
                          const TDesC &aUrl,
                          const TDesC &aBitrate ) = 0;

    /**
     *CIRPreset::SetUrlL()
     *sets the channel server url
     *@param aServerName ,server name
     *@param aUrl ,server url
     *@param aBitrate ,server bitrate
     */
    virtual void SetUrlL( const TDesC &aServerName,
                          const TDesC &aUrl,
                          TInt aBitrate )=0;

    /**
     *CIRPreset::SetChannelType()
     *sets the channel type
     *@param aChannelType userdefined,isds,adhoc
     */
    virtual void SetChannelType( TInt aChannelType )=0;

    /**
     *CIRPreset::GetId()
     *gets the channel id
     *@return TInt
     */
    virtual TInt GetId() const=0;

    /**
     * CIRPreset::GetLangId()
     * gets the channel channelangid
     * @return TDesC
     */
    virtual const TDesC& GetLangId() const = 0;

    /**
     * CIRPreset::GetLangName()
     * gets the channel language name
     * @return TDesC
     */
    virtual const TDesC& GetLangName() const = 0;

    /**
     * CIRPreset::GetCountryId()
     * gets the channel countryid
     * @return TDesC
     */
    virtual const TDesC& GetCountryId() const = 0;

    /**
     * CIRPreset::GetCountryName()
     * gets the channel country name
     * @return TDesC
     */
    virtual const TDesC& GetCountryName() const = 0;

    /**
     * CIRPreset::GetLastModifiedTime()
     * gets the channel last modified time stamp
     * @return TDesC
     */
    virtual const TDesC& GetLastModifiedTime() const = 0;

    /**
     * CIRPreset::GetDescription()
     * gets the channel description
     * @return TDesC
     */
    virtual const TDesC& GetDescription() const = 0;

    /**
     * CIRPreset::GetShortDescription()
     * gets the channel  short description
     * @return TDesC
     */
    virtual const TDesC& GetShortDescription() const = 0;

    /**
     * CIRPreset::GetMusicStoreStatus()
     * gets the channel  musicStoreEnabled flag
     * @return TDesC
     */
    virtual const TDesC& GetMusicStoreStatus() const = 0;

    /**
     * CIRPreset::GetChannelType()
     * gets the channel  Type
     * @return TInt
     */
    virtual TInt GetChannelType() const = 0;

    /**
     * CIRPreset::GetGenreId()
     * gets the channel  genreid
     * @return TDesC
     */
    virtual const TDesC& GetGenreId() const = 0;

    /**
     * CIRPreset::GetGenreName()
     * gets the channel  genre name
     * @return TDesC
     */
    virtual const TDesC& GetGenreName() const = 0;

    /**
     * CIRPreset::GetAdvertisementUrl()
     * gets the channel  advertisement url
     * @return TDesC
     */
    virtual const TDesC& GetAdvertisementUrl() const = 0;

    /**
    *CIRPreset::GetAdvertisementInUse()
    *gets the channel  advertisement In Use
    *@return TDesC
    */
    virtual const TDesC& GetAdvertisementInUse() const = 0;
	/**
	*CIRPreset::GetImgUrl()
	*gets the channel  image url
	*@return TDesC
	*/
    virtual const TDesC& GetImgUrl() const = 0;

    /**
     * CIRPreset::GetUrlCount()
     * gets the channel  url count
     * @return TDesC
     */
    virtual TInt GetUrlCount() const = 0;

	virtual CIRChannelServerUrl& GetUrl(TInt aIndex) const = 0;
    /*
     * get the channel url at specified position
     *
     */
    virtual const TDesC& GetChannelUrlAt( TInt aIndex ) const = 0;

    /**
     *CIRPreset::GetAvailableBitrates()
     *gets the bitrate array
     *@return RArray<TInt>
     */
    virtual RArray<TInt>& GetAvailableBitrates() = 0;

    /**
     * CIRPresetImpl::CIRIsdsPresetToCIRPresetImplL()
     * used to convert the isdspresets to plugnized presets.
     * Does a deep copy for all the member variables
     * @param CIRIsdsPreset&,the preset to be copied from
     */
    virtual void CIRIsdsPresetToCIRPresetImplL( const CIRIsdsPreset& aIsdsPreset ) = 0;

	/**
	*CIRPreset::CopyPresetDataL()
	*copy data to CIRIsdsPreset
	*@param CIRIsdsPreset&,the preset to be copied into
	*/
	virtual void CopyPresetDataL(CIRIsdsPreset& aIsdsPreset) const =0;

	 /**
     * Sets the logo data for this preset for a specific size.
     *
     * @param   aData           Raw image data to set.
     * @param   aSize           Size of the logo in pixels.
     */
    virtual void SetLogoDataL( const TDesC8& aData ) = 0;

    /**
     * Returns the logo data for this preset.
     *
     * If the returned descriptor's length is zero, then it means
     * that the preset does not have a logo.
     */
    virtual  const TDesC8& GetLogoData() const = 0;

    /*
     * Returns the played times of the channel
     */
    virtual TInt GetPlayedTimes() const = 0;

    /*
     * Set the played times of the channel
     */
    virtual void SetPlayedTimes( TInt aPlayedTimes ) = 0;
    
    /*
     * Get Rename status
     */
    virtual TBool GetRenamed() const = 0;
    
    /*
     * Set Rename status
     */
    virtual void SetRenamed() = 0;
    };

#endif // C_IRPRESET_H
