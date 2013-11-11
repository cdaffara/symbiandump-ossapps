/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Part of SyncML Data Synchronization Plug In Adapter
*
*/



// INCLUDE FILES
#include <txtrich.h>
#include <smsclnt.h>
#include <smutset.h>
#include <smuthdr.h>
#include <smscmds.h>
#include <sysutil.h> 
#include <gsmuelem.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfldst.h>
#include "smsadaptermsvapi.h"
#include "logger.h" 
#include "vmessageparser.h"
#include "smsdataproviderdefs.h"


// CONSTANTS

_LIT16(KSmsNonUnicodeChars, "èéùìòÇØøÅåÆæßÉ£$¥¡ÄÖÑÜ§¿äöñüà");
        
// OTHER DEFINITIONS



// ============================= LOCAL FUNCTIONS ===============================



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSmsAdapterMsvApi::NewL
// -----------------------------------------------------------------------------
//
 CSmsAdapterMsvApi* CSmsAdapterMsvApi::NewL()
    {
    CSmsAdapterMsvApi* self = new( ELeave ) CSmsAdapterMsvApi;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CSmsAdapterMsvApi::~CSmsAdapterMsvApi()
// -----------------------------------------------------------------------------
//
CSmsAdapterMsvApi::~CSmsAdapterMsvApi()
    {
    LOGGER_ENTERFN( "CSmsAdapterMsvApi::~CSmsAdapterMsvApi" );
    SAFEDELETE( iContactsDb );
	SAFEDELETE( iMtm );
	SAFEDELETE( iMtmReg );     
    SAFEDELETE( iSession );
    LOGGER_LEAVEFN( "CSmsAdapterMsvApi::~CSmsAdapterMsvApi" );
    }
       
// -----------------------------------------------------------------------------
// CSmsAdapterMsvApi::CSmsAdapterMsvApi
// -----------------------------------------------------------------------------
//
CSmsAdapterMsvApi::CSmsAdapterMsvApi()
    {     
    }

// -----------------------------------------------------------------------------
// CSmsAdapterMsvApi::ConstructL
// -----------------------------------------------------------------------------
//
void CSmsAdapterMsvApi::ConstructL()
    {
    LOGGER_ENTERFN( "CSmsAdapterMsvApi::ConstructL" );

    iSession = CMsvSession::OpenSyncL( *this );
    iMtmReg = CClientMtmRegistry::NewL( *iSession );    
	iMtm = static_cast<CSmsClientMtm*>( iMtmReg->NewMtmL(KUidMsgTypeSMS) ); 

	iFs = iSession->FileSession();
	iMessageDrive = MessageServer::CurrentDriveL( iFs );
	
	iContactsDb = CContactDatabase::OpenL();

    LOGGER_LEAVEFN( "CSmsAdapterMsvApi::ConstructL" ); 
    }

// -----------------------------------------------------------------------------
// CSmsAdapterMsvApi::AddSML
// -----------------------------------------------------------------------------
//
void CSmsAdapterMsvApi::AddSML(
    CVMessageParser& aSm,
    TMsvId aFolder,                                                                                                                                                                                                        
    TMsvId& aSmId )
    {
    LOGGER_ENTERFN( "CSmsAdapterMsvApi::AddSML" );
   
    if (!ValidFolderL( aFolder ))
        {
		LOGGER_WRITE( "AddSML: wrong folder" );
        User::Leave( KErrArgument );
        }
           
    // Set first default flags  
    TMsvEntry newEntry;  
    newEntry.iType = KUidMsvMessageEntry;
    newEntry.iServiceId = KMsvLocalServiceIndexEntryId;
    newEntry.iMtm = KUidMsgTypeSMS;
    newEntry.SetVisible(EFalse);
    newEntry.SetInPreparation(ETrue);           

    // Create new message entry
    CMsvEntry* entry = iSession->GetEntryL( aFolder );
    CleanupStack::PushL( entry );   
    entry->CreateL( newEntry );
    aSmId = newEntry.Id();  
    entry->SetEntryL(newEntry.Id());
    
    // Create message header

    CParaFormatLayer* paraFormatLayer = CParaFormatLayer::NewL();
    CleanupStack::PushL( paraFormatLayer );
    CCharFormatLayer* charFormatLayer = CCharFormatLayer::NewL();
    CleanupStack::PushL( charFormatLayer );
    CRichText* richText = CRichText::NewL( paraFormatLayer, charFormatLayer );
    CleanupStack::PushL( richText ); 
    
    CSmsPDU::TSmsPDUType pduType;
    
    if (aFolder == KMsvGlobalInBoxIndexEntryId)
        {
        pduType = CSmsPDU::ESmsDeliver;
        }
    else if (aFolder == KMsvGlobalOutBoxIndexEntryId ||
             aFolder == KMsvDraftEntryId ||
             aFolder == KMsvSentEntryId)  
        {
        pduType = CSmsPDU::ESmsSubmit;
        }
    else if (aSm.iRecipients.Count() > 0)
        {
        pduType = CSmsPDU::ESmsSubmit;
        }
    else
        {
        pduType = CSmsPDU::ESmsDeliver;
        }       
    
    CSmsHeader* smsHeader = CSmsHeader::NewL( pduType, *richText );
    CleanupStack::PushL( smsHeader );
    
    // Set the message header in the entry's store
    CMsvStore* store = entry->EditStoreL();
    CleanupStack::PushL( store );
    smsHeader->StoreL( *store );
    store->StoreBodyTextL( *richText );
    store->CommitL();
    
    CleanupStack::PopAndDestroy( store );
    CleanupStack::PopAndDestroy( smsHeader );
    CleanupStack::PopAndDestroy( richText );
    CleanupStack::PopAndDestroy( charFormatLayer );
    CleanupStack::PopAndDestroy( paraFormatLayer );
    CleanupStack::PopAndDestroy( entry );
   
	DoUpdateSML( aSmId, aSm, ETrue );
	LOGGER_LEAVEFN( "CSmsAdapterMsvApi::AddSML" );
    }

// -----------------------------------------------------------------------------
// CSmsAdapterMsvApi::ReplaceSML
// -----------------------------------------------------------------------------
//
 void CSmsAdapterMsvApi::ReplaceSML( TMsvId aSmId, CVMessageParser& aSm ) 
    {
    LOGGER_ENTERFN( "CSmsAdapterMsvApi::ReplaceSML" );
	DoUpdateSML( aSmId, aSm, EFalse );
	LOGGER_LEAVEFN( "CSmsAdapterMsvApi::ReplaceSML" );
    }
    
// -----------------------------------------------------------------------------
// CSmsAdapterMsvApi::DeleteSML
// -----------------------------------------------------------------------------
//
 void CSmsAdapterMsvApi::DeleteSML( TMsvId aSmId )
    {
    LOGGER_ENTERFN( "CSmsAdapterMsvApi::DeleteSML" );
    
    iMtm->SwitchCurrentEntryL( aSmId );
    
    TMsvEntry tEntry = iMtm->Entry().Entry();  		
    if (tEntry.iType != KUidMsvMessageEntry || tEntry.iMtm != KUidMsgTypeSMS)
        {
        LOGGER_WRITE( "Not SMS entry" );
        User::Leave(KErrNotSupported);
        }
    
	iMtm->SwitchCurrentEntryL( tEntry.Parent() );
	iMtm->Entry().DeleteL( aSmId );
	LOGGER_LEAVEFN( "CSmsAdapterMsvApi::DeleteSML" );
    }
    
// -----------------------------------------------------------------------------
// CSmsAdapterMsvApi::DeleteUserFolderL
// -----------------------------------------------------------------------------
//
 TInt CSmsAdapterMsvApi::DeleteUserFolderL( TMsvId aUid )
    {
    LOGGER_ENTERFN( "CSmsAdapterMsvApi::DeleteUserFolderL" );
    
    iMtm->SwitchCurrentEntryL( aUid );
    CMsvEntry& entry = iMtm->Entry();
    TMsvEntry tEntry = entry.Entry();  		
    
    if (tEntry.iType != KUidMsvFolderEntry || tEntry.Parent() != KMsvMyFoldersEntryIdValue)
        {
        LOGGER_WRITE( "Not correct folder" );
        User::Leave(KErrNotSupported);
        }
        
    CMsvEntrySelection* children = entry.ChildrenL();
    TInt count = children->Count();
    delete children;
    
  	if (count > 0)
	    {
	    LOGGER_WRITE( "Folder not empty" );
	    return KErrInUse;
	    }
	    
	tEntry.SetReadOnly(EFalse);
	entry.ChangeL( tEntry );
       
	iMtm->SwitchCurrentEntryL( tEntry.Parent() );	
	iMtm->Entry().DeleteL( aUid );
	
	LOGGER_LEAVEFN( "CSmsAdapterMsvApi::DeleteUserFolderL" );
	return KErrNone;
    }   

// -----------------------------------------------------------------------------
// CSmsAdapterMsvApi::RetrieveSML
// -----------------------------------------------------------------------------
//
 void CSmsAdapterMsvApi::RetrieveSML(
    TMsvId aSmId,
    TMsvId& aParent,
    CVMessageParser& aSm,
    TBool& aUnread)
    {
    LOGGER_ENTERFN( "CSmsAdapterMsvApi::RetrieveSML" );
                          
    iMtm->SwitchCurrentEntryL( aSmId );
    iMtm->LoadMessageL();
    
    CRichText& mtmBody = iMtm->Body();
	aSm.StoreMessageBodyL( mtmBody );
       
    TMsvEntry tEntry = iMtm->Entry().Entry();
    
    aUnread = tEntry.Unread();
	if (aUnread)
		{
		aSm.iStatus = KVMsgStatusUnread;
		}
	else
		{
		aSm.iStatus = KVMsgStatusRead;
		}
	
	aSm.iUniversalTime = tEntry.iDate;	
	aSm.iHomeTime = HomeTimeFromUniversalTime( aSm.iUniversalTime );
    
	aParent = tEntry.Parent();	
	switch (aParent)
		{
		case KMsvGlobalInBoxIndexEntryId:
			aSm.iFolder = KFolderInbox;
			break;
		case KMsvGlobalOutBoxIndexEntryId:
			aSm.iFolder = KFolderOutbox;
			break;
		case KMsvDraftEntryId:
			aSm.iFolder = KFolderDraft;
			break;
		case KMsvSentEntryId:
			aSm.iFolder = KFolderSent;
			break;
		case KMsvMyFoldersEntryIdValue:
		    aSm.iFolder = KFolderMyFolders;    
			break;
		default:
		    TPtrC folderName;
		    TTime time;
		    TBool found = FindUserFolderL(aParent, folderName, time);
		    if (found && folderName.Length() <= KMaxFolderNameLength)
		        {
		        aSm.iFolder = folderName;
		        }
		    else
		        {
		        LOGGER_WRITE_1( "Not folder name found for folder: %d", aParent );
		        }       	
		}
		LOG( aSm.iFolder );
		
	const CSmsHeader& smsHeader = iMtm->SmsHeader();
	
	if ( smsHeader.Type() == CSmsPDU::ESmsDeliver )
	    {
		TPtrC fromAddr = smsHeader.FromAddress();
		aSm.ParseTelephoneNumber( fromAddr, aSm.iSender );
		}
	else
		{
		const CMsvRecipientList& recipients = iMtm->AddresseeList();
		for (TInt i = 0; i < recipients.Count(); i++)
			{
			CVMessageParser::TTelephoneNumber recipientInfo;
			aSm.ParseTelephoneNumber( recipients[i], recipientInfo );
			aSm.iRecipients.Append( recipientInfo );
			}
		}	

	LOGGER_LEAVEFN( "CSmsAdapterMsvApi::RetrieveSML" );
    }

// -----------------------------------------------------------------------------
// CSmsAdapterMsvApi::SendSML
// -----------------------------------------------------------------------------
//
 void CSmsAdapterMsvApi::SendSML( TMsvId aSmId )
    {
    LOGGER_ENTERFN( "CSmsAdapterMsvApi::SendSML" );
   
	iMtm->SwitchCurrentEntryL( aSmId );

	TMsvEntry msvEntry = iMtm->Entry().Entry();	
	
	if (msvEntry.Parent() != KMsvGlobalOutBoxIndexEntryId)
		{
 		LOGGER_WRITE_1( "Wrong folder, parent: %d", msvEntry.Parent() );
 		return;
 		}

	iMtm->LoadMessageL();

	msvEntry.SetInPreparation( EFalse );
	msvEntry.SetSendingState( KMsvSendStateWaiting );
	msvEntry.iDate.UniversalTime();
	
	iMtm->RestoreServiceAndSettingsL();
	CSmsHeader& header = iMtm->SmsHeader();
	
	CSmsSettings* sendOptions = CSmsSettings::NewL();
	CleanupStack::PushL( sendOptions );
		
	sendOptions->CopyL( iMtm->ServiceSettings() );
	sendOptions->SetDelivery( ESmsDeliveryImmediately );

	TSmsDataCodingScheme::TSmsAlphabet dataCoding = TSmsDataCodingScheme::ESmsAlphabet7Bit;
	CRichText& msgBody = iMtm->Body();
	HBufC* msgBodyBuf = HBufC::NewLC( msgBody.DocumentLength() );
	TPtr16 ptrBody = msgBodyBuf->Des();
	msgBody.Extract( ptrBody, 0, msgBody.DocumentLength() );
	LOG(ptrBody);

	for (TInt i = 0; i < ptrBody.Length(); i++)
		{
		if (IsUnicode( ptrBody[i] ))
			{
			LOGGER_WRITE_1( "Character %d is unicode", i );
			dataCoding = TSmsDataCodingScheme::ESmsAlphabetUCS2;
			break;
			}
		}

	sendOptions->SetCharacterSet( dataCoding );
	CleanupStack::PopAndDestroy( msgBodyBuf ); 
		
	header.SetSmsSettingsL( *sendOptions );
	if( header.Message().ServiceCenterAddress().Length() == 0 )
		{
		LOGGER_WRITE( "header.Message().ServiceCenterAddress().Length() == 0" );    
		
		CSmsSettings* serviceSettings = &( iMtm->ServiceSettings() );
        
		if (!serviceSettings->ServiceCenterCount())
			{
			LOGGER_WRITE("Cervice Center not found, could not send message");
			User::Leave( KErrCompletion );
			}
		else
			{
			CSmsServiceCenter* sc =  &serviceSettings->GetServiceCenter( serviceSettings->DefaultServiceCenter() );
			header.Message().SetServiceCenterAddressL( sc->Address() );	
			}				
		}

    const CMsvRecipientList& addrList = iMtm->AddresseeList();
    if ( addrList.Count() == 0 )
        {
		LOGGER_WRITE( "SendSML: no recipient" );
        User::Leave( KErrGeneral );
        }
   
	CMsvEntry* entry = &( iMtm->Entry() );
	entry->ChangeL( msvEntry );	
	iMtm->SaveMessageL();

	CMsvEntrySelection* sel = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL( sel );
	sel->AppendL( entry->EntryId() );

	TBuf8<1> dummy;
	CMsvOperationActiveSchedulerWait* waiter = CMsvOperationActiveSchedulerWait::NewLC();
	waiter->iStatus = KRequestPending;
	CMsvOperation* op = iMtm->InvokeAsyncFunctionL( ESmsMtmCommandScheduleCopy, *sel, dummy, waiter->iStatus );
	CleanupStack::PushL( op );
	waiter->Start();
	
	LOGGER_WRITE_1( "InvokeAsyncFunctionL: status %d", waiter->iStatus.Int());
    
	CleanupStack::PopAndDestroy( 4 ); // op, waiter, sel, sendOptions	
	
	LOGGER_LEAVEFN( "CSmsAdapterMsvApi::SendSML" );
    }
    
// -----------------------------------------------------------------------------
// CSmsAdapterMsvApi::HandleSessionEventL
// -----------------------------------------------------------------------------
//
void CSmsAdapterMsvApi::HandleSessionEventL( TMsvSessionEvent aEvent, TAny*, TAny*, TAny* )
    {
    LOGGER_WRITE_1( "CSmsAdapterMsvApi::HandleSessionEventL: %d", aEvent);
    
    switch ( aEvent )
        {
        case EMsvCloseSession: // The client should immediately close the session with the Message Server.
        case EMsvServerTerminated: // The Message Server has been terminated.
                                   // All clients must close their sessions immediately. 
            {
            if (iSession)
                {
				SAFEDELETE( iMtm );
				SAFEDELETE( iMtmReg );
                SAFEDELETE( iSession );
                }
            }   
            break;
        
        default:
            // Nothing is done
            break;
        } 
    }

// -----------------------------------------------------------------------------
// CSmsAdapterMsvApi::MsvSession
// -----------------------------------------------------------------------------
//
CMsvSession* CSmsAdapterMsvApi::MsvSessionL()
	{
	if (!iSession)
		{
		User::Leave( KErrGeneral );
		}
	return iSession;
	}

// -----------------------------------------------------------------------------
//	CSmsAdapterMsvApi::CleanFolderGetMsvIdsL
// -----------------------------------------------------------------------------
//
CMsvEntrySelection* CSmsAdapterMsvApi::CleanFolderGetMsvIdsL(TMsvId aFolderId)
    {
    LOGGER_ENTERFN( "CMsvEntrySelection::CleanFolderGetMsvIdsL" );
    
    CMsvEntry* cEntry = iSession->GetEntryL( KMsvRootIndexEntryId );
    CleanupStack::PushL( cEntry );
    
    cEntry->SetEntryL( aFolderId );
    cEntry->SetSortTypeL( TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, EFalse ) );
    CMsvEntrySelection* msvEntrySelection = cEntry->ChildrenWithMtmL( KUidMsgTypeSMS );
    
    CleanupStack::PopAndDestroy( cEntry );
    CleanupStack::PushL( msvEntrySelection );
    
    for (TInt i = 0; i < msvEntrySelection->Count(); i++)
        {
        CMsvEntry* entry = iSession->GetEntryL( msvEntrySelection->At(i) );
        CleanupStack::PushL( entry );
        TMsvEntry tEntry = entry->Entry();
        tEntry.SetReadOnly( EFalse );
        entry->ChangeL( tEntry );
        DeleteSML( msvEntrySelection->At(i) );
        CleanupStack::PopAndDestroy(); //entry
        }
        
    CleanupStack::Pop(msvEntrySelection);
    LOGGER_LEAVEFN( "CSmsAdapterMsvApi::CleanFolderGetMsvIdsL" );
    return msvEntrySelection;
    }
    
// -----------------------------------------------------------------------------
//	CSmsAdapterMsvApi::CleanFolderL
// -----------------------------------------------------------------------------
//
void CSmsAdapterMsvApi::CleanFolderL(TMsvId aFolderId)
    {
    CMsvEntrySelection* msvEntrySelection = CleanFolderGetMsvIdsL(aFolderId);
    delete msvEntrySelection;
    }
    
// -----------------------------------------------------------------------------
// CSmsAdapterMsvApi::CleanUserFoldersL
// Cleans all user folders from SMS messages
// -----------------------------------------------------------------------------
void CSmsAdapterMsvApi::CleanUserFoldersL() 
    {
    LOGGER_ENTERFN( "CSmsAdapterMsvApi::CleanUserFoldersL" );
    
    // Get the folder	
	CMsvEntry* msvEntry = iSession->GetEntryL(KMsvMyFoldersEntryIdValue);
	CleanupStack::PushL(msvEntry);
	
	// Find all of it's childs
	CMsvEntrySelection* folders = msvEntry->ChildrenWithTypeL(KUidMsvFolderEntry);
	CleanupStack::PopAndDestroy(msvEntry); 
	CleanupStack::PushL(folders);
	
	TInt error(KErrNone);

    for (TInt index = 0; index < folders->Count(); index++)
        {
        TMsvId folderId = folders->At(index);
        
        if (folderId != KMsvMyFoldersTemplatesFolderId)
            {
            CleanFolderL(folderId);
            error = DeleteUserFolderL(folderId);
            if (error != KErrNone)
                {
                // Note: folder is not deleted if contains other message items (like MMS)
                // In this case DeleteUserFolderL returns KErrInUse.
                LOGGER_WRITE_1("iMsvApi->DeleteUserFolderL failed with %d", error);
                }
            }
        }
	
	CleanupStack::PopAndDestroy(folders);
	
	// Delete also SMS messages directly under My Folders
	CleanFolderL(KMsvMyFoldersEntryIdValue);
	LOGGER_LEAVEFN( "CSmsAdapterMsvApi::CleanUserFoldersL" );
    }
   
// -----------------------------------------------------------------------------
//	CSmsAdapterMsvApi::DiskSpaceBelowCriticalLevelL
// -----------------------------------------------------------------------------
//
TBool CSmsAdapterMsvApi::DiskSpaceBelowCriticalLevelL( TInt aDataSize )
    {
    return SysUtil::DiskSpaceBelowCriticalLevelL( &iFs, aDataSize, iMessageDrive );
    }

// -----------------------------------------------------------------------------
//	CSmsAdapterMsvApi::UpdateSMStatusL
// -----------------------------------------------------------------------------
//
void CSmsAdapterMsvApi::UpdateSMStatusL( TMsvId aSmId, CVMessageParser &aSm )
    {
	LOGGER_ENTERFN( "CSmsAdapterMsvApi::UpdateSMStatusL" );

	iMtm->SwitchCurrentEntryL( aSmId );

	CMsvEntry& msvEntry = iMtm->Entry();
	const TMsvEntry& oldEntry = msvEntry.Entry();
	
	TMsvEntry newEntry( oldEntry );

	// STATUS
	if (aSm.iStatus.Compare( KVMsgStatusUnread ) == 0)
		{
		newEntry.SetUnread( ETrue );
		}
	else if (aSm.iStatus.Compare( KVMsgStatusRead ) == 0)
		{
		newEntry.SetUnread( EFalse );
		}
	else 
		{
		LOGGER_WRITE( "Unexpected status, not updated" );
		LOG( aSm.iStatus );
		}

	msvEntry.ChangeL( newEntry );
	LOGGER_LEAVEFN( "CSmsAdapterMsvApi::UpdateSMStatusL" );
    }

// -----------------------------------------------------------------------------
//	CSmsAdapterMsvApi::DoUpdateSML
// -----------------------------------------------------------------------------
//
void CSmsAdapterMsvApi::DoUpdateSML( TMsvId aSmId, CVMessageParser &aSm, TBool aNewSm )
    {
	LOGGER_WRITE_1( "CSmsAdapterMsvApi::DoUpdateSML: %d", aSmId );
	TInt i;

	iMtm->SwitchCurrentEntryL( aSmId );
	iMtm->LoadMessageL();	
	CMsvEntry& msvEntry = iMtm->Entry();
	const TMsvEntry& oldEntry = msvEntry.Entry();
	TMsvEntry newEntry(oldEntry);

	// Set message status
	if (aSm.iStatus.Compare( KVMsgStatusUnread ) == 0)
		{
		newEntry.SetUnread( ETrue );
		}
	else if (aSm.iStatus.Compare( KVMsgStatusRead ) == 0)
		{
		newEntry.SetUnread( EFalse );
		}
	else if (aNewSm)
		{
		newEntry.SetUnread( EFalse ); // by default msg is not unread, if we don't know
		}	

	// Set time. store format is universal
	if ( aSm.iUniversalTime.Int64() > 0 )
		{
		newEntry.iDate = aSm.iUniversalTime;
		}
	else if ( aSm.iHomeTime.Int64() > 0 )
		{
		newEntry.iDate = UniversalTimeFromHomeTime( aSm.iHomeTime );
		}
	else
		{
		newEntry.iDate.UniversalTime();
		}	
	
	// ADDRESS INFORMATION
	TMsvId parent = newEntry.Parent();
	LOGGER_WRITE_1( "Parent is %d", parent );
	
	TBuf<KNameMaxLength> addrBookName;
	
	CSmsHeader& header = iMtm->SmsHeader();
	
	if (header.Type() == CSmsPDU::ESmsDeliver)
		{
		if (aSm.iSender.iName.Length() > 0)
			{
			newEntry.iDetails.Set( aSm.iSender.iName );
			header.SetFromAddressL( aSm.iSender.iName );
			}
		else if (aSm.iSender.iNumber.Length() > 0)
			{
			FetchNameFromContactsL(aSm.iSender.iNumber, addrBookName);
			if (addrBookName.Length() > 0)
				{
				newEntry.iDetails.Set( addrBookName );
				header.SetFromAddressL( addrBookName );
				}
			else
				{		
				newEntry.iDetails.Set( aSm.iSender.iNumber );
				header.SetFromAddressL( aSm.iSender.iNumber );
				}
			}
		CSmsMessage& smsMsg = header.Message();
		CSmsPDU& smsPdu = smsMsg.SmsPDU();
		CSmsDeliver* smsDeliver = reinterpret_cast<CSmsDeliver*>( &smsPdu );	
		smsDeliver->SetServiceCenterTimeStamp( newEntry.iDate );
		}
	else // message to be sent
		{
		if (!aNewSm)
			{
			const CMsvRecipientList& addrList = iMtm->AddresseeList();
    		TInt numOldAddr = addrList.Count();		
    		for (i = 0; i < numOldAddr; i++)
    			{
    			iMtm->RemoveAddressee( i );
    			}	
			}		
		
		TInt numRecipients = aSm.iRecipients.Count();
		
		if (numRecipients > 0)
			{
			if (aSm.iRecipients[0].iName.Length() > 0)
				{
				newEntry.iDetails.Set( aSm.iRecipients[0].iName );
				}
			else if (aSm.iRecipients[0].iNumber.Length() > 0)
				{
				FetchNameFromContactsL(aSm.iRecipients[0].iNumber, addrBookName);
				if (addrBookName.Length() > 0)
					{
					newEntry.iDetails.Set( addrBookName );
					}
				else
					{
					newEntry.iDetails.Set( aSm.iRecipients[0].iNumber );
					}	
				}
			}
			
		for (i = 0; i < numRecipients; i++)
			{
			if (aSm.iRecipients[i].iNumber.Length() > 0)
				{
				if (aSm.iRecipients[i].iName.Length() > 0)
					{
					iMtm->AddAddresseeL( aSm.iRecipients[i].iNumber, aSm.iRecipients[i].iName );
					}
				else
					{
					FetchNameFromContactsL( aSm.iRecipients[i].iNumber, addrBookName );
					iMtm->AddAddresseeL( aSm.iRecipients[i].iNumber, addrBookName );
					}	
				}
			} 			
		} // else

	// MESSAGE BODY
	LOGGER_WRITE( "Add message body" );
	CRichText& mtmBody = iMtm->Body();
	mtmBody.Reset();
	aSm.LoadMessageBodyL( mtmBody );

    TBuf<KSmsDescriptionLength> description;
	description.Zero();
	if (aSm.iMessageBody)
		{
		TPtrC16 leftBody = aSm.iMessageBody->Left( KSmsDescriptionLength );	
		description.Copy( leftBody );
		
		for (i = 0; i < description.Length(); i++)
			{
			if (description[i] == '\n' || description[i] == '\r')
				{
				description[i] = ' ';
				}
			}	
		newEntry.iDescription.Set( description );
		}
		
	newEntry.SetVisible( ETrue );
    newEntry.SetComplete( ETrue );
    newEntry.SetInPreparation( EFalse );    
    newEntry.SetSendingState( KMsvSendStateUponRequest );		

	msvEntry.ChangeL( newEntry );

	LOGGER_WRITE( "Save message" );
	iMtm->SaveMessageL(); 
	
	LOGGER_LEAVEFN( "CSmsAdapterMsvApi::DoUpdateSML" );		
    }

// -----------------------------------------------------------------------------
// CSmsAdapterMsvApi::ValidUserFolder
// -----------------------------------------------------------------------------
//
TBool CSmsAdapterMsvApi::ValidFolderL( TMsvId aFolder, TBool aOutboxValid )
    {
    TBool valid(EFalse);
    
    switch ( aFolder )
        {
        case KMsvGlobalInBoxIndexEntryId:
            valid = ETrue;  
            break;
        case KMsvDraftEntryId:
           valid = ETrue;  
           break;        
        case KMsvSentEntryId:
           valid = ETrue;  
           break;
        case KMsvGlobalOutBoxIndexEntryId:
            if (aOutboxValid)
                {
                valid = ETrue;
                }
            break;
        case KMsvMyFoldersEntryIdValue:
           valid = ETrue;  
           break;             
        default:
            valid = FindUserFolderL(aFolder);
        }
    
    return valid;          
    }

// -----------------------------------------------------------------------------
// CSmsAdapterMsvApi::FindUserFolderL
// -----------------------------------------------------------------------------
//    
TBool CSmsAdapterMsvApi::FindUserFolderL(TMsvId aFolder, TPtrC& aName, TTime& aDate)
    {
    TBool found(EFalse);
    
    CMsvEntry* entry = iSession->GetEntryL( KMsvMyFoldersEntryIdValue );
    CleanupStack::PushL( entry );
     
    CMsvEntrySelection* selection = entry->ChildrenL();
    CleanupStack::PushL( selection );
    
    TMsvId serviceId;
    TMsvEntry entryT;

    for( TInt i = 0; i < selection->Count(); i++ )
        {
        User::LeaveIfError( iSession->GetEntry( selection->At( i ), serviceId, entryT ) );
        
        if ( !entryT.Deleted() && entryT.iType == KUidMsvFolderEntry && entryT.Id() == aFolder )
            {
            found = ETrue;
            aDate = entryT.iDate;
            aName.Set(entryT.iDetails);
            break;
            }
        }
    
    CleanupStack::PopAndDestroy( 2 ); // entry, selection    
    
    return found;        
    }
    
// -----------------------------------------------------------------------------
// CSmsAdapterMsvApi::FindUserFolderL
// -----------------------------------------------------------------------------
//    
TBool CSmsAdapterMsvApi::FindUserFolderL(TMsvId aFolder)
    {
    TPtrC name;
    TTime time;
    
    return FindUserFolderL(aFolder, name, time); 
    }    
    
// -----------------------------------------------------------------------------
// CSmsAdapterMsvApi::FindUserFolderL
// -----------------------------------------------------------------------------
//    
TBool CSmsAdapterMsvApi::FindUserFolderL(const TDesC& aName, TMsvId& aFolder)
    {
    CMsvEntry* entry = iSession->GetEntryL( KMsvMyFoldersEntryIdValue );
    CleanupStack::PushL( entry );
     
    CMsvEntrySelection* selection = entry->ChildrenL();
    CleanupStack::PushL( selection );
    
    TBool found(EFalse);
    TMsvId serviceId;
    TMsvEntry entryT;

    for( TInt i = 0; i < selection->Count(); i++ )
        {
        User::LeaveIfError( iSession->GetEntry( selection->At( i ), serviceId, entryT ) );
        
        if ( !entryT.Deleted() && entryT.iType == KUidMsvFolderEntry && 
            aName.Compare(entryT.iDescription) == 0 )
            {
            found = ETrue;
            aFolder = entryT.Id();
            break;
            }
        }
    
    CleanupStack::PopAndDestroy( 2 ); // entry, selection    
    
    return found;           
    }

// -----------------------------------------------------------------------------
// CSmsAdapterMsvApi::IsUnicode
// -----------------------------------------------------------------------------
//
TBool CSmsAdapterMsvApi::IsUnicode( const TUint16 aValue )
    {
	if ( aValue > 0x7F && KSmsNonUnicodeChars().Locate( aValue ) < 0 )
		{
		LOGGER_WRITE_1( "IsUnicode: Found UC char %d", aValue );
		return ETrue;
		}
	else
		{
		return EFalse;
		}
    }

// -----------------------------------------------------------------------------
// CSmsAdapterMsvApi::MoveSML
// Moves SM to another folder.
// -----------------------------------------------------------------------------
void CSmsAdapterMsvApi::MoveSML( TMsvId aSmId, TMsvId aParentId )
    {
    LOGGER_ENTERFN( "CSmsAdapterMsvApi::MoveSML starts" );
    
    if ( !ValidFolderL( aParentId ) )
        {
		LOGGER_WRITE( "MoveSML: wrong folder" );
        User::Leave( KErrArgument );
        }
	
	// Find the parent
	CMsvEntry* clientEntry( NULL );
	
	// Find this entry and put it to cleanup stack
	clientEntry = iSession->GetEntryL( aSmId );
	CleanupStack::PushL( clientEntry );
	
	// Check that this is a SMS message
	TMsvEntry entryT = clientEntry->Entry();
	if (entryT.iType != KUidMsvMessageEntry || entryT.iMtm != KUidMsgTypeSMS)
	    {
	    LOGGER_WRITE( "MoveSML: wrong entry type" );
	    User::Leave( KErrArgument );
	    }
   
	// Find parent id, we'll be moving it's childs
	TMsvId parentId = entryT.Parent();
	
	// Make sure that the parent has changed
	if (parentId != aParentId)
		{
	    // Set parent as context
	    clientEntry->SetEntryL( parentId );

		// Move the child item to another branch, use temporary waiter object
		CMsvOperationActiveSchedulerWait* w = CMsvOperationActiveSchedulerWait::NewLC();
		CMsvOperation* op = clientEntry->MoveL( aSmId, aParentId, w->iStatus );
		w->Start();
		SAFEDELETE( op );
		CleanupStack::PopAndDestroy();
		
		}
	else
		{
		LOGGER_WRITE( "CSmsAdapterMsvApi::MoveSML, identical parents." );
		}		  
	
 	CleanupStack::PopAndDestroy(); // entry
 	
 	LOGGER_LEAVEFN( "CSmsAdapterMsvApi::MoveSML" );
    }

// -----------------------------------------------------------------------------
// CSmsAdapterMsvApi::AddUserFolderL
// Creates new user folder
// -----------------------------------------------------------------------------        
TInt CSmsAdapterMsvApi::AddUserFolderL( TMsvId& aFolder, const TDesC& aName )
    {
    LOGGER_ENTERFN( "CSmsAdapterMsvApi::AddUserFolderL" );
    LOG(aName);
        
    // Make sure that we are not going to add same folder twise
    TBool found(EFalse);
    found = FindUserFolderL(aName, aFolder);
    if ( found )
        {
        LOGGER_WRITE( "Folder already exists" );
        return KErrNone;
        } 
        
    CMsvEntry* entry = iSession->GetEntryL(KMsvMyFoldersEntryIdValue);
    CleanupStack::PushL( entry );
    
    TTime date;
    date.UniversalTime();      
    
    TMsvEntry folderEntry;
    folderEntry.iType = KUidMsvFolderEntry;
    folderEntry.iMtm = KUidMsvLocalServiceMtm;
    folderEntry.iDetails.Set(aName);   
    folderEntry.iServiceId = KMsvLocalServiceIndexEntryIdValue;
    folderEntry.iSize = sizeof(folderEntry);
    folderEntry.iDate = date;
    folderEntry.SetStandardFolder(EFalse);
    folderEntry.SetVisible(ETrue);
    folderEntry.SetComplete(ETrue);
    folderEntry.SetInPreparation(EFalse); 
    folderEntry.SetReadOnly(EFalse);  
    
    entry->CreateL(folderEntry);
    CleanupStack::PopAndDestroy(entry);
    
    aFolder = folderEntry.Id();
    LOGGER_LEAVEFN( "CSmsAdapterMsvApi::AddUserFolderL" );
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CSmsAdapterMsvApi::UpdateUserFolderL
// Updates user folder (changes name)
// -----------------------------------------------------------------------------    
TInt CSmsAdapterMsvApi::UpdateUserFolderL( TMsvId aFolder, const TDesC& aName )
    {
    LOGGER_ENTERFN( "CSmsAdapterMsvApi::UpdateUserFolderL" );
    LOG(aName);
    
    CMsvEntry* entry = iSession->GetEntryL( aFolder );
    CleanupStack::PushL( entry );
    
    TMsvEntry tEntry = entry->Entry();
    
    if ( tEntry.iType != KUidMsvFolderEntry )
        {
        CleanupStack::PopAndDestroy( entry );
        LOGGER_WRITE( "No message folder" );
        return KErrGeneral;
        }
       
    tEntry.iDetails.Set(aName);   
    tEntry.iDescription.Set(aName);
    
    entry->ChangeL(tEntry);
    
    CleanupStack::PopAndDestroy( entry );
    LOGGER_LEAVEFN( "CSmsAdapterMsvApi::UpdateUserFolderL" );
    return KErrNone;
    } 

// -----------------------------------------------------------------------------
// CSmsAdapterMsvApi::UniversalTimeFromHomeTime
// Converts local time to universal time.
// -----------------------------------------------------------------------------    
TTime CSmsAdapterMsvApi::UniversalTimeFromHomeTime( TTime aTime )
	{	
	TLocale locale;
	locale.Refresh();
	
	TTimeIntervalSeconds universalTimeOffset = locale.UniversalTimeOffset();
	aTime -= universalTimeOffset;
	
	if (locale.QueryHomeHasDaylightSavingOn())
    	{
    	TTimeIntervalHours daylightSaving( 1 );
    	aTime -= daylightSaving;
    	}	
	
	return aTime;
	}

// -----------------------------------------------------------------------------
// CSmsAdapterMsvApi::HomeTimeFromUniversalTime
// Converts universal time to local time.
// -----------------------------------------------------------------------------    
TTime CSmsAdapterMsvApi::HomeTimeFromUniversalTime( TTime aTime )
	{
	TLocale locale;
	locale.Refresh();
	
	TTimeIntervalSeconds universalTimeOffset = locale.UniversalTimeOffset();
	aTime += universalTimeOffset;
	
	if (locale.QueryHomeHasDaylightSavingOn())
    	{
    	TTimeIntervalHours daylightSaving( 1 );
    	aTime += daylightSaving;
    	}
	
	return aTime;
	}
	
// -----------------------------------------------------------------------------
// CSmsAdapterMsvApi::FetchNameFromContactsL
// Searches contact name of given number from phone book
// If not found, empty descriptor is returned.
// -----------------------------------------------------------------------------    
void CSmsAdapterMsvApi::FetchNameFromContactsL(const TDesC& aNumber, TDes& aName)
	{
	LOGGER_ENTERFN( "CSmsAdapterMsvApi::FetchNameFromContactsL" );
	LOG(aNumber);
	
	aName.Zero(); 
	
	const TInt KNumDigitsToMatch(8);			
	CContactIdArray* contactIds = iContactsDb->MatchPhoneNumberL(aNumber, KNumDigitsToMatch);		
	if (contactIds->Count() != 1)
		{
		delete contactIds;
		return;
		}
	CleanupStack::PushL(contactIds);		
 
	CContactItem* item = iContactsDb->ReadContactLC((*contactIds)[0]);
	CContactItemFieldSet& fieldSet = item->CardFields();
		
	TPtrC familyName;
	TPtrC givenName;
	TInt pos;
	
	pos = fieldSet.Find(KUidContactFieldFamilyName);
	if (pos >= 0)
		{
		CContactItemField& itemField=fieldSet[pos];
		if (!(itemField.IsHidden()) && !(itemField.IsDisabled()))
			{
			CContactTextField* textField = itemField.TextStorage();
			familyName.Set(textField->Text());
			}				
		}
	pos = fieldSet.Find(KUidContactFieldGivenName);
	if (pos >= 0)
		{
		CContactItemField& itemField=fieldSet[pos];
		if (!(itemField.IsHidden()) && !(itemField.IsDisabled()))
			{
			CContactTextField* textField = itemField.TextStorage();
			givenName.Set(textField->Text());
			}				
		}	
	
	TInt spaceLeft = aName.MaxLength();
	
	if (familyName.Length() <= spaceLeft)
		{
		aName.Append(familyName);
		aName.Trim();
		spaceLeft -= aName.Length();
		}
		
	if ((givenName.Length() + 1) <= spaceLeft)
		{
		aName.Append(' ');
		aName.Append(givenName);
		aName.Trim();
		}
		
	LOG(aName);	
	
	CleanupStack::PopAndDestroy(2); // item, contactIds
	LOGGER_LEAVEFN( "CSmsAdapterMsvApi::FetchNameFromContactsL" );
	}
		
// End of file
