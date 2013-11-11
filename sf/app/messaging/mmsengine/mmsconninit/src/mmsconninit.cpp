/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  mmsconninit implementation
*
*/


   
// INCLUDE FILES   
#include <in_sock.h>
#include <es_enum.h>
#include <e32property.h>
#include <sacls.h>
#include <mpmpropertydef.h> 

// USERINCLUDE FILES
#include "mmsconninit.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
const TInt32 KMmsShortTimeout = 30; // seconds
// Interval must be long enough to give the connection time to recover
const TInt32 KMmsRetryInterval = 10; // seconds
const TInt KMmsSecondsToMilliseconds = 1000000;
// Only one retry - if that does not work, better reschedule the operation normally
const TInt KMmsMaxRetryCount = 1;
const TInt KMaxProxyPortLength = 6;
_LIT( KHttpSchemePart1, "http" );
_LIT( KHttpSchemePart2, "://" );
const TInt KMmsScheme1Length = 4;
const TInt KMmsHTTPSchemeLength = 7;


// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// ============================== LOCAL FUNCTIONS ==============================
// ============================== MEMBER FUNCTIONS =============================

// -----------------------------------------------------------------------------
// CMmsConnectionInitiator
// -----------------------------------------------------------------------------
//
CMmsConnectionInitiator::CMmsConnectionInitiator() : 
    CActive( EPriorityStandard )
    {   
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CMmsConnectionInitiator::ConstructL()
    {
    CActiveScheduler::Add( this );
    iTimerRunning = EFalse;
    }

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CMmsConnectionInitiator* CMmsConnectionInitiator::NewL()
    {
    CMmsConnectionInitiator* self = new ( ELeave ) CMmsConnectionInitiator;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    #ifdef _MMSCONNINIT_LOGGING_
    TMmsConnInitLogger::Log( _L("MmsConnInit constructed ") );
    #endif
    return self;
    }
    
// -----------------------------------------------------------------------------
// ~CMmsConnectionInitiator
// -----------------------------------------------------------------------------
//
CMmsConnectionInitiator::~CMmsConnectionInitiator()
    {
    // cancel closes our timer if it is running
    Cancel();
    // iConnection, iClientStatus and iArray are owned by caller
    
    // Just to be sure the Access point property goes, delete it
    // Never mind the error - this is the best effort.
    // If it does not work, what can we do. Question mark. (To avoid "worrying commets")
#ifdef _MMSCONNINIT_LOGGING_
    TMmsConnInitLogger::Log( _L("deleting KMPMPropertyKey") );    
#endif      
    RProperty::Delete( KMPMCathegory, KMPMPropertyKeyMMS );
    
    #ifdef _MMSCONNINIT_LOGGING_
    TMmsConnInitLogger::Log( _L("MmsConnInit destroyed ") );
    #endif
    }

// -----------------------------------------------------------------------------
// ConnectL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmsConnectionInitiator::ConnectL( 
    RConnection&  aConnection,
    CArrayFixFlat<TUint32>& aAccessPointArray, 
    TRequestStatus& aStatus )
    {
#ifdef _MMSCONNINIT_LOGGING_
    TMmsConnInitLogger::Log( _L("MmsConnInit::ConnectL") );
#endif
    iClientStatus  = &aStatus;
    iConnection    = &aConnection;
    iArray         = &aAccessPointArray;
    *iClientStatus = KRequestPending;
    iIndex = 0;
    iTimerRunning = EFalse;
    iRetryCount = 0;

    // No reason to continue if the array is empty
    if( aAccessPointArray.Count() == 0 )
        {
        User::RequestComplete( iClientStatus, KErrArgument );
        return; 
        }     
    
    if ( !CheckNetworkL() )
        {
        User::RequestComplete( iClientStatus, KErrCouldNotConnect );
        return;
        }
 
    // Let's not start the AO if we do not find IAP
    for( iIndex = 0; iIndex < iArray->Count(); ++iIndex )
        {
        TUint32 ap; // IAP to be resolved
        // index is safe
        if( ( FindIapL( iArray->At( iIndex ), ap ) ) )
            {
            // Let's override the preferences
            iPrefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);            
            //Start the connection
            StartConnectionL( ap );
            iStartTime.UniversalTime();
            SetActive();
            return;  
            }           
        // IAP not found but let's keep on looping
        }    
    //We looped through the array and did not succeed
    //We do not have any error value in the iStatus because we 
    //did not even try to connect
    User::RequestComplete( iClientStatus, KErrCouldNotConnect );  
    return;
    }

// -----------------------------------------------------------------------------
// GetParametersL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmsConnectionInitiator::GetParametersL( 
    TUint32 aAccessPointLink,
    TUint32& aAccessPoint,
    HBufC*& aServerUri,
    TBool& aProxyUsed,
    HBufC8*& aProxyAddress )
    {
    // Every function reading the commsDB are trapped because 
    // they may leave if the field is empty. We still may be 
    // able to continue browsing the DB further.
    // Exception: Functions reading long text are not trapped 
    // because they allocate memory.

#ifdef _MMSCONNINIT_LOGGING_    
    TMmsConnInitLogger::Log( _L("MmsConnInit::GetParametersL") );
#endif

    // Initialise OUT parameters
    aServerUri = NULL;
    aAccessPoint = 0;
    aProxyUsed = EFalse;
    aProxyAddress = NULL;
    
    // Initialise used variables
    TInt error = KErrNone;
    TUint32 num32Value = 0;
    TBuf <KCommsDbSvrMaxFieldLength> textValue; 
    TInt length = 0;
    CCommsDbTableView* view = NULL;

    // Connect to Database server and CommDb
    CCommsDatabase* db = CCommsDatabase::NewL( EDatabaseTypeUnspecified );
    CleanupStack::PushL( db );

    // Start from WAP_ACCESS_POINT table -> opening view to it
    view = db->OpenViewMatchingUintLC(
        TPtrC( WAP_ACCESS_POINT ),
        TPtrC( COMMDB_ID ), 
        aAccessPointLink );

    // If no record for 'aAccessPointLink' there's nothing to look for
    if( view->GotoFirstRecord() != KErrNone )
        {
        CleanupStack::PopAndDestroy( view );
        CleanupStack::PopAndDestroy( db );
        return;
        }

    // Get aServerUri (WAP_START_PAGE in CommDb)
    // ReadColumnLengthL only leaves if the column does not exist at all -
    // that means that the database structure is totally wrong, and we can't continue
    view->ReadColumnLengthL( TPtrC( WAP_START_PAGE ), length );
    // ReadLongTextL only leaves if the column does not exist in the table
    // or we are out of memory.
    aServerUri = view->ReadLongTextLC( TPtrC( WAP_START_PAGE ) );
    // If the function did not leave, we have aServerUri on the cleanup stack
    // It's length may be 0, but the pointer itself is not NULL
    if( length != 0 )
        {
        CheckWapStartPageL( aServerUri ); // tries to ensure proper syntax
        }
        
    // Get link to bearer
    // This function only leaves if the column does not exist in the table
    // This means the structure of the database is incorrect, and we cannot continue
    view->ReadTextL( TPtrC( WAP_CURRENT_BEARER ), textValue );
    
    // view must be deleted, but it is not on top of the CleanupStack
    CleanupStack::Pop( aServerUri );
    CleanupStack::PopAndDestroy( view );
    view = NULL;
    
    if( textValue.Length() == 0 )
        {
        // Impossible to continue because bearer (textValue) is needed to browse further
        CleanupStack::PopAndDestroy( db );
        return;
        }

    // Put aServerUri back on the cleanup stack
    CleanupStack::PushL( aServerUri );

    // Create new view to bearer table
    view = db->OpenViewMatchingUintLC( 
        textValue, 
        TPtrC( WAP_ACCESS_POINT_ID ), 
        aAccessPointLink );
    
    // There should be only one record with aAccessPointLink
    if( view->GotoFirstRecord() != KErrNone )
        {
        // If no record for 'aAccessPointLink' there's nothing to look for
        CleanupStack::PopAndDestroy( view ); // view
        CleanupStack::Pop( aServerUri );
        CleanupStack::PopAndDestroy( db );
        return;
        }

    // Get AccessPoint
    // This function leaves if the column has NULL value
    TRAP( error, view->ReadUintL( TPtrC( WAP_IAP ), num32Value ) );  
    
    // Current view not needed any more
    CleanupStack::PopAndDestroy( view ); // view
    view = NULL;
    
    if( error == KErrNone )
        {
        aAccessPoint = num32Value;
        }
    else
        {
        CleanupStack::Pop( aServerUri );
        CleanupStack::PopAndDestroy( db );
        return;
        }
    
    // Create view to IAP table
    view = db->OpenViewMatchingUintLC( TPtrC( IAP ), TPtrC( COMMDB_ID ), aAccessPoint );
    
    // There should be only one record with aAccessPoint
    if( view->GotoFirstRecord() != KErrNone )
        {
        // no record found
        CleanupStack::PopAndDestroy( view ); // view
        CleanupStack::Pop( aServerUri );
        CleanupStack::PopAndDestroy( db );
        aAccessPoint = 0; // Client will know that the procedure failed
        return;        
        }
        
    // Get servicetype and -identifier
    TRAP( error,
        {
        view->ReadTextL( TPtrC( IAP_SERVICE_TYPE ), textValue ); // read service type
        view->ReadUintL( TPtrC( IAP_SERVICE ), num32Value ); // read service identifier
        });
    if( error )
        {
        // Leave occurred, cleaning up
        CleanupStack::PopAndDestroy( view ); // view
        CleanupStack::Pop( aServerUri );
        CleanupStack::PopAndDestroy( db );
        aAccessPoint = 0; // Client will know that the procedure failed
        return;        
        }

    // Current view not needed any more
    CleanupStack::PopAndDestroy( view );
    view = NULL;
    
    // Create view to Proxy table
    view = db->OpenViewOnProxyRecordLC( num32Value, textValue );
    // There should be only one record matching service identifier and service type
    if( view->GotoFirstRecord() != KErrNone )
        {
        CleanupStack::PopAndDestroy( view ); // view
        CleanupStack::Pop( aServerUri );
        CleanupStack::PopAndDestroy( db );
        return;
        }
                
    // Get information whether proxy is used or not
    TRAP( error, view->ReadBoolL( TPtrC( PROXY_USE_PROXY_SERVER ), aProxyUsed ) );
    if ( error != KErrNone )
        {
        CleanupStack::PopAndDestroy( view ); // view
        CleanupStack::Pop( aServerUri );
        CleanupStack::PopAndDestroy( db );
        return;
        }
    
    // If proxy is to be used, get the related information
    if( aProxyUsed )
    	{
        // Get proxy port number
        TRAP( error, view->ReadUintL( TPtrC( PROXY_PORT_NUMBER ), num32Value ) );
        if( error != KErrNone )
            {
            CleanupStack::PopAndDestroy( view ); // view
            CleanupStack::Pop( aServerUri );
            CleanupStack::PopAndDestroy( db );
            return;
            }
            
        if( num32Value > KMaxTUint16 )
            {
            // Proxy port has too big value, access point is invalid
            CleanupStack::PopAndDestroy( view ); // view
            CleanupStack::Pop( aServerUri );
            CleanupStack::PopAndDestroy( db );
            
            User::Leave( KErrTooBig );
            }

    	// Following LIT is the proxy port number beginning with ":"
        _LIT( KProxyPort,":%d" );
        HBufC* tempProxyPort = NULL;  
        HBufC* tempProxyServerName = NULL;
        tempProxyServerName = view->ReadLongTextLC( TPtrC( PROXY_SERVER_NAME ) );
        tempProxyServerName->Des().Trim(); // remove possible spaces
        
        tempProxyPort = HBufC::NewLC( KMaxProxyPortLength );
        TPtr ptr2( tempProxyPort -> Des() );
        ptr2.Format( KProxyPort,num32Value ); 
        
        // Create a TPtr object to aProxyAddress
        // Proxy address must always be us-ascii or we are in trouble.
        TPtr ptr( tempProxyServerName -> Des() );
        aProxyAddress = HBufC8::NewLC(
            ( tempProxyServerName->Length() ) + ( tempProxyPort->Length() ) ); 
        aProxyAddress->Des().Copy( ptr );
        aProxyAddress->Des().Append( ptr2 );
        CleanupStack::Pop( aProxyAddress );
        CleanupStack::PopAndDestroy( tempProxyPort );
        CleanupStack::PopAndDestroy( tempProxyServerName );
        } // aProxyUsed
        
	CleanupStack::PopAndDestroy( view ); // view
	view = NULL;
    CleanupStack::Pop( aServerUri );
    CleanupStack::PopAndDestroy( db );

#ifdef _MMSCONNINIT_LOGGING_
    TPtr temp = aServerUri->Des();
    TMmsConnInitLogger::Log( _L("aServerUri: %S"), &temp );
    TMmsConnInitLogger::Log( _L("aAccessPoint: %d"), aAccessPoint ); 
    TMmsConnInitLogger::Log( _L("aProxyUsed: %d"), aProxyUsed );
    /*
    if( aProxyAddress != NULL )
        {
        TMmsConnInitLogger::Log( _L("aProxyAddress: %S"), &temp );
        }
    */        
#endif
    }
    
// -----------------------------------------------------------------------------
// CheckNetworkL
// -----------------------------------------------------------------------------
//
TBool CMmsConnectionInitiator::CheckNetworkL()
    {
#ifdef __WINS__
    // Emulator has network always "available"
    return ETrue;
#endif // __WINS__

    TInt value = 0;
    TInt error = RProperty::Get( KUidSystemCategory, KUidNetworkStatus.iUid, value );
    User::LeaveIfError( error );
    
#ifdef _MMSCONNINIT_LOGGING_
    if ( value == ESANetworkAvailable )
        {
        TMmsConnInitLogger::Log( _L("Network available") );
        }
    else
        {
        TMmsConnInitLogger::Log( _L("Network NOT available, value = %d"), value );
        }
#endif

    return ( value == ESANetworkAvailable );
    }

// -----------------------------------------------------------------------------
// CMmsConnectionInitiator::CheckWapStartPageL
// 
// Private function used only by GetParametersL. Made just for splitting too big
// GetParameters function. 
// Adds http scheme in the beginning of wap start page if it is missing. It does
// not add it into comms database. 
// 
// -----------------------------------------------------------------------------
//
void CMmsConnectionInitiator::CheckWapStartPageL(HBufC*& aUri)
    {
     
    aUri->Des().Trim();  // remove spaces 
     
    if ( aUri->Find(KHttpSchemePart2) > 0)
        {
        ;// OK, nothing needs to be done;
        }
    else if (aUri->Find(KHttpSchemePart2) == 0)
        {
        // NOK, adding 'KHttpSchemePart1' to the beginning of the uri
        HBufC* tmp = HBufC::NewMaxLC( aUri->Length() + KMmsScheme1Length );
        tmp->Des().Copy( KHttpSchemePart1() );
        tmp->Des().Append( aUri->Des() );
        CleanupStack::Pop( tmp );
        CleanupStack::PopAndDestroy( aUri );
        aUri = tmp;
        CleanupStack::PushL(aUri);  
             
        }
    else // HttpScheme2 not found in this case
        {
        // NOK, adding 'KHttpScheme' to the beginning of the uri
        
        HBufC* tmp = HBufC::NewMaxLC( aUri->Length() + KMmsHTTPSchemeLength );
        tmp->Des().Copy( KHttpSchemePart1() );
        tmp->Des().Append( KHttpSchemePart2() );
        tmp->Des().Append( aUri->Des() );

        CleanupStack::Pop( tmp );
        CleanupStack::PopAndDestroy( aUri );
        aUri = tmp;
        CleanupStack::PushL(aUri);        
        }
    }

// -----------------------------------------------------------------------------
// CMmsConnectionInitiator::FindIapL
// WAP access Point is given as in-parameter and IAP is 
// retruned as out-parameter
// Return value is EFalse if something goes wrong
// Leavings are trapped in case of error in reading CommsDb
// -----------------------------------------------------------------------------
//
TBool CMmsConnectionInitiator::FindIapL( TUint32 aWapAP, TUint32& aIap ) 
    {
    aIap = 0;
    TUint32 num32Value = 0;
    TInt error = KErrNone;
    CCommsDatabase* db = CCommsDatabase::NewL( EDatabaseTypeUnspecified );
    CleanupStack::PushL( db );
    CCommsDbTableView* view;

    TBuf<KCommsDbSvrMaxFieldLength> textValue; // Max limit defined by database

    // Start from the WAP access point table
    // If there is no WAP access point table, the function leaves.
    view=db->OpenViewMatchingUintLC( 
        TPtrC( WAP_ACCESS_POINT ),
        TPtrC( COMMDB_ID ), 
        aWapAP );
    if ( view->GotoFirstRecord() != KErrNone )
        {
        CleanupStack::PopAndDestroy( view );
        CleanupStack::PopAndDestroy( db );
        return EFalse;
        }

    // Link to bearer table
    TRAP( error, view->ReadTextL( TPtrC( WAP_CURRENT_BEARER ), textValue ) );
    if ( error != KErrNone)
        {
        CleanupStack::PopAndDestroy( view );
        CleanupStack::PopAndDestroy( db );
        return EFalse;
        }
    
    CleanupStack::PopAndDestroy( view ); // view
    view = NULL;
    
    // Found WAP access point. Now we need the WAP bearer
    view=db->OpenViewMatchingUintLC( textValue, TPtrC( WAP_ACCESS_POINT_ID ), 
        aWapAP );
    if ( view->GotoFirstRecord() != KErrNone )
        {
        CleanupStack::PopAndDestroy( view );
        CleanupStack::PopAndDestroy( db );
        return EFalse;
        }
    
    //Let's trap the leave if there is no IAP and try to continue normally
    TRAP( error, view->ReadUintL( TPtrC( WAP_IAP ), num32Value ) );
    CleanupStack::PopAndDestroy( view );
    CleanupStack::PopAndDestroy( db );
    if ( error == KErrNone )
        {
        aIap = num32Value;
        return ETrue;
        } 
    else
        {  
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CMmsConnectionInitiator::StartConnectionL
// Checks if there is already connections open and checks if the 
// connection could be reused or not.
// -----------------------------------------------------------------------------
//
void CMmsConnectionInitiator::StartConnectionL(TUint32 aIap)
    {
#ifdef _MMSCONNINIT_LOGGING_    
    TMmsConnInitLogger::Log( _L("CMmsConnectionInitiator::StartConnectionL") );
#endif
    TUint count; 
    TPckgBuf<TConnectionInfo> connInfo;
    TUint i;
    TUint32 iap;
    HBufC* gprsApn1 = NULL; 
    HBufC* gprsApn2 = NULL; 
    TUint32 pdpType1 = 0;    
    TUint32 pdpType2 = 0;      
    iap = aIap;

#ifdef _MMSCONNINIT_LOGGING_    
    TMmsConnInitLogger::Log( _L("iConnection->EnumerateConnections") );
#endif   
    iConnection->EnumerateConnections( count );

#ifdef _MMSCONNINIT_LOGGING_    
    TMmsConnInitLogger::Log( _L("Connection count: %d"),count);
#endif
   
    if ( count > 0 && FindGprsParametersL( aIap,pdpType1,gprsApn1 ) )
        {
        CleanupStack::PushL(gprsApn1);
        for ( i=1; i<=count; i++ )
            {         
            if ( gprsApn2 )
                {
                delete gprsApn2;
                gprsApn2 = NULL;
                } 
#ifdef _MMSCONNINIT_LOGGING_    
    TMmsConnInitLogger::Log( _L("iConnection->GetConnectionInfo ") );
#endif 
            iConnection->GetConnectionInfo( i,connInfo );
#ifdef _MMSCONNINIT_LOGGING_    
    TMmsConnInitLogger::Log( _L("FindGprsParametersL ") );
#endif
            if ( FindGprsParametersL( connInfo().iIapId,pdpType2,gprsApn2 ) 
                && pdpType1 == pdpType2 
                && *gprsApn1 == *gprsApn2 ) 
                {
#ifdef _MMSCONNINIT_LOGGING_    
    TMmsConnInitLogger::Log( _L("APN MATCH !!!!!!!") );
#endif
                iap = connInfo().iIapId;  
                }
            }
        }    
    if ( gprsApn2 )
        {
        delete gprsApn2;
        }
    if ( gprsApn1 )
        {
        CleanupStack::PopAndDestroy( gprsApn1 );
        }

    iPrefs.SetIapId( iap );
#ifdef _MMSCONNINIT_LOGGING_    
    TMmsConnInitLogger::Log( _L("iConnection->Start ") );
#endif
    TInt error = RProperty::Define( KMPMCathegory, KMPMPropertyKeyMMS, KMPMPropertyTypeMMS ); 
    if ( error == KErrNone || error == KErrAlreadyExists )
        {
        // If the setting of the value does not succeed, a disconnect dialog
        // may be displayed - but there is nothing we can do about it.
        // We try our best.
#ifdef _MMSCONNINIT_LOGGING_    
        TMmsConnInitLogger::Log( _L("setting KMPMProperty key to %d "), iap );
#endif
        error = RProperty::Set( KMPMCathegory, KMPMPropertyKeyMMS, iap );
        }
    // iConnection cannot be active when we come here.
    // And it is not possible to ask ir RConnection is active
    iConnection->Start( iPrefs, iStatus );
    return;
    }

// -----------------------------------------------------------------------------
// CMmsConnectionInitiator::FindGprsParametersL
// Solves PDP type and GPRS Access Point. 
// -----------------------------------------------------------------------------

TBool CMmsConnectionInitiator::FindGprsParametersL(
    TUint32 aIap, TUint32& aPdpType, HBufC*& aGprsApn )
    {
#ifdef _MMSCONNINIT_LOGGING_    
    TMmsConnInitLogger::Log( _L("CMmsConnectionInitiator::FindGprsParameters") );
    TMmsConnInitLogger::Log( _L("IAP to analyze: %d"),aIap);
#endif
    TUint32 service=0;
    TBuf  <KCommsDbSvrMaxFieldLength> textValue; 
    CCommsDbTableView* view = NULL; 

    CCommsDatabase* db = CCommsDatabase::NewL( EDatabaseTypeUnspecified );
    CleanupStack::PushL( db ); 
    view = db->OpenViewMatchingUintLC( TPtrC( IAP ), TPtrC( COMMDB_ID ), aIap );
    if ( view->GotoFirstRecord() == KErrNone )
        {
        view->ReadTextL( TPtrC( IAP_SERVICE_TYPE ), textValue );
#ifdef _MMSCONNINIT_LOGGING_  
        TMmsConnInitLogger::Log (_L("IAP_SERVICE_TYPE: "));
        TMmsConnInitLogger::Log(textValue); 
#endif
        if ( textValue.Compare( TPtrC( OUTGOING_GPRS ) ) != 0 )
            {
            CleanupStack::PopAndDestroy( view );
            CleanupStack::PopAndDestroy( db );
            return EFalse;
            }
        view->ReadUintL( TPtrC( IAP_SERVICE ), service );
        }
    else
        {
        CleanupStack::PopAndDestroy( view );
        CleanupStack::PopAndDestroy( db );
        return EFalse;
        }
    CleanupStack::PopAndDestroy( view ); //view
    view = NULL;

    view = db->OpenViewMatchingUintLC( textValue, TPtrC( COMMDB_ID ), service );
    if ( view->GotoFirstRecord() == KErrNone )
        {
        aGprsApn = view->ReadLongTextLC( TPtrC( GPRS_APN ) );
        view->ReadUintL( TPtrC( GPRS_PDP_TYPE ),aPdpType );               
#ifdef _MMSCONNINIT_LOGGING_ 
        TMmsConnInitLogger::Log( _L("GPRS_APN: "));    
        TMmsConnInitLogger::Log(aGprsApn->Des()); 
        TMmsConnInitLogger::Log( _L("aPdpType: %d"),aPdpType);
#endif   
        CleanupStack::Pop( aGprsApn );
        CleanupStack::PopAndDestroy( view );
        CleanupStack::PopAndDestroy( db );
        }
    else
        {
        CleanupStack::PopAndDestroy( view );
        CleanupStack::PopAndDestroy( db );
        return EFalse;
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// RunL
// -----------------------------------------------------------------------------
//
void CMmsConnectionInitiator::RunL()
    {
    // immediately release the access point
#ifdef _MMSCONNINIT_LOGGING_
    TMmsConnInitLogger::Log( _L("deleting KMPMPropertyKeyMMS") );    
#endif      
    RProperty::Delete( KMPMCathegory, KMPMPropertyKeyMMS );
    
    iConnection->Progress( iProgress );
#ifdef _MMSCONNINIT_LOGGING_     
    TMmsConnInitLogger::Log( _L("MmsConnInit::RunL, status: %d"), iStatus.Int() );    
    TMmsConnInitLogger::Log( _L(" - progress stage: %d, progress error: %d"), iProgress.iStage, iProgress.iError );
#endif      

    // In error situation, set iStatus to the error
    // error form iProgress overrides status only if status is KErrNone
    // The behavior of lower level components has changed.
    // We are trying to adjust our behavior to the new situation.
    
    if ( iTimerRunning )
        {
        // own timer timed out
        iTimer.Close();
        iStatus = KErrCompletion;
        iTimerRunning = EFalse;
        }
    
    if ( iStatus.Int() == KErrNone )
        {
        iStatus = iProgress.iError;
        }
        
    if ( iStatus.Int() == KErrNotFound || iStatus.Int() == KErrGeneral )
        {
        // We should never pass KErrNotFound upwards as it will
        // prevent retry loop, and might prevent fetching altogether.
        // KErrGeneral does not mean anything, so we try if we can find
        // a better error code from progress.
        if ( iProgress.iError < KErrGeneral )
            {
            // We have some error code in iProgress, and it is not
            // KErrNotFound or KErrGeneral
            iStatus = iProgress.iError;
            }
        else
            {
            // This is better than KErrNotFound.
            // It does not tell why the connection was not created,
            // but at least it leads to retry loop
            iStatus = KErrCouldNotConnect;
            }
        }

    if( iStatus.Int() == KErrNone )
        {
        // iConnection.start completes with no error
        // In this succesfull case we set the client status
        // to index+1 of the AP array
#ifdef _MMSCONNINIT_LOGGING_ 
        TMmsConnInitLogger::Log( _L("- Connected") );
#endif
        User::RequestComplete( iClientStatus, ( iIndex + 1 ) );    
        return;
        }
    else // status not equal to KErrNone
        { 
        TTime now;
        now.UniversalTime();
        TTimeIntervalSeconds timeout = 0;
        TInt overflow;
        if ( iStatus.Int() == KErrTimedOut )
            {
#ifdef _MMSCONNINIT_LOGGING_ 
            TMmsConnInitLogger::Log( _L("- Timed out") );
#endif
            overflow = now.SecondsFrom( iStartTime, timeout );
            if ( overflow == KErrNone &&
                timeout.Int() < KMmsShortTimeout &&
                iRetryCount < KMmsMaxRetryCount )
                {
                // worth retrying after a few seconds
                iRetryCount++;
                TInt error = iTimer.CreateLocal();
                if ( error == KErrNone )
                    {
                    // timer wants microseconds
                    // iTimer cannot be running as it was closed just a while ago
                    // And we just said "CreateLocal()" so this is a brand new timer.
                    iTimer.After( iStatus, KMmsRetryInterval * KMmsSecondsToMilliseconds );
                    iTimerRunning = ETrue;
                    SetActive();
                    return;
                    }
                }
            }
        
#ifdef _MMSCONNINIT_LOGGING_ 
        TMmsConnInitLogger::Log( _L("- Not connected") );
#endif
        // Because we now completed with an error we must re-issue the 
        // request if there are still items in the array
        TUint32 ap; //IAP to be resolved
        if ( iStatus != KErrCompletion )
            {
            // KErrCompletion means our timer completed
            // and we must retry the same access point.
            // If we have some other error, we must try next access point
            iIndex++;
            }
        else
            {
#ifdef _MMSCONNINIT_LOGGING_ 
            TMmsConnInitLogger::Log( _L("- Timer completed - retry %d"), iRetryCount );
#endif
            }
        for( ; iIndex < iArray->Count(); ++iIndex )
            {
            // index is safe
            if( ( FindIapL( iArray->At( iIndex ), ap ) ) )
                {
                StartConnectionL( ap );
                SetActive();
                return;
                }      
            // Loop until AP found
            }   
        // Looped through the whole array with no success
        User::RequestComplete( iClientStatus, iStatus.Int() );
        return;
        }
    } 
    
// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
TInt CMmsConnectionInitiator::RunError(TInt aError)
    {
    User::RequestComplete( iClientStatus, aError );
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CMmsConnectionInitiator::DoCancel
// Cancels any outstanding request
// -----------------------------------------------------------------------------
//
void CMmsConnectionInitiator::DoCancel()
    {
    //This function is called by Cancel() if there is an
    //request outstanding.
#ifdef _MMSCONNINIT_LOGGING_
    TMmsConnInitLogger::Log( _L("deleting KMPMPropertyKeyMMS") );    
#endif      
    RProperty::Delete( KMPMCathegory, KMPMPropertyKeyMMS );
    if ( iTimerRunning )
        {
        iTimer.Cancel();
        iTimer.Close();
        iTimerRunning = EFalse;
        }
    iConnection->Close();
    User::RequestComplete( iClientStatus, KErrCancel );
#ifdef _MMSCONNINIT_LOGGING_ 
    TMmsConnInitLogger::Log( _L("MmsConnInit cancelled") );
#endif
    }

// -----------------------------------------------------------------------------
#ifdef _MMSCONNINIT_LOGGING_
const TInt KLogBufferLength = 256;
_LIT(KLogDir, "mmss");
_LIT(KLogFile, "mmsconninit.txt");

void TMmsConnInitLogger::Log(TRefByValue<const TDesC> aFmt,...)
    {
    VA_LIST list;
    VA_START(list, aFmt);

    // Print to log file
    TBuf<KLogBufferLength> buf;
    buf.FormatList(aFmt, list);

    // Write to log file
    RFileLogger::Write(KLogDir, KLogFile, EFileLoggingModeAppend, buf);
    }
#endif    

// =========================== OTHER EXPORTED FUNCTIONS ========================

//  End of File  
