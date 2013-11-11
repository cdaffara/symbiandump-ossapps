/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "ccalendarobserver.h"


CCalendarObserver* CCalendarObserver::NewL()
    {
    CCalendarObserver* self= new (ELeave) CCalendarObserver();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); 
    return self;
    }

void CCalendarObserver::ConstructL()
    {
    iSession = CCalSession::NewL();    
    /*_LIT(KFileName,"calendarfile");
    TRAPD(err, session->CreateCalFileL(KFileName));
    // ignore KErrAlreadyExists leave code
    if (err != KErrAlreadyExists)
        {
        User::LeaveIfError(err);
        }*/
    // Open the calendar file
    iSession->OpenL(iSession->DefaultFileNameL());
    iWaiter = new (ELeave) CActiveSchedulerWait();
    iCalEntryView = CCalEntryView::NewL(*iSession, *this);
    iWaiter->Start();
    }

CCalendarObserver::CCalendarObserver()
    {    
    }

CCalendarObserver::~CCalendarObserver()
    {
    
    delete iCalEntryView;
    delete iSession;    
    delete iWaiter;
    }

void DestroyRPointerArray(TAny* aPtr)
    {
    RPointerArray<CCalEntry>* self = static_cast<RPointerArray<CCalEntry>*> (aPtr);
    self->ResetAndDestroy();
    }

void CCalendarObserver::AddEntryL()
    {
    _LIT(KDescription,"Meeting");
    const TDesC8& guidDes = KGuid;
    iUid = guidDes.AllocL();        
    CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, iUid, CCalEntry::EMethodNone, 0);    
    CleanupStack::PushL(entry);
            
    // For an appointment, the time as well as the date is relevant
    TDateTime startTime(2010, EJuly, 07, 10, 0, 0, 0);
    TDateTime endTime(2010, EJuly, 07, 16, 0, 0, 0);
    
    TCalTime calstart;
    TCalTime calstop;
    calstart.SetTimeLocalL(startTime);
    calstop.SetTimeLocalL(endTime);    
    entry->SetStartAndEndTimeL(calstart, calstop); 
    // Set the description text
    entry->SetDescriptionL(KDescription);
    
    RPointerArray<CCalEntry> array;    
    CleanupClosePushL(array);
    array.AppendL(entry);
    
    TInt numEntriesAdded = 0;
    iCalEntryView->StoreL(array, numEntriesAdded);    

    CleanupStack::PopAndDestroy(&array);    
    CleanupStack::PopAndDestroy(entry);    
    }

void CCalendarObserver::UpdateCalenderEntryL()
    {
    _LIT(KOldEntry,"Meeting");
    _LIT(KNewEntry,"Updated Meeting");
    
    RPointerArray<CCalEntry> entryarray; 
    CleanupStack::PushL(TCleanupItem(DestroyRPointerArray, &entryarray));
    
    iCalEntryView->FetchL(KGuid,entryarray);    
        for( int i = 0; i < entryarray.Count(); i++)
            {
            CCalEntry* entry = entryarray[i];
            if ( entry->EntryTypeL() == CCalEntry::EAppt )
                {
                if ( entry->DescriptionL().Compare ( KOldEntry ) == 0)            
                    {                    
                    entry->SetDescriptionL(KNewEntry);
                    break;
                    }
                }
            }
    TInt numEntriesAdded = 0;
    iCalEntryView->StoreL(entryarray, numEntriesAdded);    
    CleanupStack::PopAndDestroy(&entryarray);   
    }

void CCalendarObserver::AddNoteL( TPtr8 aMemo)
    {	
    const TDesC8& guidDes = KGuid;
    iUid = guidDes.AllocL();        
    CCalEntry* entry = CCalEntry::NewL(CCalEntry::ENote, iUid, CCalEntry::EMethodNone, 0);    
    CleanupStack::PushL(entry);    
    
    HBufC16* buf16 = HBufC16::NewL(aMemo.Length());
    buf16->Des().Copy(aMemo);

    // Set the description text
    entry->SetDescriptionL(buf16->Des());
    
    RPointerArray<CCalEntry> array;    
    CleanupClosePushL(array);
    array.AppendL(entry);
    
    TInt numEntriesAdded = 0;
    iCalEntryView->StoreL(array, numEntriesAdded);    
    
    delete buf16;
    CleanupStack::PopAndDestroy(&array);    
    CleanupStack::PopAndDestroy(entry);
    }

void CCalendarObserver::DeleteEntryL()
    {
   
    RPointerArray<CCalEntry> entryarray; 
    CleanupStack::PushL(TCleanupItem(DestroyRPointerArray, &entryarray));       
    
    //Fetch the entry to be deleted and delete it
    iCalEntryView->FetchL(KGuid,entryarray);
    CCalEntry* entry = entryarray[0];
    
    iCalEntryView->DeleteL(*entry);
    CleanupStack::PopAndDestroy(&entryarray);
    
    }

void CCalendarObserver::DeleteNoteEntryL()
    {
    RPointerArray<CCalEntry> entryarray; 
    CleanupStack::PushL(TCleanupItem(DestroyRPointerArray, &entryarray));       
    
    //Fetch the entry to be deleted and delete it
    iCalEntryView->FetchL(KGuid,entryarray);    
    for( int i = 0; i < entryarray.Count(); i++)
        {
        CCalEntry* entry = entryarray[i];
        if ( entry->EntryTypeL() == CCalEntry::ENote )
            {
            iCalEntryView->DeleteL(*entry);
            continue;
            }
        }
    CleanupStack::PopAndDestroy(&entryarray);
    }

void CCalendarObserver::UpdateNoteEntryL( TPtr8 aOldString, TPtr8 aNewString )
    {
    RPointerArray<CCalEntry> entryarray; 
    CleanupStack::PushL(TCleanupItem(DestroyRPointerArray, &entryarray));       
    HBufC16* buf16 = HBufC16::NewL(aOldString.Length());
    buf16->Des().Copy(aOldString);
    HBufC16* newbuf16 = HBufC16::NewL(aNewString.Length());
    newbuf16->Des().Copy(aNewString);
    TBuf<32> oldstring;
    oldstring.Copy( buf16->Des() );
    //Fetch the entry to be deleted and delete it
    iCalEntryView->FetchL(KGuid,entryarray);    
    for( int i = 0; i < entryarray.Count(); i++)
        {
        CCalEntry* entry = entryarray[i];
        if ( entry->EntryTypeL() == CCalEntry::ENote )
            {
            if ( entry->DescriptionL().Compare ( oldstring ) == 0)            
                {
                TBuf<32> newstring;
                newstring.Copy(newbuf16->Des());
                entry->SetDescriptionL( newstring);
                break;
                }
            }
        }
    delete buf16;
    delete newbuf16;
    TInt numEntriesAdded = 0;
    iCalEntryView->StoreL(entryarray, numEntriesAdded);    
    CleanupStack::PopAndDestroy(&entryarray);
    }

// Called during calendar entry view creation
void CCalendarObserver::Progress(TInt /*aProgress*/)
    {    
    }

// Called on completion of calendar entry view creation
void CCalendarObserver::Completed(TInt /*aError*/)
    {
     iWaiter->AsyncStop();
    }

// Returns whether or not progress notification is required
TBool CCalendarObserver::NotifyProgress()
    {
        // Progress notification is required
        return ETrue;
    }
