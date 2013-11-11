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
* Description:  Session command interface
*
*/

#ifndef CSESSIOHANDLER_H
#define CSESSIOHANDLER_H
NONSHARABLE_CLASS( TSessionHandler )
    {
    public:
        /**
         * Constructor.
         */
        TSessionHandler(MVtProtocolHandler* aProtocolHandler);
    private:
        MVtProtocolHandler* iProtocolHandler;
    };

NONSHARABLE_CLASS( CSessionCommand ): public CBase,
                                      public MVtSessionCommand
    {
    public:
        /**
         * Contructor.
         */
        CSessionCommand(TSessionHandler* aSessionHandler);
        
        /**
         * Destructor.
         */
        ~CSessionCommand();

        /**
         * This function is valid only in the EIdle state.  It is a no-op when
         * invoked in any other state.  It causes the protocol to transition
         * to the ESetup state.  The terminal remains in the EInitializing state during
         * the transition.
         *
         *
         * @param aInitInfo
         *         A reference to a TVtInitInfo structure which set Mona on and off
         *         
         * @leave   This method can leave with one of the following error codes
         *          KErrNoMemory if the SDK failed to allocate memory during this operation
         * @returns A unique command id for asynchronous completion
         **/
        virtual TInt InitProtocolL(TVtInitInfo& aInitInfo);

        /**
         * For an incoming track (MVTVideoSink) this function  pauses sending
         * media to the sink (output device) and stops the sink.  It then does the protocol
         * negotiations with the remote terminal to pause the logical channel for
         * the specified track.
         *
         * For outgoing, it pauses the sending of media from the source and calls Stop() on the
         * source.  It also performs any necessary protocol negotiations with the remote terminal.
         * EVtCommandPause will be sent to the observer when the processing completes.
         *
         * @returns A unique command id for asynchronous completion
         **/
        virtual TInt PauseVideoL(MVTVideoSource& aDataSource);
        virtual TInt PauseVideoL(MVTVideoSink& aDataSink);
        virtual TInt PauseAudioL(MVTAudioSource& aAudioSource);

        /**
         * Resume a previously paused incoming or outgoing track.  For incoming,
         * this function starts resumes playing out the media to the appropriate
         * sink based on the current settings.  For outgoing it resumes encoding
         * and sending media from the source.
         *
         * EVtCommandResume will be invoked will be invoked on the observer when the processing completes.   
         *
         * @returns A unique command id for asynchronous completion
         **/
        virtual TInt ResumeVideoL(MVTVideoSource& aDataSource);
        virtual TInt ResumeVideoL(MVTVideoSink& aDataSink);
        virtual TInt ResumeAudioL(MVTAudioSource& aAudioSource);

        /**
         * This function is valid only in the ESetup and EInitializing state.  It is a
         * no-op when invoked in the EIdle state
         *
         * It causes the protocol to transition back to the EIdle state.  The
         * terminal remains in the EResetting state during the transition.
         *
         * While resetting, the protocol de-allocates all resources resources that
         * had been previously allocated.  When it completes, ResetComplete is called
         * and the protocol reverts to the EIdle state.
         *
         * @leave   This method can leave with one of the following error codes
         *          KErrNoMemory if the SDK failed to allocate memory during this operation
         * @returns A unique command id for asynchronous completion
         **/
        virtual TInt ResetProtocolL();

        /**
         * This function can be invoked only in the ESetup state.  The terminal starts connecting with the remote
         * terminal based on the specified options and capabilities.
         * The EVtCommandConnect command completion event will be passed to the observer
         * when connect completes.
         * Details about the negotiated session may be obtained by calling the GetSessionParamsL API.
         * GetSessionParamsL may be called after call setup is started to get the list of available channels
         * and their capabilities.
         * Incoming tracks may be opened before ConnectL completes and will be indicated via the
         * EVtIndicationIncommingTrack event.
         *
         * @param aComm
         *         An optional pointer to a comm server to provide comm source and sink end-points.
         * @returns A unique command id for asynchronous completion
         **/
        virtual TInt ConnectToProtocolL(MCommServer* aComm);

        /**
         * Allows an application to provide a media source to be associated with a logical channel
         * of communication with the peer.  Sources should be added after the EVtIndicationOutgoingTrack
         * indication is received for a newly established logical channel.  The media type and
         * channel id associated with the logical channel are specified as part of the indication.
         * This function accepts a MVtVideoSource which provides additional functionality
         * for advertizing capability and exposing configuration APIs.  
         * Data sources could be of the following types:
         * a)raw media sources like camera, microphone etc.
         * b)sources of compressed data like file, gateway component etc.
         *
         * @param aChannelId
         *          Indicates the channel id to be associated with this source.
         * @param aDataSource
         *          reference to the data source
         * @leave   This method can leave with one of the following error codes
         *          KErrNotSupported if the format of the sources/sinks is incomtible with what the SDK can handle
         *          KPVErrInvalidState if invoked in the incorrect state
         *          KErrNoMemory if the SDK failed to allocate memory during this operation
         * @return A unique command id for asynchronous completion
         */
        virtual TInt AddVideoSourceL(const TUint aChannelId, MVTVideoSource &aDataSource);
        virtual TInt AddAudioSourceL(const TUint aChannelId, MVTAudioSource &aDataSource);

        /**
         * Allows an application to provide a media sink for rendering an incoming media bitstream in a
         * logical channel of communication with the peer.
         * AddDataSinkL can be called only for established incoming logical channels identified by a unique
         * channel id.
         * Regular incoming channels are established by the peer and are
         * indicated using the EVtIndicationIncomingTrack indication.
         * This function takes in PV extension to MVtVideoSink or MVtAudioSink which provides additional functionality
         * for advertizing capability and exposing configuration APIs.  
         * EVtCommandAddDataSink event is sent to the observer on completion of this call.
         *
         * @param aChannelId
         *          Indicates the channel id to be associated with this sink.
         * @param aDataSink The data sink to be added
         *
         * @return A unique command id for asynchronous completion
         **/
        virtual TInt AddVideoSinkL(const TUint aChannelId, MVTVideoSink &aDataSink);
        virtual TInt AddAudioSinkL(const TUint aChannelId, MVTAudioSink &aDataSink);

        /**
         * This API is to allow the user to cancel all pending requests.  The current request being
         * processed, if any, will also be aborted.
         * EVtCommandCancelAllCommands will be passed to the command observer on completion.
         * @returns A unique command id for asynchronous completion
         **/
        virtual TInt CancelAllCommandsL( );

        /**
         * The Disconnect call is valid only when invoked in the EConnecting, and
         * EConnected states.  It causes the terminal to transition to the
         * EDisconnecting state.  All the media tracks both incoming and outgoing
         * will be closed on invoking Disconnect. On completion, the terminal
         * goes to the ESetup state.
         *
         * It is a no-op when called in any other state.
         *
         * This is an asynchronous request.  The EvtCommandDisconnect event will be
         * sent to the observer when the request processing is complete.  This
         * is the only event the Phone application should expect after calling
         * Disconnect.
         *
         * @returns A unique command id for asynchronous completion
         **/
        virtual TInt DisconnectFromProtocolL();

        /**
         * This API is to allow for extensibility of the protocol interface.
         * It allows a caller to ask for an instance of a particular interface object to be returned.
         * The mechanism is analogous to the COM IUnknown method.  The interfaces are identified with
         * an interface ID that is a UUID as in DCE and a pointer to the interface object is
         * returned if it is supported.  Otherwise the returned pointer is NULL.
         * @param aType
         * @param aProtocolCommand
         * @exception not_supported
         * leaves if the specified interface id is not supported.
         **/
        virtual TInt GetProtocolInterfaceL(TVtConfigType aType, MVtProtocolCommand*& aProtocolCommand);

        /**
         * This APIis to be used to release an interface that was previously obtained using
         * QueryInterfaceL.
         * @param aType
         * @param 
         * @exception not_supported
         * leaves if the specified interface id is not supported.
         **/
        virtual TInt DeleteProtocolInterfaceL(TVtConfigType aType, MVtProtocolCommand* aProtocolCommand); 
    private:
        CVideoSourceProxy* iVideoSource;
        CDisplaySinkProxy* iDisplaySink;
        TSessionHandler*   iSessionHandler;
    };
#endif
//  End of File 
