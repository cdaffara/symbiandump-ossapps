/*
* Copyright (c) 2003-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Entry wrapper class mixin interface definition
*
*/


#ifndef MMSENTRYWRAPPER_H
#define MMSENTRYWRAPPER_H

//  INCLUDES
#include    <e32base.h>
#include    <msvids.h>
#include    <cmsvattachment.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMmsHeaders;
class CMsvMimeHeaders;
class MMmsEntry;

// CLASS DECLARATION

/**
*  Mixin class for entry wrapper that CMmsEncode and CMmsDecode need
*/
class MMmsEntryWrapper
    {
    public:  // Virtual functions
    
        /**
        * Provide an edit store for editing messages
        * Ownership of the store is transferred. Caller must delete the store
        * @return CMsvStore* if successful, leaves if unsuccessful
        */
        virtual CMsvStore* EditStoreL() = 0;

        /**
        * provide a read store for editing messages
        * Ownership of the store is transferred. Caller must delete the store
        * @return CMsvStore* if successful, leaves if unsuccessful
        */
        virtual CMsvStore* ReadStoreL() = 0;

        /**
        * reads index entry contents from current entry
        * @param TMsvEntry reference to entry to be filled with data
        * @return error code, KErrNone if successful
        */
        virtual TInt GetIndexEntry( TMsvEntry& aTMsvEntry ) = 0;
       
        /**
        * stores index entry contents to current entry
        * @param TMsvEntry reference to entry to be written to disk
        * @return error code KErrNone, if successful
        */
        virtual TInt ChangeIndexEntry( TMsvEntry& aTMsvEntry ) = 0;

        /**
        * Sets context to specified message entry
        * @param aId ID of the new message entry.
        * @return error code, KErrNone if successful
        */
        virtual TInt SetCurrentEntry( TMsvId aEntryId ) = 0;

        /**
        * returns total size of all attachments including mime headers
        * and binary attachment data. Linked attachments are included
        * This function is needed to allocate the encoding buffer.
        * @param aStore Read store for the message entry
        * @return combined size of all attachments, including linked attachments
        */
        virtual TInt AttachmentsSizeL( CMsvStore& aStore ) = 0;

        /**
        * Check if disk space is below critical level when storing data.
        * @param aDataSize amount of data to be added. If aDataSize == 0,
        *     returns info if disk space is already below critical level.
        * @return error code KErrDiskFull if data does not fit,
        *     KErrNone if data fits.
        *     May return some other error code if appropriate.
        *     KErrNone always means it is all right to save the data.
        */
        virtual TInt DiskSpaceBelowCriticalLevelL( TInt aDataSize ) = 0;

        /**
        * Store CMmsHeaders to message store for main message entry.
        * @param aMmsHeaders headers to store.
        * @param aStore store to be used. If not given, function gets and commits store
        * @return error code if not successful, KErrNone if successful.
        */
        virtual TInt StoreMmsHeadersL( CMmsHeaders& aMmsHeaders, CMsvStore* aStore ) = 0;

        /**
        * Restore CMmsHeaders from message store for main message entry.
        * @param aMmsHeaders headers to restore.
        * @param aStore store to be used. If not given, function gets and closes store
        * Leaves if cannot restore headers.
        */
        virtual void RestoreMmsHeadersL( CMmsHeaders& aMmsHeaders, CMsvStore* aStore  ) = 0;
        
        /**
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
        virtual TInt CreateFileAttachmentL(
            CMsvStore& aStore,
            const TDesC& aFileName,
            const TPtrC8& aAttachmentData,
            CMsvMimeHeaders& aMimeHeaders,
            TMsvAttachmentId& aAttachment,
            TInt& aAttachmentSize,
            TInt32& aDRMFlags) = 0;

        /**
        * Deletes a message store entry.
        * @param aEntryIndex is the index of the entry, which will be deleted. 
        * If the Entry has children, all children are destroyed too.
        * @return error code if not successful, KErrNone if successful.
        */
        virtual TInt DeleteEntry( TMsvId aEntryIndex ) = 0;

        /**
        * Check if aFileName would form a valid filename.
        * Cannot use path because absolute path is not known. Only filename is checked
        * @param aFileName suggestion of a filename.
        * @return ETrue if filename is valid, EFalse if not.
        */
        virtual TBool IsValidFilename(
            const TPtrC& aFileName ) = 0;

        /**
        * Read the decoding flag from the service entry.
        *     (Info stored in MMS service entry)
        * @return ETrue = Log add details while decoding
        *     EFalse = Don't log details.
        */
        virtual TBool GetDecodingFlag() = 0;

        /**
        * Read the dump flag from the service entry.
        *     The dump flag tells if all incoming messages are dumped to a file.
        * @return ETrue = dump binary message to file while decoding
        *     EFalse = No binary dump wanted
        */
        virtual TBool GetDumpFlag() = 0;
        
        /**
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
        */
        virtual void CreateEmptyFileAttachmentL(
            CMsvStore& aStore,
            const TDesC& aFileName,
            CMsvMimeHeaders& aMimeHeaders,
            TMsvAttachmentId& aAttachment) = 0;
            
        /**
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
        virtual TInt ProcessAttachmentDataL(
            const TPtrC8& aAttachmentData,
            TInt32& aDRMFlags ) = 0;
        
        /**
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
        virtual TInt FinalizeAttachmentL(
            CMsvStore& aStore,
            TInt& aAttachmentSize,
            TInt32& aDRMFlags ) = 0;


    };

#endif      // MMSENTRYWRAPPER_H
            
// End of File
