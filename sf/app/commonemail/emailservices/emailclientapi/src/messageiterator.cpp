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
* Description: This file implements class CMessageIterator.
*
*/

#include "emailmessage.h"
#include "messageiterator.h"
#include "emailapiutils.h"
#include "CFSMailPlugin.h"
#include "emailapiutils.h"
#include "emailclientapi.hrh"
#include "MFSMailIterator.h"

// number of messages in chunk to retrive from protocol plugin. Actual chunk
// size is one less because last element is used for reference to next chunk
// retrieval. See ReadNextChunkL() for details.
const TInt KMessageChunkSize = 5;

const TInt KUndefined = -1;

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CMessageIterator* CMessageIterator::NewL( 
    MFSMailIterator* aIterator,
    CPluginData& aPluginData,
    TUint aCount )
    {
    CMessageIterator* iter = new ( ELeave ) 
        CMessageIterator( aIterator, aPluginData, aCount );
    return iter;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CMessageIterator::~CMessageIterator()
    {
    iPluginData.ReleaseInstance();
    TInt count = iMessageArray.Count();
    iMessageArray.ResetAndDestroy();
    for ( TInt i = count; i < iFsMessageArray.Count(); i++ )
        {
        delete iFsMessageArray[i];
        }
    iFsMessageArray.Reset();
    delete iIterator;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TEmailTypeId CMessageIterator::InterfaceId() const
    {
    return KEmailIFUidMessageIterator;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CMessageIterator::Release()
    {
    delete this;
    }

// -----------------------------------------------------------------------------
// Returns next message
// Messages are retrieved from protocol plugin in chunks of x messages and
// when NextL has be called for x times, next chunk is retrieved. This
// is trade-off between RAM consumption and speed.
// -----------------------------------------------------------------------------
MEmailMessage* CMessageIterator::NextL()
    {
    if ( iCursor == KUndefined )
        {
        // Buffer empty, read the first chunk
        if ( !ReadNextChunkL() )
            {
            // No messages found, 
            // Reset the cursor and return NULL
            iCursor = KUndefined;
            iStartMsgId = TFSMailMsgId();
            return NULL;
            }
        // Items found, return the first item in the buffer
        iCursor = 0;
        }
    else
        {
        // Iterate to the next item
        iCursor++;
        }
    return ReadFromChunkL();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
MEmailMessage* CMessageIterator::PreviousL()
    {
    if ( iCursor == KUndefined )
        {
        iStartMsgId = TFSMailMsgId();
        // Buffer empty, client should first call NextL
        return NULL;
        }
    // Iterate to the previous item
    iCursor--;
    return ReadFromChunkL();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TBool CMessageIterator::ReadNextChunkL()
    {
    User::LeaveIfNull( iIterator );
    
    TBool chunkFound = EFalse;

    // Delete previous items if exist
    CleanCache();
    iHasMoreNextItems = iIterator->NextL(
            iStartMsgId,
            KMessageChunkSize,
            iFsMessageArray );
    if ( iFsMessageArray.Count() > 0 )
        {
        chunkFound = ETrue;
        // Set the cursor to the first item 
        iCursor = 0;
        iHasMorePrevItems = ETrue;
        }
    return chunkFound;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TBool CMessageIterator::ReadPreviousChunkL()
    {
    User::LeaveIfNull( iIterator );

    TBool chunkFound = EFalse;

    // Delete previous items if exist
    CleanCache();
    RPointerArray<CFSMailMessage> tmp;
    iHasMorePrevItems = iIterator->PreviousL(
            iStartMsgId,
            KMessageChunkSize, 
            tmp );
    TInt count = tmp.Count();
    if ( count > 0 )
        {
        // It seems that items have been found
        // Set the cursor to the end of array
        iCursor = count - 1;
        iHasMoreNextItems = ETrue;
        chunkFound = ETrue;
        // Revise the order of the buffer.
        // Create complete cache, so it is easier to iterate, 
        // as now we have to start from the end of cache
        for ( TInt i = count; i > 0; i-- )
            {
            CFSMailMessage* msg = tmp[i-1];
            iFsMessageArray.AppendL( msg );
            AddToCacheL( msg );
            }
        }

    return chunkFound;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
MEmailMessage* CMessageIterator::ReadFromChunkL()
    {
    if ( iCursor >= KMessageChunkSize )
        {
        if ( iHasMoreNextItems )
            {
            iStartMsgId = iFsMessageArray[iCursor-1]->GetMessageId();
            if ( !ReadNextChunkL() )
                {
                // No more items found, 
                // set the cursor to the previous item,
                // and return NULL.
                iCursor--;
                return NULL;
                }
            }
        else
            {
            // No more items found, 
            // set the cursor to the previous item,
            // and return NULL.
            iCursor--;
            return NULL;
            }
        }
    else if ( iCursor < 0 )
        {
        iStartMsgId = iFsMessageArray[0]->GetMessageId();
        if ( iHasMorePrevItems )
            {
            if ( !ReadPreviousChunkL() )
                {
                // Buffer is empty now, client should call nextL,
                // and start reading from the start of the buffer
                iCursor = KUndefined;
                return NULL;
                }
            }
        else
            {
            // Buffer is empty now, client should call nextL,
            // and start reading from the start of the buffer
            iCursor = KUndefined;   
            return NULL;
            }
        }

    if ( iCursor < iFsMessageArray.Count() )
        {
        /* There are items to read in the cache */
        if ( iCursor >= iMessageArray.Count() )
            {
            AddToCacheL( iFsMessageArray[iCursor] );
            }
        return iMessageArray[iCursor];
        }
    else
        {
        // No more items found,
        // set the cursor to the previous item,
        // and return NULL.
        iCursor--;
        return NULL;
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CMessageIterator::AddToCacheL( CFSMailMessage* aFsMsg )
    {
    CEmailMessage* message = CEmailMessage::NewL( iPluginData, aFsMsg, EAPIOwns );
    iMessageArray.AppendL( message );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CMessageIterator::CleanCache()
    {
    // This is strange loop indeed
    // Both arrays has items that are deleted, iMessageArray has objects that point to iFsMessageArray
    // Index in both arrays goes syncronously
    // To prevent double destruction, first delete iMessageArray onjects and then the rest of the iFsMessageArray
    TInt count = iMessageArray.Count();
    iMessageArray.ResetAndDestroy();
    for ( TInt i = count; i < iFsMessageArray.Count(); i++ )
        {
        iFsMessageArray.Remove( count );
        }
    iFsMessageArray.Reset();    
    }
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TUint CMessageIterator::Count() const
    {
    return iCount;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CMessageIterator::ConstructL()
    {
    iPlugin = iPluginData.ClaimInstanceL();
    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CMessageIterator::CMessageIterator( 
    MFSMailIterator* aIterator,
    CPluginData& aPluginData,
    TUint aCount ) :
    iIterator( aIterator ),
    iPluginData( aPluginData ),
    iStartMsgId( TFSMailMsgId() ),
    iState( EReadNextMessageChunk ),
    iFsMessageArray( KMessageChunkSize ),
    iCount( aCount ),
    iCursor ( KUndefined ),
    iFirstMsgId( TFSMailMsgId() ),
    iHasMoreNextItems( ETrue ),
    iHasMorePrevItems( ETrue )
    {
    }

// End of file
