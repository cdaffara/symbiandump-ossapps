/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Utility classes.
*
*/



// INCLUDES
#include <SyncMLTransportProperties.h>
#include <eikenv.h>
#include <bautils.h>
#include <collate.h>
#include <StringLoader.h>
#include <avkon.rsg>
#include <SettingEnforcementInfo.h> // VSettingEnforcementInfo
#include <featmgr.h>
#include "nsmldmdef.h"
#include "nsmldmsyncutil.h"
#include "nsmldmsyncdebug.h"


/*****************************************************************************
 * class TUtil
 *****************************************************************************/


// ----------------------------------------------------------------------------
// Panic
// ----------------------------------------------------------------------------
//
void TUtil::Panic(TInt aReason)
    {
	_LIT(KPanicCategory,"NSmlDMSyncUtil");

	User::Panic( KPanicCategory, aReason ); 
    }

// ----------------------------------------------------------------------------
// StrCopy
// String copy with lenght check.
// ----------------------------------------------------------------------------
//
void TUtil::StrCopy( TDes8& aTarget, const TDesC& aSource )
    {
    aTarget.Copy( aSource.Left(aTarget.MaxLength() ) );
    }

// ----------------------------------------------------------------------------
// StrCopy
// String copy with lenght check.
// ----------------------------------------------------------------------------
//
void TUtil::StrCopy( TDes& aTarget, const TDesC8& aSource )
    {
	aTarget.Copy( aSource.Left(aTarget.MaxLength() ) );
    }

// ----------------------------------------------------------------------------
// StrCopy
// String copy with lenght check.
// ----------------------------------------------------------------------------
//
void TUtil::StrCopy( TDes& aTarget, const TDesC& aSource )
    {
	aTarget.Copy( aSource.Left( aTarget.MaxLength() ) );
    }

// ----------------------------------------------------------------------------
// StrAppend
// ----------------------------------------------------------------------------
//
void TUtil::StrAppend( TDes& aTarget, const TDesC& aSource )
    {
    if ( aSource.Length() == 0 )
    	{
    	return;
    	}
    TInt free = aTarget.MaxLength() - aTarget.Length();
	if ( aSource.Length() <= free )
		{
		aTarget.Append( aSource );
		}
    }

// ----------------------------------------------------------------------------
// StrInsert
// ----------------------------------------------------------------------------
//
void TUtil::StrInsert( TDes& aTarget, const TDesC& aSource )
    {
	TInt free = aTarget.MaxLength() - aTarget.Length();
	if (aSource.Length() <= free)
		{
		aTarget.Insert(0, aSource);
		}
    }

// ----------------------------------------------------------------------------
// StrToInt
// ----------------------------------------------------------------------------
//
TInt TUtil::StrToInt( const TDesC& aText, TInt& aNum )
	{
    TLex lex( aText );
    TInt err = lex.Val( aNum ); 
	return err;
	}

//
//-----------------------------------------------------------------------------
// IsEmpty
// Function returns ETrue if string only contains white space
// or has no characters.
//-----------------------------------------------------------------------------
//
TBool TUtil::IsEmpty( const TDesC& aText )
	{
	TInt len = aText.Length();
	for ( TInt index=0; index < len; index++ )
		{
		TChar character = aText[index];
		if ( !character.IsSpace() )
			{
			return EFalse;
			}
		}
	return ETrue;
	}

// -----------------------------------------------------------------------------
// SyncTimeLC
// -----------------------------------------------------------------------------
//
HBufC* TUtil::SyncTimeLC( TTime aLastSync )
	{
	FLOG( "[OMADM] TUtil::SyncTimeLC" );
	
	TTime homeTime = ConvertUniversalToHomeTime( aLastSync );
	
	HBufC* hBuf = HBufC::NewLC( KBufSize255 );
	TPtr ptr = hBuf->Des();

	if ( IsToday( homeTime ) )
		{
		FLOG( "[OMADM] TUtil::SyncTimeLC time" );
		
		TBuf<KBufSize> timeFormat;
		HBufC* buf = StringLoader::LoadLC( R_QTN_TIME_USUAL_WITH_ZERO );
		TUtil::StrCopy( timeFormat, *buf );
		CleanupStack::PopAndDestroy( buf );
		homeTime.FormatL( ptr, timeFormat );
		
		FLOG( "[OMADM] TUtil::SyncTimeLC time done" );
		}
	else
		{
		FLOG( "[OMADM] TUtil::SyncTimeLC date" );
		
		TBuf<KBufSize> dateFormat;
        HBufC* buf = StringLoader::LoadLC( R_QTN_DATE_USUAL_WITH_ZERO );
        TUtil::StrCopy( dateFormat, *buf );
        CleanupStack::PopAndDestroy( buf );
		homeTime.FormatL(ptr, dateFormat);
		
		FLOG( "[OMADM] TUtil::SyncTimeLC date done" );
		}
    FLOG( "[OMADM] TUtil::SyncTimeLC Done" );
	return hBuf;
	}

// -----------------------------------------------------------------------------
// ConvertUniversalToHomeTime
// -----------------------------------------------------------------------------
//
TTime TUtil::ConvertUniversalToHomeTime( const TTime& aUniversalTime )
    {
    TTime time( aUniversalTime );   // time stores UTC time.

    TLocale locale;
    TTimeIntervalSeconds universalTimeOffset( locale.UniversalTimeOffset() );
    
    // Add locale's universal time offset to universal time.
    time += universalTimeOffset;    // time stores Local Time.

    // If home daylight saving in effect, add one hour offset.
    if ( locale.QueryHomeHasDaylightSavingOn() )
        {
        TTimeIntervalHours daylightSaving(1);
        time += daylightSaving;
        }

    return time;
    }

// -----------------------------------------------------------------------------
// IsToDay
// -----------------------------------------------------------------------------
//
TBool TUtil::IsToday(TTime aTime)
	{
	FLOG( "[OMADM] TUtil::IsToday" );
	
	TTime now;
	now.HomeTime();
	TInt day1 = now.DayNoInYear();
	TInt day2 = aTime.DayNoInYear();
	TTimeIntervalDays daysBetween = now.DaysFrom( aTime );
	
	if ( day1 != day2 )
		{
		FLOG( "[OMADM] TUtil::IsToday False 1" );
		return EFalse;
		}
		
	if ( daysBetween.Int() > 0 )
		{
		FLOG( "[OMADM] TUtil::IsToday False 2" );
		return EFalse;
		}

    FLOG( "[OMADM] TUtil::IsToday True" );
	return ETrue; 
	}


// -----------------------------------------------------------------------------
// TUtil::GetDateTextL
// -----------------------------------------------------------------------------
//
void TUtil::GetDateTextL(TDes& aText, TTime aDateTime)
	{
	TTime homeTime = ConvertUniversalToHomeTime( aDateTime );
	HBufC* hBuf = StringLoader::LoadLC( R_QTN_DATE_USUAL_WITH_ZERO );
    homeTime.FormatL( aText, *hBuf );
    CleanupStack::PopAndDestroy( hBuf );
	}


// -----------------------------------------------------------------------------
// TUtil::GetTimeTextL
// -----------------------------------------------------------------------------
//
void TUtil::GetTimeTextL( TDes& aText, TTime aDateTime )
	{
	TTime homeTime = ConvertUniversalToHomeTime( aDateTime );
	HBufC* hBuf = StringLoader::LoadLC( R_QTN_TIME_USUAL_WITH_ZERO );
    homeTime.FormatL( aText, *hBuf );
    CleanupStack::PopAndDestroy( hBuf );
	}


// -----------------------------------------------------------------------------
// TUtil::GetDateTimeTextL
// -----------------------------------------------------------------------------
//
void TUtil::GetDateTimeTextL( TDes& aText, TTime aDateTime )
	{
	TDateTime dt = aDateTime.DateTime();
	aText.Format(_L("%02d.%02d.%04d %02d:%02d:%02d"), dt.Day()+1,
	                                                  dt.Month()+1,
	                                                  dt.Year(),
	                                                  dt.Hour(),
	                                                  dt.Minute(),
	                                                  dt.Second() );
	}


// ---------------------------------------------------------
// TUtil::SettingEnforcementStateL
// 
// Checks if setting enforcement is activated.
// ---------------------------------------------------------
TBool TUtil::SettingEnforcementStateL()
    {
FeatureManager::InitializeLibL();
if(!FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
{
		FeatureManager::UnInitializeLib();
		    return EFalse;
}
else
{
	FeatureManager::UnInitializeLib();
	CSettingEnforcementInfo* info = CSettingEnforcementInfo::NewL();
	CleanupStack::PushL(info);

	TBool active = EFalse;
	User::LeaveIfError(info->EnforcementActive(ESyncMLEnforcement, active));
	CleanupStack::PopAndDestroy(info);

	return active;
}

    }


// ---------------------------------------------------------
// TUtil::SettingEnforcementState
// 
// Checks if setting enforcement is activated.
// ---------------------------------------------------------
TBool TUtil::SettingEnforcementState()
    {
    TBool ret = EFalse;

    TRAP_IGNORE(ret = SettingEnforcementStateL());

    return ret;
    }


/*****************************************************************************
 * class TURIParser
 *****************************************************************************/

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
TURIParser::TURIParser( const TDesC& aStr ) : iOriginalStr( aStr )
    {
    SkipHttp();
    }

// ----------------------------------------------------------------------------
// GetUriWithoutPort
// URI format: "http://serveraddress/documentname/"
// ----------------------------------------------------------------------------
//
void TURIParser::GetUriWithoutPort( TDes& aText )
	{
	aText = KNullDesC;
	TPtrC http = ParseHttp();
	TPtrC address = ParseAddress();
	TPtrC document = ParseDocument();
	TUtil::StrAppend( aText, http );
	TUtil::StrAppend( aText, address );
	
	if (document.Length() > 0)
		{
		TUtil::StrAppend( aText, KSlash );
		TUtil::StrAppend( aText, document );
		}
	else if ( iSlash )
		{
		TUtil::StrAppend( aText, KSlash );
		}
	}
	
// ----------------------------------------------------------------------------
// GetURI
// URI format: "http://serveraddress:port/documentname/"
// ----------------------------------------------------------------------------
//
void TURIParser::GetUri( TDes& aText, const TInt aPort )
	{
	aText = KNullDesC;
	TPtrC http = ParseHttp();
	TPtrC address = ParseAddress();
	TPtrC document = ParseDocument();
	TUtil::StrAppend( aText, http );
	TUtil::StrAppend( aText, address );
	
	TBuf<KBufSize32> buf;
	buf.Num( aPort );
	TUtil::StrAppend( aText, KColon );
	TUtil::StrAppend( aText, buf );
	
	if ( document.Length() > 0 )
		{
		TUtil::StrAppend( aText, KSlash );
		TUtil::StrAppend( aText, document );
		}
	else if ( iSlash )
		{
		TUtil::StrAppend( aText, KSlash );
		}
	}

// ----------------------------------------------------------------------------
// Port
// ----------------------------------------------------------------------------
//
TInt TURIParser::Port()
	{
	TInt port = KErrNotFound;
	
	TInt err = ParsePort( port );
	if ( err != KErrNone )
		{
		return KErrNotFound;
		}
	return port;
	}

// ----------------------------------------------------------------------------
// DefaultPort
// 
// ----------------------------------------------------------------------------
//
TInt TURIParser::DefaultPort()
	{
	TInt pos = iOriginalStr.FindF( KHttpsHeader );
	if ( pos != KErrNotFound )
		{
		return KDefaultHttpsPort;
		}
	return KDefaultHttpPort;
	}

// ----------------------------------------------------------------------------
// SkipHttp
// Set pointer to the end of "http://".
// ----------------------------------------------------------------------------
//
void TURIParser::SkipHttp()
	{
    TInt start = KErrNotFound;
    TInt end   = KErrNotFound;
    TStr str(iOriginalStr);
    
    GetHttpPos( start, end );
    
    if ( start != KErrNotFound )
    	{
    	iPtr.Set( str.Right( end + 1 ) );
    	}
    else
    	{
    	iPtr.Set( str.Right(0) );
    	}
	}

// ----------------------------------------------------------------------------
// ParseHttp
// 
// ----------------------------------------------------------------------------
//
TPtrC TURIParser::ParseHttp()
	{
    TInt start = KErrNotFound;
    TInt end   = KErrNotFound;
    
    GetHttpPos(start, end);
    
    if (start == KErrNotFound)
    	{
    	return TPtrC();
    	}
    
   	TStr str(iOriginalStr);
    return str.SubStr(start, end);
	}

// ----------------------------------------------------------------------------
// GetHttpPos 
// ----------------------------------------------------------------------------
//
void TURIParser::GetHttpPos( TInt& aStartPos, TInt& aEndPos )
	{
	aEndPos = KErrNotFound;
	
	aStartPos = iOriginalStr.FindF( KHttpHeader );
	if ( aStartPos != KErrNotFound )
		{
		aEndPos = aStartPos + KHttpHeader().Length() - 1;
		return;
		}
	
	aStartPos = iOriginalStr.FindF( KHttpsHeader );
	if ( aStartPos != KErrNotFound )
		{
		aEndPos = aStartPos + KHttpsHeader().Length() - 1;
		return;
		}
	}

// ----------------------------------------------------------------------------
// ParseAddress
// ----------------------------------------------------------------------------
TPtrC TURIParser::ParseAddress()
	{
	const TChar KSlash('/');
	const TChar KColon(':');
	TStr str( iPtr );
    TInt firstSlash = str.Find( 0, KSlash );
	TInt firstcolon = str.Find( 0, KColon );
	TBool portFound = EFalse;
	iSlash = EFalse;
	if ( firstSlash != KErrNotFound ) 
	    {
		 iSlash = ETrue;
	    }
	if (firstcolon != KErrNotFound)
		{
		if ( firstSlash == KErrNotFound )
			{
			portFound = ETrue;
			}
		else if ( firstcolon < firstSlash )
			{
			portFound = ETrue;
			}
		}
    
	if ( portFound )
		{
		// address is text before first colon
		return str.Left( firstcolon - 1 );
		}
	else if (firstSlash != KErrNotFound)
		{
		// address is text before first slash
		return str.Left( firstSlash - 1 );
		}
	else
		{
		// address is all text
		return str.Right( 0 );
		}
	}

// ----------------------------------------------------------------------------
// ParseDocument
// ----------------------------------------------------------------------------
TPtrC TURIParser::ParseDocument()
	{
	const TChar KSlash('/');
	
	TStr str(iPtr);
    TInt firstSlash = str.Find( 0, KSlash );
    
	if ( firstSlash != KErrNotFound )
		{
		// document is all text after first slash
		return str.Right( firstSlash + 1 );
		}
	return TPtrC();
	}
	
// ----------------------------------------------------------------------------
// ParsePort
// ----------------------------------------------------------------------------
// 
TInt TURIParser::ParsePort( TInt& aNumber )
	{
	TPtrC port = ParsePort();
	if ( port.Length() == 0 )
		{
		return KErrNotFound;
		}
	return TUtil::StrToInt( port, aNumber );
	}

// ----------------------------------------------------------------------------
// ParsePort
// ----------------------------------------------------------------------------
// 
TPtrC TURIParser::ParsePort()
	{
	const TChar KSlash('/');
	const TChar KColon(':');
	
	TStr str(iPtr);
    TInt firstSlash = str.Find( 0, KSlash );
	TInt firstColon = str.Find( 0, KColon );
    
	if ( firstSlash != KErrNotFound )
		{
		if ( firstColon != KErrNotFound && firstColon < firstSlash )
			{
		    // port number is text between colon and slash 
		    return str.SubStrEx( firstColon, firstSlash );
			}
		}
	else 
		{
		if ( firstColon != KErrNotFound )
			{
		    // port number is all text after colon 
		    return str.Right(firstColon+1);
			}
		}
	return TPtrC();
	}

/*****************************************************************************
 * class TStr
 *****************************************************************************/

// ----------------------------------------------------------------------------
// TStr::TStr
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
TStr::TStr( const TDesC& aStr ) : iStr( aStr )
    {
    }

// ----------------------------------------------------------------------------
// TStr::Mid
// ----------------------------------------------------------------------------
//
TPtrC TStr::Mid( TInt aPos )
    {
	if ( ( aPos < 0 ) || ( aPos >= iStr.Length() ) )
	    {
		return TPtrC();  // return empty descriptor
	    }
	return iStr.Mid( aPos );
    }

// ----------------------------------------------------------------------------
// TStr::Mid
// ----------------------------------------------------------------------------
//
TPtrC TStr::Mid( TInt aPos, TInt aLen )
    {
	TInt len = iStr.Length();
	if ( ( aPos < 0 ) || ( aPos >= len ) )
	    {
		return TPtrC();
	    }
	if ( ( aLen <= 0 ) || ( aPos + aLen > len ) )
	    {
		return TPtrC();
	    }

	return iStr.Mid( aPos, aLen );
    }

// ----------------------------------------------------------------------------
// TStr::Right
// ----------------------------------------------------------------------------
//
TPtrC TStr::Right( TInt aPos )
    {
	return Mid( aPos );
    }

// ----------------------------------------------------------------------------
// TStr::Left
// ----------------------------------------------------------------------------
//
TPtrC TStr::Left( TInt aPos )
    {
	TInt len = iStr.Length();

	if ( ( aPos < 0 ) || ( len == 0 ) )
	    {
		return TPtrC();
	    }

	TInt pos = len;
	if ( ( aPos + 1 ) < len )
	    {
		pos = aPos + 1;
	    }

	return iStr.Left( pos );
    }

// ----------------------------------------------------------------------------
// TStr::SubStr
// ----------------------------------------------------------------------------
//
TPtrC TStr::SubStr( TInt aStartPos, TInt aEndPos )
    {
	return Mid( aStartPos, aEndPos - aStartPos + 1 );
    }

// ----------------------------------------------------------------------------
// TStr::SubStrEx
// ----------------------------------------------------------------------------
//
TPtrC TStr::SubStrEx( TInt aStartPos, TInt aEndPos )
    {
	return Mid( aStartPos + 1, aEndPos - aStartPos - 1 );
    }

// ----------------------------------------------------------------------------
// TStr::LastPos
// ----------------------------------------------------------------------------
//
TInt TStr::LastPos()
    {
	return iStr.Length() - 1;
    }

// ----------------------------------------------------------------------------
// TStr::Len
// ----------------------------------------------------------------------------
//
TInt TStr::Len()
    {
    return iStr.Length();
    }

// ----------------------------------------------------------------------------
// TStr::Compare
// ----------------------------------------------------------------------------
//
TBool TStr::Compare( TInt aPos, TChar aChar )
    {
	TInt len = iStr.Length();
	if ( ( aPos < 0) || ( aPos >= len ) )
	    {
		return EFalse;
	    }
	TChar ch = iStr[aPos];
	if ( ch == aChar )
	    {
		return ETrue;
	    }
	return EFalse;
    }

// ----------------------------------------------------------------------------
// TStr::Find
// ----------------------------------------------------------------------------
//
TInt TStr::Find(TInt aPos, TChar aChar)
    {
	TInt len = iStr.Length();
	if ( (aPos < 0) || (aPos >= len) )
	    {
		return KErrNotFound;
	    }
	    
	TPtrC ptr(iStr.Mid(aPos)); // move to possition aPos
	return ptr.Locate(aChar);
    }

//*****************************************************************************
//* class CNSmlDMActiveCaller
//*****************************************************************************

// ----------------------------------------------------------------------------
// CNSmlDMActiveCaller::NewL
// ----------------------------------------------------------------------------
//
CNSmlDMActiveCaller* CNSmlDMActiveCaller::NewL( 
                                       MNSmlDMActiveCallerObserver* aObserver )
    {
    CNSmlDMActiveCaller* self = new(ELeave) CNSmlDMActiveCaller( aObserver );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CNSmlDMActiveCaller::~CNSmlDMActiveCaller()
    {
	Cancel();
	iTimer.Close();
    }

// ----------------------------------------------------------------------------
// CNSmlDMActiveCaller::CNSmlDMActiveCaller
// ----------------------------------------------------------------------------
//
CNSmlDMActiveCaller::CNSmlDMActiveCaller( 
                                        MNSmlDMActiveCallerObserver* aObserver )
                                      : CActive(CActive::EPriorityStandard )
    {
	iObserver = aObserver;
    }

// ----------------------------------------------------------------------------
// CNSmlDMActiveCaller::ConstructL
// ----------------------------------------------------------------------------
//
void CNSmlDMActiveCaller::ConstructL()
    {
	User::LeaveIfError(iTimer.CreateLocal());
	CActiveScheduler::Add(this);
    }

// ----------------------------------------------------------------------------
// CNSmlDMActiveCaller::DoCancel
// ----------------------------------------------------------------------------
//
void CNSmlDMActiveCaller::DoCancel()
    {
	iTimer.Cancel();
    }

// ----------------------------------------------------------------------------
// CNSmlDMActiveCaller::RunL
// ----------------------------------------------------------------------------
//
void CNSmlDMActiveCaller::RunL()
    {
    iObserver->HandleActiveCallL( );
    }

// ----------------------------------------------------------------------------
// CNSmlDMActiveCaller::RunError
// ----------------------------------------------------------------------------
//
TInt CNSmlDMActiveCaller::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// CNSmlDMActiveCaller::CompleteSelf
//
// This function calls this class RunL.
// ----------------------------------------------------------------------------
//
void CNSmlDMActiveCaller::CompleteSelf()
    {
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete( status, KErrNone );
    }

// ----------------------------------------------------------------------------
// CNSmlDMActiveCaller::Start
// ----------------------------------------------------------------------------
//
void CNSmlDMActiveCaller::Start( TInt aCallId, TInt aMilliseconds )
    {
	if (IsActive())
		{
		return;
		}
	
	iCallId = aCallId;

	if (aMilliseconds <= 0)
		{
		CompleteSelf();  // no delay - complete right away
		}
	else
		{
	    iTimer.After( iStatus, aMilliseconds*1000 );
	    SetActive();
		}
    }

// End of File
