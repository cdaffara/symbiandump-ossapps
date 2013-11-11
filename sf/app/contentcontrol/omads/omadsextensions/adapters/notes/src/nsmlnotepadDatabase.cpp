/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  notepad database
*
*/


#include <sysutil.h>

#include <txtetext.h>
#include "nsmlnotepadDatabase.h"
#include <nsmlnotepaddatastoreformat.rsg>
#include <nsmlconstants.h>

#include <calentryview.h>
#include <calentry.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <calsession.h>
#include <CalenInterimUtils2.h>

#include "nsmlnotepadlogging.h"

// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::NewL
// -----------------------------------------------------------------------------
//
CNSmlNotepadDatabase* CNSmlNotepadDatabase::NewL()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::NewL(): begin");
	
	CNSmlNotepadDatabase *instance = new(ELeave) CNSmlNotepadDatabase();
	CleanupStack::PushL(instance);
	instance->ConstructL();
	CleanupStack::Pop(instance);
	
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::NewL(): end");
	return instance;
	}

// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::~CNSmlNotepadDatabase()
// -----------------------------------------------------------------------------
//
CNSmlNotepadDatabase::~CNSmlNotepadDatabase()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::~CNSmlNotepadDatabase(): begin");
	
	CloseAgendaSessions();

	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::~CNSmlNotepadDatabase(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::CNSmlNotepadDatabase()
// -----------------------------------------------------------------------------
//
CNSmlNotepadDatabase::CNSmlNotepadDatabase()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::CNSmlNotepadDatabase(): begin");
	
	iOpened = EFalse;
	
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::CNSmlNotepadDatabase(): end");
	}	
	
// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::ConstructL
// -----------------------------------------------------------------------------
//
void CNSmlNotepadDatabase::ConstructL()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::ConstructL: begin");
	
	// Instantiate the Session variable
	iSession = CCalSession::NewL();
	
    _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::ConstructL: end");
	}

// -----------------------------------------------------------------------------
// TInt CNSmlNotepadDatabase::OpenL(const TDesC& aStoreName)
// -----------------------------------------------------------------------------
//
void CNSmlNotepadDatabase::OpenL(const TDesC& aStoreName)
    {
    _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::OpenL(): begin");
    CloseAgendaSessions();
        
    TInt err = KErrNone;    
    iSession = CCalSession::NewL();
    
    TRAP( err,iSession->OpenL( aStoreName ) );
    if(err == KErrNotFound)
        {
        iSession->CreateCalFileL( aStoreName );
        iSession->OpenL( aStoreName );
        }
    else if( err != KErrNone )
        {
        CloseAgendaSessions();
        User::Leave(err);
        }

    iEntryView = CCalEntryView::NewL(*iSession);
    iInstanceView = CCalInstanceView::NewL(*iSession);

    if(iEntryView && iInstanceView)
        {
        iOpened = ETrue;
        }
    else
        {
        CloseAgendaSessions();
        User::LeaveIfError(KErrNotFound);
        }
    
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::OpenL(): end");
    }

// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::AddNoteL
// -----------------------------------------------------------------------------
//
void CNSmlNotepadDatabase::AddNoteL(CNpdItem& aNote, TInt &aKey)
    {
    _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::AddNoteL(): begin");   
   
    TInt successCount = 0;

    CCalenInterimUtils2* interimUtils = CCalenInterimUtils2::NewL();
    CleanupStack::PushL(interimUtils);
    
    HBufC8* guid = interimUtils->GlobalUidL();
    CleanupStack::PopAndDestroy(interimUtils);
    
    CleanupStack::PushL(guid);
    CCalEntry* entry = CCalEntry::NewL(CCalEntry::ENote, guid,
            CCalEntry::EMethodNone, 0);
    CleanupStack::Pop(guid);
    
    CleanupStack::PushL(entry);

    // set the description
    TDesC* noteContent(aNote.Content());
    entry->SetDescriptionL(*noteContent);

    // store the entry in to calendar file
    RPointerArray<CCalEntry> array;
    CleanupRPtrArrayPushL(array); 

    array.AppendL(entry);
    CleanupStack::Pop(entry);
    iEntryView->StoreL(array, successCount);
    
    if(successCount)
        {
        HBufC* content = HBufC::NewL(entry->DescriptionL().Length());
        content->Des().Copy(entry->DescriptionL());
        aNote.Set(entry->LocalUidL(),entry->LastModifiedDateL().TimeUtcL(), content);
        aKey = entry->LocalUidL();
        CleanupStack::PopAndDestroy(&array);
        _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::AddNoteL(): inside if(successCount) after array");
        }
    else
        {
        User::Leave(KErrGeneral);
        }
	
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::AddNoteL(): end");
	
	User::Leave(KErrNone);
    }

// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::DeleteNoteL
// -----------------------------------------------------------------------------
//
void CNSmlNotepadDatabase::DeleteNoteL(const TInt aKey)
    {
    _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::DeleteNoteL(): begin");
    
    CCalEntry* entryTobeDeleted(NULL);
    entryTobeDeleted = iEntryView->FetchL(aKey);
    if(entryTobeDeleted)
        {
        CleanupStack::PushL(entryTobeDeleted);
        iEntryView->DeleteL(*entryTobeDeleted);
        CleanupStack::Pop(entryTobeDeleted);
        }
    
    delete entryTobeDeleted;
    entryTobeDeleted = NULL;
    _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::DeleteNoteL(): end");
    }

// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::Close
// -----------------------------------------------------------------------------
//
void CNSmlNotepadDatabase::Close()  // can be deprecated ????
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::Close(): begin");
	iOpened = EFalse;
    _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::Close(): end");        
	}
	
// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::GetNoteL
// -----------------------------------------------------------------------------
//
void CNSmlNotepadDatabase::GetNoteL(const TInt aKey, CNpdItem& aItem)
    {
    _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::GetNoteL(): begin");
	TInt err(KErrNone);
	if(!iOpened)
		{
		_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::GetNoteL(): not opened");
		User::Leave(KErrNotReady);
		}
	
    if(aKey)
        {
        CCalEntry* entryTobeFetched(NULL);
        entryTobeFetched = iEntryView->FetchL(aKey);
        if(entryTobeFetched)
            {
            CleanupStack::PushL(entryTobeFetched); 
            HBufC* content = HBufC::NewL(entryTobeFetched->DescriptionL().Length());
            content->Des().Copy(entryTobeFetched->DescriptionL());
            
            aItem.Set(aKey, entryTobeFetched->LastModifiedDateL().TimeUtcL(), content);
            
            //CleanupStack::Pop(aItem);
            CleanupStack::PopAndDestroy(entryTobeFetched);
            }
        }   
    
    _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::GetNoteL(): end");
    User::Leave(err);
    }

// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::UpdateNoteL
// -----------------------------------------------------------------------------
//
void CNSmlNotepadDatabase::UpdateNoteL(const TInt aKey, CNpdItem& aItem)
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::UpdateNoteL(): begin");

	if(!iOpened)
		{
		_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::UpdateNoteL(): not ready");
		User::Leave(KErrNotReady);
		}

	RPointerArray<CCalEntry> array;
    CleanupRPtrArrayPushL(array);

    // fetch the created note using the localuid
    CCalEntry* entryTobeUpdated(NULL);
    entryTobeUpdated = iEntryView->FetchL(aKey);
    if(entryTobeUpdated)
        {
    	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::UpdateNoteL(): entry found");
        }
    else
        {
    	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::UpdateNoteL(): entry not found");
        User::Leave(KErrNotFound);
        }
    CleanupStack::PushL(entryTobeUpdated);

    // update the description
    HBufC* content = HBufC::NewL(aItem.Content()->Length());
    content = aItem.Content();

    entryTobeUpdated->SetDescriptionL(*content);

    array.AppendL(entryTobeUpdated);
    CleanupStack::Pop(entryTobeUpdated);
    TBool success(EFalse);
    // update the entry
    iEntryView->UpdateL(array, success);

    if(success)
        {
        aItem.SetModified(entryTobeUpdated->LastModifiedDateL().TimeUtcL());
        CleanupStack::PopAndDestroy(&array);
        }
    else
        {
        User::Leave(KErrGeneral);
        }
	
    _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::UpdateNoteL(): end");	
    }

// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::ResetL
// -----------------------------------------------------------------------------
//
TInt CNSmlNotepadDatabase::ResetL()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::ResetL(): begin");
	
    TInt err(KErrNone);
    CArrayPtr<CNpdItem>* arrNpdItem = NULL;
    
    // Fetch all the available notes from the db
    arrNpdItem = FetchItemsLC();
      
    // Delete the Notes
    for( TInt count = 0; count < arrNpdItem->Count(); count++ )
        {
        CCalEntry* entryTobeDeleted(NULL);
        entryTobeDeleted = iEntryView->FetchL( arrNpdItem->At(count)->Key() );
        if(entryTobeDeleted)
            {
            CleanupStack::PushL(entryTobeDeleted);
            iEntryView->DeleteL(*entryTobeDeleted);
            CleanupStack::Pop(entryTobeDeleted);
            }        
        delete entryTobeDeleted;
        entryTobeDeleted = NULL;
        }
    CleanupStack::PopAndDestroy(arrNpdItem);
    _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::ResetL(): begin");
    return err;
	}

// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::IsOpen
// -----------------------------------------------------------------------------
//
TBool CNSmlNotepadDatabase::IsOpen()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::IsOpen(): begin");
	_NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::IsOpen(): end");
    return iOpened;
    }


// ------------------------------------------------------------------------------------------------
// CArrayPtr<CNpdItem>* CNSmlNotepadDatabase::FetchItemsLC(TBool aIncludeText,
// ------------------------------------------------------------------------------------------------
CArrayPtr<CNpdItem>* CNSmlNotepadDatabase::FetchItemsLC(TBool aIncludeText,
                                                    TInt* aKey)
    {
    _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::FetchItemsLC(): begin") ;

    CArrayPtr<CNpdItem>* res = new (ELeave) CArrayPtrFlat<CNpdItem> (8);
    CleanupPtrArrayPushL(res);
        
    if(!aKey)
        {
        _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::FetchItemsLC(): In aKey case");
        RPointerArray<CCalInstance> array;
        CleanupRPtrArrayPushL(array);
        _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::FetchItemsLC(): After cleaupstack");
        
        TCalTime startDate;
        startDate.SetTimeLocalL(TDateTime(1900, EJanuary, 1, 0, 0, 0, 0));
        TCalTime endDate;
        endDate.SetTimeLocalL(TDateTime(2100, EJanuary, 30, 0, 0, 0, 0));
        CalCommon::TCalTimeRange timeRange(startDate, endDate);
    
        iInstanceView->FindInstanceL(array,CalCommon::EIncludeNotes, timeRange);
        CNpdItem* entry = NULL;
        TInt i = 0;

        while (i < array.Count())
            {
            entry = CNpdItem::NewLC();
            entry->SetKey( array[i]->Entry().LocalUidL() );
            entry->SetModified( array[i]->Entry().LastModifiedDateL().TimeUtcL());
            if(aIncludeText)
                {
                _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::FetchItemsLC(): aIncludeText true");
                HBufC* content = HBufC::NewL( array[i]->Entry().DescriptionL().Length() );
                content->Des().Copy( array[i]->Entry().DescriptionL() );                
                entry->SetContent( content );
                }
            res->AppendL(entry);
            _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::FetchItemsLC(): After res->AppendL(entry)");
            CleanupStack::Pop(entry);
            _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::FetchItemsLC(): After popping (entry)");
            i++;
            }
        _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::FetchItemsLC(): before pop and destroy");
        CleanupStack::PopAndDestroy(&array);
        _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::FetchItemsLC(): After While Loop");
        }
    else
        {
        _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::FetchItemsLC(): In else case");
        CCalEntry* entryTobeFetched(NULL);
        entryTobeFetched = iEntryView->FetchL(*aKey);
        _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::FetchItemsLC(): After Fetching entry");
        if(entryTobeFetched)
            {
            CleanupStack::PushL(entryTobeFetched); 
            CNpdItem* entry = CNpdItem::NewLC();
            entry->SetKey( entryTobeFetched->LocalUidL() );
            entry->SetModified( entryTobeFetched->LastModifiedDateL().TimeUtcL() );
            if(aIncludeText)
                {
                HBufC* content = HBufC::NewL( entryTobeFetched->DescriptionL().Length() );
                content->Des().Copy( entryTobeFetched->DescriptionL() );                
                entry->SetContent( content );
                }
            res->AppendL(entry);
            CleanupStack::Pop(entry);
            CleanupStack::PopAndDestroy(entryTobeFetched);
            }
        }
    _NOTEPAD_DBG_FILE("CNSmlNotepadDatabase::FetchItemLC(): end");
    return res;
    }

// -----------------------------------------------------------------------------
// CNSmlNotepadDatabase::CloseAgendaSessions
// -----------------------------------------------------------------------------
//
void CNSmlNotepadDatabase::CloseAgendaSessions()
    {
    if(iInstanceView)
        {
        delete iInstanceView;
        iInstanceView = NULL;
        }
    if(iEntryView)
        {
        delete iEntryView;
        iEntryView = NULL;
        }
    if(iSession)
        {
        delete iSession;
        iSession = NULL;
        }
    }



// -----------------------------------------------------------------------------
// CNpdItem class implementations
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// CNpdItem::~CNpdItem
// -----------------------------------------------------------------------------
//
CNpdItem::~CNpdItem()
	{
	_NOTEPAD_DBG_FILE("CNpdItem::~CNpdItem: begin");
	delete iContent;
	_NOTEPAD_DBG_FILE("CNpdItem::~CNpdItem: end");
	}
// -----------------------------------------------------------------------------
// CNpdItem::NewL
// -----------------------------------------------------------------------------
//
CNpdItem* CNpdItem::NewLC()
	{
	_NOTEPAD_DBG_FILE("CNpdItem::NewLC: begin");
	CNpdItem* tmp = new (ELeave) CNpdItem();
	CleanupStack::PushL(tmp);
	//tmp->ConstructL(0, TTime(), NULL);
	_NOTEPAD_DBG_FILE("CNpdItem::NewLC: end");
	return tmp;
	}

// -----------------------------------------------------------------------------
// CNpdItem::NewL
// -----------------------------------------------------------------------------
//
CNpdItem* CNpdItem::NewLC(TInt aKey, const TTime& aLastModified, HBufC *aContent)
	{
	_NOTEPAD_DBG_FILE("CNpdItem::NewLC(TInt aKey, const TTime& aLastModified, HBufC *aContent): begin");
	CNpdItem* tmp = new (ELeave) CNpdItem(aKey,aLastModified,aContent);
	CleanupStack::PushL(tmp);
	//tmp->ConstructL(aKey, aLastModified, aContent);
	_NOTEPAD_DBG_FILE("CNpdItem::NewLC(TInt aKey, const TTime& aLastModified, HBufC *aContent): end");
	return tmp;
	}

// -----------------------------------------------------------------------------
// CNpdItem::CNpdItem()
// -----------------------------------------------------------------------------
//
CNpdItem::CNpdItem() : iKey(0), iLastModified(Time::MinTTime()), iContent(NULL)
	{
	_NOTEPAD_DBG_FILE("NpdItem::CNpdItem(): begin");
	_NOTEPAD_DBG_FILE("NpdItem::CNpdItem(): end");
	}

// -----------------------------------------------------------------------------
// CNpdItem::CNpdItem(TInt aKey,const TTime& aLastModified, HBufC *aContent)
// -----------------------------------------------------------------------------
//
CNpdItem::CNpdItem(TInt aKey, 
					const TTime& aLastModified, 
					HBufC *aContent) : iKey(aKey), 
										iLastModified(aLastModified), 
										iContent(aContent)
	{
	_NOTEPAD_DBG_FILE("NpdItem::CNpdItem(): begin");
	_NOTEPAD_DBG_FILE("NpdItem::CNpdItem(): end");
	}
	

// -----------------------------------------------------------------------------
// CNpdItem::Key
// -----------------------------------------------------------------------------
//
TInt CNpdItem::Key() const
	{
	_NOTEPAD_DBG_FILE("CNpdItem::Key(): begin");
	_NOTEPAD_DBG_FILE("CNpdItem::Key(): end");
	return iKey;
	}
// -----------------------------------------------------------------------------
// CNpdItem::LastModified
// -----------------------------------------------------------------------------
//
TTime CNpdItem::LastModified() const
	{
	_NOTEPAD_DBG_FILE("CNpdItem::LastModified(): begin");
	_NOTEPAD_DBG_FILE("CNpdItem::LastModified(): end");
	return iLastModified;
	}
// -----------------------------------------------------------------------------
// CNpdItem::Content
// -----------------------------------------------------------------------------
//
HBufC* CNpdItem::Content()
	{
	_NOTEPAD_DBG_FILE("NpdItem::Content(): begin");
	_NOTEPAD_DBG_FILE("NpdItem::Content(): end");
	return iContent;
	}
// -----------------------------------------------------------------------------
// CNpdItem::Set
// -----------------------------------------------------------------------------
//
void CNpdItem::Set(TInt aKey, const TTime& aLastModified, HBufC *aContent)
	{
	_NOTEPAD_DBG_FILE("CNpdItem::Set(): begin");
	iKey = aKey;
	iLastModified = aLastModified;
    SetContent(aContent);
    _NOTEPAD_DBG_FILE("CNpdItem::Set(): end");
	}

// -----------------------------------------------------------------------------
// CNpdItem::Set
// -----------------------------------------------------------------------------
//
void CNpdItem::SetKey(TInt aKey)
    {
    _NOTEPAD_DBG_FILE("CNpdItem::SetKey(): begin");
     iKey = aKey;
    _NOTEPAD_DBG_FILE("CNpdItem::SetKey(): end");
    }

// -----------------------------------------------------------------------------
// CNpdItem::SetContent
// -----------------------------------------------------------------------------
//
void CNpdItem::SetContent(HBufC *aContent)
	{
	_NOTEPAD_DBG_FILE("CNpdItem::SetContent(): begin");
    iContent = aContent;
    _NOTEPAD_DBG_FILE("CNpdItem::SetContent(): end");
	}

// -----------------------------------------------------------------------------
// CNpdItem::SetModified
// -----------------------------------------------------------------------------
//
void CNpdItem::SetModified(const TTime& aLastModified)
	{
	_NOTEPAD_DBG_FILE("CNpdItem::SetModified(): begin");
	iLastModified = aLastModified;
	_NOTEPAD_DBG_FILE("CNpdItem::SetModified(): end");
	}

// End of File
