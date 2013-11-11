/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Attachment handling helper functions functions
*
*/



#ifndef MMSATTACHMENTHANDLER_H
#define MMSATTACHMENTHANDLER_H

// INCLUDES
#include <e32def.h>
#include <cmsvattachment.h> // for TMsvAttachmentId

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMsvStore;
class RFs;
class RFile;
class TDesC8;
class TDriveUnit;
class CMsvMimeHeaders;

// CLASS DECLARATION

/**
* Attachment handling functions that can be called from both client and
* server side
*
* @lib mmsgenutil.lib
* @since 3.2
*/
NONSHARABLE_CLASS( CMmsAttachmentHandler ):public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aEntryWrapper MMS entry wrapper hiding difference between
        *     CMsvServerEntry and CMsvEntry.
        */
        IMPORT_C static CMmsAttachmentHandler* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CMmsAttachmentHandler();

    public: // New functions

        /**
        * Returns total size of all attachments including mime headers
        * and binary attachment data. Linked attachments are included
        * This function is needed to allocate the encoding buffer.
        * @param aStore Read store for the message entry
        * @return combined size of all attachments, including linked attachments
        */
        IMPORT_C static TInt AttachmentsSizeL( CMsvStore& aStore );
        
        /**
        * Check if aFileName would be a valid filename.
        * Cannot use path because absolute path is not known. Only filename is checked
        * @param aFs open file system handle.
        * @param aFileName suggestion of a filename.
        * @return ETrue if filename is valid, EFalse if not.
        */
        IMPORT_C static TBool IsValidFilename( RFs& aFs, const TPtrC& aFileName );

        /**
        * Create attachment entry and copy specified file to binary file storage.
        * @param aStore an open EditStore. Caller must commit store
        *     (several attachments can be added before committing store)
        * @param aFile opened file handle, source of the attachment.
        *     File must be in public location to be accessible.
        *     Caller must close the file afterwards.
        * @param aFs File session handle. Needed to check disk space
        * @param aMessageDrive the drive the message store is on.
        *     (needed for disk space check)
        * @param aMimeType mime type of the attachmet.
        * @param aMimeHeaders mime headers for the attachment. If content type is not
        *     defined, function adds the mime type and subtype from aMimeType
        *     Suggested filename in mimeHeaders is used as attachment name
        * @param aAttachmentInfo must be initialized to a CMsvAttachment::EMsvFile
        *     on return contains data about the attachment.
        *     Ownership of attachmentinfo is transferred to attachment manager,
        *     it must not be deleted by caller.
        *     Attachment manager will be destroyed when store is deleted.
        *     After that aAttachmentInfo will no longer be available.
        * @param aAttaId attachment id of the newly created attachment
        */
        IMPORT_C static void CreateAttachmentL(
            CMsvStore& aStore,
            RFile& aFile,
            RFs& aFs,
            TDriveUnit aMessageDrive,
            TDesC8& aMimeType,
            CMsvMimeHeaders& aMimeHeaders,
            CMsvAttachment* aAttachmentInfo,
            TMsvAttachmentId& aAttaId);
            
        /**
        * Create text/plain attachment. <br>
        * Creates a text attachment from descriptor.
        * Has option to convert all unicode paragraph separator marks to line feeds.
        * Converts text from unicode (ucs-2) to utf-8 before storing it.
        * @param aStore an open EditStore. Caller must commit store
        *     (several attachments can be added before committing store)
        * @param aAttachmentId returned ID of the new attachment entry
        * @param aText UNICODE text to be added as a text/plain attachment.
        * @param aFile suggested filename for the attachment
        * @param aFs File session handle. Needed to check disk space
        * @param aMessageDrive the drive the message store is on.
        *     (needed for disk space check)
        * @param aConvertParagraphSeparator flag to tell if the function 
        *     will search for all 0x2029 characters (Unicode paragraph
        *     separator) and replace them with 0x000a (line feed).
        *     aConvertParagraphSeparator == ETrue: convert
        *     aConvertParagraphSeparator == EFalse: do not convert
        */
        IMPORT_C static void CreateTextAttachmentL(
            CMsvStore& aStore,
            TMsvAttachmentId& aAttachmentId,
            const TDesC& aText,
            const TDesC& aFile,
            RFs& aFs,
            TDriveUnit aMessageDrive,
            TBool aConvertParagraphSeparator = ETrue );
            
        /**
        * Create text/plain attachment. <br>
        * Creates a text attachment from descriptor.
        * Has option to convert all unicode paragraph separator marks to line feeds.
        * Converts text from unicode (ucs-2) to utf-8 before storing it.
        * @param aStore an open EditStore. Caller must commit store
        *     (several attachments can be added before committing store)
        * @param aAttachmentId returned ID of the new attachment entry
        * @param aFile open file read handle to unicode plain text file
        * @param aFs File session handle. Needed to check disk space
        * @param aMessageDrive the drive the message store is on.
        *     (needed for disk space check)
        */
        IMPORT_C static void CreateUTF8TextAttachmentFromFileL(
            CMsvStore& aStore,
            TMsvAttachmentId& aAttachmentId,
            RFile& aFile,
            RFs& aFs,
            TDriveUnit aMessageDrive
            );
            
        
    public: // Functions from base classes

    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

        /**
        * C++ constructor.
        * @param aEntryWrapper MMS entry wrapper hiding difference between
        *     CMsvServerEntry and CMsvEntry
        */
        CMmsAttachmentHandler( );

        /**
        * Constructor.
        */
        void ConstructL();
        
        /**
        * Remove an attachment by attaId.
        * Symbian synchronous attachment manager can only remove by index.
        * 
        * @param aAttaId id of the attachment to be removed
        * @param aStore reference to open edit store
        */
        static void RemoveAttachmentL( TMsvAttachmentId aAttaId, CMsvStore& aStore );
        
        
    };
    
/**
* Reads and writes single lines of text to or from a file.
*
* @lib mmsgenutil.lib
* @since 3.2
*/

NONSHARABLE_CLASS ( TMmsFileText )
	{
public:
	enum TFileState
		{
		EStartOfFile,
		ENormal,
		EReverse
		};
public:
	TMmsFileText();
	void Set(RFile& aFile);
	TInt Read(TDes& aDes);
private:
	TInt CheckForTerminator(TBool& anAnswer);
	TInt FillBuffer();
private:
	const TText* iNext;
	const TText* iEnd;
	TFileState iState;
	RFile iFile;
	TBuf8<0x100> iReadBuf; 
	};
	
#endif      // MMSATTACHMENTHANDLER_H   
            
// End of File
