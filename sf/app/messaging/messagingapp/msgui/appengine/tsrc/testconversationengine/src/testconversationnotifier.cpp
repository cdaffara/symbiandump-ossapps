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

#include "testconversationnotifier.h"

#include <mcsresultsobserver.h>
#include <mcsconversationlistchangeobserver.h>
#include <mcsconversationchangeobserver.h>
#include <mcscachingstatusobserver.h>


TestConversationNotifier::TestConversationNotifier()
                            :iResultsObserver(NULL), 
                            iConversationListChangeObserver(NULL), 
                            iConversationChangeObserver(NULL),
                            iCachingStatusObserver(NULL) 
{
}

void TestConversationNotifier
::SetResultNotifier(MCsResultsObserver* aResultsObserver)
{
    iResultsObserver = aResultsObserver;
}

void TestConversationNotifier::RemoveResultNotifier( )
{
    iResultsObserver = NULL;
}

void TestConversationNotifier
::SetConversationListNotifier(MCsConversationListChangeObserver* 
                                    aConversationListChangeObserver)
{
    iConversationListChangeObserver = aConversationListChangeObserver;
}

void TestConversationNotifier::RemoveConversationListNotifier( )
{
    iConversationListChangeObserver = NULL;        
}

void TestConversationNotifier
::SetConversationChangeNotifier(MCsConversationChangeObserver* 
                                    aConversationChangeObserver)
{
    iConversationChangeObserver = aConversationChangeObserver;
}

void TestConversationNotifier::RemoveConversationChangeNotifier()
{
    iConversationChangeObserver = NULL;
}

void TestConversationNotifier
::SetCachingStatusNotifier(MCsCachingStatusObserver* aCachingStatusObserver)
{
    iCachingStatusObserver = aCachingStatusObserver;
}

void TestConversationNotifier::RemoveCachingStatusNotifier()
{
    iCachingStatusObserver = NULL;
}

void TestConversationNotifier
::SendListResultUpdate(RPointerArray<CCsConversationEntry>& 
                                        aConversationEntryList,
                                        TInt& aTotalCount)
{
    // Pass the results to the observer
    if ( iResultsObserver )
    {
        iResultsObserver->Conversations(aConversationEntryList,aTotalCount);
    }
}

void TestConversationNotifier
::UpdateConversationEntryModify(CCsConversationEntry& aConversationEntry)
{
    // Pass the results to the observer
    if ( iConversationChangeObserver )
    {
        iConversationChangeObserver->ModifyConversation(aConversationEntry);
    }
}

void TestConversationNotifier
::UpdateConversationEntryAdd(CCsConversationEntry& aConversationEntry)
{
    // Pass the results to the observer
    if ( iConversationChangeObserver )
    {
        iConversationChangeObserver->AddConversation(aConversationEntry);
    }
}

void TestConversationNotifier
::UpdateConversationEntryDelete(CCsConversationEntry& aConversationEntry)
{
    // Pass the results to the observer
    if ( iConversationChangeObserver )
    {
        iConversationChangeObserver->DeleteConversation(aConversationEntry);
    }
}

void TestConversationNotifier
::SendClientListUpdate(RPointerArray<CCsClientConversation>& 
                                        aConversationClientList)
{
    // Pass the results to the observer
     if ( iResultsObserver )
     {
     iResultsObserver->ConversationList(aConversationClientList);
     }
}

void TestConversationNotifier
::UpdateConversationClientModify(CCsClientConversation& aConversationClient)
{
    if ( iConversationListChangeObserver )
    {
        iConversationListChangeObserver->
            ModifyConversationList(aConversationClient);
    }
}
    
void TestConversationNotifier
::UpdateConversationClientAdd(CCsClientConversation& aConversationClient)
{
    if ( iConversationListChangeObserver )
    {
        iConversationListChangeObserver->
            AddConversationList(aConversationClient);
    }
}

void TestConversationNotifier
::UpdateConversationClientDelete(CCsClientConversation& aConversationClient)
{
    if ( iConversationListChangeObserver )
    {
        iConversationListChangeObserver->
                DeleteConversationList(aConversationClient);
    }
}

