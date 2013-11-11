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
* Description:  Harvester message plugin
*
*/


//  INCLUDES 

#include <e32base.h>
#include <mtclreg.h>
#include <mmsclient.h>
#include <smsclnt.h>
#include <smuthdr.h>
#include <smsclnt.h>
#include <smut.h>
#include <gsmuelem.h>
#include <msventry.h>
#include <msvuids.h>
#include <mmsconst.h>
#include "cmessageharvester.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmessageharvesterTraces.h"
#endif



// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CMessageHarvester::NewL
// ---------------------------------------------------------------------------
//
CMessageHarvester* CMessageHarvester::NewL(  CMessagePlugin& aMessagePlugin, CMsvSession& aSession )
	{
	CMessageHarvester* self = new (ELeave) CMessageHarvester(  aMessagePlugin, aSession );
	return self;
	}

// ---------------------------------------------------------------------------
// CMessageHarvester::~CMessageHarvester
// ---------------------------------------------------------------------------
//
CMessageHarvester::~CMessageHarvester()
	{   
	Cancel();  
    delete iChildren;
    iToBeSearchedFolders.Close();
	}
	  
// ---------------------------------------------------------------------------
// CMessageHarvester::CMessageHarvester
// ---------------------------------------------------------------------------
//
CMessageHarvester::CMessageHarvester( CMessagePlugin& aMessagePlugin, CMsvSession& aSession  ) :
	CActive(CActive::EPriorityIdle),
	iMessagePlugin( aMessagePlugin ),
	iMsvSession( aSession ),
	iState(EStateIdle)
	{
	CActiveScheduler::Add( this );
	}

// ---------------------------------------------------------------------------
// CMessageHarvester::StartHarvesting
// ---------------------------------------------------------------------------
//	
void CMessageHarvester::StartHarvestingL()
	{
	// First, get message folders
	GetMessageFoldersL();
	
	// Then, harvest them
	iState = EStateNextFolder;
	HandleNextRequest();
	}

// ---------------------------------------------------------------------------
// CMessageHarvester::AddFolderWithSubFoldersL
// ---------------------------------------------------------------------------
//		
void CMessageHarvester::AddFolderWithSubFoldersL( const TMsvId& aFolderId )
	{
	// Add a folder to the search list
	User::LeaveIfError(iToBeSearchedFolders.Append(aFolderId));	
	CMsvEntry* folderEntry = CMsvEntry::NewL(iMsvSession, aFolderId, TMsvSelectionOrdering(KMsvGroupByStandardFolders, EMsvSortByDetails, EFalse));
	CleanupStack::PushL(folderEntry);	
	ListSubFoldersL(*folderEntry);
	CleanupStack::PopAndDestroy(folderEntry);
	}

// ---------------------------------------------------------------------------
// CMessageHarvester::ListSubFoldersL
// ---------------------------------------------------------------------------
//	
void CMessageHarvester::ListSubFoldersL(  const CMsvEntry& aFolderEntry ) 
	{
	// Only list "folder" children
    CMsvEntrySelection* children = aFolderEntry.ChildrenL();
	CleanupStack::PushL( children );

	for ( TInt i=0; i < children->Count(); i++ )
		{
		TMsvId& childId = ( *children )[i];
		const TMsvEntry& childEntry ( aFolderEntry.ChildDataL( childId ) );
		if ( ( childEntry.iType == KUidMsvServiceEntry ) || 
    		 ( childEntry.iType == KUidMsvFolderEntry ) )
		    {
		    // Add this folder to search list
            AddFolderWithSubFoldersL( childId );		
		    }
		}
	CleanupStack::PopAndDestroy();  // children
	}
	
// ---------------------------------------------------------------------------
// CMessageHarvester::GetMessageFoldersL
// ---------------------------------------------------------------------------
//
void CMessageHarvester::GetMessageFoldersL()
    {
    // Get list of folders
    CMsvEntry* rootEntry;
    rootEntry = CMsvEntry::NewL( iMsvSession, KMsvRootIndexEntryIdValue,
    TMsvSelectionOrdering( KMsvGroupByStandardFolders, EMsvSortByDetails, EFalse ) );
    CleanupStack::PushL( rootEntry );
	ListSubFoldersL( *rootEntry);
	CleanupStack::PopAndDestroy( rootEntry );
    }

			
// ---------------------------------------------------------------------------
// CMessageHarvester::GetNextFolderL
// ---------------------------------------------------------------------------
//		
void CMessageHarvester::GetNextFolderL()
    {
    if ( iToBeSearchedFolders.Count() > 0 )
        
    	{
    	// Open the new folder
        CMsvEntry* folder = CMsvEntry::NewL(
	                            iMsvSession, 
	                            iToBeSearchedFolders[0],
	                            TMsvSelectionOrdering( KMsvGroupByStandardFolders, EMsvSortByDetails ));
        
        iCurrentFolder = iToBeSearchedFolders[0];
        
        // Get the message items from the folder
        CleanupStack::PushL(folder);        
        iChildren = folder->ChildrenWithTypeL( KUidMsvMessageEntry );
        CleanupStack::PopAndDestroy( folder );
        
        // Remove current folder from the queue
        iToBeSearchedFolders.Remove(0);
        iState = EStateNextMessage;
        HandleNextRequest();
        }
    else
        {
        // No more folders in the queue
        iState = EStateIdle;
        HandleNextRequest();      
        }
    }

			
// ---------------------------------------------------------------------------
// CMessageHarvester::GetMessageId
// ---------------------------------------------------------------------------
//		    
void CMessageHarvester::GetMessageIdL()
    { 
    if (iChildren && iChildren->Count() > 0)
        {
        // Take first item from list of message items, and give it for indexing        
        iMessagePlugin.MessageItemL((*iChildren)[0], ECPixAddAction, iCurrentFolder);        
        
        // Delete the item from list
        iChildren->Delete( 0 );   
        if ( iChildren->Count() > 0 )
            {
            // Get the next message
            iState = EStateNextMessage;
            HandleNextRequest();      
            }
        else 
            {
            // No messages in this folder
            iState = EStateNextFolder;
            HandleNextRequest();      
            }
        }
    else 
        {
        // Current folder dosen't contain any message items so change folder
        iState = EStateNextFolder;
        HandleNextRequest();      
        }
    }
      
// -----------------------------------------------------------------------------
// CMessageHarvester::DoCancel
// -----------------------------------------------------------------------------
//   
void CMessageHarvester::DoCancel()
	{
	}

// -----------------------------------------------------------------------------
// CMessageHarvester::RunL
// -----------------------------------------------------------------------------
//   
void CMessageHarvester::RunL()	
	{
	
	 // Simple Round-Robin scheduling.
    Deque();
    CActiveScheduler::Add( this );
    
	switch (iState)
		{
	       case EStateIdle:
	            {
	            // Delete current folder items
	            delete iChildren; 
	            iChildren = NULL;
	            
	            // Started idling, close the folders
	            iToBeSearchedFolders.Reset();

	            // Update harvester status
	            iMessagePlugin.HarvestingCompleted(KErrNone);	            
	            break; 
	    	    }    
		    case EStateNextMessage:
		        {
		        // Get the next message
		        GetMessageIdL();	
		        break;	        
		        }
	        case EStateNextFolder:
	            {
	            // Delete current folder items
	            delete iChildren; 
	            iChildren = NULL;
	            
	            // Start harvesting folders. 
	            GetNextFolderL();	         
				break;
	            }
		}
	}
	         				
// -----------------------------------------------------------------------------
// CMessageHarvester::RunError
// -----------------------------------------------------------------------------
//   
TInt CMessageHarvester::RunError( TInt aError )	
	{
	iState = EStateIdle;
    iMessagePlugin.HarvestingCompleted(aError);
	return KErrNone;
	}
	
// ---------------------------------------------------------------------------
// SetNextRequest
// ---------------------------------------------------------------------------
//
void CMessageHarvester::HandleNextRequest()
    {
    OstTraceFunctionEntry0( CMESSAGEHARVESTER_HANDLENEXTREQUEST_ENTRY );
    if (!IsActive() && iMessagePlugin.GetHarvesterState())
        {
        SetActive();
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        }
    OstTraceFunctionExit0( CMESSAGEHARVESTER_HANDLENEXTREQUEST_EXIT );
    }

void CMessageHarvester::ResumeRequest()
    {
    OstTraceFunctionEntry0( CMESSAGEHARVESTER_RESUMEREQUEST_ENTRY );
    if(iState != EStateIdle)
        {        
        HandleNextRequest();
        }
    OstTraceFunctionExit0( CMESSAGEHARVESTER_RESUMEREQUEST_EXIT );
    }
// End of File
