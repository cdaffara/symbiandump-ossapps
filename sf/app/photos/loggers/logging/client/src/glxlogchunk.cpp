/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Logging to chunk wrappe for MC Photos
*
*/



// CLASS HEADER
#include "glxlogchunk.h"

// EXTERNAL INCLUDES
#include <e32svr.h>

// LOCAL DECLARATIONS
namespace
	{
	// size for the chunks
	const TInt KLogChunkSize = 1000000; // 1 million bytes
	// name for chunk 1
	_LIT( KLogChunk1Name, "MC_Photos_Logchunk1" );
	// name for chunk 2
	_LIT( KLogChunk2Name, "MC_Photos_Logchunk2" );
	// extension for log file
	_LIT( KLogFileExtension, ".log" );
	// carriage return / line feed
	_LIT8( KLogCR, "\r" );
	_LIT8( KLogLF, "\n" );
	// constant for machine word alignment
	const TInt KBytesInAWord = sizeof( TInt32 );
	}
	
inline void Panic(TInt aReason)
    {
	_LIT(applicationName,"Glx Logging");
    User::Panic(applicationName, aReason);
    }

// -----------------------------------------------------------------------------
// RGlxLogUtility::Open
// -----------------------------------------------------------------------------
TInt RGlxLogUtility::Open( const TDesC& aName, TBool aReadOnly )
	{
	// open the chunk
	return iChunk.OpenGlobal( aName, aReadOnly );
	}

// -----------------------------------------------------------------------------
// RGlxLogUtility::CreateL
// -----------------------------------------------------------------------------
void RGlxLogUtility::CreateL( const TDesC& aName )
    {
	// create the chunk, leave if error
	TInt error = iChunk.CreateGlobal( aName, KLogChunkSize, KLogChunkSize  );
	// dont treate already exists as an error
	if( KErrAlreadyExists == error )
		{
		// open in read-write
		User::LeaveIfError( iChunk.OpenGlobal( aName, EFalse ) );
		}
	else
		{
		User::LeaveIfError( error );
		}

    // initialise the iChunk to all zeros.
    Mem::FillZ( iChunk.Base(), iChunk.Size() );
    // initialise current address to base
    SetCurrentAddress( BaseAddress() );
    // initialise last logged address to base
    SetLastLoggedAddress( BaseAddress() );
    }

// -----------------------------------------------------------------------------
// RGlxLogUtility::Close
// -----------------------------------------------------------------------------
void RGlxLogUtility::Close()
	{
	iChunk.Close();
	}

// -----------------------------------------------------------------------------
// RGlxLogUtility::ChunkSize
// -----------------------------------------------------------------------------
TInt RGlxLogUtility::ChunkSize()
	{
	return iChunk.Size();
	}
    
// -----------------------------------------------------------------------------
// RGlxLogUtility::Id
// -----------------------------------------------------------------------------
TObjectId RGlxLogUtility::Id()
	{
	// take chunk base address
	TUint8* ptr_in_chunk = iChunk.Base();
	// cast that to TObjectId*
	TObjectId* ptr_as_tobjectid = reinterpret_cast< TObjectId* >( ptr_in_chunk );
	// return the id
	return *ptr_as_tobjectid;
	}

// -----------------------------------------------------------------------------
// RGlxLogUtility::SetId
// -----------------------------------------------------------------------------
void RGlxLogUtility::SetId( TObjectId aId )
	{
	// take chunk base address
	TUint8* ptr_in_chunk = iChunk.Base();
	// cast that to TObjectId*
	TObjectId* ptr_as_tobjectid = reinterpret_cast< TObjectId* >( ptr_in_chunk );
	// assign the id in place
	*ptr_as_tobjectid = aId;
	}

// -----------------------------------------------------------------------------
// RGlxLogUtility::CurrentAddress
// -----------------------------------------------------------------------------
TUint8* RGlxLogUtility::CurrentAddress()
	{
	// take chunk base address plus object id
	TUint8* ptr_in_chunk = iChunk.Base() + sizeof( TObjectId );
	// cast that to TInt*
	TUint32* ptr_as_tint = reinterpret_cast< TUint32* >( ptr_in_chunk );
	// dereference that pointer to read the 32 bits that are the address
	TUint32 value_of_pointer = *ptr_as_tint;
	// then return the value as TUint8*
	return reinterpret_cast< TUint8* >( value_of_pointer );
	}

// -----------------------------------------------------------------------------
// RGlxLogUtility::SetCurrentAddress
// -----------------------------------------------------------------------------
void RGlxLogUtility::SetCurrentAddress( TUint8* aValue )
	{
	// take chunk base address plus object id
	TUint8* ptr_in_chunk = iChunk.Base() + sizeof( TObjectId );
	// cast that to TInt*
	TUint32* ptr_as_tint = reinterpret_cast< TUint32* >( ptr_in_chunk );
	// assign the addres to TInt
	TUint32 new_value = reinterpret_cast< TUint32 >( aValue );
	// ensure we're byte aligned - ARM requires 32 bit alignment to machine word
	// boundary!!
	TInt remainder = new_value % KBytesInAWord;
	if ( remainder > 0 )
	    {
	    new_value += ( KBytesInAWord - remainder );
	    }
	// set the new value to the chunk
	*ptr_as_tint = new_value;
	}

// -----------------------------------------------------------------------------
// RGlxLogUtility::LastLoggedAddress
// -----------------------------------------------------------------------------
TUint8* RGlxLogUtility::LastLoggedAddress()
	{
	// take chunk base address plus object id plus one pointer
	TUint8* ptr_in_chunk = 
		iChunk.Base() + sizeof( TObjectId ) + sizeof( TUint8* );
	// cast that to TInt*
	TUint32* ptr_as_tint = reinterpret_cast< TUint32* >( ptr_in_chunk );
	// dereference that pointer to read the 32 bits that are the address
	TUint32 value_of_pointer = *ptr_as_tint;
	// then return the value as TUint8*
	return reinterpret_cast< TUint8* >( value_of_pointer );
	}

// -----------------------------------------------------------------------------
// RGlxLogUtility::SetLastLoggedAddress
// -----------------------------------------------------------------------------
void RGlxLogUtility::SetLastLoggedAddress( TUint8* aValue )
	{
	// take chunk base address plus object id plus one pointer
	TUint8* ptr_in_chunk = 
		iChunk.Base() + sizeof( TObjectId ) + sizeof( TUint8* );
	// cast that to TInt*
	TUint32* ptr_as_tint = reinterpret_cast< TUint32* >( ptr_in_chunk );
	// assign the addres to TInt
	TUint32 new_value = reinterpret_cast< TUint32 >( aValue );
	// ensure we're byte aligned - ARM requires 32 bit alignment to machine word
	// boundary!!
    TInt remainder = new_value % KBytesInAWord;
	if ( remainder > 0 )
	    {
	    new_value += ( KBytesInAWord - remainder );
	    }	
	// set the new value to the chunk
	*ptr_as_tint = new_value;
	}

// -----------------------------------------------------------------------------
// RGlxLogUtility::BaseAddress
// -----------------------------------------------------------------------------
TUint8* RGlxLogUtility::BaseAddress()
	{
	// take chunks base address
	TUint8* base = iChunk.Base();
	// calculate the topmost write address, our header is 
	// TObjectId and two TUint8*
	return base + sizeof( TObjectId ) + sizeof( TUint8* ) * 2;
	}

// -----------------------------------------------------------------------------
// RGlxLogUtility::LastAddress
// -----------------------------------------------------------------------------
TUint8* RGlxLogUtility::LastAddress()
	{
	// return chunks base address plus its size
	return iChunk.Base() + iChunk.Size();
	}

// -----------------------------------------------------------------------------
// RGlxLogClient::Open
// -----------------------------------------------------------------------------
EXPORT_C TInt RGlxLogClient::Open( TObjectId aId )
    {
    // try to open first log chunk, in read-write mode
    TInt err = iLogUtility.Open( KLogChunk1Name(), EFalse );
    if ( err != KErrNone )
        {
        return err;
        }
    // check id
    TObjectId id = iLogUtility.Id();
    if ( id  == TObjectId( 0 ) )
        {
        // no id set, so reserve this for us and use this chunk
        iLogUtility.SetId( aId );
        }
    // check if our id was there?
    else if( id != aId )
        {
        // not our chunk, try second chunk in read-write mode
        err = iLogUtility.Open( KLogChunk2Name(), EFalse );
        if ( err != KErrNone )
            {
            return err;
            }
        // check id
        id = iLogUtility.Id();
        if ( id  == TObjectId( 0 ) )
            {
            // no id, reserve this for us and use this chunk
            iLogUtility.SetId( aId );
            }
        else if ( id != aId )
            {
            // both chunks already reserved, return error
            return KErrNotFound;
            }
       	}
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RGlxLogClient::Close
// -----------------------------------------------------------------------------
EXPORT_C void RGlxLogClient::Close()
	{
    iLogUtility.Close();
	}

// -----------------------------------------------------------------------------
// RGlxLogClient::Write
// -----------------------------------------------------------------------------
EXPORT_C void RGlxLogClient::Write( const TDesC8& aLogEntry )
    {
    // get the size of log entry (in bytes)
    TInt num_bytes = aLogEntry.Size();
    // take current address and chunk size to ensure log entry fits
    TUint8* current_address = iLogUtility.CurrentAddress();
    // calculate the new current address, we write the size and the data
    TUint8* after_write = current_address + num_bytes + sizeof( TInt );
    // chck that we fit
    if( after_write >= iLogUtility.LastAddress() )
    	{
    	// we dont fit in the end
    	// need to mark the old current_address so that
    	// manager knows we jumped to the start
	    TInt* last_indicator_ptr = 
	    	reinterpret_cast< TInt* >( current_address );
	   	// assign KErrNotFound there
		*last_indicator_ptr = KErrNotFound;
    	// write this entry to the base address
    	current_address = iLogUtility.BaseAddress();
    	}
    // we need to store the size in the chunk first so take a TInt*
    TInt* size_ptr = reinterpret_cast< TInt* >( current_address );
    // assign new value in place
    *size_ptr = num_bytes;
    // increase address
    current_address += sizeof( TInt );
    // copy the data, first target, then source and last number of bytes
    Mem::Copy( current_address, aLogEntry.Ptr(), num_bytes );
    // and set the new current address
    iLogUtility.SetCurrentAddress( current_address + num_bytes );
    }

// -----------------------------------------------------------------------------
// RGlxLogManager::CreateL
// -----------------------------------------------------------------------------
EXPORT_C void RGlxLogManager::CreateL()
    {
    // connect to the file server
    User::LeaveIfError( iFs.Connect() );
    
    // create two log chunks
    iLogUtility1.CreateL( KLogChunk1Name );
    iLogUtility2.CreateL( KLogChunk2Name );
    }

// -----------------------------------------------------------------------------
// RGlxLogManager::Release
// -----------------------------------------------------------------------------
EXPORT_C void RGlxLogManager::Release()
    {
    // close file server handle
    iFs.Close();
    
    // release both log chunks
    iLogUtility1.Close();
    iLogUtility2.Close();
    }

// -----------------------------------------------------------------------------
// RGlxLogManager::CommitToFileL
// -----------------------------------------------------------------------------
EXPORT_C void RGlxLogManager::CommitToFileL( const TDesC& aFolder )
	{
	// open the file
	RFile file;
	CleanupClosePushL( file );
	
	// chunk1
	// create the file name
	TFileName fileName;
	fileName = aFolder;
	fileName.Append( KLogChunk1Name );
	fileName.Append( KLogFileExtension );
	// try to open the file	
	TInt err = file.Open( iFs, fileName, EFileWrite );
	if ( err == KErrNotFound )
	    {
	    // file doesn't exist so create it
	    err = file.Create( iFs, fileName, EFileWrite );
	    }
	User::LeaveIfError( err );
	// write the first chunk to file
	CommitToFileL( iLogUtility1, file );
	// close the file
	file.Close();
	
	// chunk2
	// reset the file name
	fileName.Zero();
	// create the file name
	fileName = aFolder;
	fileName.Append( KLogChunk2Name );
	fileName.Append( KLogFileExtension );
	// try to open the file
	err = file.Open( iFs, fileName, EFileWrite );
	if ( err == KErrNotFound )
	    {
	    // file doesn't exist so create it
	    err = file.Create( iFs, fileName, EFileWrite );
	    }
	User::LeaveIfError( err );
	// write the second chunk to file
	CommitToFileL( iLogUtility2, file );

	CleanupStack::PopAndDestroy( &file );
	}

// -----------------------------------------------------------------------------
// RGlxLogManager::CommitToFileL
// -----------------------------------------------------------------------------    
void RGlxLogManager::CommitToFileL( RGlxLogUtility& aUtility, RFile& aFile )
    {
    // Need to explicitly move to the end of the file as it's not done
    // automatically on call to Write
    TInt pos = 0;
    TInt err = aFile.Seek( ESeekEnd, pos );
    User::LeaveIfError( err );
    TPtr8 logEntryPtr( 0, 0 );
    TInt logEntrySize = 0;
    
    // what's previously been logged to the file?
    TUint8* lastLoggedAddress = aUtility.LastLoggedAddress();
    
    // how much more has been added to the chunk?
    TUint8* currentAddress = aUtility.CurrentAddress();
    
    // write each of the chunk's logged entries to the file 
    TInt32* sizePtr = NULL;
    while ( currentAddress != lastLoggedAddress && err == KErrNone )
        {
        // get the logEntry's size
        sizePtr = reinterpret_cast< TInt32* >( lastLoggedAddress );
        logEntrySize = *sizePtr;
        if ( logEntrySize == KErrNotFound )
            {
            // logged entries have wrapped around the end of the chunk
            // so start from the beginning
            lastLoggedAddress = aUtility.BaseAddress();
            sizePtr = reinterpret_cast< TInt32* >( lastLoggedAddress );
            logEntrySize = *sizePtr;
            }
        // set an error - this will be reset to KErrNone if we succeed
        // with finding and writing a log entry
        err = KErrNotFound;
        if ( logEntrySize > 0 )
            {
            // move the current address to the data
            lastLoggedAddress += sizeof( TInt32 );
            // extract the log entry's data 
            logEntryPtr.Set( lastLoggedAddress, logEntrySize, logEntrySize );
            // write the log entry to the file
            err = aFile.Write( logEntryPtr );
            // append carriage return and line feed to the entry
            err = aFile.Write( KLogCR );
            err = aFile.Write( KLogLF );
            // ensure we align to a multiple of a 4-byte boundary
            TInt remainder = logEntrySize % KBytesInAWord;
            if ( remainder > 0 )
                {
                // not aligned so add some padding
                logEntrySize += ( KBytesInAWord - remainder );
                }
 
            // update the last logged address
            lastLoggedAddress += logEntrySize;
            }
        }
    // update the last logged address
    aUtility.SetLastLoggedAddress( lastLoggedAddress );
	// 
	// commit the data
	err = aFile.Flush();
	User::LeaveIfError( err );
    }
