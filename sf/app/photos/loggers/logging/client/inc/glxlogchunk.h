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
* Description:   Logging to chunk wrapper for MC Photos
*
*/



#ifndef T_GLXLOGCHUNK_H
#define T_GLXLOGCHUNK_H

// EXTERNAL INCLUDES
#include <e32std.h>
#include <f32file.h>

/**
 * RGlxLogUtility.
 * Encapsulates the use of RChunk for log writing
 */
NONSHARABLE_CLASS( RGlxLogUtility )
    {
    public:
        /**
         * Opens the log chunk with given name
         * @param aName, the full name of the chunk
         * @param aReadOnly, whether to open in readonly or read-write
         */
        TInt Open( const TDesC& aName, TBool aReadOnly );

        /**
         * Creates the log chunk with given name
         * @param aName, the full name of the chunk
         */
        void CreateL( const TDesC& aName );

        /**
         * Closes the opened chunk. This needs to be called
         * before an object of this class goes out of scope
         */
        void Close();
        
        /**
         * @return the size of the log chunk
         */
        TInt ChunkSize();
        
        /**
         * @return the chunk unique Id (set by client)
         */
        TObjectId Id();

        /**
         * Sets the unique id for the log chunk
         * @param aId the id to set for the chunk
         */
        void SetId( TObjectId aId );
        
        /**
         * @return Current chunk write address
         */
        TUint8* CurrentAddress();

        /**
         * Sets the current write address
         * @param New write address
         */
        void SetCurrentAddress( TUint8* aValue );

        /**
         * @return Last address written to file
         */
        TUint8* LastLoggedAddress();

        /**
         * Sets the last logged address
         * @param New last logged address
         */
        void SetLastLoggedAddress( TUint8* aValue );

        /**
         * @return The topmost writable address of the chunk
         */
        TUint8* BaseAddress();

        /**
         * @return The bottom most writable address of the chunk
         */
        TUint8* LastAddress();

    private:

        /// Own: chunk to write to
        RChunk iChunk;
    };

/**
 * RGlxLogClient.
 * Client API for log writing to chunk
 */
NONSHARABLE_CLASS( RGlxLogClient )
    {
    public:

		/**
		 * Opens the log chunk with the given unique id
		 * @param aId unique id for the log writing thread
		 */
        IMPORT_C TInt Open( TObjectId aId );
        
        /**
         * Closes the log handle, needs to be called before an
         * object of this class goes out of scope
         */
        IMPORT_C void Close();

        /**
         * Writes a descriptor to the log
         * @param aLogEntry the log entry to write
         */
        IMPORT_C void Write( const TDesC8& aLogEntry );

    private:

    	/// Own: log utility
        RGlxLogUtility iLogUtility;

    };
    
/**
 * RGlxLogManager.
 * Management API for log creation and committing to file
 */
NONSHARABLE_CLASS( RGlxLogManager )
    {
    public:

        /**
         * Creates the log chunks
         */
        IMPORT_C void CreateL();
        
        /**
         * Releases the log chunks
         */
		IMPORT_C void Release();
 
        /**
         * Writes the log to a file
         * One file is created per chunk.
         * @param aFolder the directory where to store the logs
         */
        IMPORT_C void CommitToFileL( const TDesC& aFolder );
        
    private:

		// helper method to write all descriptors of a chunk to a 
		// file
        void CommitToFileL( RGlxLogUtility& aUtility, RFile& aFile );
        
    private:
 
    	/// Own: log utility
        RGlxLogUtility iLogUtility1;
    	/// Own: log utility
        RGlxLogUtility iLogUtility2;    
        /// Own: file server handle
        RFs iFs;

    };

#endif // T_GLXLOGCHUNK_H
    
 