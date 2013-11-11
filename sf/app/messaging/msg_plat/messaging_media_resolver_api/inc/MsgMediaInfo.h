/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*           Messaging Media Info class.
*           Containing detailed information about (media) files.
*
*/



#ifndef __MSGMEDIAINFO_H
#define __MSGMEDIAINFO_H

// INCLUDES

#include <e32std.h>
#include <e32base.h> // CBase
#include <f32file.h>
#include <apmstd.h>

#include <MsgMedia.hrh>

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS

class TDataType;
class CDRMHelper;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
*
*/
class MMediaInfoObserver
    {
    public:
        
        /**
        *
        */
        virtual void MediaInfoParsed() = 0;
    };

/**
* Base class for different media type info classes.
*
* @lib MsgMedia.lib
* @since 3.1
*/
NONSHARABLE_CLASS( CMsgMediaInfo ) : public CActive
    {
    public:  // New methods

        /**
        * Factory method that creates this object.
        *
        * @param    aFile       IN File handle to create info class from.
        * @param    aMimeType   IN Mime type of the file.
        * @param    aMediaType  IN Media type of the file.
        * @return   pointer to instance
        */
        IMPORT_C static CMsgMediaInfo* NewL(
            RFile& aFile, 
            TDataType& aMimeType,
            TMsgMediaType aMediaType );

        /**
        * Destructor
        */
        virtual ~CMsgMediaInfo();

        /**
        * Fills data structure with basic file information & media information.
        *
        * @param    aFile       IN File handle to create info class from.
        * @leave    Leaves with "KErrAbort" if object is deleted while
        *           ParseFileInformationL is not yet ready. (Might happen e.g. with
        *           video and audio files.)
        * @leave    Leaves if opening file fails. See RFile::Open().
        */
        IMPORT_C void ParseInfoDetails( RFile& aFile, CDRMHelper& aDRMHelper, MMediaInfoObserver& aObserver );

        /**
        * Deprecated.
        *
        * Use non-leaving ParseInfoDetails(...) instead.
        */
        IMPORT_C void ParseInfoDetailsL( RFile& aFile, CDRMHelper& aDRMHelper, MMediaInfoObserver& aObserver );

        /**
        * Updates file info for existing media info.
        *
        * Does not resolve the media information again! Only updates
        * basic file information (name, size, etc).
        *
        * @param    aFile       IN New file handle for existing media info.
        */
        IMPORT_C void SetFileL( RFile& aFile );

        /**
        * A convenience function for checking whether the media content was 
        * corrupt. This method should be called only after ParseInfoDetailsL.
        * Just maps certain error codes (iParseError) to corrupt.
        *
        * @return ETrue, if media content is corrupt,
        *         EFalse, if media content is not corrupt,
        *                 or media content is not parsed.
        */
        IMPORT_C TBool Corrupt() const;
        
        /**
        * Get full file path.
        *
        * @return   Full path with file name. Can be "null descriptor"!
        */    
        inline TPtrC FullFilePath() const;
        
        /**
        * Get file size.
        *
        * @return   Size of the file this ínformation was created from.
        */
        inline TInt FileSize() const;

        /**
        * Get access time of file.
        *
        * @param    aTime   OUT Modification time of the file this ínformation was created from.
        */
        inline void ModificationTime( TTime& aTime ) const;

        /**
        * Get mime type
        *
        * @return   Mime type of the file this ínformation was created of.
        */
        inline TPtrC8 MimeType() const;

        /**
        * Get media type
        *
        * @return   media type of the file this ínformation was created of.
        */
        inline TMsgMediaType MediaType() const;

        /**
        * Get protection flags.
        *
        * @return   Protection flags as defined in MmsMedia.h
        */
        inline TUint32 Protection() const;

        /**
        * Set protection flags.
        * Normally set by CMsgMediaResolver.
        *
        * @param    aProtection Protection flags as defined in FileProtectionResolver.h
        */
        inline void SetProtection( TUint32 aProtection );

        /**
        * Get DRM Content URI.
        *
        * @return   Pointer to DRM Content URI,
        *           NULL if not defined.
        */
        inline HBufC8* DRMContentURI() const;

        /**
        * Set DRM Content URI.
        * Normally set by CMmsMediaResolver.
        *
        * @param    aContentURI Pointer to DRM Content URI.
        *           Takes ownership.
        */
        inline void SetDRMContentURI( HBufC8* aContentURI );

        /**
        * Get duration.
        *
        * @return   Duration of an object in milliseconds.
        */
        inline TInt Duration() const;
    
        /**
        * Get handler application UID.
        *
        * @return   UID of the handler application,
        *           KNullUid if no handler found.
        */
        inline TUid HandlerApplicationUid() const;
    
        /**
        * Set handler application UID.
        * Normally set by CMmsMediaResolver.
        *
        * @param    aHandlerAppUid  UID of the handler application,
        *                           KNullUid if no handler found.
        */
        inline void SetHandlerApplicationUid( TUid aHandlerAppUid );

        /**
        * Get "Parsed" flag.
        *
        * @return   ETrue,  if all media info is successfully resolved,
        *           EFalse, otherwise (e.g. ParseFileInformationL is not
        *                   called or an error happened during 
        *                   ParseFileInformationL).
        */
        inline TBool Parsed() const;
        
        /**
        * Get parsing error code.
        *
        * @return   ETrue,  if all media info is successfully resolved,
        *           EFalse, otherwise (e.g. ParseFileInformationL is not
        *                   called or an error happened during 
        *                   ParseFileInformationL).
        */
        inline TInt ParseError() const;
        
    protected: // From base classes

        /**
        * From CActive
        */
    	void DoCancel();
        
        /**
        * From CActive
        */
    	void RunL();
	
        /**
        * From CActive
        */
    	TInt RunError( TInt aError );
	
    protected:
        
        /**
        * Constructor.
        *
        * @param    aMimeType   IN Mime type of the file.
        * @param    aMediaType  IN Media type of the file.
        */
        CMsgMediaInfo( TDataType& aMimeType, TMsgMediaType aMediaType );

        /**
        * 2nd phase constructor.
        *
        * @param    aFileName   IN File name to create info class from.
        */
        void ConstructL( RFile& aFile );

        /**
        * Complete self.
        */
        void CompleteSelf( TInt aError );

        /**
        * Complete observer.
        */
        void CompleteObserver();

        /**
        * Freezes DRM rights (with "Consume API") if needed.
        */
        void FreezeRights();
        
        /**
        * Releases DRM rights (with "Consume API") if frozen earlier.
        */
        void ReleaseRights();


    protected: // data
        // Required information for class
        MMediaInfoObserver* iObserver;
        CDRMHelper* iDRMHelper;
        RFile iFile;
        
        TDataType       iMimeType;
        HBufC*          iFullFilePath;
        TInt            iFileSize;
        TTime           iModificationTime;
        TMsgMediaType   iMediaType;
        TInt            iDuration;
        TUint32         iProtection;
        HBufC8*         iContentURI;
        
        TBool           iParsed;
        TBool           iConsumed;
        TInt            iParseError;
        TUid            iHandlerAppUid;
        };

#include <MsgMediaInfo.inl>

#endif // __MSGMEDIAINFO_H
