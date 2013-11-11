/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <msgitem.h>
#include <ccsdefs.h>
#include <ccsconversationentry.h>
#include <ccsclientconversation.h>
#include <conversationsenginedefines.h>
#include "testconversationenginestub.h"
#include "testconversationnotifier.h"
#include "convergedmessage.h"

TestConversationEngineStub* TestConversationEngineStub::Instance()
{
    static TestConversationEngineStub* iStubEngine 
                                = new TestConversationEngineStub();
    return iStubEngine;
}

void TestConversationEngineStub::InitL()
{
    iNotifier = new TestConversationNotifier();
    iConversationID = 1234;
    iTimeStamp = 123456789;
    iMsgType = ECsSMS;
    iUnReadCount = 3;

    _LIT(Kname , "9343434343");
    _LIT(KDescription , "This is the test msg.");

    iContactID = Kname;
    iDescription = KDescription;
    iDeletedEntry = NULL;
}

TestConversationEngineStub::TestConversationEngineStub()
{
    TRAP_IGNORE(InitL());
}

TestConversationEngineStub::~TestConversationEngineStub()
{
    iConversationClientList.ResetAndDestroy();
    iConversationEntryList.ResetAndDestroy();
    delete iNotifier;
}

void TestConversationEngineStub
::SetResultObserver(MCsResultsObserver *aResultObserver)
{
    iNotifier->SetResultNotifier(aResultObserver);
}

void TestConversationEngineStub::RemoveResultObserver()
{
    iNotifier->RemoveResultNotifier();
}

void TestConversationEngineStub
::SetConversationChangeObserver(MCsConversationChangeObserver* aObserver,
            CCsClientConversation* /*aClientConversation*/)
{
    iNotifier->SetConversationChangeNotifier(aObserver);
}
    
void TestConversationEngineStub::RemoveConversationChangeObserver()
{
    iNotifier->RemoveConversationChangeNotifier();
}

void TestConversationEngineStub
::SetConversationListChangeObserver(MCsConversationListChangeObserver* 
                                        aConversationListChangeObserver)
{
    iNotifier->SetConversationListNotifier(aConversationListChangeObserver);
}

void TestConversationEngineStub::RemoveConversationListChangeObserver()
{
    iNotifier->RemoveConversationListNotifier();
}

void TestConversationEngineStub
::MarkConversationRead(const TInt aConversationId)
{
    for (TInt i = 0; i < iConversationEntryList.Count(); ++i)
         {
             if (iConversationEntryList[i]->EntryId() == aConversationId)
                 {
                     iConversationEntryList[i]->
                         ChangeAttributes(ECsAttributeNone, ECsAttributeUnread);
                     break;
                 }
          }
}

void TestConversationEngineStub
::DeleteConversation(const TInt aConversationId)
{
    for (TInt i = 0; i < iConversationEntryList.Count(); ++i)
         {
             if (iConversationEntryList[i]->EntryId() == aConversationId)
                 {
                     iDeletedEntry  = iConversationEntryList[i];
                     iConversationEntryList.Remove(i);
                 }
          }
}

TInt TestConversationEngineStub::GetConvListSize()
{
    return iConversationEntryList.Count();
}

TInt TestConversationEngineStub::GetUnreadCount()
{
    TInt unRead = 0;
    for (TInt i = 0; i < iConversationEntryList.Count(); ++i)
     {
         if (iConversationEntryList[i]->IsAttributeSet(ECsAttributeUnread))
             {
                 unRead++;
             }
      }
    return unRead;
}

TInt TestConversationEngineStub::GetConversationID()
{
    return iConversationID;
}

TInt TestConversationEngineStub::GetTimeStamp()
{
    return iTimeStamp;
}

TInt TestConversationEngineStub::GetMsgType()
{
    return iMsgType;
}

TDesC& TestConversationEngineStub::GetContactID()
{
    return iContactID;
}

TDesC& TestConversationEngineStub::GetDescription()
{
    return iDescription;
}

RPointerArray<CCsClientConversation>& 
TestConversationEngineStub::GetConversationClientList()
{
    return iConversationClientList;
}

CCsConversationEntry*
TestConversationEngineStub
::CreateConversationEntryL(TInt aConvID, bool aUnread)
{
    CCsConversationEntry* conversationEntry = CCsConversationEntry::NewL();
    CleanupStack::PushL(conversationEntry);

    conversationEntry->SetType(iMsgType);
    conversationEntry->SetEntryId(aConvID);

    conversationEntry->SetTimeStampL(iTimeStamp);

    conversationEntry->SetContactL(iContactID);

    conversationEntry->SetDescriptionL(iDescription);
    
    if (aUnread)
        {
        conversationEntry->ChangeAttributes(ECsAttributeUnread, 
                                                ECsAttributeNone);
        conversationEntry->SetConversationDir(0/*MsgDirectionIncoming*/);
        }
    else
        {
        conversationEntry->SetConversationDir(1/*MsgDirectionOutgoing*/);
        conversationEntry->ChangeAttributes(ECsAttributeSent, 
                                                ECsAttributeNone);
        }
   
    CleanupStack::Pop(conversationEntry);
    
    return conversationEntry;
}

void TestConversationEngineStub
::PrepareConversationListL(TInt aConvID, TInt aToatalCnt, TInt aUnreadCnt)
{
    // create conversation entries
    for ( TInt iloop = 0; iloop < aToatalCnt; iloop++ )
        {
        CCsConversationEntry* conversationEntry 
                = CreateConversationEntryL(aConvID++, (aUnreadCnt-- > 0));
        CleanupStack::PushL(conversationEntry);
    
        // append conversation entries to list
        iConversationEntryList.AppendL(conversationEntry);

        CleanupStack::Pop(conversationEntry);
        }
}

//Prerae a list with : conv id, no of unread msgs, 
void TestConversationEngineStub::UpdateConversationList()
{
    TInt error;
    TInt totalCount = 5;
    TRAP(error, PrepareConversationListL(iConversationID, totalCount, iUnReadCount));    
    iNotifier->SendListResultUpdate(iConversationEntryList,totalCount);
}

void TestConversationEngineStub::UpdateConvEntry(TInt aConversationId)
{
    for (TInt i = 0; i < iConversationEntryList.Count(); ++i)
     {
         if (iConversationEntryList[i]->EntryId() == aConversationId)
         {
             iNotifier->
             UpdateConversationEntryModify(*iConversationEntryList[i]);
             break;
         }
      }
}

void TestConversationEngineStub::UpdateDeletedConvEntry()
{
    if (iDeletedEntry)
    {
        iNotifier->UpdateConversationEntryDelete(*iDeletedEntry);
        delete iDeletedEntry;
        iDeletedEntry = NULL;
    }
}

void TestConversationEngineStub::UpdateAddConvEntryL()
{
    CCsConversationEntry*
    conversationEntry = CreateConversationEntryL(9999/*iConversationID - 1*/, true);
    CleanupStack::PushL(conversationEntry);
    iConversationEntryList.AppendL(conversationEntry);
    CleanupStack::Pop(conversationEntry);
    iNotifier->UpdateConversationEntryAdd(*conversationEntry);
}

void TestConversationEngineStub::UpdateAddConvEntry()
{
    TInt error;
    TRAP(error, UpdateAddConvEntryL());
}

void TestConversationEngineStub::UpdateAddConvClientL()
{
    _LIT(KFName , "New User");
    
    //need to free alloced descriptor
    TBuf<10>  BufName(KFName);
    HBufC * fname = BufName.AllocL();
    CleanupStack::PushL(fname);
    
    CCsConversationEntry* 
    convEntry = CreateConversationEntryL(iConversationID - 1, true);
    CleanupStack::PushL(convEntry);
    
    CCsClientConversation* 
    convClient = CreateConversationClientL(fname,4000, 2, *convEntry);
    CleanupStack::PushL(convClient);
    
    iConversationClientList.AppendL(convClient);
    
    CleanupStack::Pop(convClient);
    CleanupStack::PopAndDestroy(convEntry);
    CleanupStack::PopAndDestroy(fname);
    iNotifier->UpdateConversationClientAdd(*convClient);
}

void TestConversationEngineStub::UpdateAddConvClient()
{
    TInt error;
    TRAP(error, UpdateAddConvClientL());
}

void TestConversationEngineStub::UpdateDeletedConvClient()
{
    CCsClientConversation* convClient = iConversationClientList[0];
    iConversationClientList.Remove(0);

    if (convClient)
    {
        iNotifier->UpdateConversationClientDelete(*convClient);
        delete convClient;
    }
}


void TestConversationEngineStub::UpdateConvClient(TInt aConversationId)
{
    for (TInt i = 0; i < iConversationClientList.Count(); ++i)
     {
         if (iConversationClientList[i]->GetConversationEntryId() 
                                                 == aConversationId)
         {
             iConversationClientList[i]->
             GetConversationEntry()->
             ChangeAttributes(ECsAttributeNone, ECsAttributeUnread);
             
             iConversationClientList[i]->SetUnreadMessageCount(0);
             
             iNotifier->
             UpdateConversationClientModify(*iConversationClientList[i]);
             break;
         }
      }
}

CCsClientConversation* 
TestConversationEngineStub::
CreateConversationClientL(const HBufC* aDispName, TInt aContactID,
                          TInt aUnreadCnt,
                          CCsConversationEntry& aConvEntry)
{
    CCsClientConversation* conversationClient = CCsClientConversation::NewL();
    CleanupStack::PushL(conversationClient);
    
    conversationClient->SetContactId(aContactID);
    conversationClient->SetUnreadMessageCount(aUnreadCnt);
    
    conversationClient->SetDisplayNameL(aDispName);

    conversationClient->SetConversationEntryL(&aConvEntry);
    
    conversationClient->SetConversationEntryId(aConvEntry.EntryId());
    
    CleanupStack::Pop(conversationClient);
    
    return conversationClient;
}

void TestConversationEngineStub
::PrepareConversationClientListL(TInt aContactID, TInt aListSize, 
                                TInt aUnRead, TInt aUnReadCnt)
{
    TInt convID = iConversationID;
    
    _LIT(KFName , "abc");
    
    //need to free alloced descriptor
    TBufC<10>  BufName(KFName);
    HBufC * fname = BufName.AllocL();
    CleanupStack::PushL(fname);
    
    for (TInt loop = 0; loop < aListSize; loop++)
        {
            CCsConversationEntry* 
            convEntry = CreateConversationEntryL(convID++,
                                  (aUnRead-- > 0 ? true : false));
            CleanupStack::PushL(convEntry);
            
            CCsClientConversation* 
            convClient = CreateConversationClientL(fname, aContactID++, aUnReadCnt++,
                                              *convEntry);
            CleanupStack::PushL(convClient);
            
            iConversationClientList.AppendL(convClient);
            
            CleanupStack::Pop(convClient);
            CleanupStack::PopAndDestroy(convEntry);
        }
    CleanupStack::Pop(fname);
}

//conversation client notifications
void TestConversationEngineStub::UpdateConversationClientList()
{
    TInt error;
    TRAP(error, PrepareConversationClientListL(2000, 5, 3, 0));
    if (error == KErrNone)
    {
        iNotifier->SendClientListUpdate(iConversationClientList);
    }
}

TInt TestConversationEngineStub
::GetClientConversationID(const TInt aContactId)
{
    for (TInt i = 0; i < iConversationClientList.Count(); ++i)
     {
         if (iConversationClientList[i]->GetContactId() ==  aContactId)
         {
             return iConversationClientList[i]->GetConversationEntryId();
         }
      }
    return -1;
}

TInt TestConversationEngineStub
::GetClientConversationID(const TDesC& aContactAddress)
{
    for (TInt i = 0; i < iConversationClientList.Count(); ++i)
     {
         if (iConversationClientList[i]->
                 GetConversationEntry()->
                 Contact()->Compare(aContactAddress) == 0)
         {
             return iConversationClientList[i]->GetConversationEntryId();
         }
      }
    return -1;
}
