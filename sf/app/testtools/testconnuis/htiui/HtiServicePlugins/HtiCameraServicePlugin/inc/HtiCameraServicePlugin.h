/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines the ECom plugin for HTI camera service.
*
*/



#ifndef HTICAMERASERVICEPLUGIN_H
#define HTICAMERASERVICEPLUGIN_H

//  INCLUDES
#include <HtiServicePluginInterface.h>
#include "EngineVideoRecording.h"
// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CEngineVideoRecording;
class MEngineVideoRecordingObserver;

// CLASS DECLARATION

/**
*  The ECom plugin for HTI camera service.
*
*/
class CHtiCameraServicePlugin : public CHTIServicePluginInterface,
                                public MEngineVideoRecordingObserver
    {

    protected:
        // commands
        enum TCommands
            {
            ECmdInitialize              = 0x01,
            ECmdPrepareVideoRecording   = 0x02,
            ECmdStartVideoRecording     = 0x05,
            ECmdPausingVideoRecording   = 0x06,
            ECmdResumeVideoRecording    = 0x07,
            ECmdStopVideoRecording      = 0x08,
            ECmdCloseVideoRecording     = 0x09,
            ECmdGetZoom                 = 0x0a,
            ECmdSetZoom                 = 0x0b,
            
            ECmdUninitialize            = 0x51,
            EResultOk                   = 0xFF // only for response message
            };


    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CHtiCameraServicePlugin* NewL();

    public: // New functions

    public:
    // Functions from base classes

    /**
     * From CHTIServicePluginInterface
     * Called by the HTI Framework when sending message to this service.
     * @param aMessage message body destinated to a servive
     * @param aPriority message priority
     */
    void ProcessMessageL(const TDesC8& aMessage,
            THtiMessagePriority aPriority);

    /**
     * From CHTIServicePluginInterface
     * Indicates to HTI Framework whether the plugin is ready to process
     * a new message or if it's busy processing previous message.
     */
    TBool IsBusy();

 
    protected:  // New functions

        /**
        * C++ default constructor.
        */
        CHtiCameraServicePlugin();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Destructor.
         */
        virtual ~CHtiCameraServicePlugin();
    
    protected:
        // Functions from base classes
        
        /**
        * From MEngineVideoRecordingObserver
        * Called asynchronously when CEngineVideoRecording::Init(), CCaeEngine::CEngineVideoRecording(), 
        * or CEngineVideoRecording::Reserve() completes.
        * Indicates if Video Recording Engine is ready for operation, 
        * the camera is reserved and its power is switched on.
        * @param aError Error code KErrNone to indicate success or a standard Symbian OS error code.
        * @return void
        */
        virtual void MevroInitComplete( TInt aError );
        
        /**
         * From MEngineVideoRecordingObserver
         * Called asynchronously when preparing of video recording completes 
         * after PrepareVideoRecordingL() has been called.
         * May be called second time with an error code after a successful preparation 
         * if video recording loses its prepared state for some reason (e.g. audio HW 
         * is reserved for some other application).
         * @param aError Error code KErrNone to indicate success or a standard Symbian OS error code.
         * @return void
         */
        virtual void MevroVideoPrepareComplete(TInt aError);
    
        /**
         * From MEngineVideoRecordingObserver
         * Called (possibly asynchronously) when video recording is running 
         * after CEngineVideoRecording::StartVideoRecording() or 
         * CEngineVideoRecording::ResumeVideoRecording() has been called.
         * @param aError Error code KErrNone to indicate success or a standard Symbian OS error code.
         * @return void
         */
        virtual void MevroVideoRecordingOn(TInt aError);
    
        /**
         * From MEngineVideoRecordingObserver
         * Called (possibly asynchronously) when video recording is paused after 
         * CEngineVideoRecording::PauseVideoRecording() has been called.
         * @param aError Error code KErrNone to indicate success or a standard Symbian OS error code.
         * @return void
         */
        virtual void MevroVideoRecordingPaused(TInt aError);
    
        /**
         * From MEngineVideoRecordingObserver
         * Called (possibly asynchronously) when video recording is completed 
         * after CEngineVideoRecording::StopVideoRecording() has been called or 
         * recording has been completed for some other reason.
         * @param aError Error code KErrNone to indicate success or a standard Symbian OS error code.
         * if aError == KErrDiskFull, then disk storage is full.
         * if aError == KErrCompletion, then clip max size was reached.
         * @return void
         */
        virtual void MevroVideoRecordingComplete(TInt aError);
    
    private:
        void HandleInitializeCmdL(const TDesC8& aData);
        void HandleUninitializeCmdL(const TDesC8& aData);
        void HandlePrepareVideoRecordingCmdL(const TDesC8& aData);
        void HandleStartVideoRecordingCmdL( const TDesC8& aData );
        void HandlePausingVideoRecordingCmdL( const TDesC8& aData );
        void HandleResumeVideoRecordingCmdL( const TDesC8& aData );
        void HandleStopVideoRecordingCmdL( const TDesC8& aData );
        void HandleCloseVideoRecordingCmdL( const TDesC8& aData );
        void HandleGetZoomCmdL( const TDesC8& aData );
        void HandleSetZoomCmdL( const TDesC8& aData );
        
        void SendOkMsgL( const TDesC8& aData );
        void SendErrorMessageL( TInt aError, const TDesC8& aDescription );
        
        TInt ParseString( const TDesC8& aRequest,
                                       TInt aOffset, TDes& aResult );

    public:     // Data

    protected:  // Data

    private:    // Data
        // Flag telling if the service is busy processing a message
        TBool                   iIsBusy;
        
        TInt                    iError;
        CEngineVideoRecording*  iVideoRecordingEngine;
        CActiveSchedulerWait*   iWaiter;

    };

#endif      // HTICAMERASERVICEPLUGIN_H

// End of File
