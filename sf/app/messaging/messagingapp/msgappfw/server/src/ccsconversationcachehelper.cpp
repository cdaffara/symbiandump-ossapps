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
 * Description:  This class helps CS Cache in processing the data.
 *
 */

// INCLUDE FILES
// SYSTEM INCLUDE FILES
#include <ccsdefs.h>
#include <ccsconversationentry.h>
#include <ccsclientconversation.h>

// USER INCLUDE FILES
#include "ccsconversationcachehelper.h"
#include "ccsconversationcache.h"
#include "ccsconversation.h"
#include "ccsconversationentry.h"
#include "ccsconversationevent.h"
#include "ccscontactsmanager.h"
#include "ccscontactsresolver.h"
#include <xqconversions.h>
#include "ccsdebug.h"

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CCsConversationCacheHelper::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CCsConversationCacheHelper*
CCsConversationCacheHelper::NewL(CCsConversationCache* aConversationCache)
{
    PRINT ( _L("Enter CCsConversationCacheHelper::NewL") );

    CCsConversationCacheHelper* self =
            new (ELeave) CCsConversationCacheHelper(aConversationCache);

    CActiveScheduler::Add(self);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    PRINT ( _L("End CCsConversationCacheHelper::NewL") );

    return self;
}

// ----------------------------------------------------------------------------
// CCsConversationCacheHelper::CCsConversationCacheHelper
// Construtor
// ----------------------------------------------------------------------------
CCsConversationCacheHelper::CCsConversationCacheHelper(
                                                       CCsConversationCache* aConversationCache) :
    CActive(EPriorityLow), iConversationCache(aConversationCache)
{
}

// ----------------------------------------------------------------------------
// CCsConversationCacheHelper::ConstructL
// Second phase constructor
// ----------------------------------------------------------------------------
void CCsConversationCacheHelper::ConstructL()
{
    iConversationEvent = NULL;
    iConversationIndex = KErrNotFound;
    iCurrentConversationIndex = KErrNotFound;

    //initialize the event list
    iConversationEventList
            = new (ELeave) RPointerArray<CCsConversationEvent> ();

    // now create all special type of conversations
    // like BT, IRDA, Unknown Drafts etc.
    TInt loop = 0;
    for (loop = 0; loop < KMaxSpecialConversations; loop++)
    {
        CreateSpecialConversationL();
    }
}

// ----------------------------------------------------------------------------
// CCsConversationCache::CreateSpecialConversationL
// Second part of constructL
// This shall create all special type of messages like BT, Unknown Draft, IRDA etc.
// Pls note this shall be placed by order at starting of cache
// ----------------------------------------------------------------------------
void CCsConversationCacheHelper::CreateSpecialConversationL()
{
    // Create a conversation for Drafts Unknown case and set it in
    // 0th position and the conversation Id for drafts unknown case is
    // KDratsUnknownConversationId
    CCsConversation* conversation = CCsConversation::NewL();

    conversation->SetConversationId(GetNextRowId());

    // add into the list
    iConversationCache->ConversationList()->AppendL(conversation);
}

// ----------------------------------------------------------------------------
// CCsConversationCacheHelper::CCsConversationCacheHelper
// Destructor
// ----------------------------------------------------------------------------
CCsConversationCacheHelper::~CCsConversationCacheHelper()
{
    Cancel();

    // delete the event list
    if (iConversationEventList)
    {
        iConversationEventList->ResetAndDestroy();
        iConversationEventList->Close();
        delete iConversationEventList;
        iConversationEventList = NULL;
    }
}
// -----------------------------------------------------------------------------
// CCsConversationCacheHelper::DoCancel()
// DoCancel implementation from CActive Object
// -----------------------------------------------------------------------------
//
void CCsConversationCacheHelper::DoCancel()
{
    // do nothing    
}
// ----------------------------------------------------------------------------
// CCsConversationCacheHelper::Run
// Called from the scheduler
// ----------------------------------------------------------------------------
void CCsConversationCacheHelper::RunL()
{
    switch (iState)
    {
        case ECsProcessConversation:
        {
            HandleProcessConversationL();
        }
            break;

        case ECsSpecialConversation:
        {
            HandleSpecialConversationL();
        }
            break;

        case ECsConversationFoundInCache:
        {
            HandleConversationInCacheL();
        }
            break;

        case ECsResolveConversation:
        {
            ResolveContact(iConversationEvent);
        }
            break;
    }
}
// ----------------------------------------------------------------------------
// CCsConversationCacheHelper::StartCacheUpdate
// this shall start looking at cache data and update if entry already exist
// shall put request to phonebook in case new data
// ----------------------------------------------------------------------------
void CCsConversationCacheHelper::StartCacheUpdate()
{
    iState = ECsProcessConversation;
    IssueRequest();
}
// ---------------------------------------------------------------------------
// CCsConversationCacheHelper::HandleProcessConversationL()
// Process Conversation and update cache
// ---------------------------------------------------------------------------
//
void CCsConversationCacheHelper::HandleProcessConversationL()
{
    iConversationEvent = GetNextEvent();
    // check to see if all the events are consumed
    // then mark cache as caching done..
    if (!iConversationEvent)
    {
        iConversationCache->CachingCompletedL();
        return;
    }

    CCsConversationEntry* conEntry =
            iConversationEvent->ClientConversation()->GetConversationEntry();

    //handle add, modify events
    if (iConversationEvent->Event() != KConversationEventDelete)
    {
        TUint8 SpecialId = NeedsSpecialProcessing(conEntry);

        if (SpecialId)
        {
            // this is for draft(unlnown)/BT/IR processing            
            iState = ECsSpecialConversation;
        }
        // check only if contact is a valid address
        else if (conEntry->Contact())
        {
            iCurrentConversationIndex
                    = iConversationCache->FindConversation(* (conEntry->Contact()));
            if (iCurrentConversationIndex != KErrNotFound)
            {
                iState = ECsConversationFoundInCache;
            }
            else
            {
                iState = ECsResolveConversation;
            }
        }
        else
        {
            // this is when contact number is NULL
            // delete the entry from temp list
            DeleteEvent(*iConversationEvent);
            iState = ECsProcessConversation;
        }
    }
    else
    {
        //handle delete event
        DeleteConversationEntryL(conEntry);
        DeleteEvent(*iConversationEvent);
        iState = ECsProcessConversation;
    }
    //move to other state
    IssueRequest();
}
// ---------------------------------------------------------------------------
// CCsConversationCacheHelper::HandleSpecialConversationL()
// Hnadle the case when the conversation has no contact,
// BlueTooth/IrDa messages
// ---------------------------------------------------------------------------
//
void CCsConversationCacheHelper::HandleSpecialConversationL()
{
    CCsConversationEntry* conEntry =
        iConversationEvent->ClientConversation()->GetConversationEntry();

    if (ECsBlueTooth == conEntry->GetType()|| ECsBlueTooth_VCard == conEntry->GetType()|| ECsBlueTooth_VCal == conEntry->GetType())
    {
        AddConversationEntryL(conEntry, KBluetoothMsgsConversationId);
    }
    else
    {
        AddConversationEntryL(conEntry, KUnknownConversationId);
    }
    DeleteEvent(*iConversationEvent);
    iState = ECsProcessConversation;
    IssueRequest();

    PRINT ( _L("CCsConversationCacheHelper::HandleSpecialConversationL") );
}
// ---------------------------------------------------------------------------
// CCsConversationCacheHelper::HandleConversationInCacheL()
// Handle the case when the conversation is found in the cache
// so there is no need for resolving the contact
// ---------------------------------------------------------------------------
//
void CCsConversationCacheHelper::HandleConversationInCacheL()
{
    CCsConversationEntry* conEntry =
            iConversationEvent->ClientConversation()->GetConversationEntry();

    AddConversationEntryL(conEntry, iCurrentConversationIndex);

    DeleteEvent(*iConversationEvent);

    // reset back the conversation index
    iCurrentConversationIndex = KErrNotFound;
    iState = ECsProcessConversation;
    IssueRequest();
}
// ----------------------------------------------------------------------------
// This function identifies if the conentry needs special processing
// Drafts message with NULL contact/Bluetooth/IRDA etc
// ----------------------------------------------------------------------------
TUint8 CCsConversationCacheHelper::NeedsSpecialProcessing(
                                                          CCsConversationEntry* aConversationEntry)
{
    if (ECsBlueTooth == aConversationEntry->GetType() || ECsBlueTooth_VCard == aConversationEntry->GetType() || ECsBlueTooth_VCal == aConversationEntry->GetType())
    {
        return 1;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Move to next state
// ---------------------------------------------------------------------------
//
void CCsConversationCacheHelper::IssueRequest()
{
    if (!IsActive())
    {
        iStatus = KRequestPending;
        TRequestStatus* status = &iStatus;
        SetActive();
        User::RequestComplete(status, KErrNone);
    }
}

// ----------------------------------------------------------------------------
// CCsConversationCacheHelper::ContactResolved
// Handle a contact resolved in the contact database
// ----------------------------------------------------------------------------
void CCsConversationCacheHelper::ResolveContact(
                                                CCsConversationEvent* aConverastionEvent)
{
    CCsContactDetail contactDetail;

    //get contact from event
    CCsConversationEntry* ConvEntry =
            aConverastionEvent->ClientConversation()->GetConversationEntry();

    if (ConvEntry)
    {
        HBufC* Contact = ConvEntry->Contact();
        if (Contact)
        {
            QString contactAddress =
                    XQConversions::s60DescToQString(Contact->Des());

            iConversationCache->ContactsManager()->resolver()->resolveContact(contactAddress,
                                                                              contactDetail);

            int contactId = contactDetail.contactId;
            TInt cIndex = iConversationCache->FindConversation(contactId);

            if (cIndex == KErrNotFound)
            {
                // Add as new conversation
                HBufC* displayName = NULL;
                if(!contactDetail.displayName.isEmpty())
                {
                    displayName=XQConversions::qStringToS60Desc(contactDetail.displayName);
                }
                TRAPD(error, AddNewConversationL( aConverastionEvent->
                                ClientConversation()->GetConversationEntry(),
                                contactId,
                                displayName));

                if (displayName) delete displayName;                

                if (error != KErrNone)
                {
                    // handle error
                }
            }
            else
            {
                // this is when two contacts are having same contact Id,
                // in that case it should add into an existing conversation
                TRAPD(error, AddConversationEntryL( 
                        aConverastionEvent->ClientConversation(
                                )->GetConversationEntry(), cIndex));
                if (error != KErrNone)
                {
                    // handle error
                }
            }

            // now remove the entry from event list
            DeleteEvent(*aConverastionEvent);

            // call start update cache
            StartCacheUpdate();
        }
    }
}

// ----------------------------------------------------------------------------
// Handles the case where conversation is already in cache
// This will add new conversation entry into the corresponding conversation
// pls note there is already a conversation exist for this entry
// hence this entry gets appended inside conversation list
// ----------------------------------------------------------------------------
void CCsConversationCacheHelper::AddConversationEntryL(
                                                       CCsConversationEntry *aConEntry,
                                                       TInt aConversationIndex)
{
    RPointerArray<CCsConversation>* conversationList =
            iConversationCache->ConversationList();

    CCsConversation* conversation = (*conversationList)[aConversationIndex];

    CCsConversationEntry* prevLatestEntry = conversation->GetLatestEntryL();

    if (prevLatestEntry)
    {
        prevLatestEntry = prevLatestEntry->CloneL();
    }
    else
    {
        prevLatestEntry = aConEntry->CloneL();
    }

    CleanupStack::PushL(prevLatestEntry);

    // save the previous unread message count
    TUint16 prevUnreadCount = conversation->GetUnreadMessageCount();

    TUint32 event;
    conversation->UpdateEntryL(aConEntry, event);

    // Send update notify to conversation list
    if (IsNotifyRequiredL(conversation,
                          prevLatestEntry,
                          KConversationListEventUpdate,
                          prevUnreadCount))
    {
        CCsConversationEntry* convEntry = conversation->GetLatestEntryL();
        CCsClientConversation* clientConvLatest =
                iConversationCache->CreateClientConvLC(conversation, convEntry);
        
        // THIS IS USED FOR SHOWING NOTIFICATIONS. NOTIFICATION IS SHOWN ONLY
        // WHEN ECsAttributeNewEntryAdded IS SET. 
        if(iConversationEvent->IsUpdateConversationEventSet() 
                && event == KConversationEventNew)
            {
            clientConvLatest->GetConversationEntry()->
            ChangeAttributes(ECsAttributeNewEntryAdded,ECsAttributeNone);
            }
        
        iConversationCache->NotifyL(clientConvLatest,
                                    KConversationListEventUpdate);
        CleanupStack::PopAndDestroy(clientConvLatest);
    }

    // Send update notify to conversations
    CCsClientConversation* clientConv =
            iConversationCache->CreateClientConvLC(conversation, aConEntry);
    iConversationCache->NotifyL(clientConv, event);

    // Cleanup
    CleanupStack::PopAndDestroy(clientConv);
    CleanupStack::PopAndDestroy(prevLatestEntry);
}

// ----------------------------------------------------------------------------
// CCsConversationCache::DeleteConversationL
// Delete conversation entry from conversation list
// there are few checks of how to notify the UI about the changes 
// for the various use cases. 
// Delete requires the conversation Id and type (SMS, MMS etc.)
// ----------------------------------------------------------------------------
void CCsConversationCacheHelper::DeleteConversationEntryL(
                                                          CCsConversationEntry* aConversationEntry)
{
    // For MMS, search the entry inside all conversations and delete.
    // For SMS, stop on first match.
    TBool stopOnFirstMatch = ETrue;

    RPointerArray<CCsConversation>* conversationList =
            iConversationCache->ConversationList();

    // the deletion needs search the entryId inside whole conversation
    // search the entry id and delete the entry
    for (TInt loop = 0; loop < iConversationCache->ConversationList()->Count(); loop++)
    {
        CCsConversation* conversation =
                static_cast<CCsConversation*> ( (*conversationList)[loop]);

        TInt indexDeletion = conversation->FindEntry(aConversationEntry);

        if (indexDeletion != KErrNotFound)
        {
            // Get the entry from cache to check the type
            CCsConversationEntry* cacheEntry =
                    conversation->GetEntryL(indexDeletion);

            if (cacheEntry->GetType() == ECsMMS || cacheEntry->GetType()
                    == ECsAudio)
            {
                stopOnFirstMatch = EFalse;
            }
            
            TUint16 prevUnreadCount = conversation->GetUnreadMessageCount();

            // Delete the conversation entry from this conversation
            conversation->DeleteEntryL(indexDeletion);

            // Notify client of conversation list change.
            if (IsNotifyRequiredL(conversation,
                                  aConversationEntry,
                                  KConversationListEventDelete,
                                  prevUnreadCount))
            {
                // Delete
                CCsClientConversation
                        * clientConv =
                                iConversationCache->CreateClientConvLC(conversation,
                                                                       aConversationEntry);
                iConversationCache->NotifyL(clientConv,
                                            KConversationListEventDelete);
                CleanupStack::PopAndDestroy(clientConv);

                // Update with latest entry
                CCsConversationEntry* conEntry =
                        conversation->GetLatestEntryL();
                if (conEntry)
                {
                    CCsClientConversation
                            * clientConv =
                                    iConversationCache->CreateClientConvLC(conversation,
                                                                           conEntry);
                    iConversationCache->NotifyL(clientConv,
                                                KConversationListEventNew);
                    CleanupStack::PopAndDestroy(clientConv);
                }
            }

            // Notify client of conversation change
            if (!conversation->IsDeleted())
            {
                CCsClientConversation
                        * clientConv =
                                iConversationCache->CreateClientConvLC(conversation,
                                                                       aConversationEntry);
                iConversationCache->NotifyL(clientConv,
                                            KConversationEventDelete);
                CleanupStack::PopAndDestroy(clientConv);
            }

            // check if all entries are deleted then 
            // delete the conversation from cache
            if (conversation->GetEntryCount() == 0
                    && !conversation->IsSpecialConversation())
            {
            
            conversationList->Remove(loop);
            delete conversation;
            //reset the counters
            loop -= 1;
            }

            // Stop searching    
            if (stopOnFirstMatch) break;
        }
    } // for
}

// ----------------------------------------------------------------------------
// CCsConversationCacheHelper::AddNewConversationL
// add new conversation into cache
// this shall be called after resolving entry from phonebook
// ----------------------------------------------------------------------------
void CCsConversationCacheHelper::AddNewConversationL(
                                                     CCsConversationEntry* aConversationEntry,
                                                     TInt32 aContactId,
                                                     const HBufC* aDisplayName)
{
    CCsConversation* conversation = CCsConversation::NewL();
    CleanupStack::PushL(conversation);

    //set conversation entry id
    // by default just set the count of list as of now, as it will be unique
    conversation->SetConversationId(GetNextRowId());

    // add the conversation entry
    conversation->AddEntryL(aConversationEntry);

    // fill firstname and lastname and contact Id
    if(aDisplayName)
    {
   		conversation->AddContactDetailsL(aContactId,
                                     aDisplayName);
    }

    // fill the phone number and contact Id
    if (aConversationEntry->Contact())
    {
        conversation->AddContactDetailsL(* (aConversationEntry->Contact()), aContactId);
    }

    // add into the list
    iConversationCache->ConversationList()->AppendL(conversation);

    // Send add notify to conversation list
    CCsClientConversation* clientConv =
            iConversationCache->CreateClientConvLC(conversation,
                                                   aConversationEntry);
    
    // THIS IS USED FOR SHOWING NOTIFICATIONS. NOTIFICATION IS SHOWN ONLY
    // WHEN ECsAttributeNewEntryAdded IS SET.
    if(iConversationEvent->IsUpdateConversationEventSet()) 
        {
        clientConv->GetConversationEntry()->
        ChangeAttributes(ECsAttributeNewEntryAdded,ECsAttributeNone);
        }

    iConversationCache->NotifyL(clientConv, KConversationListEventNew);
    // send the new conversation event as well, so that if there are nay listeners for the cv
    // then they will get the notification.
    iConversationCache->NotifyL(clientConv, KConversationEventNew);
    CleanupStack::PopAndDestroy(clientConv);

    CleanupStack::Pop(conversation);

    PRINT ( _L("CCsConversationCacheHelper::AddNewConversationL - Conversation Added") );
}

// ----------------------------------------------------------------------------
// CCsConversationCache::GetPendingEventCount
// Pending event count
// ----------------------------------------------------------------------------
TInt CCsConversationCacheHelper::GetPendingEventCount()
{
    return iConversationEventList->Count();
}

// ---------------------------------------------------------------------------
// Get next event from event list
// ---------------------------------------------------------------------------
//
CCsConversationEvent* CCsConversationCacheHelper::GetNextEvent()
{
    // check in case there are no events in cache
    if (iConversationEventList->Count() > 0)
    {
        return (*iConversationEventList)[0];
    }
    return NULL;
}

// ---------------------------------------------------------------------------
// Find CCsConversationEvent from event Q based on ConversationEntryId.
// ---------------------------------------------------------------------------
TInt CCsConversationCacheHelper::FindEvent(
                                           const CCsConversationEvent& aConvEvent)
{
    // find the event inside event list and send the index
    // KErrNotFound in  case None
    TInt
            index =
                    iConversationEventList->Find(&aConvEvent,
                                                 CCsConversationEvent::CompareByEntryId);

    return index;
}

// ----------------------------------------------------------------------------
// CCsConversationCache::GetNextRowId
// Get the conversation list count where to add next conversation
// ----------------------------------------------------------------------------
TInt16 CCsConversationCacheHelper::GetNextRowId()
{
    // increment the index and return
    iConversationIndex++;
    return iConversationIndex;
}

// ---------------------------------------------------------------------------
// Find CCsConversationEvent based on ConversationEntryId and delete it.
// ---------------------------------------------------------------------------
//
void CCsConversationCacheHelper::DeleteEvent(
                                             const CCsConversationEvent& aConvEvent)
{
    TInt index = FindEvent(aConvEvent);

    if (KErrNotFound != index)
    {
        CCsConversationEvent* conversationEvent =
                (*iConversationEventList)[index];
        iConversationEventList->Remove(index);
        delete conversationEvent;
    }
}

// ----------------------------------------------------------------------------
// CCsConversationCache::IsNotifyRequiredL
// checks whether notification for conversation list is required
// ----------------------------------------------------------------------------
TBool CCsConversationCacheHelper::IsNotifyRequiredL(
                                                    CCsConversation* aConversation,
                                                    CCsConversationEntry* aConversationEntry,
                                                    TUint32 aEvent,
                                                    TUint16 aPreviousUnreadMsgsCount)
{
    TUint16 presentUnreadMessageCount = aConversation->GetUnreadMessageCount();
    CCsConversationEntry* latestEntry = aConversation->GetLatestEntryL();

    // Add & Update
    if (aEvent & KConversationListEventNew || aEvent
            & KConversationListEventUpdate)
    {
        // Check whether deleted is ongoing. Don't Notify.
        if (aConversation->IsDeleted())
        {
            return EFalse;
        }

        // Check whether latest entry is same and unread count has not changed. Don't Notify.
        if (CCsConversationEntry::Compare(*latestEntry, *aConversationEntry)
                == 0 && (presentUnreadMessageCount == aPreviousUnreadMsgsCount)
                && (latestEntry->GetSendState()
                        == aConversationEntry->GetSendState()))
        {
            return EFalse;
        }
    }

    // Delete
    if (aEvent & KConversationListEventDelete)
    {
        // Not Last entry during delete. Don't Notify.
        if (aConversation->IsDeleted() && aConversation->GetEntryCount() > 0)
        {
            return EFalse;
        }
    }

    // Notify.
    return ETrue;
}

// ----------------------------------------------------------------------------
// CCsConversationCacheHelper::ConversationEventList
// Pls refer to .h file
// ----------------------------------------------------------------------------
RPointerArray<CCsConversationEvent>*
CCsConversationCacheHelper::ConversationEventList()
{
    return iConversationEventList;
}

//end of file
