/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  File Transfer Controller implementation
*
*/


// INCLUDE FILES
#include <obexutilsmessagehandler.h>
#include <btmsgtypeuid.h>    // BT Message Uid

#include "sconinboxhandler.h"
#include "debug.h"
#include "sconconsts.h"


// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSConInboxHandler::NewL()
// Two-phase constructor
// -----------------------------------------------------------------------------
//
CSConInboxHandler* CSConInboxHandler::NewL()
    {
    TRACE_FUNC_ENTRY;
    CSConInboxHandler* self = new (ELeave) CSConInboxHandler();
    
    TRACE_FUNC_EXIT;
    return self;
    }

// -----------------------------------------------------------------------------
// CSConInboxHandler::~CSConInboxHandler()
// Default destructor
// -----------------------------------------------------------------------------
//
CSConInboxHandler::~CSConInboxHandler()
    {
    TRACE_FUNC;
    }

// -----------------------------------------------------------------------------
// CSConInboxHandler::CreateInboxAttachmentL( CObexBufObject*& aObject,
//                                              CBufFlat*& aBuffer )
// Creates an attachment to device's Inbox
// -----------------------------------------------------------------------------
//  
TInt CSConInboxHandler::CreateInboxAttachmentL( CObexBufObject*& aObject,
                                                CBufFlat*& aBuffer )
    {
    TRACE_FUNC_ENTRY;
    TInt ret( KErrNone );
    iDrive = GetMessageCenterDriveL();
    
    delete aObject;
    aObject = CObexBufObject::NewL( NULL );
    
    iFile = RFile();
    
    TRAP( ret, TObexUtilsMessageHandler::CreateInboxAttachmentL( aObject, 
    KUidMsgTypeBt, iMsvIdParent, iFile ) );
        
    if( aBuffer )
        {
        aBuffer->Reset();
        delete aBuffer;
        }
        
    aBuffer = CBufFlat::NewL( KSConBufferSize );
    aBuffer->ResizeL( KSConBufferSize );
    
    TObexRFileBackedBuffer bufferdetails( *aBuffer, iFile, 
    CObexBufObject::EDoubleBuffering );
    aObject->SetDataBufL( bufferdetails );
    LOGGER_WRITE_1( "CSConInboxHandler::CreateInboxAttachmentL() : returned %d", ret );
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSConInboxHandler::SaveObjToInboxL( CObexBufObject*& aObject )
// Saves the object to the Inbox
// -----------------------------------------------------------------------------
//  
TInt CSConInboxHandler::SaveObjToInboxL( CObexBufObject*& aObject )
    {
    TRACE_FUNC_ENTRY;
    TInt ret( KErrNone );
    
    if( iDrive == EDriveE )
        {
        ret = TObexUtilsMessageHandler::GetMmcFileSystemStatus();
        LOGGER_WRITE_1( "GetMmcFileSystemStatus returned %d", ret );
        }
    else
        {
        ret = TObexUtilsMessageHandler::GetFileSystemStatus();
        LOGGER_WRITE_1( "GetFileSystemStatus returned %d", ret );
        }
       
    if( ret == KErrNone )
        {
        TInt tempDrive = GetMessageCenterDriveL();
    
        if( tempDrive == iDrive )
            {
            LOGGER_WRITE_1( "before SaveObjToInboxL in SaveObjToInboxL %d", ret );
            TRAP( ret, TObexUtilsMessageHandler::SaveObjToInboxL( aObject, 
            iFile, iMsvIdParent ) );
            LOGGER_WRITE_1( "after SaveObjToInboxL in SaveObjToInboxL %d", ret );
            }
        else
            {
            ret = KErrGeneral;
            }   
            
        if( ret != KErrNone )
            {
            LOGGER_WRITE_1( "before RemoveInboxEntriesL in SaveObjToInboxL %d", ret );
            TObexUtilsMessageHandler::RemoveInboxEntriesL( aObject, 
            iMsvIdParent );
            LOGGER_WRITE_1( "after RemoveInboxEntriesL in SaveObjToInboxL %d", ret );
            }
        }
        
    LOGGER_WRITE_1( "CSConInboxHandler::SaveObjToInboxL( CObexBufObject* aObject ) : returned %d", ret );
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSConInboxHandler::AbortInboxOperation( CObexBufObject*& aObject )
// Aborts the storing operation
// -----------------------------------------------------------------------------
//  
TInt CSConInboxHandler::AbortInboxOperation( CObexBufObject*& aObject )
    {
    TRACE_FUNC_ENTRY;
    TInt ret( KErrNone );
    TRAP( ret, TObexUtilsMessageHandler::RemoveInboxEntriesL( aObject, 
    iMsvIdParent ) );
    LOGGER_WRITE_1( "CSConInboxHandler::AbortInboxOperation() returned %d", ret );
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSConInboxHandler::GetMessageCenterDriveL()
// Returns the current message center drive
// -----------------------------------------------------------------------------
//  
TInt CSConInboxHandler::GetMessageCenterDriveL()
    {
    TRACE_FUNC_ENTRY;
    TInt drive;
    CMsvSession* msvSession = CMsvSession::OpenSyncL( *this );
    CleanupStack::PushL( msvSession );
    TDriveUnit driveUnit = msvSession->CurrentDriveL();
    drive = driveUnit.operator TInt();
    CleanupStack::PopAndDestroy( msvSession );
    LOGGER_WRITE_1( "CSConInboxHandler::GetMessageCenterDriveL() returned %d", drive );
    return drive;
    }

// -----------------------------------------------------------------------------
// CSConInboxHandler::HandleSessionEventL( TMsvSessionEvent aEvent, 
//                              TAny* aArg1, 
//                              TAny* aArg2, 
//                              TAny* aArg3 )
// Implementation of MMsvSessionObserver::HandleSessionEventL
// -----------------------------------------------------------------------------
//      
void CSConInboxHandler::HandleSessionEventL( TMsvSessionEvent /*aEvent*/, 
                                TAny* /*aArg1*/, 
                                TAny* /*aArg2*/, 
                                TAny* /*aArg3*/ )
    {
    }
    
// -----------------------------------------------------------------------------
// CSConInboxHandler::CSConInboxHandler()
// Default constructor
// -----------------------------------------------------------------------------
//  
CSConInboxHandler::CSConInboxHandler()
    {
    }   

// End of file

