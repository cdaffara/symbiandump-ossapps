/*
* Copyright (c) 2003 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Entry wrapper for server side components using CMmsEncode and CMmsDecode
*
*/



#ifndef MMSSERVERENTRY_H
#define MMSSERVERENTRY_H

//  INCLUDES
#include <mmsvattachmentmanager.h>
#include <s32file.h>
#include "mmsentrywrapper.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CDRMMessageParser;
class CMsvStore;
class CMmsDrmCaf;

// CLASS DECLARATION

/**
* Entry wrapper implementing MMmsEntryWrapper for server side.
*
* @lib mmscodec.lib
* @since 2.1
*/
class CMmsServerEntry :public CBase, public MMmsEntryWrapper
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aFs opened file system handle.
        * @param aServerEntry reference to a server entry,
        *     originally refers to the entry to be handled.
        * @param aServiceId TMsvId of MMS service entry.
        */
        IMPORT_C static CMmsServerEntry* NewL( RFs& aFs,
            CMsvServerEntry& aServerEntry,
            TMsvId aServiceId );
        
        /**
        * Destructor.
        */
        virtual ~CMmsServerEntry();

    public: // Functions from base classes
    
        /**
        * From MMmsEntryWrapper: Provide an edit store for editing messages
        * Ownership of the store is transferred. Caller must delete the store
        * @return CMsvStore* if successful, leaves if unsuccessful
        */
        CMsvStore* EditStoreL();

        /**
        * From MMmsEntryWrapper: provide a read store for editing messages
        * Ownership of the store is transferred. Caller must delete the store
        * @return CMsvStore* if successful, leaves if unsuccessful
        */
        CMsvStore* ReadStoreL();

        /**
        * From MMmsEntryWrapper: reads index entry contents from current entry
        * @param TMsvEntry reference to entry to be filled with data
        * @return error code, KErrNone if successful
        */
        TInt GetIndexEntry( TMsvEntry& aTMsvEntry );
       
        /**
        * From MMmsEntryWrapper: stores index entry contents to current entry
        * @param TMsvEntry reference to entry to be written to disk
        * @return error code KErrNone, if successful
        */
        TInt ChangeIndexEntry( TMsvEntry& aTMsvEntry );

        /**
        * From MMmsEntryWrapper: Sets context to specified message entry
        * @param aId ID of the new message entry.
        * @return error code, KErrNone if successful
        *     if not successful, context is not changed
        */
        TInt SetCurrentEntry( TMsvId aId );

        /**
        * From MMmsEntryWrapper: Returns total size of all attachments including mime headers
        * and binary attachment data. Linked attachments are included
        * This function is needed to allocate the encoding buffer.
        * @param aStore Read store for the message entry
        * @return combined size of all attachments, including linked attachments
        */
        TInt AttachmentsSizeL( CMsvStore& aStore );

        /**
        * From MMmsEntryWrapper: Check if disk space is below critical level.
        * @param aDataSize amount of data to be added. If aDataSize == 0,
        *     returns info if disk space is already below critical level.
        * @return error code KErrDiskFull if data does not fit,
        *     KErrNone if data fits.
        *     May return some other error code if appropriate.
        *     KErrNone always means it is all right to save the data.
        */
        TInt DiskSpaceBelowCriticalLevelL( TInt aDataSize );

        /**
        * From MMmsEntryWrapper: Store CMmsHeaders to messaging store.
        *     ( main message entry )
        * @param aMmsHeaders headers to store.
        * @param aStore store to be used. If not given, function gets and commits store
        * @return error code if not successful, KErrNone if successful.
        */
        TInt StoreMmsHeadersL( CMmsHeaders& aMmsHeaders, CMsvStore* aStore );

        /**
        * From MMmsEntryWrapper: Restore CMmsHeaders from messaging store.
        *     ( main message entry )
        * @param aMmsHeaders headers to restore.
        * @param aStore store to be used. If not given, function gets and closes store
        * Leaves if cannot restore headers.
        */
        void RestoreMmsHeadersL( CMmsHeaders& aMmsHeaders, CMsvStore* aStore );

        /**
        * From MMmsEntryWrapper: Save attachment data and MIME headers.
        * Create attachment, save data to file and save MIME headers.
        *     Must check first if enough disk space.
        *     DRM handling is done here if needed.
        *     Return code tells if whole attachment was removed.
        *     This is the one-shot function that does everything:
        *     Creates attachent, opens file, writes data, flushes it,
        *     closes file, saves MIME headers.
        *     Caller must commit and close store
        * @param aStore edit store for current entry
        * @param aFileName Suggested filename.
        * @param aAttachmentData Pointer to the attachment binary data.
        * @param aMimeHeaders Mime header structure to be saved.
        * @param aAttachment id of the newly created attachment if successful
        * @param aAttachmentSize size of the attachment binary data + mime headers
        * @param aDRMFlags structure where DRM flags are collected.
        *     These flags will ultimately be collected together for all
        *     attachments and stored into iMtmData1 of the parent entry.
        *     Therefore the function must only OR new flags into the structure
        *     and must not change any other bits (the same value may be reused
        *     for all attachments collecting the flags together).
        * @return Error code must indicate if attachment was removed (DRM) so that
        *     the first attachment id may be adjusted. (Decode must keep track
        *     of the first attachment in case root is not specified for
        *     multipart/related structure.
        *     KErrNone = All well.
        *     KDRMRemoved = all well otherwise, but DRM attachment was removed,
        *         and first attachment id must be adjusted accordingly.
        *     Some other error code means a more serious error.
        */
        TInt CreateFileAttachmentL(
            CMsvStore& aStore,
            const TDesC& aFileName,
            const TPtrC8& aAttachmentData,
            CMsvMimeHeaders& aMimeHeaders,
            TMsvAttachmentId& aAttachment,
            TInt& aAttachmentSize,
            TInt32& aDRMFlags);

        /**
        * From MMmsEntryWrapper: Deletes a message store entry.
        * @param aEntryIndex is the index of the entry, which will be deleted. 
        * If the Entry has children, all children are destroyed too.
        * @return error code if not successful, KErrNone if successful.
        */
        TInt DeleteEntry( TMsvId aEntryIndex );

        /**
        * From MMmsEntryWrapper: Check if aFileName would be a valid filename.
        * Cannot use path because absolute path is not known. Only filename is checked
        * @param aFileName suggestion of a filename.
        * @return ETrue if filename is valid, EFalse if not.
        */
        TBool IsValidFilename( const TPtrC& aFileName );

        /**
        * From MMmsEntryWrapper: Read the decoding flag from the service entry.
        *     (Info stored in MMS service entry)
        * @return ETrue = Log add details while decoding
        *     EFalse = Don't log details.
        */
        TBool GetDecodingFlag();

        /**
        * From MMmsEntryWrapper: Read the flag that tells if all incoming
        *     messages are dumped to a file. (Info stored in MMS service entry)
        * @return ETrue = dump binary message to file while decoding
        *     EFalse = No binary dump wanted
        */
        TBool GetDumpFlag();
        
        /**
        * From MMmsEntryWrapper
        * Create Empty attachment file and save Mime headers.
        *     DRM check is done here.
        *     This function is the initialization function for chunked
        *         attachment handling.
        *     This function creates attachent, opens file, and saves MIME headers.
        *
        * @since v3.1
        * @param aStore edit store for current message entry
        *      Caller must keep store open until FinalizeAttachmentL has been called.
        * @param aFileName Suggested filename.
        * @param aMimeHeaders Mime header structure to be saved.
        * @param aAttachment id of the newly created attachment if successful
        * @param aAttachmentSize size of the attachment binary data + mime headers
        */
        void CreateEmptyFileAttachmentL(
            CMsvStore& aStore,
            const TDesC& aFileName,
            CMsvMimeHeaders& aMimeHeaders,
            TMsvAttachmentId& aAttachment);
            
        /**
        * From MMmsEntryWrapper
        * Write attachment data into file.
        * Call DRM handler if needed.
        *
        * @since v3.1
        *
        * @param aAttachmentData Pointer to the attachment binary data.
        * @param aDRMFlags structure where DRM flags are collected.
        *     These flags will ultimately be collected together for all
        *     attachments and stored into iMtmData1 of the parent entry.
        *     Therefore the function must only OR new flags into the structure
        *     and must not change any other bits (the same value may be reused
        *     for all attachments collecting the flags together).
        * @return Error code must indicate if attachment was removed (DRM) so that
        *     KErrNone = All well.
        *     KDRMRemoved = all well otherwise, but DRM attachment was removed,
        *         and first attachment id must be adjusted accordingly.
        *     Some other error code means an error.
        * Leaves if memory runs out
        */
        TInt ProcessAttachmentDataL(
            const TPtrC8& aAttachmentData,
            TInt32& aDRMFlags );
        
        /**
        * From MMmsEntryWrapper
        * Finalize the attachment.
        * Flush buffers and close file, calculate total size.
        * Caller must commit the store after this
        *
        * @since v3.1
        *
        * @param aStore edit store for current entry
        * @param aAttachmentSize size of the attachment binary data + mime headers
        * @param aDRMFlags structure where DRM flags are collected.
        *     These flags will ultimately be collected together for all
        *     attachments and stored into iMtmData1 of the parent entry.
        *     Therefore the function must only OR new flags into the structure
        *     and must not change any other bits (the same value may be reused
        *     for all attachments collecting the flags together).
        * @return Error code must indicate if attachment was removed (DRM) so that
        *     KErrNone = All well.
        *     KDRMRemoved = all well otherwise, but DRM attachment was removed,
        *         and first attachment id must be adjusted accordingly.
        *     Some other error code means an error.
        * Leaves if memory runs out
        */
        TInt FinalizeAttachmentL(
            CMsvStore& aStore,
            TInt& aAttachmentSize,
            TInt32& aDRMFlags );


    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

        /**
        * C++ constructor.
        * @param aFs opened file system handle.
        * @param aServerEntry reference to a server entry,
        *     originally refers to the entry to be handled.
        */
        CMmsServerEntry( RFs& aFs, CMsvServerEntry& aServerEntry );

        /**
        * @param aServiceId TMsvId of MMS service entry.
        */
        void ConstructL( TMsvId aServiceId );
        
        /**
        * Closes stream and file in case they are open
        */
        void Reset();


    public:     // Data
    
    protected:  // Data

    private:    // Data
        RFs& iFs;                      // file system handle
        CMsvServerEntry& iServerEntry; // message entry
        TMsvId iServiceId;             // MMS service entry id
        TMsvId iMessageEntry;          // Id of the current main message entry
        TInt   iMessageDrive;          // messages are on C: drive by default,
                                       // may be moved to other drive
        TBool  iDump;                  // dump flag
        TBool  iLogAllDecoded;         // detailed logging flag
        TBool  iDRMNeeded;             // Server entry keeps track of DRM handling
        RFile  iAttaFile;              // file handle for writing the attachment
        TBool  iFileOpen;              // keep track if we have an open file
        TMsvAttachmentId iAttaId;      // current attachment
        TBool  iRemoveDRM;             // keep track if attachment has to be removed
        
// DRM handlind using CAF. The DRM class is now a C-class
        CMmsDrmCaf* iDrmHandler;          

        

    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

    };

#endif      // MMSSERVERENTRY_H   
            
// End of File
