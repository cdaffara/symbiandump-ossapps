/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Imports a m3u playlist file
*
*/


#ifndef MPXM3UPLAYLISTIMPORTER_H
#define MPXM3UPLAYLISTIMPORTER_H

#include <mpxplaylistpluginobserver.h>
#include <f32file.h>
#include <charconv.h>


class CMPXMediaArray;

/**
*  CMPXM3uPlaylistImporter imports a m3u playlist file. Each instance can
*  only handle the import of one m3u file and the processing starts as
*  soon as the object is instantiated.
*
*  When processing is complete, CMPXM3uPlaylistImporter will invoke
*  callback through MMPXPlaylistPluginObserver interface. Once completed,
*  the importing service cannot be restarted. A new instance is required
*  if the client wishes to import another m3u playlist file.
*/
class CMPXM3uPlaylistImporter : public CActive
    {
public: // Constructors and destructor
    
    /**
    * Two-phased constructor
    *
    * @param aFs a file session handle
    * @param aObserver reference to the playlist plugin observer
    * @param aPlaylistUri URI of the playlist file to be internalized
    * @param aTopCharacterSet top character set for the current locale
    * @param aAvailableCharacterSet available character set in the system
    * @param aStatus caller's request status
    * @return object of constructed
    */
    IMPORT_C static CMPXM3uPlaylistImporter* NewL(
                RFs* aFs,
                MMPXPlaylistPluginObserver* aObserver,
                const TDesC& aPlaylistUri,
                const CArrayFix<CCnvCharacterSetConverter::SCharacterSet>& aTopCharacterSet,
                const CArrayFix<CCnvCharacterSetConverter::SCharacterSet>& aAvailableCharacterSet,
                TRequestStatus& aStatus );

    /**
    * Destructor
    */
    IMPORT_C virtual ~CMPXM3uPlaylistImporter();
    
protected:

    /**
    * Constructor
    *
    * @param aFs a file session handle
    * @param aObserver reference to the playlist plugin observer
    * @param aTopCharacterSet top character set for the current locale
    * @param aAvailableCharacterSet available character set in the system
    * @param aStatus caller's request status
    */
    IMPORT_C CMPXM3uPlaylistImporter(
                RFs* aFs,
                MMPXPlaylistPluginObserver* aObserver,
                const CArrayFix<CCnvCharacterSetConverter::SCharacterSet>& aTopCharacterSet,
                const CArrayFix<CCnvCharacterSetConverter::SCharacterSet>& aAvailableCharacterSet,
                TRequestStatus& aStatus );
    
    /**
    * 2nd phase constructor
    *
    * @param aPlaylistUri URI of the playlist file to be internalized
    */
    IMPORT_C virtual void ConstructL( const TDesC& aPlaylistUri );
   
    /**
    *  From CActive
    *  Handles an active object's request completion event
    */
    IMPORT_C virtual void RunL();

    /**
    * From CActive
    * Implements cancellation of an outstanding request.
    */
    IMPORT_C virtual void DoCancel();

    /**
    * Does a step of the task
    */
    IMPORT_C virtual void DoTaskStep();
    
    /**
    * Does a step of the task, leave if there is an error
    */
    IMPORT_C virtual void DoTaskStepL();

private:

    /**
    * Reads data from playlist file to the buffer
    */
    void ReadPlaylistFileToBufferL();
    
    /**
    * Auto detects the character encoding from the supplied character
    * set
    * @param aSample a sample of the file
    * @param aCharSetId auto-detected character set for the supplied
    *        sample
    * @param aCharacterSet a character set to detect the encoding from
    * @return KErrNone if a character set is found; otherwise
    *         KErrNotFound.
    */
    TInt DetectCharacterSetL(
        const TDesC8& aSample,
        const CArrayFix<CCnvCharacterSetConverter::SCharacterSet>& aCharacterSet,
        TUint& aCharSetId);
    
    /**
    * Parses the buffer where playlist file was read to.
    */
    void ParsePlaylistBufferL(
            CMPXMediaArray& aPlaylist,
            TInt& aInvalidItemCount);

    /**
    * Reads next line from iBuffer and stores it to iLine. Returns EFalse
    * if there are no more new lines.
    */
    TBool ReadNextLineL();

    /**
    * Decides what to do with an iLine read from iBuffer
    */
    void ProcessLineL(
            CMPXMediaArray& aPlaylist,
            TInt& aInvalidItemCount);

    /**
    * Parses path or extended info from an iLine and stores them to iItem
    */
    TInt ParseLineL(
            CMPXMedia* aItem,
            TInt& aInvalidItemCount);

    /**
    * Parses and returns an absolute path if aPath is relative to playlist
    * file's path. If path is not valid or it doesn't exist, error code is
    * returned in aError.
    *
    * caller assumes ownership of the returned HBufC
    */
    HBufC* ParseAbsolutePathLC(
            const TDesC& aPath,
            TInt& aError);

    /**
    * compose CMPXMedia to be sent back to the client
    */
    void ComposePlaylistL();
    
    /**
    * Reset data members for after completing the current request
    */
    void Cleanup();
    
    /**
    * Notify client of the specified error through MMPXPlaylistPluginObserver
    * interface
    */
    void NotifyClient(TInt aError);

protected:

    enum TMPXM3UImporterState
        {
        EMPXM3UReadBufferWithAutoDetectEncoding,
        EMPXM3UParseWithAutoDetectEncoding,
        EMPXM3UComposePlaylistMedia
        };
                         
protected:    // Data

    HBufC*                              iBuffer;
    TPtrC                               iBufferPtr;

    TPtrC                               iPlaylistFilePath;

    HBufC*                              iLine;

    RFs*                                iFs;      // not owned
    MMPXPlaylistPluginObserver*         iObserver;// not owned
    
    TBool                               iExtendedFormat;
    
    TRequestStatus*                     iCallerStatus;

    TInt                                iEndLineNumber;  
    TInt                                iCurrentLineNumber;
    TBool                               iMoreToDo;
    TBool                               iEndOfFile;
    CMPXMedia*                          iItem;

    CMPXMediaArray*                     iAutoEncodingPlaylistArray;
    TInt                                iAutoEncodingInvalidItems;
    
    CMPXMedia*                          iPlaylist;
    
    TMPXM3UImporterState                iState;
    
    const CArrayFix<CCnvCharacterSetConverter::SCharacterSet>&     iTopCharacterSet;
    const CArrayFix<CCnvCharacterSetConverter::SCharacterSet>&     iAvailableCharacterSet;
    };

#endif   // MPXM3UPLAYLISTIMPORTER_H
            

