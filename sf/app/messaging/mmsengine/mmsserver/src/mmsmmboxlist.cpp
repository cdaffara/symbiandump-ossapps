/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     State machine for mmbox list
*
*/



// INCLUDE FILES
#include    <logcli.h>
#include    <msvids.h>
#include    <msventry.h>
#include    <msvstd.h>
#include    <e32std.h> // TTime

// the rest are local includes
#include    "mmsmmboxlist.h"
#include    "mmsconst.h"
#include    "mmsencode.h"
#include    "mmsdecode.h"
#include    "mmsheaders.h"
#include    "mmsmmboxviewheaders.h"
#include    "mmsserverentry.h"
#include    "mmssettings.h"
#include    "mmssession.h"
#include    "mmserrors.h"
#include    "MmsServerDebugLogging.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  


// CONSTANTS
_LIT( K1970, "19700000:000000.000000" );    // 1-Jan 1970 0:00:00
// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMmsMmboxList::CMmsMmboxList
//
// ---------------------------------------------------------
//
CMmsMmboxList::CMmsMmboxList( RFs& aFs ):
    CMmsBaseOperation( aFs )
    {
    }

// ---------------------------------------------------------
// CMmsMmboxList::ConstructL
//
// ---------------------------------------------------------
//
void CMmsMmboxList::ConstructL( CMmsSettings* aMmsSettings )
    {
    CMmsBaseOperation::ConstructL( aMmsSettings );
    iMmsHeaders = CMmsHeaders::NewL( iMmsSettings->MmsVersion() );
    iOldNotifications = new ( ELeave ) CMsvEntrySelection;
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CMmsMmboxList::NewL
//
// ---------------------------------------------------------
//
CMmsMmboxList* CMmsMmboxList::NewL( RFs& aFs, CMmsSettings* aMmsSettings  )
    {
    CMmsMmboxList* self = new ( ELeave ) CMmsMmboxList( aFs );
    CleanupStack::PushL( self );
    self->ConstructL( aMmsSettings );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CMmsMmboxList::~CMmsMmboxList
//
// ---------------------------------------------------------
//
CMmsMmboxList::~CMmsMmboxList()
    {
    Cancel(); // has to be called first
    delete iOldNotifications;
    delete iMmsHeaders;
    }

// ---------------------------------------------------------
// CMmsMmboxList::StartL
//
// ---------------------------------------------------------
//
void CMmsMmboxList::StartL(
            CMsvEntrySelection& aSelection,
            CMsvServerEntry& aServerEntry,
            TMsvId aService,
            TRequestStatus& aStatus )
    {
    LOG( _L("CMmsMmboxList::StartL") );

    // Make sure that the aSelection is empty
    aSelection.Reset();
    CMmsBaseOperation::InitializeL( aSelection, aServerEntry, aService );
    iMmsHeaders->Reset();

    iOldNotifications->Reset();
    // get current notifications from mmbox folder 
    iMmboxFolder = iMmsSettings->MMBoxFolder();
    if ( iError == KErrNone )
        {
        iError = iServerEntry->SetEntry( iMmboxFolder );
        }
    // If cannot access MMBoxFolder, we are in trouble
    // When iError not equal to KErrNone, the operation will complete after running through RunL
    if ( iError == KErrNone )
        {
        TInt err;
        err = iServerEntry->GetChildrenWithMtm( KUidMsgMMSNotification, *iOldNotifications );
        if (err != KErrNone)
            {
            return;
            }
			iOldQuotaEntryId = OldQuotaEntryL();
        }
    else
        {
        iOldQuotaEntryId = KMsvNullIndexEntryId;
        }
    iServerEntry->SetEntry( KMsvNullIndexEntryId );

    Queue( aStatus );
    FallThrough();   
    }

// ---------------------------------------------------------
// CMmsMmboxList::DoCancel
//
// ---------------------------------------------------------
//
void CMmsMmboxList::DoCancel()
    {
    LOG( _L("CMmsMmboxList::DoCancel") );
    CMmsBaseOperation::DoCancel();
    }

// ---------------------------------------------------------
// CMmsMmboxList::EncodePDUL
//
// ---------------------------------------------------------
//
void CMmsMmboxList::EncodePDUL()
    {
    LOG( _L("CMmsMmboxList::EncodePDU") );

    // As no entry exists to be encoded, mmsheaders are not restored and stored.

    //  Set message type
    iMmsHeaders->SetMessageType( KMmsMessageTypeMboxViewReq );

    // Request mmbox total and quota information 
    CMmsMMBoxViewHeaders& viewHeaders = iMmsHeaders->MMBoxViewHeadersL();
    viewHeaders.SetMmsTotals( KMmsYes );
    viewHeaders.SetMmsQuotas( KMmsYes );

    // Set TransactionId
    TBufC8<KMMSMAXTIDLENGTH> tid;
    tid.Des().NumUC( AllocateTID(), EHex );
    iMmsHeaders->SetTidL( tid );

    // Encode the mmboxview request
    iEncoder->EncodeHeadersL( *iMmsHeaders, *iEncodeBuffer );

    FallThrough();
    }

// ---------------------------------------------------------
// CMmsMmboxList::SubmitTransactionL
//
// ---------------------------------------------------------
//
void CMmsMmboxList::SubmitTransactionL()
    {
    LOG( _L("CMmsMmboxList::SubmitTransaction"));

    if ( !iConnected )
        {
        if ( iError == KErrNone )
            {
            iError = KErrCouldNotConnect;
            }
        }
      
    // This check is needed only when running tests in global mode
    // if length of URI is 0, Symbian code will panic    
    if ( !iUri )
        {
        if ( !iMmsSettings->LocalMode() )
            {
            iError = KMmsErrorNoURI1;
            }
        }
    else if ( iUri->Des().Length() == 0 && !iMmsSettings->LocalMode() )
        {
        iError = KMmsErrorNoURI1;
        }

    if ( iError != KErrNone )
        {
        FallThrough();
        return;
        }

    if ( !iMmsSettings->LocalMode() )
        {
        // Send
        iMmsSession->SendMessageL(
            iUri->Des(),
            *iEncodeBuffer,
            *iEncoder,
            *iDecoder,
            iStatus );
        SetActive();
        }
    else
        {
        LocalModeFetchL();
        }
    }

// ---------------------------------------------------------
// CMmsMmboxList::CreateEntryL
//
// ---------------------------------------------------------
//
void CMmsMmboxList::CreateEntryL()
    {
    LOG( _L("CMmsMmboxList CreateEntry"));
    // Create an mms entry under mmbox folder
    if ( iOldQuotaEntryId != KMsvNullIndexEntryId )
        {
        // Reuse old quota
        iEntryUnderConstruction = iOldQuotaEntryId;
        }
    else
        {
        // If no old quota entry exists, we create a new one
        iError = iServerEntry->SetEntry( iMmboxFolder );
        if ( iError == KErrNone )
            {
            // set all relevant flags in tMsvEntry
            TMsvEntry tEntry;
            tEntry.iMtm = KUidMsgTypeMultimedia;
            tEntry.iDate.UniversalTime();
            SetFirstFlagsToNewEntry( tEntry );

            iError = iServerEntry->CreateEntry( tEntry );
            if ( iError == KErrNone )
                {
                iEntryUnderConstruction = tEntry.Id();
                }
            }
        iServerEntry->SetEntry( KMsvNullIndexEntryId );
        }
    FallThrough();
    }

// ---------------------------------------------------------
// CMmsMmboxList::DecodeResponseL
//
// ---------------------------------------------------------
//
void CMmsMmboxList::DecodeResponseL()
    {
    LOG( _L("CMmsMmboxList::DecodeResponseL"));
    if( iEncodeBuffer->Size() < 1 )
        {
        iError  = KErrCorrupt; 
        } 
    if( iError != KErrNone ) 
        {
        FallThrough();
        return;
        }

    iMmsHeaders->Reset();    
    // Decode mmbox view confirmation
    TInt numAtt = 0;
    TInt dataStart = 0;
    TInt startInBuffer = 0;
    TInt length = iEncodeBuffer->Size();
    iDecoder->DecodeHeadersL( *iEntryWrapper,
        *iMmsHeaders, 
        *iEncodeBuffer,
        startInBuffer,
        length,
        &numAtt,
        &dataStart );

    // The content type have to be mmbox view conf. 
    if ( iMmsHeaders->MessageType() != KMmsMessageTypeMboxViewConf )
        {
        // the content type is not correct. Don't save the headers
        if ( iMmsHeaders->MessageType() == KMmsMessageTypeMSendConf &&
             iMmsHeaders->ResponseStatus() == KMmsErrorUnsupportedMessage ) 
            {
            iError = KErrNotSupported;
            }
        else
            {
            iError = KErrCorrupt;
            }
        }
        
    if ( iError == KErrNone )
        {
        if ( iOldQuotaEntryId != KMsvNullIndexEntryId )
            {
            // We must make the entry read only again
            iError = iServerEntry->SetEntry( iOldQuotaEntryId );
            if ( iError == KErrNone )
                {
                TMsvEntry tEntry = iServerEntry->Entry();
                tEntry.SetReadOnly( EFalse );
                iServerEntry->ChangeEntry( tEntry );
                }
            iServerEntry->SetEntry( KMsvNullIndexEntryId );
            }
        iError = iEntryWrapper->SetCurrentEntry( iEntryUnderConstruction );
        }
    
    // save the mmbox view confirmation headers, 
    // as the decode does not save them
    if ( iError == KErrNone )
        {
        iError = iEntryWrapper->StoreMmsHeadersL( *iMmsHeaders, NULL );
        }
       
    // decode rest of the PDUs if we have any.
    TInt start = dataStart; // start of multipart data.
    for ( TInt i = 0; i < numAtt && iError == KErrNone; i++ )
        {
        length = iEncodeBuffer->Size() - start;
        // We extract next data part, but we don't use the mime headers that would be returned
        iDecoder->ExtractNextDataPartL( *iEncodeBuffer,
            start, // start of next data part
            dataStart, // start of actual data
            length ); // length of data

        // Create a new entry for the mmbox description PDU
        iError = iServerEntry->SetEntry( iMmboxFolder );
        TMsvEntry descEntry;
        if ( iError == KErrNone )
            {
            // set all relevant flags in tMsvEntry
            descEntry.iMtm = KUidMsgMMSNotification;
            SetFirstFlagsToNewEntry( descEntry );
            iError = iServerEntry->CreateEntry( descEntry );
            }

        if ( iError == KErrNone )
            {                         
            // Decode description PDU
            iMmsHeaders->Reset();
            TInt attaNum = 0;
            TInt attaDataStart = 0;
            iDecoder->DecodeHeadersL( *iEntryWrapper,
                *iMmsHeaders, 
                *iEncodeBuffer,
                dataStart,
                length,
                &attaNum,
                &attaDataStart );

            iError = iEntryWrapper->SetCurrentEntry( descEntry.Id() );
            }
            
        // Checking the content type
    	if( iError == KErrNone )
	        {
	        // The content type has to be mmbox view description.
    	    if( iMmsHeaders->MessageType() != KMmsMessageTypeMBoxDescr )
    	        {
                // Content type is not correct.
                iError = KErrCorrupt;
                LOG( _L("ERROR: Wrong content type"));
            	}
            else 
         	    {
                // Mark mmbox notification specific flags
    	        // Mark as notification. This information is important for UI.
                descEntry.iMtmData1 = KMmsMessageMNotificationInd; 
                descEntry.iMtmData2 = KMmsStoredInMMBox;
                descEntry.iMtmData2 |= KMmsNotifyResponseSent;
    	        }
	        }
        if( iError == KErrNone )
            {
            // Set subject etc. to the notification entry
            iError = StoreContentToNotificationEntryL( descEntry );
            }
        if ( iError == KErrNone )
            {
            // Save the headers as the decoder does not save them
            iError = iEntryWrapper->StoreMmsHeadersL( *iMmsHeaders, NULL );
            }
        if ( iError == KErrNone )
            {
            // add new notification to iSelection list
            iSelection->AppendL( descEntry.Id() );
            }
        } // for loop
  
    iServerEntry->SetEntry( KMsvNullIndexEntryId );
    FallThrough();
    }

// ---------------------------------------------------------
// CMmsMmboxList::MoveEntryL
//
// ---------------------------------------------------------
//
void CMmsMmboxList::MoveEntryL()
    {
    LOG( _L("CMmsMmboxList::MoveEntryL"));
    if ( iError != KErrNone )
        {
        FallThrough();
        return;
        }

    // Delete old notifications  
    iError = RemoveOldNotifications();

    // make new notifications visible
    if ( iError == KErrNone )
        {
        iError = MakeNewNotificationsVisible();
        }
        
    // Finalize the new quota entry - if we created a new one and did not
    // just reuse the old entry
    
    TMsvEntry tEntry;
    
    if ( iError == KErrNone || iOldQuotaEntryId == KMsvNullIndexEntryId )
        {
        iError = iServerEntry->SetEntry( iEntryUnderConstruction );
        if ( iError == KErrNone )
            {
            tEntry = iServerEntry->Entry();
            SetFlagsToEntryBeingFinalized( tEntry );
            // update the time the quota entry was updated
            tEntry.iDate.UniversalTime();
            tEntry.iError = KErrNone; // clear possible old error
            
            // Quota entry must not be visible.
            // Otherwise the shows the number of notifications in mmboxfolder wrong.
            tEntry.SetVisible( EFalse );
            iError = iServerEntry->ChangeEntry( tEntry );
            if ( iError == KErrNone )
                {
                iEntryUnderConstruction = KMsvNullIndexEntryId;
                }
            }
        iServerEntry->SetEntry( KMsvNullIndexEntryId );
        }
        
    FallThrough();
    }

// ---------------------------------------------------------
// CMmsMmboxList::FinalizeL()
//
// ---------------------------------------------------------
//
void CMmsMmboxList::FinalizeL()
    {
    LOG( _L("CMmsMmboxList::FinalizeL"));
    
    TInt error;
    
    // In case something has failed. Delete the new quota entry
    // - but only in case it was just created
    if ( iError != KErrNone )
        {
        error = iServerEntry->SetEntry( iMmboxFolder );
        if ( error == KErrNone )
            {
            // we can only delete, if we can set context to parent
            if ( iOldQuotaEntryId == KMsvNullIndexEntryId )
                {
                iServerEntry->DeleteEntry( iEntryUnderConstruction );
                }
            
            // if iSelection contains now notifications, the notifications
            // are not ready yet. They can be destroyed.
            if ( iSelection->Count() > 0 )
                {
                iServerEntry->DeleteEntries( *iSelection );
                }
            }
        iEntryUnderConstruction = KMsvNullIndexEntryId;
        
        iSelection->Reset();

        // we don't care if the iOldNotifications is empty or not. 
        // If the old notifications are not deleted, they can stay
        // in the mmbox folder. The array is reseted anyway.
        iOldNotifications->Reset();
        
        // set iError to old quota entry, if the entry exists
        if ( iOldQuotaEntryId != KMsvNullIndexEntryId )
            {

            LOG( _L("- old quota entry exists"));

            error = iServerEntry->SetEntry( iOldQuotaEntryId );
            if ( error == KErrNone )
                {
                TMsvEntry tEntry = iServerEntry->Entry();
                tEntry.iError = iError;
            
                error = iServerEntry->ChangeEntry( tEntry );
                if ( error == KErrNone )
                    {
                    LOG( _L("- iError saved to old quota entry."));                
                    }
                }
            }

        iServerEntry->SetEntry( KMsvNullIndexEntryId );
        }

    CMmsBaseOperation::FinalizeL();
    }

// ---------------------------------------------------------
// CMmsMmboxList::SetFirstFlagsToNewEntry
//
// ---------------------------------------------------------
//
void CMmsMmboxList::SetFirstFlagsToNewEntry( TMsvEntry& aEntry )
    {
    aEntry.iType = KUidMsvMessageEntry;
    aEntry.iServiceId = iService;
    aEntry.SetVisible( EFalse );
    aEntry.SetComplete( EFalse );
    aEntry.SetInPreparation( ETrue );
    aEntry.SetReadOnly( EFalse );
    aEntry.SetNew( ETrue );
    aEntry.SetUnread( ETrue );
    }

// ---------------------------------------------------------
// CMmsMmboxList::SetFlagsToEntryBeingFinalized
//
// ---------------------------------------------------------
//
void CMmsMmboxList::SetFlagsToEntryBeingFinalized( TMsvEntry& aEntry )
    {
    aEntry.iMtmData1 |= KMmsMessageMobileTerminated;
    aEntry.SetReadOnly( ETrue );
    aEntry.SetVisible( ETrue ); 
    aEntry.SetInPreparation( EFalse );
    aEntry.SetComplete( ETrue );
    }

// ---------------------------------------------------------
// CMmsMmboxList::RemoveOldNotifications
//
// ---------------------------------------------------------
//
TInt CMmsMmboxList::RemoveOldNotifications()
    {
    LOG( _L("CMmsMmboxList::RemoveOldNotifications"));

    TInt error = iServerEntry->SetEntry( iMmboxFolder );
    if ( error == KErrNone && iOldNotifications->Count() > 0 )
        {
        error = iServerEntry->DeleteEntries( *iOldNotifications );
        if ( error == KErrNone )
            {
            iOldNotifications->Reset();
            }
        }

    iServerEntry->SetEntry( KMsvNullIndexEntryId );
    return error;
    }

// ---------------------------------------------------------
// CMmsMmboxList::MakeNewNotificationsVisible
//
// ---------------------------------------------------------
//
TInt CMmsMmboxList::MakeNewNotificationsVisible()
    {
    LOG( _L("CMmsMmboxList::MakeNewNotificationsVisible"));
    TInt count = iSelection->Count();
    TInt error = KErrNone;
    for ( TInt i = count - 1; i >=0 && error == KErrNone; i-- )
        {
        error = iServerEntry->SetEntry( iSelection->At( i ) );
        if ( error == KErrNone )
            {                
            TMsvEntry tEntry = iServerEntry->Entry();
            SetFlagsToEntryBeingFinalized( tEntry );
            error = iServerEntry->ChangeEntry( tEntry );
            if ( error == KErrNone )
                {
                iSelection->Delete( i );
                }
            }
        }
    iServerEntry->SetEntry( KMsvNullIndexEntryId );
    return error;    
    }

// ---------------------------------------------------------
// CMmsMmboxList::StoreContentToNotificationEntryL
//
// ---------------------------------------------------------
//
TInt CMmsMmboxList::StoreContentToNotificationEntryL( TMsvEntry aEntry )
    {
    LOG( _L("CMmsMmboxList::SetContentToNotificationL"));
    
    // Expiry interval must be changed to absolute time,
    // otherwise it makes no sense.
    if ( iMmsHeaders->ExpiryDate() == 0 )
        {
        TTime time;
        // handle expiry in universal time
        time.UniversalTime();
        time += TTimeIntervalSeconds( iMmsHeaders->ExpiryInterval() );
        iMmsHeaders->SetExpiryDate(
            ( time.MicroSecondsFrom( TTime( KMmsYear1970String ) ).Int64() ) / KMmsMillion );
        }
        
    // If the notifications does not contains the date value from the server, 
    // arrival time is set to notification.
    // Otherwise the date from the server.

    LOG( _L("Mark time"));

    if ( iMmsHeaders->Date() == 0 )
        {
        aEntry.iDate.UniversalTime();
        }
    else
        {
        aEntry.iDate = ServerDate();
        } 
    
    // Mark subject
    if ( iMmsHeaders->Subject().Length() > 0 )
        {
        aEntry.iDescription.Set( iMmsHeaders->Subject() );
        }

    // Mark size of the notification
    aEntry.iSize = iMmsHeaders->Size();

    // Mark message class. 
    // Default is personal
    if ( iMmsHeaders->MessageClass() == EMmsClassAdvertisement )
        {
        aEntry.iMtmData1 |= KMmsMessageAdvertisement;
        }
    else if (iMmsHeaders->MessageClass() == EMmsClassInformational )
        {
        aEntry.iMtmData1 |= KMmsMessageInformational;
        }
    else
        {
        // keep LINT happy
        }
        
    switch ( iMmsHeaders->MessagePriority() )
        {
        case KMmsPriorityNormal:
            aEntry.SetPriority( EMsvMediumPriority );
            break;
        case KMmsPriorityLow:
            aEntry.SetPriority( EMsvLowPriority );
            break;
        case KMmsPriorityHigh:
            aEntry.SetPriority( EMsvHighPriority );
            break;
        default:            
            // if not defined default is normal
            aEntry.SetPriority( EMsvMediumPriority );
            break;
        }
        
    // mark sender
    // If the sender is a phone number, add alias.
    HBufC* buffer = HBufC::NewL( KMmsMaxDescription );
    TPtr pBuffer = buffer->Des();
    
    if ( TMmsGenUtils::GenerateDetails( iMmsHeaders->Sender(),
        pBuffer, KMmsMaxDescription, iFs ) == KErrNone )
        {
        aEntry.iDetails.Set( pBuffer );
        }
    else
        {
        // We come here only if there was an fatal error in GenerateDetails.
        // Even if we don't find the alias, we have something in the string
        aEntry.iDetails.Set( iMmsHeaders->Sender() );
        }

    TInt error = iServerEntry->ChangeEntry( aEntry );
    delete buffer;
    buffer = NULL;
    // iServerEntry is not set to KMsvNullIndexEntryId, 
    // as the next function demands that the iServerEntry is set to aEntry.
    return error;
    }

// ---------------------------------------------------------
// CMmsMmboxList::OldQuotaEntryL
//
// ---------------------------------------------------------
//
TMsvId CMmsMmboxList::OldQuotaEntryL()
    {
    LOG( _L("CMmsMmboxList::OldQuotaEntryL"));

    // iServerEntry context must be set to iMmboxFolder
    // before calling this function
    TMsvId oldQuotaEntryId = KMsvNullIndexEntryId;
    
    // show invisible entries.
    TMsvSelectionOrdering ordering =
        TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue );
    iServerEntry->SetSort( ordering );

    CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection; 
    CleanupStack::PushL( selection );
    TInt error = iServerEntry->GetChildrenWithMtm( KUidMsgTypeMultimedia, *selection );
    TInt count = selection->Count();

    LOG2( _L("- %d mms entries"), count );

    if( error == KErrNone && count > 0 )
        {
	    for( TInt i = selection->Count() - 1 ; i >= 0; i-- ) 
            {

    	    LOG2( _L("- processing at selection[%d]"), i );

            TMsvId entryId = selection->At( i );
            error = iServerEntry->SetEntry( entryId );
            if ( error == KErrNone )
                {
    	        TMsvEntry entry = iServerEntry->Entry();	
	            // old quota entry is complete, but not visible	
    	        if ( ( !entry.Visible() ) && entry.Complete() )
                    {
    	            oldQuotaEntryId = entryId;
                    i = -1;
                    
                    LOG( _L("- Old quota found"));
                    
	                }
                }
            }
        }
    CleanupStack::PopAndDestroy( selection );
        
    return oldQuotaEntryId;
    }

// ---------------------------------------------------------
// CMmsMmboxList::ServerDate
//
// ---------------------------------------------------------
//
TTime CMmsMmboxList::ServerDate()
    {
    
    LOG( _L("CMmsMmboxList::ServerDate"));

    TInt64 inSeconds = iMmsHeaders->Date();

    TTime y1970( K1970 );

    // 1970 presented as microseconds after January 1st, 0 AD nominal Gregorian.
    TInt64 ms1970 = y1970.Int64();

    // If not defined in message headers return 0
    if ( inSeconds == 0 )
        {
        return TTime(0);
        }

    // microseconds after 1.1. 1970
    TInt64 msAfter1970;
    msAfter1970 = KMmsMillion * inSeconds;

    return TTime( ms1970 + msAfter1970 );
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsMmboxList::LocalModeFetchL()
    {
// This functionality has no meaning in hardware
// It is available in WINS to help module testing
    
#ifdef __WINS__
    RFs fs; // We need a separate session to be able to set the session path
    RFile file; // local mode message
    iError = fs.Connect();
    if ( iError != KErrNone )
        {
        FallThrough();
        }
    
    CleanupClosePushL( fs );
    
    CDir* fileList = NULL;
  
    TEntry* entry = new( ELeave ) TEntry; // allocated from heap to save stack space
    CleanupStack::PushL( entry );
    HBufC* filename = HBufC::NewL( KMaxFileName );
    CleanupStack::PushL( filename );
    TPtr fileNamePtr = filename->Des();
    // use different directory in order to be able to simulate
    // both mmbox view and fetching the corresponding messages
    fileNamePtr.Copy( KMmsMMBoxDirectory );
    TInt err = fs.SetSessionPath( fileNamePtr );
    err = fs.Entry( fileNamePtr, *entry );
    TInt count = 0;
    if ( err == KErrNone )
        {
        TFindFile* finder = new( ELeave ) TFindFile( fs ); // allocated from heap to save stack space
        CleanupStack::PushL( finder );
        _LIT( KWild, "*" );

        err = finder->FindWildByPath( KWild, &fileNamePtr, fileList );
        CleanupStack::PopAndDestroy( finder );
        
        if ( fileList )
            {
            CleanupStack::PushL( fileList );
            count = fileList->Count();
            }
        }
        
        
    if ( count > 0 )
        {
        TParse parse;    
        parse.Set( ( ( *fileList )[0] ).iName, &fileNamePtr, NULL );
        fileNamePtr.Copy( parse.FullName() );
        TInt size = fs.Entry( parse.FullName(), *entry );
        size = entry->iSize;
        
        iError = file.Open( fs, fileNamePtr, EFileShareExclusive );
        if ( iError == KErrNone )
            {
            iEncodeBuffer->Reset();
            TRAP(iError, iEncodeBuffer->ResizeL( size ));
            if ( iError == KErrNone )
                {
                TPtr8 pos = iEncodeBuffer->Ptr( 0 );
                file.Read( 0, pos, size );
                }
            file.Close();
            // delete the file after the view has been fetched
            fs.Delete( fileNamePtr );
            }
        }

    if ( fileList )
        {
        CleanupStack::PopAndDestroy( fileList );
        }
            
    CleanupStack::PopAndDestroy( filename );
    CleanupStack::PopAndDestroy( entry );
    CleanupStack::PopAndDestroy( &fs ); // close fs
#endif

    FallThrough();
    }
    

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File
