/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef MVTENGMEDIAHANDLER_H
#define MVTENGMEDIAHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <mmfdatabuffer.h>
#include <mmfutilities.h>
#include <mmfcontrollerframework.h>

typedef TInt TVtCommandId;                   
typedef TInt TVtCommandStatus;               
typedef TDesC8 TVtMIMEType;

const TUid KVTUidYUVFrameBuffer = {0xFFFFFF0d};
const TInt KVtUidDataSrcPrime	= 0xFFFFFF08;
const TInt KVtUidDataSrcPlay	= 0xFFFFFF09;
const TInt KVtUidDataSrcPause	= 0xFFFFFF0A;
const TInt KVtUidDataSrcStop	= 0xFFFFFF0B;

_LIT8(KVtVideoMIMETypeH263, "/video/x-pv/compressed/h263");
_LIT8(KVtVideoMIMETypeM4V, "/video/x-pv/compressed/m4v");
_LIT8(KVtVideoMIMETypeH264, "/video/x-pv/compressed/h264");
_LIT8(KVtAudioMIMETypeAMRIF2, "/audio/x-pv/compressed/amr/if2");
_LIT8(KVtAudioMIMETypeAMRWBIF2, "/audio/x-pv/compressed/amrwb/if2");
_LIT8(KVtVideoMIMETypeYUV420, "/video/x-pv/raw/yuv420");

enum TVt3G324MSupported
    {
    EVt3G324MMissing,
    EVt3G324MSupported
    }; 
    
enum TVtConfigType
    {
    EVtH324Config,
    EVtVideoEncoderConfig
    };

enum TVtCommandType {
    EVtCommandInit,
    EVtCommandGetSDKInfo,
    EVtCommandGetProtocolState = 3,
    EVtCommandReset,
    EVtCommandAddDataSource = 6,
    EVtCommandRemoveDataSource,
    EVtCommandAddDataSink,
    EVtCommandRemoveDataSink,
    EVtCommandConnect,
    EVtCommandDisconnect,
    EVtCommandPause,
    EVtCommandResume,
    EVtCommandGetProtocolInterface =20,
    EVtCommandDeleteProtocolInterface,
    EVtCommandCancelAllCommands
    };

enum TVtIndicationType 
    {
    EVtIndicationVideoSpatialTamporalTradeOffCommand,
    EVtIndicationVideoSpatialTamporalTradeOffIndication,
    EVtIndicationUserInputCapability = 6,
    EVtIndicationIncomingTrack = 41,
    EVtIndicationOutgoingTrack,
    EVtIndicationDisconnect,
    EVtIndicationClosingTrack,
    EVtIndicationCloseTrack,
    EVtIndicationPauseTrack,
    EVtIndicationResumeTrack    
    };
      
enum TVtMediaType      
    {
    EVtAudio = 1,
    EVtVideo,
    EVtControl,
    EVtData,
    EVtUserInput,
    EVtMediaNone
    };
    
enum TVtAudioOutputControlCommand   
    {
	  EVtAudioOutputControlGetMaxVolume,
	  EVtAudioOutputControlGetVolume,
	  EVtAudioOutputControlSetVolume,
	  EVtAudioOutputControlGetBalance,
	  EVtAudioOutputControlSetBalance
	  };    

const TUint8 EVtIncoming = 1;

enum TUserInputType
    {
    EVtUiiDTMF,
    EVtUiiAlphaNumeric
    };

struct TVtInitInfo
    {
    TBool iAMRWBOn;
    TBool iDisableMpeg4;
    };

struct TVtIndicationEvent
    {
    public:    
        /**
         * Constructor.
         */
        TVtIndicationEvent(TInt aEventType, const TUint8 *aLocalBuffer) : iEventType(aEventType), iLocalBuffer(aLocalBuffer) 
        {}
    public:
        TInt iEventType;
        const TUint8 *iLocalBuffer;
    };

struct TVtCommandResponse
    {
    public:        
        /**
         * Constructor.
         */
        TVtCommandResponse(TVtCommandType aCmdType, TInt aCmdId, TInt iCmdStatus) : iCmdType(aCmdType), iCmdId(aCmdId), iCmdStatus(iCmdStatus)
        {}
    public:
        TVtCommandType iCmdType;
        TInt iCmdId;
        TInt iCmdStatus;
    };

struct TVtErrorEvent
    {
    public:
        /**
         * Constructor.
         */
        TVtErrorEvent(TInt aEventType) : iEventType(aEventType) {}
    public:
        TInt iEventType;
    };
    
struct TVtMMFDataBuffer
    {
    public:
        /**
         * Constructor.
         */
        TVtMMFDataBuffer(CMMFBuffer* aMmfBuffer,TSize  aFrameSize, TPtr8& aPtr) : iMmfBuffer(aMmfBuffer), iFrameSize(aFrameSize), iPtr(aPtr) {}

        /**
         * Get the YUV frame size.
         * @return The frame size, in pixels
         */
        TSize GetFrameSize() {return iFrameSize;}

        /**
         * Get MMF buffer.
         * @return the MMF buffer
         */
        CMMFBuffer* GetMMFBuffer() {return iMmfBuffer;}
        
        /**
         * Get MMF buffer.
         * @return the MMF buffer
         */
        const CMMFBuffer* GetMMFBuffer() const {return iMmfBuffer;}

        /**
        *  @return Returns a reference to the data buffer
        **/
        TPtr8& Data() {return iPtr;}

        /**
        *  @return Returns the frame size of the contained buffer.
        **/
        const TSize GetFrameSize() const {return iFrameSize;}
        
        /**
        *  @return Returns a reference to the data buffer
        **/
        const TPtr8& Data() const {return iPtr;}
    private:
        CMMFBuffer* iMmfBuffer;
        TSize  iFrameSize;
        TPtr8&  iPtr;
    };

class MVTVideoInput
    {
    public:

        /**
         * Set the video frame format.  This must be from the list of supported formats.
         * @param "aFormat" The mime string describing the video frame format.
         * @exception Can leave with one of the system wide error codes
         */
        virtual void SetFormatL(const TDesC8& aFormat) {}

        /**
         * Set the video frame rate.  This must be within the range of supported frame rates
         * for the current frame size.
         * @param "aFrameRate" The video frame rate to set.
         * @exception Can leave with one of the system wide error codes
         */
        virtual void SetFrameRateL(TReal32 aFrameRate) {}

        /**
         * Set the video frame size
         * @param "aSize" The video frame size, in pixels
         * @exception Can leave with one of the system wide error codes
         */
        virtual void SetVideoFrameSizeL(const TSize& aSize) {}

        /**
         * Get the video frame size
         * @param "aSize" The video frame size, in pixels
         * @exception Can leave with one of the system wide error codes
         */
        virtual void GetVideoFrameSizeL(TSize& aSize) const {}
        
        /**
         * This API returns multimedias type supported by the data source/sink - 
         * Audio, Video, Data etc.  Each supported type is indicated by a MIME type structure.
         * @return 
         **/
        virtual const RArray<TDesC8* >& GetMultimediaTypesL() const {}
    };

class MVTVideoOutput
    {
    public:

        /**
        *  Sets the data format using MIME string.
        *  @param aFormat The format as a MIME string.
        **/
        virtual void SetFormatL(const TDesC8& aFormat) {}

        /**
         * Set the video frame size
         * @param "aSize" The video frame size, in pixels
         * @exception Can leave with one of the system wide error codes
         **/
        virtual void SetVideoFrameSizeL(const TSize& aSize) {}

        /**
         * Get the video frame size
         * @param "aSize" The video frame size, in pixels
         * @exception Can leave with one of the system wide error codes
         **/
        virtual void GetVideoFrameSizeL(TSize& aSize) const {}

        /**
         * This API returns multimedias type supported by the data source/sink - 
         * Audio, Video, Data etc.  Each supported type is indicated by a MIME type structure.
         * @return 
         **/
        virtual const RArray<TDesC8* >& GetMultimediaTypesL() const {}
    };

//This class is empty
class MVTAudioSource
    {
    };

//This class is empty
class MVTAudioSink
    {
    };

class MVTVideoSource;
class MVTVideoSink : public MVTVideoOutput
    {
    public:
        /**
         * Constructor.
         */
        MVTVideoSink(TUid aType): iDataSinkType(aType) {}

        /**
         * Method called by a data source to request the data sink to empty aBuffer of data.
         *
         * This is a pure virtual function that each derived class must implement.
         * This method is used when a data sink is passively waiting for requests from a supplier ie a data source
         * to empty a buffer.  The data sink must call the BufferEmptiedL member on aSupplier when it has emptied
         * the buffer of it's data - the data sink can either make this callback synchronously or asynchronously.
         * 
         * @param   "aBuffer"
         *          The full buffer that needs emptying of it's data
         *
         * @param   "aSupplier" 
         *          The data source that supplied the data. The data sink needs this to make the BufferEmptiedL
         *          callback on aSupplier to indicate to the data source that the data sink has finished with the buffer.
         *
         * @param   "aMediaId"
         *          This identifies the type of media eg audio or video and the stream id.
         *          This parameter is required in cases where the source can supply data
         *          of more than one media type and/or multiple strams of data
         */
        virtual void EmptyBufferL(
            TVtMMFDataBuffer aDataBuffer,
            MVTVideoSource* aSupplier,
            TMediaId aMediaId ) {}

        /**
         * Method called by a data source to pass back an filled buffer to the sink
         *
         * This is a pure virtual function that each derived class must implement.
         * This method is used as the callback when the data sink actively requests a supplier ie a data source
         * to fill a buffer by calling the data sources FillBufferL.
         * When the data sink gets this callback it knows that the buffer has been filled and is ready to be emptied
         * 
         * @param   "aBuffer"
         *          The buffer that has been filled by a data source and is now available for processing
         */
         virtual void BufferFilledL( CMMFBuffer* aBuffer ) = 0;

        /**
         * Method to indicate whether the data sink can create a buffer.
         *
         * This is a pure virtual function that each derived class must implement.
         * 
         * @return  ETrue if the data sink can create a buffer else EFalse
         */
        virtual TBool CanCreateSinkBuffer() {return EFalse;}

        /**
         * Returns a buffer created by the data sink
         *
         * This is a pure virtual function that each derived class must implement.
         *
         * @param   "aMediaId"
         *          This identifies the type of media eg audio or video and the stream id.
         *          This parameter is required in cases where the source can supply data
         *          of more than one media type and/or multiple strams of data.
         *
         * @param   "aReference"
         *          This must be written to by the method to indicate whether the created buffer is
         *          a 'reference' buffer.  A 'reference' buffer is a buffer that is owned by the sink
         *          and should be used in preference to the source buffer provided the source buffer
         *          is also not a reference buffer.
         * .  
         * @return  The created buffer
         */
        virtual CMMFBuffer* CreateSinkBufferL(
            TMediaId aMediaId,
            TBool &aReference ) {return NULL;}

        /**
         * Method to 'logon' the data sink to the same thread that sink will be consuming data in.
         *
         * This method may be required as the thread that the data sink was created in is not always
         * the same thread that the data transfer will take place in.  Therefore any thread specific
         * initialisation needs to be performed in the SinkThreadLogon rather than in the creation 
         * of the data sink.
         *
         * This is a virtual function that a derrived data sink can implement if any thread specific
         * initialisation is required and/or the data sink can create any asynchronous events.
         * 
         * 
         * @param   "aEventHandler"
         *          This is an MAsyncEventHandler to handle asynchronous events that occur during the
         *          transfer of multimedia data.  The event handler must be in the same thread as the data transfer
         *          thread - hence the reason it is passed in the SinkThreadLogon as opposed to say the constructor.
         *
         *
         * @return  KErrNone if successful, otherwise a system wide error code.
         */
        virtual TInt SinkThreadLogon( MAsyncEventHandler& aEventHandler ) {return KErrNone;}

        /**
         * Method to 'logoff' the data sink from the same thread that sink consumes data in.
         *
         * This method may be required as the thread that the data sink is deleted in may not be
         * the same thread that the data transfer took place in.  Therefore any thread specific
         * releasing of resources needs to be performed in the SinkThreadLogoff rather than in the destructor
         *
         * This is a virtual function that a derrived data sink can implement if any thread specific
         * releasing of resources is required.
         */
        virtual void SinkThreadLogoff() {}

        /**
         * Method to 'prime' the data sink
         *
         * This is a virtual function that a derrived data sink can implement if 
         * any data sink specific 'priming' is required
         */
        virtual TInt SinkPrimeL() {return 0;}

        /**
         * Method to 'play' the data sink
         *
         * This is a virtual function that a derrived data sink can implement if 
         * any data sink specific action is required prior to 'playing'ie the start of data transfer
         */
        virtual TInt SinkPlayL() {return 0;}

        /**
         * Method to 'pause' the data sink
         *
         * This is a virtual function that a derrived data sink can implement if 
         * any data sink specific action is required to 'pause'
         */
        virtual TInt SinkPauseL() {return 0;}

        /**
         * Method to 'stop' the data sink
         *
         * This is a virtual function that a derrived data sink can implement if 
         * any data sink specific action is required to 'stop'
         */
        virtual TInt SinkStopL() {return 0;}
    private:
        TUid iDataSinkType;
    };
    
class MVTVideoSource : public MVTVideoInput
    {
    public:
            
        /**
         * Constructor.
         */
    MVTVideoSource(TUid aType): iDataSourceType(aType) {}
    
        /**
         * Method called by a data sink to request the data source to fill aBuffer with data.
         *
         * This is a pure virtual function that each derived class must implement.
         * This method is used when a data source is passively waiting for requests from a consumer ie a data sink
         * to fill a buffer.  The data source must call the BufferFilledL member on aConsumer when it has filled
         * the buffer with data - the data source can either make this callback synchronously or asynchronously.
         * 
         * @param   "aBuffer"
         *          The buffer that needs filling with data
         *
         * @param   "aConsumer" 
         *          The data sink that consumes the data. The data source needs this to make the BufferFilledL
         *          callback on aConsumer when the data source has completed filling the aBuffer.
         *
         * @param   "aMediaId"
         *          This identifies the type of media eg audio or video and the stream id.
         *          This parameter is required in cases where the source can supply data
         *          of more than one media type and/or multiple strams of data eg a multimedia file
         */
    virtual void FillBufferL(
            CMMFBuffer* aBuffer,
            MVTVideoSink* aConsumer,
            TMediaId aMediaId ) {}
    
        /**
         * Method called by a data sink to pass back an emptied buffer to the source
         *
         * This is a pure virtual function that each derived class must implement.
         * This method is used as the callback when the data source actively requests a consumer ie a data sink
         * to empty a buffer by calling the data sinks EmptyBufferL.
         * When the data source gets this callback it knows that the buffer has been emptied and can be reused
         * 
         * @param   "aBuffer"
         *          The buffer that has been emptied by a data sink and is now available for reuse
         */
    virtual void BufferEmptiedL( CMMFBuffer* aBuffer ) = 0;
    
        /**
         * Method to indicate whether the data source can create a buffer.
         *
         * This is a pure virtual function that each derived class must implement.
         * 
         * @return  ETrue if the data source can create a buffer else EFalse
         */
    virtual TBool CanCreateSourceBuffer() {return EFalse;}
    
        /**
         * Returns a buffer created by the data source
         *
         * This is a pure virtual function that each derived class must implement.
         *
         * @param   "aMediaId"
         *          This identifies the type of media eg audio or video and the stream id.
         *          This parameter is required in cases where the source can supply data
         *          of more than one media type and/or multiple strams of data eg a multimedia file
         *
         * @param   "aReference"
         *          This must be written to by the method to indicate whether the created buffer is
         *          a 'reference' buffer.  A 'reference' buffer is a buffer that is owned by the source
         *          and should be used in preference to the sink buffer provided the sink buffer
         *          is also not a reference buffer
         * .  
         * @return  The created buffer
         */
    virtual CMMFBuffer* CreateSourceBufferL(
                TMediaId aMediaId,
                TBool &aReference ) {return NULL;}
    
        /**
         * Returns a buffer created by the data source
         *
         * This is a virtual function that a derived class can implement.
         * This can be used in preference to the above CreateSourceBufferL method in cases where
         * the source buffer creation has a dependancy on the sink buffer
         *
         * @param   "aMediaId"
         *          This identifies the type of media eg audio or video and the stream id.
         *          This parameter is required in cases where the source can supply data
         *          of more than one media type and/or multiple strams of data eg a multimedia file
         *
         * @param   "aSinkBuffer"
         *          The sink buffer the nature of which may influence the creation of the source buffer
         *
         * @param   "aReference"
         *          This must be written to by the method to indicate whether the created buffer is
         *          a 'reference' buffer.  A 'reference' buffer is a buffer that is owned by the source
         *          and should be used in preference to the sink buffer provided the sink buffer is not a reference buffer
         * .  
         * @return  The created buffer
         */
    virtual CMMFBuffer* CreateSourceBufferL(
                TMediaId aMediaId,
                CMMFBuffer& aSinkBuffer,
                TBool &aReference ) {return NULL;}
    
        /**
         * Method to 'logon' the data source to the same thread that source will be supplying data in.
         *
         * This method may be required as the thread that the data source was created in is not always
         * the same thread that the data transfer will take place in.  Therefore any thread specific
         * initialisation needs to be performed in the SourceThreadLogon rather than in the creation 
         * of the data source.
         *
         * This is a virtual function that a derrived data source can implement if any thread specific
         * initialisation is required and/or the data source can create any asynchronous events.
         * 
         * 
         * @param   "aEventHandler"
         *          This is an MAsyncEventHandler to handle asynchronous events that occur during the
         *          transfer of multimedia data.  The event handler must be in the same thread as the data transfer
         *          thread - hence the reason it is passed in the SourceThreadLogon as opposed to say the constructor.
         *
         *
         * @return  KErrNone if successful, otherwise a system wide error code.
         */
    virtual TInt SourceThreadLogon( MAsyncEventHandler& aEventHandler ) {return KErrNone;}
    
        /**
         * Method to 'logoff' the data source from the same thread that source supplies data in.
         *
         * This method may be required as the thread that the data source is deleted in may not be
         * the same thread that the data transfer took place in.  Therefore any thread specific
         * releasing of resources needs to be performed in the SourceThreadLogoff rather than in the destructor
         *
         * This is a virtual function that a derrived data source can implement if any thread specific
         * releasing of resources is required.
         */
    virtual void SourceThreadLogoff() {}
    
        /**
         * Method to 'prime' the data source
         *
         * This is a virtual function that a derrived data source can implement if 
         * any data source specific 'priming' is required
         */
    virtual TInt SourcePrimeL() {return 0;}
    
        /**
         * Method to 'play' the data source
         *
         * This is a virtual function that a derrived data source can implement if 
         * any data source specific action is required prior to 'playing'ie the start of data transfer
         */
    virtual TInt SourcePlayL() {return 0;}
    
        /**
         * Method to 'pause' the data source
         *
         * This is a virtual function that a derrived data source can implement if 
         * any data source specific action is required to 'pause'
         */
    virtual TInt SourcePauseL() {return 0;}
    
        /**
         * Method to 'stop' the data source
         *
         * This is a virtual function that a derrived data source can implement if 
         * any data source specific action is required to 'stop'
         */
    virtual TInt SourceStopL() {return 0;}
    private:
        TUid iDataSourceType;
    };

class MCommServer
    {
    };

class MVtProtocolCommand
    {
    public:
    enum TVtProtocolState 
        {     
        EIdle,           
        EInitializing, 
        ESetup, 
        EConnecting,
        EConnected,
        EDisconnecting,
        EResetting
        };
    };

class MVtProtocolHandler 
    {
    public:
        
       /**
        * Handle an event that has been generated.
        *
        * @param "aResponse"  "The response to a previously issued command."
        */
	      virtual void HandleSessionCommandEventL(const TVtCommandResponse& aResponse) = 0;

	      /**
	       * Handle an informational event that has been generated.
	       *
	       * @param "aEvent" "The event to be handled."
	       */
	      virtual void HandleSessionInformationalEventL(const TVtIndicationEvent& aEvent) = 0;

          /**
	       * Handle an error event that has been generated.
	       *
	       * @param "aEvent" "The event to be handled."
	       */
	      virtual void HandleSessionErrorEventL(const TVtErrorEvent& aEvent) = 0;

	      /** 
	      *  Signals completion of the audio output control command.
	      *  @param aId The command id of the completed command.
	      *  @param aCmd The command type.
	      *  @param aContextData The context data passed in with the command.
	      *  @param aStatus The command completion status.
	      **/
	      virtual void HandleAudioOutputControlCommandComplete(TInt aId, TVtAudioOutputControlCommand aCmd, TAny *aContextData ,TInt aStatus) = 0;
	      /**
	       * Handle an event that has been generated.
         *
	       * @param "aResponse"  "The response to a previously issued command."
	       */
	      virtual void HandleVideoEncoderCommandCompletedL(const TVtCommandResponse& aResponse) = 0;

	      /**
	       * Handle an event that has been generated.
         *
	       * @param "aEvent" "The event to be handled."
	       */
	      virtual void HandleVideoEncoderInformationalEventL(const TVtIndicationEvent& aEvent) = 0;

        /**
	       * Handle an event that has been generated.
         *
         * @param "aResponse"  "The response to a previously issued command."
	       */
	      virtual void HandleH324MConfigCommandCompletedL(const TVtCommandResponse& aResponse) = 0;

	      /**
	       * Handle an event that has been generated.
         *
	       * @param "aEvent" "The event to be handled."
	       */
	      virtual void HandleH324MConfigInformationalEventL(const TVtIndicationEvent& aEvent) = 0;
	      virtual ~MVtProtocolHandler() {}
    };


class MVtSessionCommand : public MVtProtocolCommand
    {
    public:
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
        virtual TInt InitProtocolL(TVtInitInfo& aInitInfo) = 0;

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
        virtual TInt PauseVideoL(MVTVideoSource& aDataSource) = 0;
        virtual TInt PauseVideoL(MVTVideoSink& aDataSink) = 0;
        virtual TInt PauseAudioL(MVTAudioSource& aAudioSource) = 0;
        
        /**
         * Resume a previously paused incoming or outgoing track.  For incoming,
         * this function starts resumes playing out the media to the appropriate
         * sink based on the current settings.  For outgoing it resumes encoding
         * and sending media from the source.
         * EVtCommandResume will be invoked will be invoked on the observer when the processing completes.       
         *
         * @returns A unique command id for asynchronous completion
         **/
        virtual TInt ResumeVideoL(MVTVideoSource& aDataSource) = 0;
        virtual TInt ResumeVideoL(MVTVideoSink& aDataSink) = 0;
        virtual TInt ResumeAudioL(MVTAudioSource& aAudioSource) = 0;

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
        virtual TInt ResetProtocolL() = 0;

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
        virtual TInt ConnectToProtocolL(MCommServer* aComm) = 0;

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
         *          KErrNoMemory if the SDK failed to allocate memory during this operation
         * @return A unique command id for asynchronous completion
         */
        virtual TInt AddVideoSourceL(const TUint aChannelId, MVTVideoSource &aDataSource) = 0;
        virtual TInt AddAudioSourceL(const TUint aChannelId, MVTAudioSource &aDataSource) = 0;

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
        virtual TInt AddVideoSinkL(const TUint aChannelId, MVTVideoSink &aDataSink) = 0;
        virtual TInt AddAudioSinkL(const TUint aChannelId, MVTAudioSink &aDataSink) = 0;

        /**
         * This API is to allow the user to cancel all pending requests.  The current request being
         * processed, if any, will also be aborted.
         * EVtCommandCancelAllCommands will be passed to the command observer on completion.
         * @returns A unique command id for asynchronous completion
         **/
        virtual TInt CancelAllCommandsL( ) = 0;

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
        virtual TInt DisconnectFromProtocolL() = 0;

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
        virtual TInt GetProtocolInterfaceL(TVtConfigType aType, MVtProtocolCommand*& aProtocolCommand) = 0;

        /**
         * This APIis to be used to release an interface that was previously obtained using
         * QueryInterfaceL.
         * @param aType
         * @param 
         * @exception not_supported
         * leaves if the specified interface id is not supported.
         **/
        virtual TInt DeleteProtocolInterfaceL(TVtConfigType aType, MVtProtocolCommand*) = 0;
        
        /**
         * Destructor.
         */
        virtual ~MVtSessionCommand() {}   
    };

class MVTUserInput
    {
    public:

        /**
        * @returns Returns the user input type.
        **/
        virtual TUserInputType GetType() = 0;
        virtual ~MVTUserInput() {}
    };

class MVtH324ConfigCommand : public MVtProtocolCommand
    {
    public:

        /**
         * This API allows the user to specify observers for the 324m interface.
         *
         * @param aHandler     the observer for command status and unsolicited informational events
         **/
        virtual void SetObserverL(MVtProtocolHandler* aHandler) = 0;

        /**
         * Sets the vendor identification data.  This does not cause the stack to issue a vendor identifiation request.
         * Set to NULL to disable sending vendor id.  If set to a valid parameter before Connect, it will cause the stack
         * to automatically send it along with the TCS message.
         * @param cc
         *         T35 Country code
         * @param ext
         *         T35 Extension
         * @param mc
         *         T35 Manufacturer code
         * @param aProduct
         *         Product number
         * @param aVersion
         *         Version number
         **/
        virtual TInt SetVendorId(TUint8 cc, TUint8 ext, TUint32 mc, const TDesC8* aProduct, const TDesC8* aVersion) = 0;
        /**
         * This API allows the user to send a videoTemporalSpatialTradeOff command to the peer.
         * It is a request to the remote encoder to adjust its encoding in accordance with the tradeoff value.
         * A value of 0 indicates a high spatial resolution and a value of 31 indicates a high frame rate.
         * The values from 0 to 31 indicate monotonically a higher frame rate. Actual values do not correspond
         * to precise values of spatial resolution or frame rate.
         *
         **/
        virtual TInt SendVideoTemporalSpatialTradeoffCommand(TUint aLogicalChannel, TUint8 aTradeoff)=0;

        /**
         * This API allows the user to send a videoTemporalSpatialTradeOff command to the peer.
         * It is an indication to the remote decoder that the local encoder has adjusted its encoding parameters
         * according to the tradeoff value.
         * A value of 0 indicates a high spatial resolution and a value of 31 indicates a high frame rate.
         * The values from 0 to 31 indicate monotonically a higher frame rate. Actual values do not correspond
         * to precise values of spatial resolution or frame rate.
         *
         **/
        virtual TInt SendVideoTemporalSpatialTradeoffIndication(TUint aLogicalChannel, TUint8 aTradeoff)=0;

        /**
         * This API allows the user to specify the supported resolutions for video for transmit and receive.
         *
         **/
        virtual TInt SetSupportedResolutions( ) = 0;

        /**
        * This API allows the user to set options for fast call setup procedures
        **/

        virtual TInt SetFastCsupOptions( ) = 0;

        /**
         * Causes the protocol to send the specified user input to the remote terminal using
         * control channel.  The user input can be either DTMF ot Alphanumeric
         * @param user_input A pointer to MVTUserInput
         * @returns A unique command id for asynchronous completion
         **/
        virtual TInt SendUserInputMessageL(MVTUserInput& user_input) = 0;        
    };
    
class MVtVideoConfigCommand : public MVtProtocolCommand
    {
    public:

        /**
         * This API allows the user to specify separate observers for the extension interface.  
         * 
         * @param aHandler     the observer for unsolicited informational events
         **/
        virtual void SetObserverL(MVtProtocolHandler* aHandler) = 0;

        /**
         * Sets the I-Frame refresh rate of the encoded output.
         *
         * @param aIFrameInterval I-Frame rate in seconds per I-Frame
         * @return True if successful, else false
         */
        virtual TInt SetIntraFrameInterval(TUint32 aIFrameInterval) = 0;
        
        /**
         * Requests the encoder to encode the next frame as an I-Frame.  If successful, the next encoded
         * frame will be an I-Frame.
         *
         * @return True for success, else false
         */
        virtual TInt RequestNextIntraFrame() = 0;
        
        /**
         * Sets the frame rate of encoded output for the specified layer.
         * @param aFrameRate Frame rate for the specified layer in frames per second.
         * @return True if successful, else false.
         */
        virtual TInt SetVideoFrameRate(TUint32 aFrameRate) = 0;
    };    

class MVtAudioConfigCommand : public MVtProtocolCommand
    {
    public:

        /**
         * This API allows the user to specify observers for the 324m interface.
         *
         * @param aHandler
         **/
        virtual void SetObserverL(MVtProtocolHandler* aHandler) = 0;

        /**
         * Method to set the playback volume to the specified value.
         *
         * This is a pure virtual function that each derived class must implement.
         * It is also an asynchronous function which will be answered with a callback.
         * 
         * @param aNewVolume
         *        An input parameter to hold the value for the requested playback volume.  
         *
         * @returns   Returns a command ID that can be used to identify a command completion result with corresponding request.
         */
        virtual TInt SetAudioVolumeL(TInt aVolume) = 0;
        
        /**
         * Method to get the maximum valid value for the playback volume.
         *
         * This is a pure virtual function that each derived class must implement.
         * It is also an asynchronous function which will be answered with a callback.
         * 
         * @param aMaxVolume
         *        An output parameter to hold the value for the maximum valid playback volume.  
         *            Note that the parameter should not be used until the callback indicates that the 
         *            method has completed.
         *
         * @returns   Returns a command ID that can be used to identify a command completion result with corresponding request.
         */
        virtual TInt GetMaxAudioVolumeL(TInt& aMaxVolume) = 0;
    };
    
class VTProtocolFactory
    {
    public:
        
        /**
         * Create one instance.
         */
        IMPORT_C static MVtSessionCommand* CreateSessionCommandL(MVtProtocolHandler* aProtocolHandler, TBool aEnableProxy, TVt3G324MSupported& a3G324MSupported);
    
        /**                                                                       
         * This function allows the application to delete an instance of a terminal 
         * and reclaim all allocated resources.  A terminal should be deleted only in 
         * the EIdle state.  An attempt to delete a terminal in any other state will 
         * result in unpredictable behavior.                                            
         *                                                                        
         * @param terminal the terminal to be deleted. 
         * 
         **/
        IMPORT_C static void DeleteSessionCommand( MVtSessionCommand* aSessionCommand ); 
    
        /**                                                                       
         * Creates an instance of a DevSound audio data source.
         *                                                                        
         * @param None
         *                                                                        
         * @returns A pointer to the interface
         **/                                                                      
        IMPORT_C static MVTAudioSource* CreateAudioSource();
    
        /**                                                                       
         * Deletes an instance of a DevSound audio data source
         * that was previously created with CreateAudioSource();
         *                                                                        
         * @param aSource The audio data source to be deleted.
         *                                                                        
         * @returns status
         **/                                                                      
        IMPORT_C static TInt DeletAudioSource(MVTAudioSource *aSource); 
    
        /**                                                                       
         * Creates an instance of a DevSound audio data sink.
         *                                                                        
         * @param None
         *                                                                        
         * @returns A pointer to the interface
         **/                                                                      
        IMPORT_C static MVTAudioSink* CreateAudioSink();
    
        /**                                                                       
         * Deletes an instance of a DevSound audio data sink
         * that was previously created with CreateAudioSink();
         *                                                                        
         * @param The audio data sink to be deleted.
         *                                                                        
         * @returns status
         **/                                                                      
        IMPORT_C static TInt DeleteAudioSink(MVTAudioSink *aSink);
    
        /**                                                                       
         * Creates an instance of a comm server of a particular name, to be used to
         * initialize the terminal.
         *                                                                        
         *                                                                        
         * @returns A pointer to a terminal or leaves if the type is invalid or the system is out of resources                                                              
         **/                                                                      
        IMPORT_C static MCommServer* CreateCommServerL(const TDesC & aName, TBool aEnableBitReversal=EFalse);
    
        /**
         * This function allows the application to delete an instance of a comm server
         * and reclaim all allocated resources.  A comm server should be deleted only when the 
         * protocol is in the EIdle state.  An attempt to delete a comm server in any other state 
         * could result in memory corruption within the protocol.  This function will leave  with
         * KErrInUse if the comm server is still in use.  However it will not check the state of the 
         * protocol that is using the comm server.
         * 
         * @param aCommServer the comm server to be deleted.
         * 
         * @returns a status code indicating success or failure
         **/
        IMPORT_C static void DeleteCommServerL(MCommServer* aCommServer);
        
        /**
         * Create instance.
         * @param aUIITyep
         * @param aTone
         */
        IMPORT_C static MVTUserInput* CreateUserInputIndication(TUserInputType aUIIType, TUint8 aTone);
        
        /**
         * Delete instance.
         * @param aUII
         */
        IMPORT_C static void DeleteUserInputIndication(MVTUserInput* aUII);
        
        /**
         * 
         * @param aAudioSink
         */
        IMPORT_C static MVtAudioConfigCommand* GetAudioConfigCommandL(MVTAudioSink* aAudioSink);
    };

#endif
//  End of File
 