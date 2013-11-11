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
* Description:  Methods for CNSmlDMSyncProfile
*
*/


#include <SyncMLTransportProperties.h>
#include "nsmldmsyncprofile.h"
#include "nsmldmdef.h"
#include "nsmldmsyncdebug.h"
#include "nsmldmsyncutil.h"

// ============================ MEMBER FUNCTIONS ==============================
//

// -----------------------------------------------------------------------------
// NewLC
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNSmlDMSyncProfile* CNSmlDMSyncProfile::NewLC( const TInt aApplicationId,
                                               RSyncMLSession* aSyncSession )
    {
    FLOG( "[OMADM] CNSmlDMSyncProfile::NewLC()" );
    
    CNSmlDMSyncProfile* self = new( ELeave ) CNSmlDMSyncProfile( aApplicationId,
                                                                 aSyncSession );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
    }

// -----------------------------------------------------------------------------
// NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNSmlDMSyncProfile* CNSmlDMSyncProfile::NewL( const TInt aApplicationId,
                                              RSyncMLSession* aSyncSession )
    {
    FLOG( "[OMADM] CNSmlDMSyncProfile::NewL:" );
    
    CNSmlDMSyncProfile* self = new( ELeave ) CNSmlDMSyncProfile( aApplicationId,
                                                                 aSyncSession );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CNSmlDMSyncProfile::~CNSmlDMSyncProfile()
    {
    FLOG( "[OMADM] CNSmlDMSyncProfile::~CNSmlDMSyncProfile:" );
    
    iHistoryLog.Close();
    iConnection.Close();
    iProfile.Close();
    }

// -----------------------------------------------------------------------------
// ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::ConstructL()
    {
    FLOG( "[OMADM] CNSmlDMSyncProfile::ConstructL:" );
    }

// -----------------------------------------------------------------------------
// Constructor.
// -----------------------------------------------------------------------------
//
CNSmlDMSyncProfile::CNSmlDMSyncProfile( const TInt aApplicationId,
                                        RSyncMLSession* aSyncSession )
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::CNSmlDMSyncProfile:" );
	
	iSyncSession = aSyncSession;
	iApplicationId = aApplicationId;
	
    iHistoryLogOpen = EFalse;
    iConnectionOpen = EFalse;
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::OpenL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::OpenL( TInt aProfileId, TInt aOpenMode )
    {
    FTRACE( FPrint(
        _L("[OMADM] CNSmlDMSyncProfile::OpenL(), aProfileId = %d, aOpenMode = %d"),
        aProfileId, aOpenMode ) );

    if ( aOpenMode == EOpenRead )
    	{
    	iProfile.OpenL( *iSyncSession, aProfileId, ESmlOpenRead );
    	}
    else
    	{
    	iProfile.OpenL( *iSyncSession, aProfileId, ESmlOpenReadWrite );
    	}
    if ( !iConnectionOpen )
		{
    	OpenConnection();
		}
    FLOG( "[OMADM] CNSmlDMSyncProfile::OpenL() completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::CreateL
// -----------------------------------------------------------------------------
//
TInt CNSmlDMSyncProfile::CreateL()
    {
    FLOG( "[OMADM] CNSmlDMSyncProfile::CreateL()" );
    
    iProfile.CreateL( *iSyncSession );
    iProfile.SetCreatorId( iApplicationId );
    iProfile.UpdateL();
    TInt id = iProfile.Identifier();
    iProfile.Close();

    FLOG( "[OMADM] CNSmlDMSyncProfile::CreateL() completed" );
    return id;
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::CreateCopyL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::CreateCopyL( TInt aProfileId )
    {
    FLOG( "[OMADM] CNSmlDMSyncProfile::CreateCopyL()" );
    
    iProfile.CreateL( *iSyncSession );
    iProfile.SetCreatorId( iApplicationId );
    iProfile.UpdateL();
    if ( !iConnectionOpen )
		{
    	OpenConnection();
		}
	if ( !iHistoryLogOpen )
		{
	    OpenHistoryLog();
		}

    CNSmlDMSyncProfile* profile = 
                CNSmlDMSyncProfile::NewL( KUidSmlSyncApp.iUid, iSyncSession );
    CleanupStack::PushL( profile );
    profile->OpenL( aProfileId, ESmlOpenRead );
    CopyValuesL( profile );
    CleanupStack::PopAndDestroy( profile );
    profile  = NULL;

    iProfile.UpdateL();
    CloseHistoryLog();
    FLOG( "[OMADM] CNSmlDMSyncProfile::CreateCopyL() completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::SaveL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::SaveL()
    {
    FLOG( "[OMADM] CNSmlDMSyncProfile::SaveL()" );
    
    iProfile.UpdateL();
    if ( iConnectionOpen )
    	{
    	iConnection.UpdateL(); // IPC call
    	}
    iProfile.Close();
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::GetName
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::GetName( TDes& aText )
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::GetName()" );
	
    TUtil::StrCopy( aText, iProfile.DisplayName() );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::SetNameL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::SetNameL( const TDesC& aText )
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::SetNameL:" );
	
	iProfile.SetDisplayNameL( aText );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::CreatorId
// -----------------------------------------------------------------------------
//
TInt CNSmlDMSyncProfile::CreatorId()
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::CreatorId:" );
	
	return iProfile.CreatorId();
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::SetCreatorId
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::SetCreatorId( TInt aCreatorId )
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::SetCreatorId:" );
	
	iProfile.SetCreatorId( aCreatorId );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::ProfileId
// -----------------------------------------------------------------------------
//
TInt CNSmlDMSyncProfile::ProfileId()
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::ProfileId()" );
    TInt retval = iProfile.Identifier();
    FTRACE( FPrint( _L(
	    "[OMADM] CNSmlDMSyncProfile::ProfileId() completed: Profile id = %d"),
	    retval ) );
    return retval;
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::DeleteAllowed
// -----------------------------------------------------------------------------
//
TBool CNSmlDMSyncProfile::DeleteAllowed()
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::DeleteAllowed:" );
	
    return iProfile.DeleteAllowed();
	}
//-----------------------------------------------------------------------------
// CNSmlDMSyncProfile::ProfileLocked()
// -----------------------------------------------------------------------------
//	
TBool CNSmlDMSyncProfile::ProfileLocked()
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::DeleteAllowed:" );
	
    return iProfile.ProfileLocked(EFalse , EFalse);
	}
// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::IsSynced
// -----------------------------------------------------------------------------
//
TBool CNSmlDMSyncProfile::IsSynced()
    {
    FLOG( "[OMADM] CNSmlDMSyncProfile::IsSynced()" );
    
    TBool retVal = EFalse;
    if ( !iHistoryLogOpen )
		{
		FLOG( "[OMADM] CNSmlDMSyncProfile::IsSynced(): Opening history log" );
	    OpenHistoryLog();
		}
		
    if ( iHistoryLogOpen )
    	{
	    FTRACE( FPrint( _L(
	        "[OMADM] CNSmlDMSyncProfile::IsSynced(): Log open! Log count = %d"),
	                                                 iHistoryLog.Count() ) );
    	if ( iHistoryLog.Count() > 0 )
    	    {
            const CSyncMLHistoryJob* job = LatestHistoryJob();
            if ( job )
            	{
            	FLOG( "[OMADM] CNSmlDMSyncProfile::IsSynced(): True" );
            	if( job->LastSuccessSyncTime() != NULL )
                   {
    	        retVal = ETrue;
                   }
            	}
            else
                {
                FLOG( "[OMADM] CNSmlDMSyncProfile::IsSynced(): False" );
                }
    	    }
    	}
    else
        {
        FLOG( "[OMADM] CNSmlDMSyncProfile::IsSynced() Could not open history log!!!" );
        }
    CloseHistoryLog();    	

    FLOG( "[OMADM] CNSmlDMSyncProfile::IsSynced() completed" );
    return retVal;
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::LastSync
// -----------------------------------------------------------------------------
//
TTime CNSmlDMSyncProfile::LastSync()
    {
    FLOG( "[OMADM] CNSmlDMSyncProfile::LastSync()" );

    if ( !iHistoryLogOpen )
		{
		FLOG( "[OMADM] CNSmlDMSyncProfile::LastSync(): Opening history log" );
	    OpenHistoryLog();
		}
    
    TTime time = 0;
    if ( iHistoryLogOpen )
    	{
	    FTRACE( FPrint( _L(
	        "[OMADM] CNSmlDMSyncProfile::LastSync(): Log open! Log count = %d"),
	                                                 iHistoryLog.Count() ) );
    	
    	if (iHistoryLog.Count() > 0)
    	    {
   	        const CSyncMLHistoryJob* job = LatestHistoryJob();
            if ( job )
            	{
                FLOG( "[OMADM] CNSmlDMSyncProfile::LastSync() timestamp" );
            	time = job->TimeStamp();
            	}
    	    }
    	}
    else
        {
        FLOG( "[OMADM] CNSmlDMSyncProfile::LastSync() Could not open history log!!!" );
        }

    CloseHistoryLog();    	

    FLOG( "[OMADM] CNSmlDMSyncProfile::LastSync() completed" );
    return time;
    }

TTime CNSmlDMSyncProfile::LastSuccessSync()
{
	FLOG( "[OMADM] CNSmlDMSyncProfile::LastSuccessSynctime()" );

    if ( !iHistoryLogOpen )
		{
		FLOG( "[OMADM] CNSmlDMSyncProfile::LastSync(): Opening history log" );
	    OpenHistoryLog();
		}
    
    TTime time = 0;
    if ( iHistoryLogOpen )
    	{
	    FTRACE( FPrint( _L(
	        "[OMADM] CNSmlDMSyncProfile::LastSync(): Log open! Log count = %d"),
	                                                 iHistoryLog.Count() ) );
    	
    	if (iHistoryLog.Count() > 0)
    	    {
   	        const CSyncMLHistoryJob* job = LatestHistoryJob();
   	        iProfileId=ProfileId();
            if ( job )
            	{
                FLOG( "[OMADM] CNSmlDMSyncProfile::LastSync() timestamp" );
            	time = job->LastSuccessSyncTime();
            	}
    	    }
    	}
    else
        {
        FLOG( "[OMADM] CNSmlDMSyncProfile::LastSync() Could not open history log!!!" );
        }

    CloseHistoryLog();    	

    FLOG( "[OMADM] CNSmlDMSyncProfile::LastSuccessSynctime() completed" );
    return time;
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::GetServerIdL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::GetServerId( TDes& aText )
    {
    FLOG( "[OMADM] CNSmlDMSyncProfile::GetServerIdL:" );
    
	TUtil::StrCopy(aText, iProfile.ServerId());
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::SetServerIdL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::SetServerIdL( const TDesC& aText )
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::SetServerIdL:" );
	
	TBuf8<KBufSize256> buf;
	TUtil::StrCopy(buf, aText);
	iProfile.SetServerIdL(buf);
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::GetServerPassword
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::GetServerPassword(TDes& aText)
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::GetServerPassword:" );
	
	TUtil::StrCopy( aText, iProfile.ServerPassword() );
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::SetServerPasswordL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::SetServerPasswordL(const TDesC& aText)
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::SetServerPasswordL:" );
	
	TUtil::StrCopy(iBuf8, aText);    // conver TDes to TDes8
	iProfile.SetServerPasswordL(iBuf8);
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::ProtocolVersion
// -----------------------------------------------------------------------------
//
TInt CNSmlDMSyncProfile::ProtocolVersion()
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::ProtocolVersion:" );
	
	TSmlProtocolVersion version = iProfile.ProtocolVersion();
	return version;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::SetProtocolVersionL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::SetProtocolVersionL(TInt aProtocolVersion)
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::SetProtocolVersionL:" );
	
	iProfile.SetProtocolVersionL((TSmlProtocolVersion) aProtocolVersion);
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::BearerType
// -----------------------------------------------------------------------------
//
TInt CNSmlDMSyncProfile::BearerType()
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::BearerType:" );
	
	TInt id = iConnection.Identifier();
	
	if (id == KUidNSmlMediumTypeInternet.iUid)
		{
		return EAspBearerInternet;
		}
	else if (id == KUidNSmlMediumTypeBluetooth.iUid)
		{
		return EAspBearerBlueTooth;
		}
	else
		{
		return EAspBearerInternet;
		}
    }
	
// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::SetBearerTypeL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::SetBearerTypeL(TInt aId)
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::SetBearerTypeL:" );

	if ( aId == EAspBearerInternet )
		{
		iConnection.CreateL( Profile(), KUidNSmlMediumTypeInternet.iUid );
		}
	else if ( aId == EAspBearerBlueTooth )
		{
		iConnection.CreateL( Profile(), KUidNSmlMediumTypeBluetooth.iUid );
    	}
	else
        {
        // Set default as Internet
        iConnection.CreateL( Profile(), KUidNSmlMediumTypeInternet.iUid );
        }
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::AccessPointL
// -----------------------------------------------------------------------------
//
TInt CNSmlDMSyncProfile::AccessPointL()
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::AccessPointL:" );
	
	TInt num = KErrNotFound;
	
	if ( iConnectionOpen )
		{
	    if ( iConnection.Identifier() == KUidNSmlMediumTypeInternet.iUid )
	        {
        	TBuf8<KBufSize32> key;
        	TBuf<KBufSize32> value;

        	GetConnectionPropertyNameL( key, EPropertyIntenetAccessPoint );
        	TUtil::StrCopy( value, iConnection.GetPropertyL( key ) );
        	User::LeaveIfError( TUtil::StrToInt(value, num) );
	        }
		}
	return num;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::SetAccessPointL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::SetAccessPointL( const TInt aId )
	{
	FTRACE(RDebug::Print(
	         _L("[OMADM] CNSmlDMSyncProfile::SetAccessPointL aId (%d)"), aId));

	if ( iConnectionOpen )
		{
	    if ( iConnection.Identifier() == KUidNSmlMediumTypeInternet.iUid )
	        { 
	       	TBuf<KBufSize32> buf;
        	TBuf8<KBufSize32> key;
        	TBuf8<KBufSize32> value;
        	
        	buf.Num(aId);
        	TUtil::StrCopy(value, buf);  // convert TDes to TDes8
        	
        	GetConnectionPropertyNameL(key, EPropertyIntenetAccessPoint);
        	
        	FTRACE(RDebug::Print(
        	    _L("[OMADM] CNSmlDMSyncProfile::SetAccessPointL key (%S) value (%S)"),
        	    &key, &value ) );
        	    
        	iConnection.SetPropertyL( key, value );
	        }
		}
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::GetHostAddressL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::GetHostAddress(TDes& aText, TInt& aPort)
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::GetHostAddress:" );
	aText = KNullDesC;
	aPort = KDefaultHttpPort;
	
    if (!iConnectionOpen)	
    	{
      	return;
    	}
	
	if (BearerType() != EAspBearerInternet)
		{
		TUtil::StrCopy(aText, iConnection.ServerURI()); // convert TDes8 to TDes
		return;  // port number handling is for internet bearer only 
		}
	
	TUtil::StrCopy( iBuf, iConnection.ServerURI() ); // convert TDes8 to TDes
	
	TURIParser parser( iBuf );
	parser.GetUriWithoutPort( aText );
	aPort = parser.Port();
	if (aPort == KErrNotFound)
		{
		aPort = parser.DefaultPort();
		}
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::SetHostAddressL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::SetHostAddressL( const TDesC& aText, const TInt aPort )
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::SetHostAddressL:" );
	
    if (!iConnectionOpen)	
    	{
    	return;
    	}
    TBuf<KBufSize256> newUrl;
    convertBackslashinUrl(aText,newUrl);
    
	if (BearerType() != EAspBearerInternet)
		{
		// port number handling is for internet bearer only 
		TUtil::StrCopy(iBuf, newUrl); 
		}
	else
		{
		TURIParser parser(newUrl);
	    parser.GetUri(iBuf, aPort);
		}
	
	TUtil::StrCopy( iBuf8, iBuf );
	iConnection.SetServerURIL( iBuf8 );
	}

void CNSmlDMSyncProfile::convertBackslashinUrl(const TDesC& aUrl, TDes& aModifiedUrl )
    {    
    TBuf<KBufSize16> backSlash(KBackSlash);  
    TBuf<KBufSize16> forwardSlash(KSlash);
    for (TInt i = 0; i < aUrl.Length(); i++)
        {
        TChar ch = aUrl[i];        
        if (ch == backSlash[0] )
            {
            aModifiedUrl.Append(forwardSlash[0]);
            }
        else
            {
            aModifiedUrl.Append(ch);
            }
        }   
    }
// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::GetUserNameL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::GetUserName(TDes& aText)
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::GetUserNameL:" );
	
	TUtil::StrCopy( aText, iProfile.UserName() );
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::SetUserNameL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::SetUserNameL(const TDesC& aText)
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::SetUserNameL:" );
	
	TUtil::StrCopy(iBuf8, aText);  // conver TDes to TDes8
	iProfile.SetUserNameL(iBuf8);
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::GetPasswordL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::GetPassword(TDes& aText)
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::GetPasswordL:" );
	
	TUtil::StrCopy( aText, iProfile.Password() );
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::SetPasswordL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::SetPasswordL(const TDesC& aText)
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::SetPasswordL:" );
	TUtil::StrCopy( iBuf8, aText );    // conver TDes to TDes8
	
	FTRACE( RDebug::Print(
	     _L("[OMADM] CNSmlDMSyncProfile::SetPasswordL iBuf8 (%S)"), &iBuf8 ) );
	iProfile.SetPasswordL( iBuf8 );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::SASyncState
// -----------------------------------------------------------------------------
//
TInt CNSmlDMSyncProfile::SASyncState()
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::SASyncState()" );
	
	TSmlServerAlertedAction state = iProfile.SanUserInteraction();

    FTRACE( FPrint(
        _L( "[OMADM] CNSmlDMSyncProfile::SASyncState() completed, state = %d" ),
        state ) );

	if (state == ESmlConfirmSync)
		{
		return ESASyncStateConfirm;
		}
	else if (state == ESmlDisableSync)
		{
		return ESASyncStateDisable;
		}
	else 
		{
		return ESASyncStateEnable;
		}
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::SetSASyncStateL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::SetSASyncStateL(TInt aState)
    {
	if (aState == ESASyncStateConfirm)
		{
		FLOG( "[OMADM] CNSmlDMSyncProfile::SetSASyncStateL:ESmlConfirmSync" );
		Profile().SetSanUserInteractionL(ESmlConfirmSync);
		}
	else if (aState == ESASyncStateDisable)
		{
		FLOG( "[OMADM] CNSmlDMSyncProfile::SetSASyncStateL:ESmlDisableSync" );
		Profile().SetSanUserInteractionL(ESmlDisableSync);
		}
	else
		{
		FLOG( "[OMADM] CNSmlDMSyncProfile::SetSASyncStateL:ESmlEnableSync" );
		Profile().SetSanUserInteractionL(ESmlEnableSync);
		}
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::SetHttpUsed
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::SetHttpUsedL(TBool aEnable)
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::SetHttpUsed:" );

    if ( iConnectionOpen )
    	{
    	if ( iConnection.Identifier() == KUidNSmlMediumTypeInternet.iUid )
		    {
        	TBuf8<KBufSize32> key;
        	TBuf8<KBufSize32> value;
        	
        	if ( aEnable )
        		{
        		value.Num( KHttpUsed ); 
        		}
        	else
        		{
        		value.Num( KHttpNotUsed );
        		}
        		
        	GetConnectionPropertyNameL( key, EPropertyHttpUsed );
        	iConnection.SetPropertyL( key, value );
		    }
    	}
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::HttpUsedL
// -----------------------------------------------------------------------------
//
TBool CNSmlDMSyncProfile::HttpUsedL()
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::HttpUsedL:" );
    TBool retVal = EFalse; 
    if ( iConnectionOpen )
    	{
    	if ( iConnection.Identifier() == KUidNSmlMediumTypeInternet.iUid )
		    {
        	TBuf8<KBufSize32> key; 
        	TBuf<KBufSize32> value;
        	TInt intValue;
        	
        	GetConnectionPropertyNameL(key, EPropertyHttpUsed);
        	TUtil::StrCopy(value, iConnection.GetPropertyL(key));
        	
            User::LeaveIfError( TUtil::StrToInt( value, intValue ) );
            
            if ( intValue == 1 )
            	{
                retVal = ETrue;
                }
		    }
    	}
    return retVal;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::GetHttpUsernameL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::GetHttpUserNameL( TDes& aText )
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::GetHttpUsernameL:" );

	aText = KNullDesC;
	
    if ( iConnectionOpen )
    	{
    	if ( iConnection.Identifier() == KUidNSmlMediumTypeInternet.iUid )
		    {
        	TBuf8<KBufSize32> key; 
        	
        	GetConnectionPropertyNameL( key, EPropertyHttpUserName );
        	TUtil::StrCopy( aText, iConnection.GetPropertyL( key ) );
		    }
    	}
  	}
	
// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::SetHttpUsernameL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::SetHttpUserNameL( const TDesC& aText )
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::SetHttpUsernameL:" );

    if ( iConnectionOpen )
    	{
    	if ( iConnection.Identifier() == KUidNSmlMediumTypeInternet.iUid )
		    {
		    TBuf8<KBufSize32> key;
	
	        GetConnectionPropertyNameL( key, EPropertyHttpUserName );
	        TUtil::StrCopy( iBuf8, aText );  // convert TDes to TDes8
	        iConnection.SetPropertyL( key, iBuf8 );
		    }
    	}
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::GetHttpPasswordL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::GetHttpPasswordL( TDes& aText )
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::GetHttpPasswordL:" );
   	aText = KNullDesC;
   	
    if ( iConnectionOpen )
    	{
    	if ( iConnection.Identifier() == KUidNSmlMediumTypeInternet.iUid )
		    {
		    TBuf8<KBufSize32> key; 
	        GetConnectionPropertyNameL( key, EPropertyHttpPassword );
            TUtil::StrCopy( aText, iConnection.GetPropertyL( key ) );
		    }
    	}
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::SetHttpPasswordL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::SetHttpPasswordL( const TDesC& aText )
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::SetHttpPasswordL:" );

    if ( iConnectionOpen )
    	{
    	if ( iConnection.Identifier() == KUidNSmlMediumTypeInternet.iUid )
		    {
        	TBuf8<KBufSize32> key;
        	
        	GetConnectionPropertyNameL( key, EPropertyHttpPassword );
        	TUtil::StrCopy( iBuf8, aText );  // convert TDes to TDes8
        	iConnection.SetPropertyL( key, iBuf8 );
    	    }
    	}
	}
 
// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::Profile
// -----------------------------------------------------------------------------
//
RSyncMLDevManProfile& CNSmlDMSyncProfile::Profile()
    {
    FLOG( "[OMADM] CNSmlDMSyncProfile::Profile()" );
    
    return iProfile;
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::LatestHistoryJob
// -----------------------------------------------------------------------------
//
const CSyncMLHistoryJob* CNSmlDMSyncProfile::LatestHistoryJob()
    {
    FLOG( "[OMADM] CNSmlDMSyncProfile::LatestHistoryJob:" );
    
    if ( !iHistoryLogOpen )
		{
        OpenHistoryLog();
		}
		
    if ( iHistoryLogOpen )
		{
	    TInt count = iHistoryLog.Count();
        FTRACE( FPrint( _L(
            "[OMADM] CNSmlDMSyncProfile::LatestHistoryJob(): Log count %d"),
            count ) );

        if ( count == 0 )
            {
            FLOG( "[OMADM] CNSmlDMSyncProfile::LatestHistoryJob(): No history" );
            return NULL;  // profile has no history job
            }
            
        // sort array
        iHistoryLog.SortEntries( CSyncMLHistoryEntry::ESortByTime );    
        
        // try to find latest sync job (start from last array entry)
        for ( TInt index = count - 1; index >= 0; index-- )
        	{
        	FTRACE( FPrint( _L(
        	    "[OMADM] CNSmlDMSyncProfile::LatestHistoryJob: count %d index %d"),
        	    count, index ) );
            //const CSyncMLHistoryEntry& entry = iHistoryLog[index];
            const CSyncMLHistoryEntry& entry = iHistoryLog.Entry( index );
            
        	FTRACE( FPrint( _L(
        	    "[OMADM] CNSmlDMSyncProfile::LatestHistoryJob: Entry type = %x"),
        	    entry.EntryType().iUid ) );
            
            const CSyncMLHistoryJob* jobEntry =
                CSyncMLHistoryJob::DynamicCast( &entry );

            if ( jobEntry )
            	{
            	FLOG( "[OMADM] CNSmlDMSyncProfile::LatestHistoryJob() completed: OK" );
            	return jobEntry;
            	}
        	}
		}

    FLOG( "[OMADM] CNSmlDMSyncProfile::LatestHistoryJob() completed: No history found" );
    return NULL;  // profile has no history job
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::OpenHistoryLog
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::OpenHistoryLog()
    {
    FLOG( "[OMADM] CNSmlDMSyncProfile::OpenHistoryLog()" );
    
    TInt id = ProfileId();
   	TRAPD( err, iHistoryLog.OpenL( Session(), id ) );

   	if ( err == KErrNone )
   		{
   		FLOG( "[OMADM] CNSmlDMSyncProfile::OpenHistoryLog(): Opened" );
   		iHistoryLogOpen = ETrue;
    	}

    FLOG( "[OMADM] CNSmlDMSyncProfile::OpenHistoryLog() completed" );
    }
    
// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::CloseHistoryLog
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::CloseHistoryLog()
    {
    FLOG( "[OMADM] CNSmlDMSyncProfile::CloseHistoryLog()" );
    
   	if ( iHistoryLogOpen )
   		{
   		FLOG( "[OMADM] CNSmlDMSyncProfile::CloseHistoryLog(): Closed " );
   		iHistoryLog.Close();
   		iHistoryLogOpen = EFalse;
    	}

    FLOG( "[OMADM] CNSmlDMSyncProfile::CloseHistoryLog() completed" );
    }
    
// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::OpenConnection
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::OpenConnection()
    {
    FLOG( "[OMADM] CNSmlDMSyncProfile::OpenConnection()" );
        
   	TInt err = KErrNone;
   	RArray<TSmlTransportId> arr;
   	
    TRAP( err, Profile().ListConnectionsL(arr) );
    if ( err != KErrNone )
    	{
    	return; 
    	}
    if ( arr.Count() == 0 )
    	{
     	arr.Close();  
    	return; // no connection
    	}
        	
    TInt transportId = arr[0];
    arr.Close();
    	
    TRAP( err, iConnection.OpenL( iProfile, transportId ) );
    if ( err == KErrNone )
    	{
    	iConnectionOpen = ETrue;
    	}

    FTRACE( FPrint( _L(
        "[OMADM] CNSmlDMSyncProfile::OpenConnection() completed, err = %d" ),
        err ) );
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::Session
// -----------------------------------------------------------------------------
//
RSyncMLSession& CNSmlDMSyncProfile::Session()
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::Session:" );
	
	return *iSyncSession;
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::GetConnectionPropertyNameL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::GetConnectionPropertyNameL( TDes8& aText,
                                                     TInt aPropertyPos )
	{
	FLOG( "[OMADM] CNSmlDMSyncProfile::GetConnectionPropertyNameL:" );
	
	// at the moment RSyncMLTransport is only needed
	// for internet connection settings
	RSyncMLTransport transport;
	CleanupClosePushL( transport );
	transport.OpenL( Session(), KUidNSmlMediumTypeInternet.iUid );

	const CSyncMLTransportPropertiesArray&  arr = transport.Properties();

    __ASSERT_DEBUG( arr.Count() > aPropertyPos, TUtil::Panic( KErrGeneral ) );

	const TSyncMLTransportPropertyInfo& info = arr.At( aPropertyPos );
	aText = info.iName;
	CleanupStack::PopAndDestroy( &transport );
	
	FLOG( "[OMADM] CNSmlDMSyncProfile::GetConnectionPropertyNameL() completed" );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncProfile::CopyValuesL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncProfile::CopyValuesL( CNSmlDMSyncProfile* aSource )
    {
	FLOG( "[OMADM] CNSmlDMSyncProfile::CopyValuesL()" );

    TBuf<KBufSize256> buf;
    TInt num = 0;
    
    num = aSource->ProtocolVersion();
    SetProtocolVersionL( num );
    if ( num == 1 )
    	{
    	aSource->GetServerId( buf );
    	SetServerIdL( buf );
    	}
    	
    num = aSource->BearerType();
    SetBearerTypeL( num );
    
     num = aSource->AccessPointL();
    SetAccessPointL( num );
    
    aSource->GetHostAddress( buf, num );
    SetHostAddressL( buf, num );
     
    aSource->GetUserName( buf );
    SetUserNameL(buf);
     
    aSource->GetPassword( buf );
    SetPasswordL( buf );
    
    num = aSource->SASyncState();
    SetSASyncStateL( num );
    
    if ( aSource->BearerType() == EAspBearerInternet)
    	{
    	num = aSource->HttpUsedL();
    	SetHttpUsedL(num);
    	
    	aSource->GetHttpUserNameL(buf);
    	SetHttpUserNameL(buf);
    	
    	aSource->GetHttpPasswordL(buf);
    	SetHttpPasswordL(buf);
    	}
	FLOG( "[OMADM] CNSmlDMSyncProfile::CopyValuesL() completed" );
    }
    
// End of File
