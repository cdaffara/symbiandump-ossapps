/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description: iSDS preset structure.
 *
 */

#include <s32strm.h>

#include "irchannelserverurl.h"
#include "irdebug.h"
#include "irisdspreset.h"

const TInt KUrlArrayMaxLength = 128;
const TInt KUrlForBitrateMaxLength = 128;

_LIT( KComma, "," );

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CIRIsdsPreset::NewL()
// Static function
// standard two phased constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CIRIsdsPreset* CIRIsdsPreset::NewL()
	{
	IRLOG_DEBUG( "CIRIsdsPreset::NewL - Entering" );
	CIRIsdsPreset* self = CIRIsdsPreset::NewLC();
	CleanupStack::Pop( self );
	IRLOG_DEBUG( "CIRIsdsPreset::NewL - Exiting." );
	return self;
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::NewLC()
// Static function
// standard two phased constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CIRIsdsPreset* CIRIsdsPreset::NewLC()
	{
	IRLOG_DEBUG( "CIRIsdsPreset::NewLC - Entering" );
	CIRIsdsPreset* self = new( ELeave )CIRIsdsPreset;
	CleanupStack::PushL( self );
	self->ConstructL();
	IRLOG_DEBUG( "CIRIsdsPreset::NewLC - Exiting." );
	return self;
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::~CIRIsdsPreset()
// standard C++ destructor
// ---------------------------------------------------------------------------
//
CIRIsdsPreset::~CIRIsdsPreset()
	{
	IRLOG_DEBUG( "CIRIsdsPreset::~CIRIsdsPreset - Entering" );
	iName.Close();
	iLanguageCode.Close();
	iLanguage.Close();
	iCountryCode.Close();
	iCountry.Close();
	iLastModified.Close();
	iMusicStoreStatus.Close();
	iAdvertisementUrl.Close();
	iAdvertisementInUse.Close();
	iImgUrl.Close();
	iLogoData.Close();
	iDescription.Close();
	iShortDesc.Close();
	iGenereName.Close();
	iGenereId.Close();

	if ( iUrlArray )
		{
		iUrlArray->ResetAndDestroy();
		}
	
	delete iUrlArray;
	IRLOG_DEBUG( "CIRIsdsPreset::~CIRIsdsPreset - Exiting." );
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::CIRIsdsPreset()
// default C++ constructor
// standard two phased constructor
// ---------------------------------------------------------------------------
//
CIRIsdsPreset::CIRIsdsPreset()
	{
	IRLOG_DEBUG( "CIRIsdsPreset::CIRIsdsPreset" );
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::ConstructL()
// standard second phase constructor
// ---------------------------------------------------------------------------
//
void CIRIsdsPreset::ConstructL()
	{
	IRLOG_DEBUG( "CIRIsdsPreset::ConstructL - Entering" );
	iUrlArray = new( ELeave )CArrayPtrFlat<CIRChannelServerUrl>( 
		KUrlArrayMaxLength );
	IRLOG_DEBUG( "CIRIsdsPreset::ConstructL - Exiting." );
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::SetId()
// Function to set iPresetId from a TDesC
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsPreset::SetId( const TDesC& aId )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::SetId - Entering" );
	TLex conv( aId );
	conv.Val( iPresetId );
	IRLOG_DEBUG( "CIRIsdsPreset::SetId - Exiting." );
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::SetId()
// Function to set iPresetId
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsPreset::SetId( TInt aId )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::SetId(TInt) - Entering" );
	iPresetId = aId;
	IRLOG_DEBUG( "CIRIsdsPreset::SetId(TInt) - Exiting." );
	}



// ---------------------------------------------------------------------------
//CIRIsdsPreset::SetNameL()
//Function to set iName from a TDesC
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsPreset::SetNameL( const TDesC& aName )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::SetNameL - Entering" );
	iName.Close();
	iName.CreateL( aName );
	IRLOG_DEBUG( "CIRIsdsPreset::SetNameL - Exiting." );
	}


// ---------------------------------------------------------------------------
//CIRIsdsPreset::SetLangInfoL()
//Function to set iLanguageCode, iLanguage
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsPreset::SetLangInfoL( const TDesC& aLangId,
	const TDesC& aLangName )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::SetLangInfoL - Entering" );
    SetLangCodeL( aLangId );
    SetLangL( aLangName );
	IRLOG_DEBUG( "CIRIsdsPreset::SetLangInfoL - Exiting." );
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::SetLangCodeL()
// Function to set iLanguageCode from a TDesC
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsPreset::SetLangCodeL( const TDesC& aLangId )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::SetLangCodeL - Entering" );
	iLanguageCode.Close();
	iLanguageCode.CreateL( aLangId );
	IRLOG_DEBUG( "CIRIsdsPreset::SetLangCodeL - Exiting." );
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::SetLangL()
// Function to set iLanguage from a TDesC
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsPreset::SetLangL( const TDesC& aLangName )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::SetLangL - Entering" );
	iLanguage.Close();
	iLanguage.CreateL( aLangName );
	IRLOG_DEBUG( "CIRIsdsPreset::SetLangL - Exiting." );
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::SetCountryCodeL()
// Function to set iCountryCode from a TDesC
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsPreset::SetCountryCodeL( const TDesC& aCountryId )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::SetCountryCodeL - Entering" );
	iCountryCode.Close();
	iCountryCode.CreateL( aCountryId );
	IRLOG_DEBUG( "CIRIsdsPreset::SetCountryCodeL - Exiting." );
	}


// ---------------------------------------------------------------------------
//CIRIsdsPreset::SetCountryNameL()
//Function to set iCountry from a TDesC
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsPreset::SetCountryNameL( const TDesC& aCountryName )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::SetCountryNameL - Entering" );
	iCountry.Close();
	iCountry.CreateL( aCountryName );
	IRLOG_DEBUG( "CIRIsdsPreset::SetCountryNameL - Exiting." );
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::SetLangInfoL()
// Function to set iCountryCode, iCountry
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsPreset::SetCountryInfoL( const TDesC& aCountryId,
	const TDesC& aCountryName )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::SetCountryInfoL - Entering" );
	SetCountryCodeL( aCountryId );
    SetCountryNameL( aCountryName );
	IRLOG_DEBUG( "CIRIsdsPreset::SetCountryInfoL - Exiting." );
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::SetLastModifiedTimeL()
// Function to set iLastModified
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsPreset::SetLastModifiedTimeL( const TDesC& aLastModified )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::SetLastModifiedTimeL - Entering" );
	iLastModified.Close();
	iLastModified.CreateL( aLastModified );
	IRLOG_DEBUG( "CIRIsdsPreset::SetLastModifiedTimeL - Exiting." );
	}


// ---------------------------------------------------------------------------
//CIRIsdsPreset::SetGenreInfoL()
//Function to set iGenereName, iGenereId
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsPreset::SetGenreInfoL( const TDesC& aGenreId,
	const TDesC& aGenreName )
	{
	IRLOG_DEBUG( "CIRIsdsPresetL::SetGenreInfoL - Entering" );

	if ( iGenereName.Length() )
		{
		RBuf newBuf;
		newBuf.CreateL(
		    iGenereName.Length() + aGenreName.Length() + KComma().Length() );
		newBuf.Append( iGenereName );
		newBuf.Append( KComma );
		newBuf.Append( aGenreName );
		iGenereName.Close();
		iGenereName.CreateL( newBuf );
		newBuf.Close();
		}
	
	if ( iGenereId.Length() )
		{
		RBuf newBuf;
        newBuf.CreateL(
            iGenereId.Length() + aGenreId.Length() + KComma().Length() );
        newBuf.Append( iGenereId );
        newBuf.Append( KComma );
        newBuf.Append( aGenreId );
        iGenereId.Close();
        iGenereId.CreateL( newBuf );
        newBuf.Close();
		}
	
	if ( !iGenereName.Length() )
		{
		iGenereName.Close();
		iGenereName.CreateL( aGenreName );
		}
	
	if ( !iGenereId.Length() )
		{
		iGenereId.Close();
		iGenereId.CreateL( aGenreId );
		}

	IRLOG_DEBUG( "CIRIsdsPreset::SetGenreInfoL - Exiting." );
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::SetImgUrlL()
// Function to set iImgUrl
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsPreset::SetImgUrlL( const TDesC& aImgUrl )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::SetImgUrlL - Entering" );
	iImgUrl.Close();
	iImgUrl.CreateL( aImgUrl );
	IRLOG_DEBUG( "CIRIsdsPreset::SetImgUrlL - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRIsdsPreset::SetLogoDataL()
// Function to set LogoData
// ---------------------------------------------------------------------------
//

EXPORT_C void CIRIsdsPreset::SetLogoDataL( const TDesC8& aData )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::SetLogoDataL - Entering" );

	iLogoData.Close();
	iLogoData.CreateL( aData );

	IRLOG_DEBUG( "CIRIsdsPreset::SetLogoDataL - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRIsdsPreset::SetAdvertisementInUseL()
// Function to set iAdvertisementInUse
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsPreset::SetAdvertisementInUseL( 
	const TDesC& aAdvertisementInUse )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::SetAdvertisementInUseL - Entering" );
	iAdvertisementInUse.Close();
	iAdvertisementInUse.CreateL( aAdvertisementInUse );
	IRLOG_DEBUG( "CIRIsdsPreset::SetAdvertisementInUseL - Exiting." );
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::SetDescriptionL()
// Function to set iDescription
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsPreset:: SetDescriptionL( const TDesC& aDescription )
	{
    IRLOG_DEBUG( "CIRIsdsPreset::SetDescriptionL - Entering" );
    iDescription.Close();
    iDescription.CreateL( aDescription );
	IRLOG_DEBUG( "CIRIsdsPreset::SetDescriptionL - Exiting." );
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::SetAdvertisementUrlL()
// Function to set iAdvertisementUrl
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsPreset::SetAdvertisementUrlL( 
	const TDesC& aAdvertisementUrl )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::SetAdvertisementUrlL - Entering" );
	iAdvertisementUrl.Close();
	iAdvertisementUrl.CreateL( aAdvertisementUrl );
	IRLOG_DEBUG( "CIRIsdsPreset::SetAdvertisementUrlL - Exiting." );
	}


// ---------------------------------------------------------------------------
//CIRIsdsPreset::SetUrlCount()
//Function to set iChannelUrlCount
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsPreset::SetUrlCount()
	{
	IRLOG_DEBUG( "CIRIsdsPreset::SetUrlCount - Entering" );
	iChannelUrlCount = iUrlArray->Count();
	IRLOG_DEBUG( "CIRIsdsPreset::SetUrlCount - Exiting." );
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::SetShortDescL()
// Function to set iShortDesc
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsPreset::SetShortDescL( const TDesC& aShortDesc )
	{
    IRLOG_DEBUG( "CIRIsdsPreset::SetShortDescL - Entering" );
    iShortDesc.Close();
    iShortDesc.CreateL( aShortDesc );
	IRLOG_DEBUG( "CIRIsdsPreset::SetShortDescL - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRIsdsPreset::SetGenreNameL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsPreset::SetGenreNameL( const TDesC& aGenreName )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::SetGenreNameL - Entering" );
	iGenereName.Close();
	iGenereName.CreateL( aGenreName );
	IRLOG_DEBUG( "CIRIsdsPreset::SetGenreNameL - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRIsdsPreset::SetGenreIdL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsPreset::SetGenreIdL( const TDesC& aGenreId )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::SetGenreIdL - Entering" );
	iGenereId.Close();
	iGenereId.CreateL( aGenreId );
	IRLOG_DEBUG( "CIRIsdsPreset::SetGenreIdL - Exiting" );
	}

// ---------------------------------------------------------------------------
//CIRIsdsPreset::SetUrlCount()
//Function to set iChannelUrlCount
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsPreset::SetUrlCount( TInt aCnt )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::SetUrlCount - Entering" );
	iChannelUrlCount = aCnt;
	IRLOG_DEBUG( "CIRIsdsPreset::SetUrlCount - Exiting." );
	}

// ---------------------------------------------------------------------------
//CIRIsdsPreset::SetUrl()
//Function to set Url parameter
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsPreset::SetUrlL( const TDesC& aServerName,
	const TDesC& aUrl, const TDesC& aBitrate )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::SetUrlL(..., const TDesC) - Entering" );
	CIRChannelServerUrl * hold = CIRChannelServerUrl::NewL();
	hold->SetServerName( aServerName );
	hold->SetServerUrl( aUrl );
	TLex conv( aBitrate );
	TInt conVal = 0;
	conv.Val( conVal );
	//if a negative value comes,set the bitrate to 0,so that it is accessable
	//to all the setting combination
	if ( conVal < 0)
		{
		conVal = 0;
		}
	
	hold->SetBitRate( conVal );
	iUrlArray->AppendL( hold );
	IRLOG_DEBUG( "CIRIsdsPreset::SetUrlL(..., const TDesC) - Exiting." );
	}


// ---------------------------------------------------------------------------
//CIRIsdsPreset::SetUrl()
//Function to set Url parameter
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsPreset::SetUrlL( const TDesC& aServerName, 
	const TDesC& aUrl, TInt aBitrate )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::SetUrlL(..., TInt) - Entering" );
	CIRChannelServerUrl * hold = CIRChannelServerUrl::NewL();
	hold->SetServerName( aServerName );
	hold->SetServerUrl( aUrl );

	//if a negative value comes,set the bitrate to 0,so that it is accessable
	//to all the setting combination
	if ( aBitrate < 0)
		{
		aBitrate = 0;
		}
	
	hold->SetBitRate( aBitrate );
	iUrlArray->AppendL( hold );
	IRLOG_DEBUG( "CIRIsdsPreset::SetUrlL(..., TInt) - Exiting." );
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::SetChannelType()
// Function to set iType
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsPreset::SetChannelType( TInt aType )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::SetChannelType - Entering" );

	if ( aType != EUserDefined && aType != EIsdsPreset && aType != EAdhoc )
		{
		aType = EUserDefined;
		}
	
	iType = aType;
	IRLOG_DEBUG( "CIRIsdsPreset::SetChannelType - Exiting." );
	}

// ---------------------------------------------------------------------------
//CIRIsdsPreset::SetMusicStoreStatusL()
//Function to set iMusicStoreStatus parameter
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsPreset::SetMusicStoreStatusL( const TDesC& aMusicStoreStatus )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::SetMusicStoreStatusL - Entering" );
	iMusicStoreStatus.Close();
	iMusicStoreStatus.CreateL( aMusicStoreStatus );	
	IRLOG_DEBUG( "CIRIsdsPreset::SetMusicStoreStatusL - Exiting" );
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::ExternalizeL()
// externalizes the data
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsPreset::ExternalizeL( RWriteStream& aWriteStream )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::ExternalizeL - Entering" );
//! externalize TInts s
	aWriteStream.WriteInt32L(iPresetId);
	aWriteStream.WriteInt32L(iType);
	aWriteStream.WriteInt32L(iChannelUrlCount);

	// Externalize RBufs
    aWriteStream << iLanguageCode;
    aWriteStream << iLanguage;
    aWriteStream << iCountryCode;
    aWriteStream << iCountry;
    aWriteStream << iGenereName;
    aWriteStream << iGenereId;
    aWriteStream << iName;
    aWriteStream << iLastModified;
    aWriteStream << iAdvertisementUrl;
    aWriteStream << iAdvertisementInUse;
    aWriteStream << iImgUrl;
    aWriteStream << iDescription;
    aWriteStream << iShortDesc;
	aWriteStream << iMusicStoreStatus;
	aWriteStream << iLogoData;

	for ( TInt cnt=0; cnt < iChannelUrlCount; ++cnt )
		{
		GetUrl( cnt ).ExternalizeL( aWriteStream );
		}
	
	IRLOG_DEBUG( "CIRIsdsPreset::ExternalizeL - Exiting." );
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::InternalizeL()
// internalizes the data
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRIsdsPreset::InternalizeL( RReadStream& aReadStream )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::InternalizeL - Entering" );
	//! Internalize TInts s
	iPresetId=aReadStream.ReadInt32L();
	iType=aReadStream.ReadInt32L();
	iChannelUrlCount=aReadStream.ReadInt32L();

	// Internalize RBufs
	iLanguageCode.Close();
	iLanguageCode.CreateL( aReadStream, KMaxTInt );

	iLanguage.Close();
	iLanguage.CreateL( aReadStream, KMaxTInt );

	iCountryCode.Close();
	iCountryCode.CreateL( aReadStream, KMaxTInt );

	iCountry.Close();
	iCountry.CreateL( aReadStream, KMaxTInt );

	iGenereName.Close();
	iGenereName.CreateL( aReadStream, KMaxTInt );

	iGenereId.Close();
	iGenereId.CreateL( aReadStream, KMaxTInt );

	iName.Close();
	iName.CreateL( aReadStream, KMaxTInt );

	iLastModified.Close();
	iLastModified.CreateL( aReadStream, KMaxTInt );

	iAdvertisementUrl.Close();
	iAdvertisementUrl.CreateL( aReadStream, KMaxTInt );

	iAdvertisementInUse.Close();
	iAdvertisementInUse.CreateL( aReadStream, KMaxTInt );

	iImgUrl.Close();
	iImgUrl.CreateL( aReadStream, KMaxTInt );

	iDescription.Close();
	iDescription.CreateL( aReadStream, KMaxTInt );

	iShortDesc.Close();
	iShortDesc.CreateL( aReadStream, KMaxTInt );

	iMusicStoreStatus.Close();
	iMusicStoreStatus.CreateL( aReadStream, KMaxTInt );


    iUrlArray->ResetAndDestroy();

	iLogoData.Close();
	iLogoData.CreateL(aReadStream, KMaxTInt);
		
	CIRChannelServerUrl* url = NULL;
	for ( TInt cnt = 0; cnt < iChannelUrlCount; ++cnt )
		{
		url = CIRChannelServerUrl::NewLC();
		url->InternalizeL( aReadStream );
		iUrlArray->AppendL( url );
		CleanupStack::Pop( url );
		}
	
	IRLOG_DEBUG( "CIRIsdsPreset::InternalizeL - Exiting." );
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::GetId()
// Function to get iPresetId
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIRIsdsPreset::GetId() const
	{
	IRLOG_DEBUG( "CIRIsdsPreset::GetId" );
	return iPresetId;
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::GetId()
// Function to get iName
// ---------------------------------------------------------------------------
//
EXPORT_C  const TDesC& CIRIsdsPreset::GetName() const 
	{
	IRLOG_DEBUG( "CIRIsdsPreset::GetName" );
	return iName;
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::GetLangId()
// Function to get iLanguageCode
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIRIsdsPreset::GetLangId() const
	{
	IRLOG_DEBUG( "CIRIsdsPreset::GetLangId" );
	return iLanguageCode;
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::GetLangName()
// Function to get iLanguage
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIRIsdsPreset::GetLangName() const
	{
	IRLOG_DEBUG( "CIRIsdsPreset::GetLangName" );
	return iLanguage;
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::GetCountryId()
// Function to get iCountryCode
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIRIsdsPreset::GetCountryId() const
	{
	IRLOG_DEBUG( "CIRIsdsPreset::GetCountryId" );
	return iCountryCode;
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::GetCountryName()
// Function to get iCountry
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIRIsdsPreset::GetCountryName() const
	{
	IRLOG_DEBUG( "CIRIsdsPreset::GetCountryName" );
	return iCountry;
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::GetLastModifiedTime()
// Function to get iLastModified
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIRIsdsPreset::GetLastModifiedTime() const 
	{
	IRLOG_DEBUG( "CIRIsdsPreset::GetLastModifiedTime" );
	return iLastModified;
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::GetDescription()
// Function to get iDescription
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIRIsdsPreset::GetDescription() const
	{
	IRLOG_DEBUG( "CIRIsdsPreset::GetDescription" );
	return iDescription;
	}

// ---------------------------------------------------------------------------
// CIRIsdsPreset::GetShortDescription()
// Function to get iDescription
// ---------------------------------------------------------------------------
//
EXPORT_C  const TDesC& CIRIsdsPreset::GetShortDescription() const 
	{
	IRLOG_DEBUG( "CIRIsdsPreset::GetShortDescription" );
	return iShortDesc;
	}

// ---------------------------------------------------------------------------
// CIRIsdsPreset::GetChannelType()
// Function to get iType
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIRIsdsPreset::GetChannelType() const
	{
	IRLOG_DEBUG( "CIRIsdsPreset::GetChannelType" );
	return iType;
	}

// ---------------------------------------------------------------------------
//CIRIsdsPreset::GetGenreId()
//Function to get iGenereId
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIRIsdsPreset::GetGenreId() const
	{
	IRLOG_DEBUG( "CIRIsdsPreset::GetGenreId" );
	return iGenereId;
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::GetGenreName()
// Function to get iGenereName
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIRIsdsPreset::GetGenreName() const
	{
	IRLOG_DEBUG( "CIRIsdsPreset::GetGenreName" );
	return iGenereName;
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::GetAdvertisementUrl()
// Function to get iAdvertisementUrl
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIRIsdsPreset::GetAdvertisementUrl() const
	{
	IRLOG_DEBUG( "CIRIsdsPreset::GetAdvertisementUrl" );
	return iAdvertisementUrl;
	}

// ---------------------------------------------------------------------------
// CIRIsdsPreset::GetAdvertisementInUse()
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIRIsdsPreset::GetAdvertisementInUse() const
	{
	IRLOG_DEBUG( "CIRIsdsPreset::GetAdvertisementInUse" );
	return iAdvertisementInUse;
	}

// ---------------------------------------------------------------------------
// CIRIsdsPreset::GetImgUrl()
// Function to get iImgUrl
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIRIsdsPreset::GetImgUrl() const
	{
	IRLOG_DEBUG( "CIRIsdsPreset::GetImgUrl" );
	return iImgUrl;
	}

// ---------------------------------------------------------------------------
// CIRIsdsPreset::GetUrlCount()
// Function to get iChannelUrlCount
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIRIsdsPreset::GetUrlCount() const
	{
	IRLOG_DEBUG( "CIRIsdsPreset::GetUrlCount" );
	return iChannelUrlCount;
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::GetUrl()
// Function to get iLanguageCode
// ---------------------------------------------------------------------------
//
EXPORT_C CIRChannelServerUrl& CIRIsdsPreset::GetUrl( TInt aInt )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::GetUrl" );
	ASSERT( aInt >= 0 && aInt < iUrlArray->Count() ); 
	return ( ( *iUrlArray->At( aInt ) ) );
	}


EXPORT_C  const TDesC& CIRIsdsPreset::GetMusicStoreStatus() const 
	{
	IRLOG_DEBUG( "CIRIsdsPreset::GetMusicStoreStatus" );
	return iMusicStoreStatus;	
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::GetLogoData()
// Function to get LogoData
// ---------------------------------------------------------------------------
//
EXPORT_C  const TDesC8& CIRIsdsPreset::GetLogoData() const 
	{
	IRLOG_DEBUG( "CIRIsdsPreset::GetLogoData" );
	return iLogoData;
	}


// ---------------------------------------------------------------------------
//CIRIsdsPreset::=()
//standard C++ copy constructor(deep copy)
// ---------------------------------------------------------------------------
//
EXPORT_C CIRIsdsPreset& CIRIsdsPreset::operator=( const CIRIsdsPreset& aPreset )
	{
    IRLOG_DEBUG( "CIRIsdsPreset::operator=  - Entering" );

    if ( &aPreset == this)
		{
		return *this;
		}

    iName.Close();
	iName.Create( aPreset.iName );

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


	iUrlArray->ResetAndDestroy();
		
	for(TInt cnt=0;cnt<aPreset.iChannelUrlCount;cnt++)
		{
		ASSERT( cnt >= 0 && cnt < aPreset.iUrlArray->Count() );
		TRAPD( errUrl, SetUrlL( aPreset.iUrlArray->At( cnt )->GetServerName(), 
		    aPreset.iUrlArray->At( cnt )->GetServerUrl(), 
		    aPreset.iUrlArray->At( cnt )->GetBitRate()) )
		
		if ( errUrl != KErrNone )
			{
			return *this ;
			}
		}
	iChannelUrlCount=aPreset.iChannelUrlCount;
	iType=aPreset.iType;  //0 for user defined 1 for isds type.
	iPresetId = aPreset.iPresetId;
	IRLOG_DEBUG( "CIRIsdsPreset::operator= - Exiting." );
 	return *this;
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::GetAvailableBitrates()
// Function to get available bitrates
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIRIsdsPreset::GetAvailableBitrates( RArray<TInt>& aBitrates ) const
	{
	IRLOG_DEBUG( "CIRIsdsPreset::GetAvailableBitrates - Entering." );
	aBitrates.Reset();
	TInt err = KErrNone;
	for ( TInt loop = 0; loop < iUrlArray->Count() && !err; ++loop )
		{
		err = aBitrates.InsertInOrder( iUrlArray->At( loop )->GetBitRate() );
		if ( err == KErrAlreadyExists )
		    {
		    // Do not allow duplicates, but strip them away.
		    err = KErrNone;
		    }
		}

    if ( err )
        {
        aBitrates.Reset();
        }
    
	IRLOG_DEBUG( "CIRIsdsPreset::GetAvailableBitrates - Exiting." );
	return err;
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::GetUrlForBitrateL()
// Function to get Url for a given bitrate
// ---------------------------------------------------------------------------
//
EXPORT_C CDesCArrayFlat* CIRIsdsPreset::GetUrlForBitrateL( TInt aBitrate )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::GetUrlForBitrateL - Entering." );
	CDesCArrayFlat* urlForBitrate = new( ELeave )CDesCArrayFlat( KUrlForBitrateMaxLength );
	CleanupStack::PushL( urlForBitrate );
	iBitrate = aBitrate;
	SortInOrderL();
	//appending the urls with bit rate less than or
	//equal to given bit rate limit
	for ( TInt loop = 0; loop < iUrlArray->Count(); ++loop )
		{
		if ( aBitrate >= iUrlArray->At( loop )->GetBitRate())
			{
			urlForBitrate->AppendL( iUrlArray->At( loop )->GetServerUrl() );
			}
		}
	
	//appending	the url	with bit rate greater than given
	//bit rate limit
	for ( TInt loop = ( iUrlArray->Count()-1 ); loop >= 0; --loop)
		{
		if ( aBitrate<iUrlArray->At( loop )->GetBitRate() )
			{
			urlForBitrate->AppendL( iUrlArray->At( loop )->GetServerUrl() );
			}
		}
	if ( urlForBitrate->MdcaCount()==0 && iUrlArray->Count() > 0 )
		{
		urlForBitrate->AppendL( iUrlArray->At( 0 )->GetServerUrl() );
		}

    CleanupStack::Pop( urlForBitrate );
	IRLOG_DEBUG( "CIRIsdsPreset::GetUrlForBitrateL - Exiting." );
	return urlForBitrate;
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::GetExactUrlForBitrateL()
// Function to get Urls exactly matching a given bitrate for display purpose
// ---------------------------------------------------------------------------
//
EXPORT_C CDesCArrayFlat* CIRIsdsPreset::GetExactUrlForBitrateL( TInt aBitrate )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::GetUrlForBitrateL - Entering." );
	CDesCArrayFlat* urlForBitrate = 
	    new( ELeave )CDesCArrayFlat( KUrlForBitrateMaxLength );
	
	CleanupStack::PushL(urlForBitrate);

	SortInOrderL();
	//appending the urls with bit rate less than or
	//equal to given bit rate limit
	for ( TInt loop = 0; loop < iUrlArray->Count(); ++loop )
		{
		if ( aBitrate == iUrlArray->At( loop )->GetBitRate() )
			{
			urlForBitrate->AppendL( iUrlArray->At( loop )->GetServerUrl() );
			}
		}

    CleanupStack::Pop( urlForBitrate );
	IRLOG_DEBUG( "CIRIsdsPreset::GetUrlForBitrateL - Exiting." );
	return urlForBitrate;
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::GetChannelUrlAtL()
// Function to get channel Url for a given index
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIRIsdsPreset::GetChannelUrlAtL( TInt aIndex ) const
	{
	IRLOG_DEBUG( "CIRIsdsPreset::GetChannelUrlAtL - Entering." );
	__ASSERT_ALWAYS( aIndex < iUrlArray->Count(), User::Leave( KErrArgument ) );
	IRLOG_DEBUG( "CIRIsdsPreset::GetChannelUrlAtL - Exiting." );
	return iUrlArray->At( aIndex )->GetServerUrl();
	}


// ---------------------------------------------------------------------------
// CIRIsdsPreset::GetChannelBitrateL()
// Function to get channel Url for a given index
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIRIsdsPreset::GetChannelBitrateL( TInt aIndex )
	{
	IRLOG_DEBUG( "CIRIsdsPreset::GetChannelUrlAtL - Entering." );
	__ASSERT_ALWAYS( aIndex < iUrlArray->Count(), User::Leave( KErrArgument ) );
	CDesCArrayFlat* urlForBitrate = 
	    new( ELeave )CDesCArrayFlat( KUrlForBitrateMaxLength );
	
	CleanupStack::PushL( urlForBitrate );
	SortInOrderL();
	//appending the urls with bit rate less than or
	//equal to given bit rate limit
	for ( TInt loop = 0; loop < iUrlArray->Count(); ++loop )
		{
		if ( iBitrate >= iUrlArray->At( loop )->GetBitRate())
			{
			urlForBitrate->AppendL( iUrlArray->At( loop )->GetServerUrl() );
			}
		}
	
	//appending	the url	with bit rate greater than given
	//bit rate limit
	for ( TInt loop = ( iUrlArray->Count() - 1 );loop >= 0; --loop )
		{
		if ( iBitrate<iUrlArray->At( loop )->GetBitRate() )
			{
			urlForBitrate->AppendL( iUrlArray->At( loop )->GetServerUrl() );
			}
		}
	
	if ( urlForBitrate->MdcaCount() == 0 && iUrlArray->Count() > 0 )
		{
		urlForBitrate->AppendL( iUrlArray->At( 0 )->GetServerUrl() );
		}
	
    for ( TInt i = 0; i < iUrlArray->Count(); ++i )
	    {
	    if ( iUrlArray->At( i )->GetServerUrl().Compare
	            ( urlForBitrate->MdcaPoint( aIndex ) ) == 0 )
		    {
		    IRRDEBUG2( "CIRIsdsPreset:: - GetChannelBitrateL", KNullDesC );
		    CleanupStack::Pop( urlForBitrate );
			return iUrlArray->At( i )->GetBitRate();
		    }
	    }
    
    CleanupStack::Pop( urlForBitrate );
	IRRDEBUG2( "CIRIsdsPreset:: - GetChannelUrlAtL Exiting", KNullDesC );
	IRLOG_DEBUG( "CIRIsdsPreset::GetChannelUrlAtL - Exiting." );
	return 0;
	}


//----------------------------------------------------------------------------
// CIRIsdsPreset::SortInOrderL()
// simple bubble shot
// to sort the  bitrates in desc order
//----------------------------------------------------------------------------
//
void CIRIsdsPreset::SortInOrderL()
	{
	IRLOG_DEBUG( "CIRIsdsPreset::SortInOrderL - Exiting." );
	//temporary holder for pointer
	CIRChannelServerUrl* holdTemp = NULL;
	TInt urlcount = iUrlArray->Count();
	//sorts the array pointers [of urls] in the descending order of bit rate
	//bubble sort algorithm is used for this
	for ( TInt sortcount = ( urlcount - 1 ); sortcount >= 0; --sortcount )
	  	{
	    for ( TInt index = 1; index <= sortcount; index++ )
	    	{
	      	if ( iUrlArray->At( index - 1 )->GetBitRate() < 
	      			iUrlArray->At( index )->GetBitRate())
		       {
		       holdTemp = iUrlArray->At( index-1 );
		       iUrlArray->At( index - 1 ) = iUrlArray->At( index );
		       iUrlArray->At( index ) = holdTemp;
		       }
	    	}
	  	}
	
	IRLOG_DEBUG( "CIRIsdsPreset::SortInOrderL - Exiting." );
	return;
	}
