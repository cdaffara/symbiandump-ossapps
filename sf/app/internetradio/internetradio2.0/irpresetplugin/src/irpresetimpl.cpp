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


#include <s32strm.h>

#include "irchannelserverurl.h"
#include "irdebug.h"
#include "irpresetimpl.h"
#include "irisdspreset.h"

const TInt KUrlArrayMaxLength = 128;


// ---------------------------------------------------------------------------
// CIRPresetImpl::NewL()
// Static function
// standard two phased constructor
// ---------------------------------------------------------------------------
//
CIRPresetImpl* CIRPresetImpl::NewL( TAny* aParams )
    {
    IRLOG_DEBUG( "CIRPresetImpl::NewL" );
    CIRPresetImpl* self = CIRPresetImpl::NewLC( aParams );
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRPresetImpl::NewL - Exiting." );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::NewLC()
// Static function
// standard two phased constructor
// ---------------------------------------------------------------------------
//
CIRPresetImpl* CIRPresetImpl::NewLC( TAny* aParams )
    {
    IRLOG_DEBUG( "CIRPresetImpl::NewLC" );
    CIRPresetImpl* self = new ( ELeave ) CIRPresetImpl;
    CleanupStack::PushL( self );
    self->ConstructL( aParams );
    IRLOG_DEBUG( "CIRPresetImpl::NewLC - Exiting." );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::ConstructL()
// standard second phase constructor
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::ConstructL( TAny* aParams )
    {
    IRLOG_DEBUG( "CIRPresetImpl::ConstructL" );
    BaseConstructL( aParams );
	iUrlArray=
		new (ELeave) CArrayPtrFlat<CIRChannelServerUrl>(
		KUrlArrayMaxLength);

    IRLOG_DEBUG( "CIRPresetImpl::ConstructL - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::~CIRPresetImpl()
// standard C++ destructor
// ---------------------------------------------------------------------------
//
CIRPresetImpl::~CIRPresetImpl()
    {
    IRLOG_DEBUG( "CIRPresetImpl::~CIRPresetImpl" );
    iLanguageCode.Close();
    iLanguage.Close();
    iCountryCode.Close();
    iCountry.Close();
    iLastModified.Close();
    iAdvertisementUrl.Close();
    iAdvertisementInUse.Close();
    iImgUrl.Close();
    iLogoData.Close();
    iDescription.Close();
    iShortDesc.Close();
    iGenereName.Close();
    iGenereId.Close();
    iMusicStoreStatus.Close();

    if ( iUrlArray )
        {
        iUrlArray->ResetAndDestroy();
        }

    delete iUrlArray;
    iBitrateArray.Close();
    IRLOG_DEBUG( "CIRPresetImpl::~CIRPresetImpl - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::SetId()
// Function to set iPresetId
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::SetId( TInt aId )
    {
    iPresetId = aId;
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::SetId()
// Function to set iPresetId from a TDesC
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::SetId( const TDesC &aId )
    {
    TLex conv( aId );
    conv.Val( iPresetId );
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::SetLangCodeL()
// Function to set iLanguageCode from a TDesC
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::SetLangCodeL( const TDesC &aLangId )
    {
    iLanguageCode.Close();
    iLanguageCode.CreateL( aLangId );
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::SetLangL()
// Function to set iLanguage from a TDesC
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::SetLangL( const TDesC& aLangName )
    {
    iLanguage.Close();
    iLanguage.CreateL( aLangName );
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::SetCountryCodeL()
// Function to set iCountryCode from a TDesC
// ---------------------------------------------------------------------------
void CIRPresetImpl::SetCountryCodeL( const TDesC &aCountryId )
    {
    iCountryCode.Close();
    iCountryCode.CreateL( aCountryId );
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::SetCountryNameL()
// Function to set iCountry from a TDesC
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::SetCountryNameL( const TDesC& aCountryName )
    {
    iCountry.Close();
    iCountry.CreateL( aCountryName );
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::SetLangInfoL()
// Function to set iLanguageCode, iLanguage
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::SetLangInfoL( const TDesC &aLangId,
                                 const TDesC& aLangName )
    {
    SetLangCodeL( aLangId );
    SetLangL( aLangName );
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::SetLangInfoL()
// Function to set iCountryCode, iCountry
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::SetCountryInfoL( const TDesC &aCountryId,
                                    const TDesC& aCountryName )
    {
    SetCountryCodeL( aCountryId );
    SetCountryNameL( aCountryName );
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::SetLastModifiedTimeL()
// Function to set iLastModified
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::SetLastModifiedTimeL( const TDesC &aLastModified )
    {
    iLastModified.Close();
    iLastModified.CreateL( aLastModified );
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::SetGenreInfoL()
// Function to set iGenereName, iGenereId
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::SetGenreInfoL( const TDesC& aGenreId,
                                  const TDesC& aGenreName )
    {
    iGenereName.Close();
    iGenereName.CreateL( aGenreName );
    iGenereId.Close();
    iGenereId.CreateL( aGenreId );
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::SetImgUrlL()
// Function to set iImgUrl
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::SetImgUrlL( const TDesC& aImgUrl )
    {
    iImgUrl.Close();
    iImgUrl.CreateL( aImgUrl );
    }

// ---------------------------------------------------------------------------
// From class CIRPresetImpl
// Sets the raw logo image data.
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::SetLogoDataL( const TDesC8& aData )
    {
    iLogoData.Close();
    iLogoData.CreateL( aData );
    }

// ---------------------------------------------------------------------------
// From class CIRPresetImpl
// Returns the raw logo image data.
// ---------------------------------------------------------------------------
//
const TDesC8& CIRPresetImpl::GetLogoData() const
    {
    return iLogoData;
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::SetAdvertisementUrlL()
// Function to set iAdvertisementUrl
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::SetAdvertisementUrlL( const TDesC& aAdvertisementUrl )
    {
    iAdvertisementUrl.Close();
    iAdvertisementUrl.CreateL( aAdvertisementUrl );
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::SetAdvertisementInUseL()
// Function to set iAdvertisementInUse
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::SetAdvertisementInUseL( const TDesC& aAdvertisementInUse )
    {
    iAdvertisementInUse.Close();
    iAdvertisementInUse.CreateL( aAdvertisementInUse );
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::SetDescriptionL()
// Function to set iDescription
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::SetDescriptionL( const TDesC& aDescription )
    {
    iDescription.Close();
    iDescription.CreateL( aDescription );
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::SetShortDescL()
// Function to set iShortDesc
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::SetShortDescL( const TDesC& aShortDesc )
    {
    iShortDesc.Close();
    iShortDesc.CreateL( aShortDesc );
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::SetUrlCount()
// Function to set iChannelUrlCount
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::SetUrlCount()
    {
    iChannelUrlCount = iUrlArray->Count();
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::SetUrlCount()
// Function to set iChannelUrlCount
// ---------------------------------------------------------------------------
void CIRPresetImpl::SetUrlCount( TInt aCnt )
    {
    iChannelUrlCount = aCnt;
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::SetChannelType()
// Function to set iType
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::SetChannelType( TInt aType )
    {
    iType = aType;
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::SetUrl()
// Function to set Url parameter
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::SetUrlL( const TDesC &aServerName,
                             const TDesC &aUrl,
                             const TDesC &aBitrate )
    {
    IRLOG_DEBUG( "CIRPresetImpl::SetUrlL" );
	CIRChannelServerUrl* hold=CIRChannelServerUrl::NewL();
    hold->SetServerName( aServerName );
    hold->SetServerUrl( aUrl );
    TLex conv( aBitrate );
    TInt conVal = 0;
    conv.Val( conVal );
    hold->SetBitRate( conVal );
    //ownership transferred to the array
    //no need to delete hold
    iUrlArray->AppendL( hold );
    IRLOG_DEBUG( "CIRPresetImpl::SetUrlL - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::SetUrl()
// Function to set Url parameter
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::SetUrlL( const TDesC &aServerName,
                             const TDesC &aUrl,
                             TInt aBitrate )
    {
    IRLOG_DEBUG( "CIRPresetImpl::SetUrlL(const TDesC &aServerName,const TDesC &aUrl,TInt aBitrate)" );
	CIRChannelServerUrl* hold=CIRChannelServerUrl::NewL();
    hold->SetServerName( aServerName );
    hold->SetServerUrl( aUrl );
    hold->SetBitRate( aBitrate );
    iUrlArray->AppendL( hold );
    IRLOG_DEBUG( "CIRPresetImpl::SetUrlL(const TDesC &aServerName,const TDesC &aUrl,TInt aBitrate) - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::SetMusicStoreStatusL()
// Function to set iMusicStoreStatus
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::SetMusicStoreStatusL( const TDesC& aMusicStoreStatus )
    {
    IRLOG_DEBUG( "CIRPresetImpl::SetMusicStoreStatus - Entering" );
    iMusicStoreStatus.Close();
    iMusicStoreStatus.CreateL( aMusicStoreStatus );
    IRLOG_DEBUG( "CIRPresetImpl::SetMusicStoreStatus - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::GetId()
// Function to get iPresetId
// ---------------------------------------------------------------------------
//
TInt CIRPresetImpl::GetId() const
    {
    return iPresetId;
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::GetLangId()
// Function to get iLanguageCode
// ---------------------------------------------------------------------------
//
const TDesC& CIRPresetImpl::GetLangId() const
    {
    return iLanguageCode;
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::GetLangName()
// Function to get iLanguage
// ---------------------------------------------------------------------------
//
const TDesC& CIRPresetImpl::GetLangName() const
    {
    return iLanguage;
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::GetCountryId()
// Function to get iCountryCode
// ---------------------------------------------------------------------------
//
const TDesC& CIRPresetImpl::GetCountryId() const
    {
    return iCountryCode;
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::GetCountryName()
// Function to get iCountry
// ---------------------------------------------------------------------------
//
const TDesC& CIRPresetImpl::GetCountryName() const
    {
    return iCountry;
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::GetLastModifiedTime()
// Function to get iLastModified
// ---------------------------------------------------------------------------
//
const TDesC& CIRPresetImpl::GetLastModifiedTime() const
    {
    return iLastModified;
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::GetGenreId()
// Function to get iGenereId
// ---------------------------------------------------------------------------
//
const TDesC& CIRPresetImpl::GetGenreId() const
    {
    return iGenereId;
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::GetGenreName()
// Function to get iGenereName
// ---------------------------------------------------------------------------
//
const TDesC& CIRPresetImpl::GetGenreName() const
    {
    return iGenereName;
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::GetAdvertisementUrl()
// Function to get iAdvertisementUrl
// ---------------------------------------------------------------------------
//
const TDesC& CIRPresetImpl::GetAdvertisementUrl() const
	{
	return iAdvertisementUrl;
	}

const TDesC& CIRPresetImpl::GetAdvertisementInUse() const
{
    return iAdvertisementInUse;
}

// ---------------------------------------------------------------------------
// CIRPresetImpl::GetImgUrl()
// Function to get iImgUrl
// ---------------------------------------------------------------------------
//
const TDesC& CIRPresetImpl::GetImgUrl() const
    {
    return iImgUrl;
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::GetUrlCount()
// Function to get iChannelUrlCount
// ---------------------------------------------------------------------------
//
TInt CIRPresetImpl::GetUrlCount() const
    {
    return iChannelUrlCount;
    }

CIRChannelServerUrl& CIRPresetImpl::GetUrl(TInt aIndex) const
{
    ASSERT( aIndex >= 0 && aIndex < iUrlArray->Count() ); 
    return ((*iUrlArray->At(aIndex)));
}

/*
 * get the channel url at specified position
 *
 */
const TDesC& CIRPresetImpl::GetChannelUrlAt(TInt aIndex) const
{
    return iUrlArray->At(aIndex)->GetServerUrl();
}

// ---------------------------------------------------------------------------
// CIRPresetImpl::GetDescription()
// Function to get iDescription
// ---------------------------------------------------------------------------
//
const TDesC& CIRPresetImpl::GetDescription() const
    {
    return iDescription;
    }

// ---------------------------------------------------------------------------
//CIRPresetImpl::GetShortDescription()
//Function to get iDescription
// ---------------------------------------------------------------------------
//
const TDesC& CIRPresetImpl::GetShortDescription() const
    {
    return iShortDesc;
    }

// ---------------------------------------------------------------------------
//CIRPresetImpl::GetMusicStoreStatus()
//Function to get iMusicStoreStatus
// ---------------------------------------------------------------------------
//
const TDesC& CIRPresetImpl::GetMusicStoreStatus() const
    {
    return iMusicStoreStatus;
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::GetChannelType()
// Function to get iType
// ---------------------------------------------------------------------------
//
TInt CIRPresetImpl::GetChannelType() const
    {
    return iType;
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl& CIRPresetImpl::operator=(const CIRPresetImpl& preset)
// assignment operator eg. preset1=preset2
// shallow copy
// ---------------------------------------------------------------------------
//
CIRPresetImpl& CIRPresetImpl::operator=( const CIRPresetImpl& aPreset )
    {
    IRLOG_DEBUG( "CIRPresetImpl::operator=" );
    if ( &aPreset == this )
        {
        return *this;
        }
    //copy data members
    iPresetId = aPreset.iPresetId;

        TRAP_IGNORE(SetNameL(aPreset.Name()))

    iLanguageCode.Close();
    iLanguageCode.Create( aPreset.iLanguageCode );

    iLanguage.Close();
    iLanguage.Create( aPreset.iLanguage );

    iCountryCode.Close();
    iCountryCode.Create( aPreset.iCountryCode );

    iCountry.Close();
    iCountry.Create( aPreset.iCountry );

    iLastModified.Close();
    iLastModified.Create( aPreset.iLastModified );

    iAdvertisementUrl.Close();
    iAdvertisementUrl.Create( aPreset.iAdvertisementUrl );

    iAdvertisementInUse.Close();
    iAdvertisementInUse.Create( aPreset.iAdvertisementInUse );

    iImgUrl.Close();
    iImgUrl.Create( aPreset.iImgUrl );

    iLogoData.Close();
    iLogoData.Create( aPreset.iLogoData );

    iDescription.Close();
    iDescription.Create( aPreset.iDescription );

    iShortDesc.Close();
    iShortDesc.Create( aPreset.iShortDesc );

    iMusicStoreStatus.Close();
    iMusicStoreStatus.Create( aPreset.iMusicStoreStatus );

    iGenereName.Close();
    iGenereName.Create( aPreset.iGenereName );

    iGenereId.Close();
    iGenereId.Create( aPreset.iGenereId );

    iChannelUrlCount = aPreset.iChannelUrlCount;
    iType = aPreset.iType; //0 for user defined 1 for isds type.
    for ( TInt cnt = 0; cnt < iChannelUrlCount; cnt++ )
        {
        ASSERT( cnt >= 0 && cnt < aPreset.iUrlArray->Count() );
        TRAPD(errUrl,SetUrlL( aPreset.iUrlArray->At(cnt)->GetServerName(),
                              aPreset.iUrlArray->At(cnt)->GetServerUrl(),
                              aPreset.iUrlArray->At(cnt)->GetBitRate() ) )
        if ( errUrl != KErrNone )
            {
            return *this;
            }
        }
    IRLOG_DEBUG( "CIRPresetImpl::operator= - Exiting." );
    return *this;
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::CIRPresetImpl()
// default C++ constructor
// standard two phased constructor
// ---------------------------------------------------------------------------
CIRPresetImpl::CIRPresetImpl()
    {
    // no implementation
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::CIRPresetImpl()
// default C++ constructor
// standard two phased constructor
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::ExternalizeL( RWriteStream& aWriteStream ) const
    {
    IRLOG_DEBUG( "CIRPresetImpl::ExternalizeL" );
    //! externalize TInts s
    aWriteStream.WriteInt32L( iPresetId );
    aWriteStream.WriteInt32L( iType );
    aWriteStream.WriteInt32L( iChannelUrlCount );

    //Externalize HBufC s
    //algo
    //1.check if data is present for the particular member variable
    //2.if yes get the length of the data and externalize the length and then the data
    //3.if no data is present write the length as 0.
    TInt len = 0;
    if ( !iLanguageCode.Length() )
        {
        len = 0;
        aWriteStream.WriteInt32L( len );
        }
    else
        {
        len = iLanguageCode.Length();
        aWriteStream.WriteInt32L( len );
        aWriteStream << iLanguageCode;
        }

    if ( !iLanguage.Length() )
        {
        len = 0;
        aWriteStream.WriteInt32L( len );
        }
    else
        {
        len = iLanguage.Length();
        aWriteStream.WriteInt32L( len );
        aWriteStream << iLanguage;
        }

    if ( !iCountryCode.Length() )
        {
        len = 0;
        aWriteStream.WriteInt32L( len );
        }
    else
        {
        len = iCountryCode.Length();
        aWriteStream.WriteInt32L( len );
        aWriteStream << iCountryCode;
        }

    if ( !iCountry.Length() )
        {
        len = 0;
        aWriteStream.WriteInt32L( len );
        }
    else
        {
        len = iCountry.Length();
        aWriteStream.WriteInt32L( len );
        aWriteStream << iCountry;
        }

    if ( !iGenereName.Length() )
        {
        len = 0;
        aWriteStream.WriteInt32L( len );
        }
    else
        {
        len = iGenereName.Length();
        aWriteStream.WriteInt32L( len );
        aWriteStream << iGenereName;
        }

    if ( !iGenereId.Length() )
        {
        len = 0;
        aWriteStream.WriteInt32L( len );
        }
    else
        {
        len = iGenereId.Length();
        aWriteStream.WriteInt32L( len );
        aWriteStream << iGenereId;
        }

    if ( !iLastModified.Length() )
        {
        len = 0;
        aWriteStream.WriteInt32L( len );
        }
    else
        {
        len = iLastModified.Length();
        aWriteStream.WriteInt32L( len );
        aWriteStream << iLastModified;
        }

    if ( !iAdvertisementUrl.Length() )
        {
        len = 0;
        aWriteStream.WriteInt32L( len );
        }
    else
        {
        len = iAdvertisementUrl.Length();
        aWriteStream.WriteInt32L( len );
        aWriteStream << iAdvertisementUrl;
        }

    if ( !iAdvertisementInUse.Length() )
        {
        len = 0;
        aWriteStream.WriteInt32L( len );
        }
    else
        {
        len = iAdvertisementInUse.Length();
        aWriteStream.WriteInt32L( len );
        aWriteStream << iAdvertisementInUse;
        }

    if ( !iImgUrl.Length() )
        {
        len = 0;
        aWriteStream.WriteInt32L( len );
        }
    else
        {
        len = iImgUrl.Length();
        aWriteStream.WriteInt32L( len );
        aWriteStream << iImgUrl;
        }

    if ( !iDescription.Length() )
        {
        len = 0;
        aWriteStream.WriteInt32L( len );
        }
    else
        {
        len = iDescription.Length();
        aWriteStream.WriteInt32L( len );
        aWriteStream << iDescription;
        }

    if ( !iShortDesc.Length() )
        {
        len = 0;
        aWriteStream.WriteInt32L( len );
        }
    else
        {
        len = iShortDesc.Length();
        aWriteStream.WriteInt32L( len );
        aWriteStream << iShortDesc;
        }

    if ( !iMusicStoreStatus.Length() )
        {
        len = 0;
        aWriteStream.WriteInt32L( len );
        }
    else
        {
        len = iMusicStoreStatus.Length();
        aWriteStream.WriteInt32L( len );
        aWriteStream << iMusicStoreStatus;
        }

    if ( !iLogoData.Length() )
        {
        len = 0;
        aWriteStream.WriteInt32L( len );
        }
    else
        {
        len = iLogoData.Length();
        aWriteStream.WriteInt32L( len );
        aWriteStream << iLogoData;
        }

    //externalizing the preset data
    for ( TInt cnt = 0; cnt < iChannelUrlCount; cnt++ )
        {
        GetUrl( cnt ).ExternalizeL( aWriteStream );
        }

    //added by Peter on June 23rd, 2009
    aWriteStream.WriteInt32L( iPlayedTimes );

	aWriteStream.WriteInt32L(iRenamed);
    IRLOG_DEBUG( "CIRPresetImpl::ExternalizeL - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::CIRPresetImpl()
// Internalizes the preset data
// required for being derived from CPSPresetInterface
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::InternalizeL( RReadStream& aReadStream )
    {
    IRLOG_DEBUG( "CIRPresetImpl::InternalizeL" );
    //! Internalize TInts s
    iPresetId = aReadStream.ReadInt32L();
    iType = aReadStream.ReadInt32L();
    iChannelUrlCount = aReadStream.ReadInt32L();
    TInt len = 0;

    //algo
    //1.get the length of the data
    //2.read that many bytes
    //3.if read length is zero. skip.
    len = aReadStream.ReadInt32L();
    iLanguageCode.Close();
    if ( len != 0 )
        {
        iLanguageCode.CreateL( aReadStream, len );
        }

    len = aReadStream.ReadInt32L();
    iLanguage.Close();
    if ( len != 0 )
        {
        iLanguage.CreateL( aReadStream, len );
        }

    len = aReadStream.ReadInt32L();
    iCountryCode.Close();
    if ( len != 0 )
        {
        iCountryCode.CreateL( aReadStream, len );
        }

    len = aReadStream.ReadInt32L();
    iCountry.Close();
    if ( len != 0 )
        {
        iCountry.CreateL( aReadStream, len );
        }

    len = aReadStream.ReadInt32L();
    iGenereName.Close();
    if ( len != 0 )
        {
        iGenereName.CreateL( aReadStream, len );
        }

    len = aReadStream.ReadInt32L();
    iGenereId.Close();
    if ( len != 0 )
        {
        iGenereId.CreateL( aReadStream, len );
        }

    len = aReadStream.ReadInt32L();
    iLastModified.Close();
    if ( len != 0 )
        {
        iLastModified.CreateL( aReadStream, len );
        }

    len = aReadStream.ReadInt32L();
    iAdvertisementUrl.Close();
    if ( len != 0 )
        {
        iAdvertisementUrl.CreateL( aReadStream, len );
        }

    len = aReadStream.ReadInt32L();
    iAdvertisementInUse.Close();
    if ( len != 0 )
        {
        iAdvertisementInUse.CreateL( aReadStream, len );
        }

    len = aReadStream.ReadInt32L();
    iImgUrl.Close();
    if ( len != 0 )
        {
        iImgUrl.CreateL( aReadStream, len );
        }

    len = aReadStream.ReadInt32L();
    iDescription.Close();
    if ( len != 0 )
        {
        iDescription.CreateL( aReadStream, len );
        }

    len = aReadStream.ReadInt32L();
    iShortDesc.Close();
    if ( len != 0 )
        {
        iShortDesc.CreateL( aReadStream, len );
        }

    len = aReadStream.ReadInt32L();
    iMusicStoreStatus.Close();
    if ( len != 0 )
        {
        iMusicStoreStatus.CreateL( aReadStream, len );
        }

    len = aReadStream.ReadInt32L();
    iLogoData.Close();
    if ( len != 0 )
        {
        iLogoData.CreateL( aReadStream, len );
        }

	//internalizing the preset data
	CIRChannelServerUrl *url = NULL;
	for(TInt cnt=0;cnt<iChannelUrlCount;cnt++)
		{
		url = CIRChannelServerUrl::NewLC();
		url->InternalizeL(aReadStream);
		iUrlArray->AppendL(url);
	    CleanupStack::Pop(url);
		}

    iPlayedTimes = aReadStream.ReadInt32L();
	iRenamed = aReadStream.ReadInt32L();
    IRLOG_DEBUG( "CIRPresetImpl::InternalizeL - Exiting." );
    }

// ---------------------------------------------------------------------------
// void CIRPresetImpl::CIRIsdsPresetToCIRPresetImplL(const CIRIsdsPreset& aIsdsPreset)
// used to convert the isdspresets to plugnized presets.
// Does a deep copy for all the member variables
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::CIRIsdsPresetToCIRPresetImplL( const CIRIsdsPreset& aIsdsPreset )
    {
    IRLOG_DEBUG( "CIRPresetImpl::CIRIsdsPresetToCIRPresetImpl" );
    iPresetId = aIsdsPreset.GetId();
    iChannelUrlCount = aIsdsPreset.GetUrlCount();
    iType = aIsdsPreset.GetChannelType(); //0 for user defined 1 for isds type.

    TRAP_IGNORE( SetNameL( aIsdsPreset.GetName() ) )
    SetLangCodeL( aIsdsPreset.GetLangId() );

    SetLogoDataL( aIsdsPreset.GetLogoData() );
    SetLangL( aIsdsPreset.GetLangName() );
    SetCountryCodeL( aIsdsPreset.GetCountryId() );
    SetCountryNameL( aIsdsPreset.GetCountryName() );
    SetLastModifiedTimeL( aIsdsPreset.GetLastModifiedTime() );
    SetAdvertisementUrlL( aIsdsPreset.GetAdvertisementUrl() );
    SetAdvertisementInUseL( aIsdsPreset.GetAdvertisementInUse() );
    SetImgUrlL( aIsdsPreset.GetImgUrl() );
    SetDescriptionL( aIsdsPreset.GetDescription() );
    SetShortDescL( aIsdsPreset.GetShortDescription() );
    SetMusicStoreStatusL( aIsdsPreset.GetMusicStoreStatus() );
    SetGenreInfoL( aIsdsPreset.GetGenreId(), aIsdsPreset.GetGenreName() );


    iUrlArray->ResetAndDestroy();

    for ( TInt cnt = 0; cnt < aIsdsPreset.GetUrlCount(); cnt++ )
        {
        ASSERT( cnt >= 0 && cnt < aIsdsPreset.iUrlArray->Count() );
        TRAPD(errUrl,SetUrlL( aIsdsPreset.iUrlArray->At(cnt)->GetServerName(),
                              aIsdsPreset.iUrlArray->At(cnt)->GetServerUrl(),
                              aIsdsPreset.iUrlArray->At(cnt)->GetBitRate() ) )
        if ( errUrl != KErrNone )
            {
            return;
            }
        }
    IRLOG_DEBUG( "CIRPresetImpl::CIRIsdsPresetToCIRPresetImpl - Exiting." );
    }
/*
 // ---------------------------------------------------------------------------
 // void CIRPresetImpl::UserDefinedPresetToCIRPresetImpl
 // used to convert the userdefined preset to plugnized presets.
 // Does a deep copy for all the member variables
 // ---------------------------------------------------------------------------
 //
 void CIRPresetImpl::UserDefinedPresetToCIRPresetImpl(const TDesC& aName,
 const TDesC& aURl,TInt aId,TInt aIndex)
 {
 IRLOG_DEBUG( "CIRPresetImpl::UserDefinedPresetToCIRPresetImpl" );
 iPresetId=aId;
 TRAP_IGNORE(SetNameL(aName))
 SetChannelType(0);//type set to user defined
 //done to match the API,new API should be exposed by DS to accept only Url
 _LIT(KServerName,"NA");
 //copy the url
 TRAPD(errUrl,SetUrlL(KServerName,aURl,0))
 if(errUrl!=KErrNone)
 {
 IRLOG_ERROR2( "CIRPresetImpl::UserDefinedPresetToCIRPresetImpl - SetUrlL left with %d.", errUrl );
 return ; // WHATTHEHECK
 }
 // manually added channels have only one channel url
 SetUrlCount(1);
 IRLOG_DEBUG( "CIRPresetImpl::UserDefinedPresetToCIRPresetImpl - Exiting." );
 }
 */

// ---------------------------------------------------------------------------
//CIRPresetImpl::CopyPresetDataL()
// copy data to CIRIsdsPreset
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::CopyPresetDataL( CIRIsdsPreset& aIsdsPreset ) const
    {
    IRLOG_DEBUG( "CIRPresetImpl::CopyPresetDataL" );
    aIsdsPreset.SetId( iPresetId );
    aIsdsPreset.SetUrlCount( iChannelUrlCount );
    aIsdsPreset.SetChannelType( iType ); //0 for user defined 1 for isds type.
    //algo
    //1.check if the CIRIsdsPreset,member contains any data
    //2.if yes delete it and set it to null
    //3.allocate memory and copy data
    aIsdsPreset.SetGenreNameL( iGenereName );
    aIsdsPreset.SetGenreIdL( iGenereId );
    aIsdsPreset.SetLangCodeL( iLanguageCode );

    aIsdsPreset.SetLogoDataL( iLogoData );
    aIsdsPreset.SetLangL( iLanguage );
    aIsdsPreset.SetCountryCodeL( iCountryCode );
    aIsdsPreset.SetCountryNameL( iCountry );
    aIsdsPreset.SetNameL( Name() );
    aIsdsPreset.SetLastModifiedTimeL( iLastModified );
    aIsdsPreset.SetAdvertisementUrlL( iAdvertisementUrl );
    aIsdsPreset.SetAdvertisementInUseL( iAdvertisementInUse );
    aIsdsPreset.SetImgUrlL( iImgUrl );
    aIsdsPreset.SetDescriptionL( iDescription );
    aIsdsPreset.SetShortDescL( iShortDesc );
    aIsdsPreset.SetMusicStoreStatusL( iMusicStoreStatus );

    aIsdsPreset.iUrlArray->ResetAndDestroy();

    for ( TInt cnt = 0; cnt < iChannelUrlCount; cnt++ )
        {
        ASSERT( cnt >= 0 && cnt < iUrlArray->Count() );
        TRAPD(errUrl,aIsdsPreset.SetUrlL( iUrlArray->At(cnt)->GetServerName(),
                                          iUrlArray->At(cnt)->GetServerUrl(),
                                          iUrlArray->At(cnt)->GetBitRate() ) )
        if ( errUrl != KErrNone )
            {
            return;
            }
        }
    IRLOG_DEBUG( "CIRPresetImpl::CopyPresetDataL - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::GetAvailableBitrates()
// Function to get available bitrates
// ---------------------------------------------------------------------------
//
RArray<TInt>& CIRPresetImpl::GetAvailableBitrates()
    {
    IRLOG_DEBUG( "CIRPresetImpl::GetAvailableBitrates" );
    iBitrateArray.Reset();
    //creates an array of bitrates(int)that are available for a particular preset
    for ( TInt loop = 0; loop < iUrlArray->Count(); loop++ )
        {
        iBitrateArray.InsertInOrder( iUrlArray->At( loop )->GetBitRate() );
        }
    IRLOG_DEBUG( "CIRPresetImpl::GetAvailableBitrates - Exiting." );
    return iBitrateArray;
    }


// ---------------------------------------------------------------------------
// CIRPresetImpl::GetPlayedTimes()
// Returns the played times of the channel
// ---------------------------------------------------------------------------
//
TInt CIRPresetImpl::GetPlayedTimes() const
    {
    return iPlayedTimes;
    }

// ---------------------------------------------------------------------------
// Set the played times of the channel
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::SetPlayedTimes( TInt aPlayedTimes )
    {
    iPlayedTimes = aPlayedTimes;
    }
    
// ---------------------------------------------------------------------------
// CIRPresetImpl::GetRenamed()
// 
// ---------------------------------------------------------------------------
//
TBool CIRPresetImpl::GetRenamed() const
    {
    return iRenamed;
    }

// ---------------------------------------------------------------------------
// CIRPresetImpl::SetRenamed()
// 
// ---------------------------------------------------------------------------
//
void CIRPresetImpl::SetRenamed()
    {
    iRenamed = ETrue;
    }
