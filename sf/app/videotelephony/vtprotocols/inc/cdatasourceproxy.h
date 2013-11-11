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


#ifndef CDATASOURCEPROXY_H
#define CDATASOURCEPROXY_H
NONSHARABLE_CLASS( CPVCameraSourceNodeProxy ) : public CBase,
                                                public MAsyncEventHandler, 
                                                public MVTVideoSink
    {
    public: //From MAsyncEventHandler
        /**
         * Send event to client.
         * @param aEvent 
         */
        virtual TInt SendEventToClient(const TMMFEvent &aEvent);
    public: //From MVTVideoSink

        /**
         * Method called by a data source to pass back an filled buffer to the sink.
         *
         * This is a pure virtual function that each derived class must implement.
         * This method is used as the callback when the data sink actively requests a supplier ie a data source
         * to fill a buffer by calling the data sources FillBufferL.
         * When the data sink gets this callback it knows that the buffer has been filled and is ready to be emptied
         * 
         * @param   "aBuffer"
         *          The buffer that has been filled by a data source and is now available for processing
         */
        void BufferFilledL(CMMFBuffer* aBuffer);
               
    public: //New
        /**
         * Constructor.
         */
        CPVCameraSourceNodeProxy();
        
        /**
         * Set consumer object.
         * @param aConsumer new object
         */
        void SetConsumer(MPVDataSinkBase* aConsumer);
    private:
        MPVDataSinkBase* iConsumer;        
    };

NONSHARABLE_CLASS( CVideoSourceProxy ) : public CBase,
                                         public MPVDataSource, 
                                         public MPVVideoInput
    {
    public: //From MPVPluginBase

        /**
         * This API returns multimedias type supported by the data source/sink - 
         * Audio, Video, Data etc.  Each supported type is indicated by a MIME type structure.
         * @return Multimedia types supported by the data source/sink.  The reference 
         * is valid until the MPVPluginBase derived object is destroyed.
         **/
        virtual const RArray<TPVMIMEType *>& GetMultimediaTypesL() const;

        /**
         * This API is to allow for extensibility of the plugin interface.
         * It allows a caller to ask for all UUIDs associated with a particular MIME type.
         * If interfaces of the requested MIME type are found within the plugin, they are added 
         * to the UUIDs array.  
         * 
         * Also added to the UUIDs array will be all interfaces which have the requested MIME 
         * type as a base MIME type.  This functionality can be turned off.
         * 
         * @param aMimeType The MIME type of the desired interfaces
         * @param aUuids An array to hold the discovered UUIDs
         * @param aExactUuidsOnly Turns on/off the retrival of UUIDs with aMimeType as a base type
         **/
        virtual void QueryUUID( const TPVMIMEType& aMimeType, RArray<TPVUuid>& aUuids, bool aExactUuidsOnly = false );

        /**
         * This API is to allow for extensibility of the plugin interface.
         * It allows a caller to ask for an instance of a particular interface object to be returned.
         * The mechanism is analogous to the COM IUnknown method.  The interfaces are identified with
         * an interface ID that is a UUID as in DCE and a pointer to the interface object is
         * returned if it is supported.  Otherwise the returned pointer is NULL.
         * @param aUuid The UUID of the desired interface
         * @param aInterfacePtr The output pointer to the desired interface
         **/
        virtual void QueryInterface( const TPVUuid& aUuid, TPVInterfacePtr& aInterfacePtr );

        /**
         *  This API is used to provide a timebase for plugins that deal with synchronized media data
         **/
        virtual void SetTimebaseL(MPVSymbianTimebaseInterface* aTimebase);
    public: //From MPVDataSourceBase

        /**
         * Method called by a MDataSink to request the data source to fill aBuffer with data.
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
        virtual void FillBufferL(CMMFBuffer* aBuffer, MPVDataSinkBase* aConsumer, TMediaId /*aMediaId*/);

        /**
         * Returns a buffer created by the data source.
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
        virtual CMMFBuffer *CreateSourceBufferL(TMediaId aMediaId, TBool &aReference);

        /**
         * Returns a buffer created by the data source.
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
        virtual CMMFBuffer *CreateSourceBufferL(TMediaId aMediaId, CMMFBuffer &aSinkBuffer, TBool &aReference);

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
        virtual TInt SourceThreadLogon( MAsyncEventHandler& aEventHandler );
        
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
        virtual void SourceThreadLogoff();

        /**
         * Method to 'prime' the data source.
         *
         * This is a virtual function that a derrived data source can implement if 
         * any data source specific 'priming' is required
         */
        virtual TInt SourcePrimeL();

        /**
         * Method to 'play' the data source.
         *
         * This is a virtual function that a derrived data source can implement if 
         * any data source specific action is required prior to 'playing'ie the start of data transfer
         */
        virtual TInt SourcePlayL();

        /**
         * Method to 'pause' the data source.
         *
         * This is a virtual function that a derrived data source can implement if 
         * any data source specific action is required to 'pause'
         */
        virtual TInt SourcePauseL();

        /**
         * Method to 'stop' the data source.
         *
         * This is a virtual function that a derrived data source can implement if 
         * any data source specific action is required to 'stop'
         */
        virtual TInt SourceStopL();

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
        virtual void BufferEmptiedL(CMMFBuffer* aBuffer);

        /**
         * Method to indicate whether the data source can create a buffer.
         *
         * This is a pure virtual function that each derived class must implement.
         * 
         * @return  ETrue if the data source can create a buffer else EFalse
         */
        virtual TBool CanCreateSourceBuffer();
    public: //From MPVVideoInput

        /**
         * Set the video frame format.  This must be from the list of supported formats.
         * @param "aFormat" The mime string describing the video frame format.
         * @exception Can leave with one of the system wide error codes
         */
        virtual void SetFormatL(const TDesC8& aFormat);
        
        /**
         * Set the video frame rate.  This must be within the range of supported frame rates
         * for the current frame size.
         * @param "aFrameRate" The video frame rate to set.
         * @exception Can leave with one of the system wide error codes
         */
        virtual void SetFrameRateL(TReal32 aFrameRate);

        /**
         * Set the video frame size.
         * @param "aSize" The video frame size, in pixels
         * @exception Can leave with one of the system wide error codes
         */
        virtual void SetVideoFrameSizeL(const TSize& aSize);

        /**
         * Get the video frame size.
         * @param "aSize" The video frame size, in pixels
         * @exception Can leave with one of the system wide error codes
         */
        virtual void GetVideoFrameSizeL(TSize& aSize) const;
    public: //new
        /**
         * Constructor.
         */
        CVideoSourceProxy(MVTVideoSource* aVideoSource);
        /**
         * ConstructL.
         */
        void ConstructL( );
        
        /**
         * NewL.
         */
        static CVideoSourceProxy* NewL(MVTVideoSource* aVideoSource);
        
        /**
         * Get member variant.
         * @return memeber variant:iVideoSource
         */
        MVTVideoSource* VideoSource() { return iVideoSource; }
    private:
        MVTVideoSource* iVideoSource;
        CPVCameraSourceNodeProxy* iCameraSourceNodeProxy;
    };

NONSHARABLE_CLASS( CAudioSourceProxy ) : public CBase, 
                                         public MVTAudioSource
    {
    public:
        MPVDataSource* iAudioSource;
    };
#endif
//  End of File
