/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header of CMPXVideoDownloadMgrInterface class.
*
*/


// Version : %version: 12 %

#ifdef USE_S60_DOWNLOAD_MANAGER 

#ifndef __MPXVIDEODLMGRIF_H__
#define __MPXVIDEODLMGRIF_H__

#include <downloadmgrclientapiext.h>
#include <mpxplaybackcommanddefs.h>

//
//  Forward Declaration
//
class CMPXVideoPlaybackController;

/*
 *  CMPXVideoDlMgrIf
 *
 *  This class is the interface between the Media Player engine and
 *  the Download Manager
 *
 *  @lib mpengine.lib
 *  @since S60 3.2
 */
class CMPXVideoDlMgrIf : public CBase,
                         public MHttpDownloadMgrObserver
{
    public:

        /*
         *  NewL
         *
         *  Symbian 2 phase constructor
         *
         *  @since S60 3.2
         *  @param aEngine   reference to the Video Playback Controller
         *  @return reference to the class
         */
        static CMPXVideoDlMgrIf* NewL( CMPXVideoPlaybackController* aController );

        /*
         *  ~CMPDownloadMgrInterface
         *
         *  destructor
         *
         *  @since S60 3.2
         */
        virtual ~CMPXVideoDlMgrIf();

        /*
         *  ConnectToDownloadL
         *
         *  Connect to the Download Manager and attach to the download
         *
         *  @since S60 3.2
         *  @param aCmd        command with download information
         *  @return            error code
         */
        void ConnectToDownloadL( CMPXCommand& aCmd );

        /*
         *  ~CMPDownloadMgrInterface
         *
         *  MHttpDownloadMgrObserver Callback method
         *
         *  @since S60 3.2
         *  @param aDownload   reference to download
         *  @param aEvent      current state and event
         */
        void HandleDMgrEventL( RHttpDownload& aDownload,
                               THttpDownloadEvent aEvent );

        /*
         *  DoHandleOpenComplete
         *
         *  Performs the commands associated with the player being opened.
         *  It sets the current download id on the playback engine.
         *  Updates the current download size also.
         *
         *  @since S60 3.2
         */
        void DoHandleOpenComplete();

        /*
         *  GetPdlStatusL
         *
         *  Retrieves the status of the download.
         *
         *  @param   aCmd   Commands where the status will be written
         *
         *  @since 9.2 Timebox
         */
        void GetPdlStatusL( CMPXCommand& aCmd );

        /*
         *  HandleClose
         *
         *  Handles closing the Download Manager Interface
         *
         *  @since S60 3.2
         */
        void HandleClose();

        /*
         *  GetDownloadRatio
         *
         *  Retrieves the download ratio
         *
         *  @since S60 3.2
         */
        TReal GetDownloadRatio();

        /*
         *  IsDownloadPaused
         *
         *  Determines if the download is paused
         *
         *  @since S60 3.2
         */
        TBool IsDownloadPaused();

        /*
         *  UpdateDownloadSizeL
         *
         *  For DRM protected clips, the download size will be updated using the file handle
         *
         *  @since S60 9.2
         */
        void UpdateDownloadSizeL();

    private:
        /*
         *  CMPDownloadMgrInterface
         *  Constructor
         *  @since S60 3.2
         *
         */
        CMPXVideoDlMgrIf();

        /*
         *  ConstructL
         *  Symbian 2nd phase Constructor
         *  @since S60 3.2
         *
         */
        void ConstructL( CMPXVideoPlaybackController* aController );

        /*
         *  DoUpdateDownloadProgress
         *
         *  Updates the prog. download listener and playback engine with
         *  the current download size
         *
         *  @since S60 3.2
         */
        void DoUpdateDownloadProgress();

        /*
         *  HandleCustomCommand
         *
         *  Sends custom commands to the playback engine
         *
         *  @since S60 3.2
         *  @param   aCommand   custom command to send
         *  @param   aCmdValue  parameter for custom command
         *  @return  error code
         */
        TInt HandleCustomCommand( TUint32 aCommand, TUint32 aCmdValue );

        /*
         *  UpdateListenerWithState
         *
         *  Gets the current state of the download and updates the
         *  listener and member variables
         *
         *  @since S60 3.2
         *
         */
        void UpdateListenerWithState();

        /*
         *  CheckForMoveError
         *
         *  Gets the error status from the Download Manager after the
         *  move is complete
         *
         *  @since S60 3.2
         *
         */
        void CheckForMoveError();

        /*
         *  HandleDownloadComplete
         *
         *  Performs the logic for a completed download
         *
         *  @since S60 3.2
         *
         */
         void HandleDownloadComplete();

        /*
         *  UpdateDownloadProgress
         *
         *  Called by CIdle iUpdateDownload to update the download size
         *  @since 3.2
         *  @param aPtr Pointer to callback class
         *  @return KErrNone
         */
        static TInt UpdateDownloadProgress( TAny* aPtr );

        void CalculateDestinationFileNameL();

        void ConvertDMgrStatetoMpxState( TInt32 dmgrState );

        //
        //  Private Members
        //  Not Owned Data
        //
        RHttpDownloadMgrApiExt        iDlMgr;
        RHttpDownload*                iCurrentDl;
        CMPXVideoPlaybackController*  iController;

        HBufC*                        iClipName;
        TFileName                     iDestinationFileName;
        TInt                          iDlId;
        TInt32                        iDownloadProgress;
        TMPXPlaybackPdDownloadState   iDownloadState;
        TInt32                        iDlTotalSize;
        TBool                         iDlMgrConnected;
        TBool                         iMoveNeeded;
        TBool                         iMovePdlFile;
        TBool                         iPlayerOpened;
        TBool                         iCodDownload;

        //
        //  Private Members
        //  Owned Data
        //
        CIdle*                        iUpdateDownload;
};

#endif __MPXVIDEODLMGRIF_H__

#endif // USE_S60_DOWNLOAD_MANAGER 

// End of file
