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


#ifndef CDATASINKPROXY_H
#define CDATASINKPROXY_H
NONSHARABLE_CLASS( CPVDisplaySinkNodeProxy ) : public CBase,
                                               public MVTVideoSource
    {
    public: //New
        /**
         * Constructor .
         */
        CPVDisplaySinkNodeProxy();
        /**
         * Set new supplier.
         * @param aSupplier
         */
        void SetSupplier(MPVDataSourceBase* aSupplier);
    public: //From MVTVideoSource
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
    private:
        MPVDataSourceBase* iSupplier;
    };

NONSHARABLE_CLASS( CDisplaySinkProxy ) : public CBase,
                                         public MPVDataSink,
                                         public MPVVideoOutput
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
    public: //From MPVDataSinkBase
        /**
         * Method called by a MDataSource to request the data sink to empty aBuffer of data.
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
        virtual void EmptyBufferL(CMMFBuffer* aBuffer, MPVDataSourceBase* aSupplier, TMediaId aMediaId);
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
        virtual void BufferFilledL(CMMFBuffer* aBuffer);
        /**
         * Method to indicate whether the data sink can create a buffer.
         *
         * This is a pure virtual function that each derived class must implement.
         * 
         * @return  ETrue if the data sink can create a buffer else EFalse
         */
        virtual TBool CanCreateSinkBuffer();
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
        virtual CMMFBuffer* CreateSinkBufferL(TMediaId aMediaId, TBool &aReference );
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
        virtual TInt SinkThreadLogon( MAsyncEventHandler& aEventHandler );
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
        virtual void SinkThreadLogoff();
        /**
         * Method to 'prime' the data sink
         *
         * This is a virtual function that a derrived data sink can implement if 
         * any data sink specific 'priming' is required
         */
        virtual TInt SinkPrimeL();
        /**
         * Method to 'play' the data sink
         *
         * This is a virtual function that a derrived data sink can implement if 
         * any data sink specific action is required prior to 'playing'ie the start of data transfer
         */
        virtual TInt SinkPlayL();
        /**
         * Method to 'pause' the data sink
         *
         * This is a virtual function that a derrived data sink can implement if 
         * any data sink specific action is required to 'pause'
         */
        virtual TInt SinkPauseL();

        /**
         * Method to 'stop' the data sink
         *
         * This is a virtual function that a derrived data sink can implement if 
         * any data sink specific action is required to 'stop'
         */
        virtual TInt SinkStopL();
    public: //From MPVVideoOutput

        /**
         * Set the video frame format.  This must be from the list of supported formats.
         * @param "aFormat" A mime string describing the video frame format.
         * @exception Can leave with one of the system wide error codes
         **/
        virtual void SetFormatL(const TDesC8& aFormat);

        /**
         * Set the video frame size
         * @param "aSize" The video frame size, in pixels
         * @exception Can leave with one of the system wide error codes
         **/
        virtual void SetVideoFrameSizeL(const TSize& aSize);

        /**
         * Get the video frame size
         * @param "aSize" The video frame size, in pixels
         * @exception Can leave with one of the system wide error codes
         **/
        virtual void GetVideoFrameSizeL(TSize& aSize) const;
    public:
        /**
         * Constructor.
         */
        CDisplaySinkProxy(MVTVideoSink* aDisplaySink);
        
        /**
         * ConstructL
         */
        void ConstructL();

        /**
         * NewL
         */
        static CDisplaySinkProxy* NewL(MVTVideoSink* aDisplaySink);
        /**
         * Get pointer object of data sink.
         * @return member variant:iDisplaySink
         */
        MVTVideoSink* DisplaySink() { return iDisplaySink; }
    private:
        MVTVideoSink* iDisplaySink;
        CPVDisplaySinkNodeProxy* iDSNodeProxy;
        
    };

NONSHARABLE_CLASS( CAudioSinkProxy ) : public CBase,
                                       public MVTAudioSink
    {
    public:
        MPVDataSink* iAudioSink;
    };
    
#endif
//  End of File 

