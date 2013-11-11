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
* Description:  CS Message Handler, This class caches the conversation server
 *                with the message data and also sends run-time updates to the
 *                server with the message data
 *
*/


// USER INCLUDES
#include "ccsmsghandler.h"
#include "msgbiouids.h"
#include "ccsdefs.h"

// SYSTEM INCLUDES
#include <smsclnt.h>                   
#include <mmsclient.h>
#include <mtclreg.h>
#include <csmsemailfields.h>
#include <txtrich.h>
#include <msvids.h>
#include <e32const.h>
#include <SendUiConsts.h>
#include <pushentry.h> // For services messages
#include <e32cmn.h>


//CONSTANTS
const TInt KMessageArrayGranularity = 50;

// NOTE:- DRAFT ENTRIES ARE NOT HANDLED IN THE PLUGIN

// ============================== MEMBER FUNCTIONS ============================
// ----------------------------------------------------------------------------
// CCsMsgHandler::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
//
CCsMsgHandler* CCsMsgHandler::NewL(MCsMsgObserver *aMsgObserver)
    {
    PRINT ( _L("Enter CCsMsgHandler::NewL") );

    CCsMsgHandler* self = new ( ELeave ) CCsMsgHandler();
    CleanupStack::PushL( self );
    self->ConstructL(aMsgObserver);
    CleanupStack::Pop( self );

    PRINT ( _L("End CCsMsgHandler::NewL") );

    return self;
    }

// ----------------------------------------------------------------------------
// CCsMsgHandler::~CCsMsgHandler
// Destructor
// ----------------------------------------------------------------------------
//
CCsMsgHandler::~CCsMsgHandler()
    {
    PRINT ( _L("Enter CCsMsgHandler::~CCsMsgHandler") );
    if(iIdle)
        {
        iIdle->Cancel();
        delete iIdle;
        iIdle=NULL;
        }
    if(iMsgPluginUtility)
        {
        delete iMsgPluginUtility;
        iMsgPluginUtility = NULL;
        }

    if(iConverstationEntryList)
        {
        iConverstationEntryList->ResetAndDestroy();
        iConverstationEntryList->Close();
        delete iConverstationEntryList;
        iConverstationEntryList = NULL;
        }

    if(iSmsMtm)
        {
        delete iSmsMtm;
        iSmsMtm = NULL;
        }

    if(iMmsMtm)
        {
        delete iMmsMtm;
        iMmsMtm = NULL;
        }

    if(iMtmRegistry)
        {
        delete iMtmRegistry;
        iMtmRegistry = NULL;
        }

    if(iSession)
        {
        delete iSession;
        iSession = NULL;
        }

    if(iMessageArray)
    {
        iMessageArray->Reset();
        iMessageArray->Close();
        delete iMessageArray;
        iMessageArray = NULL;
    }

    PRINT ( _L("End CCsMsgHandler::~CCsMsgHandler") );
    }

// ----------------------------------------------------------------------------
// CCsMsgHandler::ConstructL
// Two Phase Construction
// ----------------------------------------------------------------------------
//
void CCsMsgHandler::ConstructL(MCsMsgObserver *aMsgObserver)
    {
    PRINT ( _L("Enter CCsMsgHandler::ConstructL") );

    iMsgObserver = aMsgObserver;

    iSession = CMsvSession::OpenSyncL(*this);

    iMsgPluginUtility = CCsMsgPluginUtility::NewL();

    iConverstationEntryList = new(ELeave) RPointerArray<CCsConversationEntry>(1);

    iMtmRegistry = CClientMtmRegistry::NewL( *iSession );

    iSmsMtm = static_cast<CSmsClientMtm*>( iMtmRegistry->
            NewMtmL( KSenduiMtmSmsUid ) );

    iMmsMtm = static_cast<CMmsClientMtm*>( iMtmRegistry->
            NewMtmL( KSenduiMtmMmsUid ) );

    iState = EReadInbox;

    iMessageArray = new (ELeave)RArray <TMsvId>(KMessageArrayGranularity);

    PRINT ( _L("End CCsMsgHandler::ConstructL") );
    }

// ----------------------------------------------------------------------------
// CCsMsgHandler::CCsMsgHandler
// Two Phase Construction
// ----------------------------------------------------------------------------
//
CCsMsgHandler::CCsMsgHandler()
    {
    }

// -----------------------------------------------------------------------------
// CCsMsgHandler::ProcessResultsL()
// ProcessResultsL fetches results for the searchsort query and
// updates the CCsServer with the fetched data
// -----------------------------------------------------------------------------
//
void CCsMsgHandler::ProcessResultsL(TMsvEntry entry)
    {
    PRINT ( _L("Enter CCsMsgHandler::ProcessResultsL"));

    // Cleanup the conversationEntry List initially.
    // So that the previous entries not cleaned up in
    // the event of Leave have been clean up.
    iConverstationEntryList->ResetAndDestroy();

    RPointerArray<HBufC> addressList;

    // Ignore hidden items during upload
    if ( entry.Visible() == EFalse )
        {
        return;
        }
    
    if ( entry.Parent() == KMsvGlobalInBoxIndexEntryIdValue ||
         entry.Parent() == KMsvSentEntryIdValue ||
         entry.Parent() == KMsvGlobalOutBoxIndexEntryIdValue )
        {
        CleanupResetAndDestroyPushL(addressList);
        
        iMsgPluginUtility->CreateContactL(iSession, entry, addressList);

        //process entry
        ExtractAddressesL(entry, ERead, addressList);
        
        // Cleanup addressList
        CleanupStack::PopAndDestroy(&addressList);
        }

    PRINT ( _L("Exit CCsMsgHandler::ProcessResultsL") );
    }

// ----------------------------------------------------------------------------
// CCsMsgHandler::HandleSessionEventL
// Implemented for MMsvSessionObserver
// ----------------------------------------------------------------------------
//
void CCsMsgHandler::HandleSessionEventL( TMsvSessionEvent aEvent,
        TAny* aArg1,
        TAny* aArg2,
        TAny* /*aArg3*/)
    {
    PRINT1 ( _L("Enter CCsMsgHandler::HandleSessionEventL aEvent=%d"),aEvent );

    CMsvEntrySelection* selection=NULL;
    TMsvId parent;

    //args
    if(aArg1 == NULL || aArg2 == NULL)
        {
        PRINT ( _L("Enter CCsMsgHandler::HandleSessionEventL arguments invalid"));
        return;
        }

    //start, processing the event
    selection= (CMsvEntrySelection*)aArg1;
    parent = *(TMsvId*)aArg2;

    // Cleanup the conversationEntry List initially.
    iConverstationEntryList->ResetAndDestroy();

    switch( aEvent )
        {
        case EMsvEntriesChanged:
        case EMsvEntriesMoved:
            {
            HandleEventL(selection,parent);
            }
            break;
            
        case EMsvEntriesDeleted:
            {
            if ( parent == KMsvGlobalInBoxIndexEntryIdValue || 
                 parent == KMsvSentEntryIdValue ||
                 parent == KMsvGlobalOutBoxIndexEntryIdValue)
                {
                for( TInt i=0 ; i < selection->Count() ; i++ )
                    {
                    TMsvId id = selection->At( i );

                    //use utility to create conversation entry
                    CCsConversationEntry *conversationEntry = 
                    iMsgPluginUtility->CreateConversationEntryL(
                            NULL,
                            id,
                            0,
                            parent,
                            NULL,
                            ECsSendStateUnknown,
                            ECsAttributeNone,
                            0);
                    CleanupStack::PushL(conversationEntry);
                    iConverstationEntryList->AppendL( conversationEntry );
                    CleanupStack::Pop(conversationEntry);

                    // call observer interface for each entry
                    iMsgObserver->HandleDeleteCompleteL(iConverstationEntryList);

                    //cleanup before next iteration
                    iConverstationEntryList->ResetAndDestroy();
                    }
                }
            }
            break;
            
        case EMsvMediaChanged:
            iMsgObserver->HandleRefreshCompleteL();
            break;
        }

    // Cleanup the conversationEntry List before function exits.
    iConverstationEntryList->ResetAndDestroy();
    PRINT ( _L("Exit CCsMsgHandler::HandleSessionEventL") );
    }

// ---------------------------------------------------------------------
// CCsMsgHandler::HandleEvent
// Handle events
// ---------------------------------------------------------------------
//
void CCsMsgHandler::HandleEventL(CMsvEntrySelection* aSelection, TMsvId aParent)
    {
    PRINT ( _L("Enter CCsMsgHandler::HandleEvent") );
    TMsvEntry entry;
    TMsvId service;
    TInt error= KErrNone;
    RPointerArray<HBufC> addressList;

    for( TInt i=0 ; i < aSelection->Count() ; i++ )
        {
        error = iSession->GetEntry(aSelection->At(i),service,entry);
        
        if (aParent == KMsvGlobalInBoxIndexEntryIdValue && KSenduiMtmSmsUidValue == entry.iMtm.iUid) 
						{
            iSmsMtm->SwitchCurrentEntryL(entry.Id());
            iSmsMtm->LoadMessageL();

            CSmsHeader& header = static_cast<CSmsClientMtm*> (iSmsMtm)->SmsHeader();
            CSmsPDU& pdu = header.Message().SmsPDU();
            TSmsDataCodingScheme::TSmsClass smsClass;

            if (pdu.Class(smsClass) && smsClass == TSmsDataCodingScheme::ESmsClass0
                && entry.ReadOnly()) 
							{
                iSmsMtm->SwitchCurrentEntryL(entry.iServiceId);
                // for class 0 sms dont create conversation entry.. 
                continue;
            	}

        	 }
        if (entry.Visible() == EFalse) 
						{
            // Do a delete if entry becomes invisible.
            // e.g) My Nokia registration messages.
            RPointerArray<CCsConversationEntry>* hiddenEntries =  
                    new (ELeave) RPointerArray<CCsConversationEntry>();
            
            CCsConversationEntry *conversationEntry = 
                    iMsgPluginUtility->CreateConversationEntryL ( 
                            NULL,
                            entry.Id(),
                            0,
                            aParent,
                            NULL,
                            ECsSendStateUnknown,
                            ECsAttributeNone,
                            0 );  
            CleanupStack::PushL(conversationEntry);
            hiddenEntries->AppendL( conversationEntry );
            CleanupStack::Pop(conversationEntry);
            
            // Delete at server
            iMsgObserver->HandleDeleteCompleteL(hiddenEntries);
            hiddenEntries->ResetAndDestroy();
            hiddenEntries->Close();
            delete hiddenEntries;
            
            break;
            }
        
        if ( (KErrNone == error) && (IsMtmSupported(entry.iMtm.iUid) ) )
            {
            // sent/inbox/Outbox entry
            if ( aParent == KMsvSentEntryIdValue ||
                 aParent == KMsvGlobalInBoxIndexEntryIdValue )
                {
                CleanupResetAndDestroyPushL(addressList);
                // currently server needs number, so getting it from header
                iMsgPluginUtility->CreateContactL(iSession, entry,
                        addressList);

                //process entry
                // entry created in sent already exists, hence an update
                ExtractAddressesL(entry, EUpdate, addressList);

                CleanupStack::PopAndDestroy(&addressList);
                }  
                
            //for drafts and outbox, the entry changes multiple times,
            // so making a check if entry has changed
            else if(aParent == KMsvGlobalOutBoxIndexEntryIdValue)
                {
                //outbox
                if(!(iMsgPluginUtility->CompareEntry(iPrevEntry,entry,aParent)))
                    {
                    TUint sendState = entry.SendingState();
                    
                    // if the entry state is sent, we dont need to process it
                    // as it is going to appear in sent items
                    if( ECsSendStateSent != iMsgPluginUtility->GetSendState(entry) )
                        {
                         CleanupResetAndDestroyPushL(addressList);
                        // currently server needs number, so getting it from header
                        iMsgPluginUtility->CreateContactL(iSession, entry,
                                addressList);
                        
                        //process entry
                        // entry created in sent already exists, hence an update
                        ExtractAddressesL(entry, EUpdate, addressList);
                        
                        iPrevEntry = entry;
                        
                        CleanupStack::PopAndDestroy(&addressList);
                        }
                    }//end check changed entry
                }
            }
        }//end for loop

    PRINT ( _L("Exit CCsMsgHandler::HandleEvent") );
    }

// ----------------------------------------------------------------------------
// CCsMsgHandler::CreateAndAddEntryL
// Creates CCsConversationEntry and adds to the list
// ----------------------------------------------------------------------------
//
void CCsMsgHandler::CreateAndAddEntryL(const TDesC& aContact, 
        const TDesC& aDescription, 
        const TMsvEntry& aEntry)
    {
    PRINT ( _L("Enter CCsMsgHandler::CreateAndAddEntryL") );

    HBufC* contact = NULL;
    HBufC* sDescription = NULL;

    if (aContact.Length()>0)
        {
        contact = aContact.AllocLC();
        }
    if (aDescription.Length()>0)
        {
        sDescription = aDescription.AllocLC();
        }

    //use utility to create conversation entry
    CCsConversationEntry *conversationEntry = iMsgPluginUtility->CreateConversationEntryL(
            contact,
            aEntry.Id(),
            aEntry.iDate.Int64(),
            aEntry.Parent(),
            sDescription,
            iMsgPluginUtility->GetSendState(aEntry),
            iMsgPluginUtility->GetMsgAttributes(aEntry),
            ExtractCsType(aEntry));
    CleanupStack::PushL(conversationEntry);

    //add to the list
    iConverstationEntryList->AppendL( conversationEntry );
    CleanupStack::Pop(conversationEntry);

    // cleanup
    if (sDescription)
        {
        CleanupStack::PopAndDestroy(sDescription);
        }
    if (contact)
        {
        CleanupStack::PopAndDestroy(contact);
        }

    PRINT ( _L("Exit CCsMsgHandler::CreateAndAddEntryL") );
    }

// ----------------------------------------------------------------------------
// CCsMsgHandler::ProcessEntryL
// Creates CCsConversationEntry and adds to the list
// ----------------------------------------------------------------------------
//
void CCsMsgHandler::ProcessEntryL( TEventType aEvent, const TDesC& aContact, 
        const TDesC& aDescription, const TMsvEntry& aEntry)
    {
    PRINT ( _L("Enter CCsMsgHandler::ProcessEntryL") );

    // create and add entry to the list
    CreateAndAddEntryL(aContact,aDescription,aEntry);

    // call cs observer interface for each entry
    switch(aEvent)
        {
        case ERead:
            {
            iMsgObserver->HandleReadCompleteL(iConverstationEntryList);
            }
            break;
        case EUpdate:
            {
            iMsgObserver->HandleUpdateCompleteL(iConverstationEntryList);
            }
            break;
        }

    // Cleanup the conversationEntry List, as we are just sending one entry at a time
    iConverstationEntryList->ResetAndDestroy();

    PRINT ( _L("Exit CCsMsgHandler::ProcessEntryL") );
    }

// ----------------------------------------------------------------------------
// CCsMsgHandler::ExtractAddressL
// Extracts the addresses in the to field and updates them to server
// ----------------------------------------------------------------------------
//
void CCsMsgHandler::ExtractAddressesL( 
        TMsvEntry aEntry, 
        TEventType aEvent, 
        RPointerArray<HBufC>& addressList )
    {
    PRINT ( _L("Enter CCsMsgHandler::ExtractAddressesL") );

    // For SMS read the whole body.
    // For bio type SMS read the iDescription
    // For other message types read the iDescription
    // Note: After the LoadMessageL() the iDescription is getting 
    // deleted so need to make a copy before that.
    TPtrC description;
    HBufC* tmpBuffer = NULL;

    if ( (aEntry.iBioType && 
	      aEntry.iBioType != KUidMsgSubTypeMmsAudioMsg.iUid ) || 
		  aEntry.iMtm == KSenduiMtmBtUid ) 
        {
        tmpBuffer = aEntry.iDescription.AllocLC();
        description.Set( tmpBuffer->Des() );         
        }    
    else if ( aEntry.iMtm == KSenduiMtmSmsUid )  
        {
        iSmsMtm->SwitchCurrentEntryL( aEntry.Id() );
        iSmsMtm->LoadMessageL();
            
        CRichText& body = iSmsMtm->Body();
        TInt smsLength = body.DocumentLength();      
        tmpBuffer = HBufC::NewLC(smsLength);
        TPtr ptr(tmpBuffer->Des());
        body.Extract(ptr, 0);
        description.Set( tmpBuffer->Des() );  
        }
    else if ( aEntry.iMtm == KSenduiMtmMmsUid  || 
	          aEntry.iMtm == KSenduiMMSNotificationUid)  
        {
        tmpBuffer = aEntry.iDescription.AllocLC();
        description.Set( tmpBuffer->Des() );         

        iMmsMtm->SwitchCurrentEntryL( aEntry.Id() );
        iMmsMtm->LoadMessageL();

        // Handle addresses
        if ( aEntry.Parent() == KMsvGlobalInBoxIndexEntryIdValue )
            {
            HBufC* address = iMmsMtm->Sender().AllocL();
            addressList.Append(address);
            }
        else if ( aEntry.Parent() == KMsvSentEntryIdValue ||
                  aEntry.Parent() == KMsvGlobalOutBoxIndexEntryIdValue )
            {
            const CMsvRecipientList& addresses = iMmsMtm->AddresseeList();
            TInt count = addresses.Count();

            for( TInt i = 0; i < count; i++)
                {
                HBufC* address = addresses[i].AllocL();
                TPtrC pureAddress = iMsgPluginUtility->PureAddress(*address);
                HBufC* address1 = pureAddress.AllocL();
                addressList.Append(address1);
                delete address;
                }
            }
        }
    else
        {
        return;
        }

    if ( addressList.Count() > 0 )
        {
        for ( int i = 0; i < addressList.Count(); i++ )
            {
            HBufC* address = addressList[i];
            ProcessEntryL(aEvent, *address, description, aEntry);
            }
        }
    else
        {
        // Unknown
        ProcessEntryL(aEvent, KNullDesC, description, aEntry);       
        }
    
    //Cleanup tmpBuffer
    if ( tmpBuffer )
        {
		 CleanupStack::PopAndDestroy();
        }

    PRINT ( _L("Exit CCsMsgHandler::ExtractAddressesL") );
    }

// -----------------------------------------------------------------------------
// CCsMsgHandler::CompareOrder()
// Method for determining the sorting behaviour of RArray of TMsvId's  
// -----------------------------------------------------------------------------
//
static TInt CompareOrder(const TMsvId& aFirst, const TMsvId& aSecond)
    {
    return aSecond - aFirst;
    }
// -----------------------------------------------------------------------------
// CCsMsgHandler::UploadMsgL()
// State machine to upload all messages 
// -----------------------------------------------------------------------------
//
TInt CCsMsgHandler::UploadMsgL() 
    {
    //Release scheduler for scheduling.    
    User::After(1);
    
    switch ( iState ) 
        {
        case EReadInbox:
            {
            UpdateMessageArrayL(KMsvGlobalInBoxIndexEntryId);
            iState = EReadSent;
            return 1;
            }

        case EReadSent:
            {
            UpdateMessageArrayL(KMsvSentEntryId);
            iState = EReadOutbox;
            return 1;
            }

        case EReadOutbox:
            {
            UpdateMessageArrayL(KMsvGlobalOutBoxIndexEntryId);
            iState = ESortEntries;            
            return 1;
            }
        case ESortEntries:
            {
            //Sort the elements in the array by descending order of TMsvId's
            TLinearOrder<TMsvId> order(CompareOrder);
            iMessageArray->Sort(order);
            iState = EProcessEntries;
            return 1;
            }            
        case EProcessEntries:
            { 
            //Process one entry at a time in sequence
            //Process the first element in the array on each call, till the end
            if(iMessageArray->Count())
                {
                CMsvEntry* msvEntry= iSession->
                        GetEntryL(iMessageArray->operator[](0));
                CleanupStack::PushL(msvEntry);
            TBool isNotClass0 = ETrue;

            if (msvEntry->Entry().iMtm.iUid == KSenduiMtmSmsUidValue && msvEntry->Entry().Parent()
                == KMsvGlobalInBoxIndexEntryIdValue) 
								{
                iSmsMtm->SwitchCurrentEntryL(msvEntry->Entry().Id());
                iSmsMtm->LoadMessageL();

                CSmsHeader& header = static_cast<CSmsClientMtm*> (iSmsMtm)->SmsHeader();
                CSmsPDU& pdu = header.Message().SmsPDU();
                TSmsDataCodingScheme::TSmsClass smsClass;

                if (pdu.Class(smsClass) && smsClass == TSmsDataCodingScheme::ESmsClass0)
									{
                    isNotClass0 = EFalse;                  
                	}
            	}
            if (isNotClass0) 
							{
                ProcessResultsL(msvEntry->Entry());
            	}
                CleanupStack::PopAndDestroy(msvEntry);
                iMessageArray->Remove(0);
                }
            else
                {
                iMessageArray->Reset();
                iMsgObserver->HandleCachingCompleted();
                return 0; //DONE 
                }            
            iState = EProcessEntries;
            return 1; 
        }
    }
    return 0;    
}

// -----------------------------------------------------------------------------
// CCsMsgHandler::UploadMsg()
// CIdle callback 
// -----------------------------------------------------------------------------
//
TInt CCsMsgHandler::UploadMsg(TAny* aArg)
    {
    CCsMsgHandler* handler = (CCsMsgHandler*) aArg; 
    TInt ok = 0;
    TRAPD(err, ok = handler->UploadMsgL());
    return ((err == KErrNone) && ok);
    }

// -----------------------------------------------------------------------------
// CCsMsgHandler::StartL()
// This function starts the state machine to fetch sms data from msvserver
// -----------------------------------------------------------------------------
//
void CCsMsgHandler::StartL()
    {
    PRINT ( _L("Enter CCsMsgHandler::Start") );

    iState = EReadInbox;
    TCallBack callback = TCallBack(UploadMsg, (TAny*) this);
    
    if(iIdle)
        {
        delete iIdle;
        iIdle = NULL;
        }
    iIdle = CIdle::NewL(CActive::EPriorityIdle);
    iIdle->Start(callback);

    PRINT ( _L("End CCsMsgHandler::Start") );
    }

// -----------------------------------------------------------------------------
// CCsMsgHandler::IsMtmSupported()
// 
// -----------------------------------------------------------------------------
//
TBool CCsMsgHandler::IsMtmSupported(long uid)	
    {
    if ( KSenduiMtmSmsUidValue == uid || \
         KSenduiMtmBtUidValue == uid || \
         KSenduiMtmMmsUidValue == uid || \
         KSenduiMtmBioUidValue == uid  || \
         KSenduiMMSNotificationUidValue == uid || \
         KUidMtmWapPush ==  TUid::Uid(uid) )
        {
        return ETrue;
        }	
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCsMsgHandler::ExtractCsType()
// Extracts the Message type based on the MTM Uid 
// -----------------------------------------------------------------------------
//
TCsType CCsMsgHandler::ExtractCsType( const TMsvEntry& aEntry)
    {
    TCsType type = ECsUnknown;
    switch(aEntry.iMtm.iUid)
        {
        case KSenduiMtmSmsUidValue:            
            type = ECsSMS;
            if (aEntry.iBioType == KMsgBioUidVCard.iUid)
                {
                type = ECsBioMsg_VCard;
                }
            break;
        case KSenduiMtmBtUidValue:
            type = ECsBlueTooth;
            break;
        case KSenduiMtmMmsUidValue:        
           if(aEntry.iBioType == KUidMsgSubTypeMmsAudioMsg.iUid)
                {
                type = ECsAudio;
                }
            else
                {
                type = ECsMMS;
                }
            break;
        case KSenduiMMSNotificationUidValue:            
            type = ECsMmsNotification;
            break;
        case KSenduiMtmBioUidValue:
            { 
			if (aEntry.iMtmData1 == KSenduiMtmBtUidValue) 
			{
           	 	if (aEntry.iBioType == KMsgBioUidVCard.iUid) 
				{
                	type = ECsBlueTooth_VCard;
            	}
            	else if (aEntry.iBioType == KMsgBioUidVCalendar.iUid)
				{
               		type = ECsBlueTooth_VCal;
            	}
           		 break;
        	}
            type = ECsBioMsg; 
            
            // based on the biotype uid set message type
            if(aEntry.iBioType == KMsgBioUidRingingTone.iUid)
                {
                type = ECsRingingTone;
                }
            else if(aEntry.iBioType == KMsgBioProvisioningMessage.iUid)
                {
                type = ECsProvisioning;
                }
		    else if (aEntry.iBioType == KMsgBioUidVCard.iUid)
                {
                type = ECsBioMsg_VCard;
                }
            else if (aEntry.iBioType == KMsgBioUidVCalendar.iUid)
                {
                type = ECsBioMsg_VCal;
       		 }
       		 else if (aEntry.iBioType == KMsgBioNokiaServiceMessage.iUid) {
            	type = ECsBioMgs_NokiaService;
        }
    }
        break;
    default:
        type = ECsUnknown;
        break;
    }
    return (type);
}
// -----------------------------------------------------------------------------
// void CCsMsgHandler::UpdateMessageArrayL(const TMsvId& aFolderId)
// Update iMessageArray with the message ID before upload message to csserver. 
// -----------------------------------------------------------------------------
//
void CCsMsgHandler::UpdateMessageArrayL(const TMsvId& aFolderId)
    {
    CMsvEntry* msvEntry = iSession->GetEntryL(aFolderId);
    CleanupStack::PushL(msvEntry);
    // Set sort order
    TMsvSelectionOrdering order;
    order.SetSorting(EMsvSortById);
    msvEntry->SetSortTypeL(order);

    CMsvEntrySelection* messages = msvEntry->ChildrenL();  
    // Cleanup msvEntry
    CleanupStack::PopAndDestroy(msvEntry);
    CleanupStack::PushL(messages);

    TInt messageCount = messages->Count();
    
    // In case of large number of message caching
    // to avoid multiple reallocation alloc memory
    // one shot.
    if( messageCount > KMessageArrayGranularity)
        iMessageArray->Reserve(messageCount);
    
    for(int i = 0; i < messageCount; i ++)
        {
        TMsvId msgId=messages->At(i);
        iMessageArray->Append(msgId);
        }
    // Cleanup messages
    CleanupStack::PopAndDestroy(messages); 
    }
// End of file

