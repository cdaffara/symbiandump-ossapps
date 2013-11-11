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
* Description:  Exports a m3u playlist file
*
*/


#ifndef MPXM3UPLAYLISTEXPORTER_H
#define MPXM3UPLAYLISTEXPORTER_H

#include <f32file.h>
#include <mpxplaylistpluginobserver.h>

/**
*  CMPXM3uPlaylistExporter exports a m3u playlist file. Each instance can
*  only handle the export of one m3u file and the processing starts as
*  soon as the object is instantiated.
*
*  When processing is complete, CMPXM3uPlaylistExporter will invoke
*  callback through MMPXPlaylistPluginObserver interface. Once completed,
*  the exporting service cannot be restarted. A new instance is required
*  if the client wishes to export another m3u playlist file.
*/
class CMPXM3uPlaylistExporter : public CActive
    {
public: // Constructors and destructor
    
    /**
    * Two-phased constructor
    *
    * @param aFs a file session handle
    * @param aObserver reference to the playlist plugin observer
    * @param aPlaylist playlist to be externalized
    * @param aFilePath File path for where the playlist should be placed
    * @param aStatus caller's request status
    * @return object of constructed
    */
    IMPORT_C static CMPXM3uPlaylistExporter* NewL(
        RFs* aFs,
        MMPXPlaylistPluginObserver* aObserver,
        const CMPXMedia& aPlaylist,
        const TDesC& aFilePath,
        TRequestStatus& aStatus );

    /**
    * Destructor
    */
    IMPORT_C virtual ~CMPXM3uPlaylistExporter();

    
protected:

    /**
    * Constructor
    *
    * @param aFs a file session handle
    * @param aObserver reference to the playlist plugin observer
    * @param aPlaylist playlist to be externalized
    * @param aStatus caller's request status
    * @return object of constructed    
    */
    IMPORT_C CMPXM3uPlaylistExporter(
        RFs* aFs,
        MMPXPlaylistPluginObserver* aObserver,
        const CMPXMedia& aPlaylist,
        TRequestStatus& aStatus );
    
    /**
    * 2nd phase constructor
    *
    * @param aFilePath File path for where the playlist should be placed
    */
    IMPORT_C virtual void ConstructL(
        const TDesC& aFilePath);    
   
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
    
private:

    /**
    * externalize a set number of the medias. This will be called until all
    * the given medias have been processed
    */
    void ExternalizeL();
    
    /**
    * Populates iLines with HBufC8's each representing one line of playlist
    * file.
    */
    void PopulateLineArrayL();

    /**
    * Converts iLine to 8-bit ASCII and places it to iLines array.
    */
    void AddLineToArrayL();

    /**
    * Creates a header string, which tells that the playlist is in the
    * extented m3u format, and stores it to iLine.
    */
    void CreateHeaderLineL();

    /**
    * If media has title defined, creates extented info tag containing
    * audio clip length and title. Info tag is stored to iLine.
    */
    void CreateExtentedInfoLineL(
            const CMPXMedia& aMedia);

    /**
    * Creates a string containing iItem's relative or absolute path (path
    * is relative if the audio clip is stored to the same directory, or 
    * it's sub-directories, as the playlist file). The stirng is stored to
    * iLine.
    */
    void CreatePathLineL(
            const CMPXMedia& aMedia,
            const TDesC& aCurrentFolder,
            TBool aAddNewLine);

    /**
    * Caluculates the length of the final playlist file, creates new buffer
    * with that length, and appends all lines from iLines to it and frees
    * iLines
    */
    void FlushLineArrayToBufferL();

    /**
    * Writes playlist data from buffer to a file.
    */
    void WritePlaylistToFileL();

    /**
    * Reset data members after processing the current request.
    */
    void Cleanup();

    /**
    * Notify client of the specified error through MMPXPlaylistPluginObserver
    * interface
    */
    void NotifyClient(TInt aError);
         
protected:    // Data

    const CMPXMedia&                    iPlaylist;
    CMPXMediaArray*                     iMedias;
    HBufC*                              iPlaylistFilePath;
    TInt                                iDriveNumber;
    TBool                               iPlaylistFileCreated;

    HBufC8*                             iPlaylistBuf;
    RPointerArray<HBufC8>               iLines;
    HBufC*                              iLine;

    RFs*                                iFs;       // not owned
    MMPXPlaylistPluginObserver*         iObserver; // not owned
    
    TInt                                iCurrentMedia;
    TBool                               iMoreToDo;

    TRequestStatus*                     iCallerStatus;    
    };

#endif   // MPXM3UPLAYLISTIMPORTER_H
            

