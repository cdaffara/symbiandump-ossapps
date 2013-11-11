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
* Description:  Class for NGA Drawer.
*
*/


#ifndef CVTENGDRAWNGA_H
#define CVTENGDRAWNGA_H

//  INCLUDES
//#include <videorenderer.h>
#include <Posting_Surface.h>
#include <Posting_Surface_Factory.h>
#include <pixelformats.h>
//#include <videoframebuffer.h>
// GCE Surface
#include "graphics/surfacemanager.h"
#include "graphics/surface.h"
#include "graphics/surfaceupdateclient.h"

#include "CVtEngRemoteVideo.h"
#include "TVtEngRenderConfigNGA.h"

// FORWARDS

class TDisplaySinkParamsNGA;

// CONSTANTS AND MACROS
const TInt KVtEngMaxSurfaceBuffers = 2;

NONSHARABLE_CLASS( TVTSurfaceBuffer )
    {
    public:

    /**
    * C++ constructor
    */
    TVTSurfaceBuffer( TUint8* aBuffer = NULL, TInt aSurfaceNo = KErrNotFound );
               
    /**
    * Returns pointer reference to stored buffer.
    */
    TUint8* &Buffer();
                
    /**
    * Returns surface no..
    */
    TInt SurfaceNo();
    
    void Set( TUint8* aBuffer, TInt aSurfaceNo );
    
    void UnSet();
                       
    private:

    // Pointer's pointer to buffer
    TUint8* iBuffer;
    TInt iSurfaceBufferNo;
    };    

/**
*  Buffer waiter observer API
*
*  @lib videoteleng.lib
*  @since Series 60 MCL
*/
NONSHARABLE_CLASS( MVtEngNGABufferWaiterObserver )
    {
    public: // New methods

        /**
        * Buffer fetched without errors.
        * @param aBuffer Pointer to fetched buffer.
        */
        virtual void BufferReadyL(
                TVTSurfaceBuffer* aBuffer ) = 0;

        /**
        * Buffer fetch error.
        * @param aError Error code.
        */
        virtual void FetchError( TInt aError ) = 0;
    };


/**
*  DNGA drawer
*
*  @lib videoteleng.lib
*  @since Series 60 MCL
*/
NONSHARABLE_CLASS( CVtEngDrawNGA ) : 
    public CVtEngRemoteVideo::CVtEngDraw,
    public MVtEngNGABufferWaiterObserver
    {
    
    public: // constructor and destructor

        /**
        * Two-phased constructor.
        * @param aObserver call back interface to Display Sink
        * @return instance of CVtEngDrawDSA
        */
        static CVtEngDrawNGA* NewL( MDisplaySinkObserver* aObserver );

        // Destructor
        ~CVtEngDrawNGA();

    private: // construction

        /**
        * C++ constructor as private.
        * @param aObserver callback interface for Display Sink
        */
        CVtEngDrawNGA( MDisplaySinkObserver* aObserver );

    private: // from CVtEngDraw

        /**
        * @see CVtEngDraw::BaseConstructL
        */
        void BaseConstructL();

        /**
        * @see CVtEngDraw::CreateSinkL
        */
        MDisplaySink* CreateSinkL();

        /**
        * @see CVtEngDraw::BaseRefreshL
        */
        void BaseRefreshL();

        /**
        * @see CVtEngDraw::BaseRefreshBitmapsL
        */
        void BaseRefreshBitmapsL();

        /**
        * @see CVtEngDraw::BaseDoCancel
        */
        void BaseDoCancel();

        /**
        * @see CVtEngDraw::BaseVerifyConfigL
        */
        void BaseVerifyConfigL();

        /**
        * @see CVtEngDraw::BaseRestartL
        */
        void BaseRestartL();

        /**
        * @see CVtEngDraw::BaseAbortNow
        */
        void BaseAbortNow();

        /**
        * @see CVtEngDraw::BaseStartDrawL
        */
        void BaseStartDrawL();

        /**
        * @see CVtEngDraw::BaseStopDraw
        */
        void BaseStopDraw();

        /**
        * @see CVtEngDraw::BaseSetConfigL
        */
        void BaseSetConfigL( const TVtEngRenderConfig& aConfig );

        /**
        * @see CVtEngDraw::BaseSetUIForegroundL
        */
        void BaseSetUIForegroundL( TBool aForeground );

        /**
        * @see CVtEngDraw::BaseVideoFrameSizeChangedL
        */
        void BaseVideoFrameSizeChangedL( const TSize& );

        /**
        * @see CVtEngDraw::BaseRequestLastFrame
        */        
        void BaseRequestLastFrame();
        
        /**
        * @see CVtEngDraw::Config
        */
        TVtEngRenderConfig& Config();
        
    private: // New mothod
       
        /**
        * Returns reference to sink.
        */
        MDisplaySink& DisplaySink();     
               
        /**
        * Base construct implementation
        */
        void DoBaseConstructL();
        
        /**
        * Base start drawing implementation
        */
        void DoBaseStartDrawL();             
    
        /**
        * Sets given buffer available to display sink.
        */
        void SetAvailableToSink( TVTSurfaceBuffer* aBuffer );     
        
        /**
        * Update sink params implementation.
        */
        void DoUpdateSinkParamsL();

        /**
        * Updates sink params.
        */
        void UpdateSinkParamsL();   

        /**
        * Create surface
        */        
        void CreateSurfaceL();
        
        /**
        * Create surface implementation
        */        
        void DoCreateSurfaceL();

        /**
        *clearing a surface buffer to given colour
        */
        void ClearSurfacebuffer( 
            TUint8* aStartAddr, 
            TInt aOffset, 
            const RSurfaceManager::TSurfaceInfoV01 aInfo, 
            TRgb &aColor );
        
        static TInt SurfaceBuffer0Ready(TAny* aAny);
        
        static TInt SurfaceBuffer1Ready(TAny* aAny);
     
        TBool DoSurfaceBuffer0Ready();
        
        TBool DoSurfaceBuffer1Ready();
   
    public: //from MVtEngNGABufferWaiterObserver
    
        /**
        * @see MVtEngNGABufferWaiterObserver::BufferReadyL
        */
        void BufferReadyL( TVTSurfaceBuffer* aBuffer );

        /**
        * @see MVtEngNGABufferWaiterObserver::FetchError
        */
        void FetchError( TInt aError );
    
        
    private:
        /**
        *  @see CActive::RunL
        */       
        void RunL();
        
    private: // New functions
        /**
        * Fills TDisplaySinkParamsNGA struct
        */
        void GetSinkParams( TDisplaySinkParamsNGA& aSinkParams );
        
    private: //inner class
        
        // Buffer class for storing buffer.
        NONSHARABLE_CLASS( TVtEngBuffer )
            {
            public:

                /**
                * C++ constructor
                */
                TVtEngBuffer();

                /**
                * Sets buffer
                */
                void Set( TVTSurfaceBuffer* aBuffer );                

                /**
                * Unsets buffer
                */
                void UnSet();

                /**
                * Returns ETrue if buffer is set
                */
                TBool IsSet() const;

                /**
                * Returns pointer reference to stored buffer.
                */
                TVTSurfaceBuffer* &SurfaceBuffer();
                
                /**
                * Returns pointer reference to stored buffer.
                */
                TUint8* &Buffer();
                                
            private:

                // Pointer's pointer to buffer
                TUint8* iBuffer;
                TVTSurfaceBuffer* iSurfaceBuffer;
            };    
            
        // Waiter class for asynchronous events.
        NONSHARABLE_CLASS( CVtEngBufferWaiter ) : public CActive
            {
            public:

                /**
                * C++ constructor.
                */
                CVtEngBufferWaiter( //CVideoRenderer& aVideoRenderer,
                    MVtEngNGABufferWaiterObserver& aObserver );

                /**
                * Destructor, frees all resources allocated by this instance.
                */
                ~CVtEngBufferWaiter();

                /**
                * Fetch new buffer (also activates listening).
                */
                TInt FetchBuffer();
                
                /**
                * One SurfaceBuffer is available
                */
                void BufferAvailable(TVTSurfaceBuffer& aBuffer); 

            private: // new methods

                /**
                * Activate buffer listening (also calls CActive::SetActive()).
                */
                void Activate();

            private: // from CActive

                /**
                * @see CActive::RunL()
                */
                void RunL();

                /**
                * @see CActive::DoCancel()
                */
                void DoCancel();

            private: // data

                // Video frame renderer (not owned)
                //CVideoRenderer* iVideoRenderer;

                // BufferWaiter observer who gets informed about buffers
                MVtEngNGABufferWaiterObserver* iObserver;      
                
                //Avaiable surface buffers
                TVTSurfaceBuffer* iBuffers[KVtEngMaxSurfaceBuffers];         
                
            };
            
        // Surface Buffer Active Call back.
        NONSHARABLE_CLASS( CActiveCallBack ) : public CActive
            {
            public:
                CActiveCallBack( TCallBack aCallBack, TInt aPriority): CActive(aPriority), iCallBack(aCallBack) 
                    {};
                CActiveCallBack();
                           
            public: // Functions from base classes
                void SetActive() 
                    { 
                    CActive::SetActive();
                    }; 
                void RunL() 
                    { 
                    iCallBack.CallBack();
                    };
                void DoCancel() 
                    {};
                           
            private: // Data
                TCallBack iCallBack;
            };

    private: // data        
    
        //Video frame renderer
        //CVideoRenderer *iVideoRenderer;
        RSurfaceManager* iSurfaceManager;
        RSurfaceUpdateSession iSurfaceUpdateSession;
        //Surface pixel format
        TUidPixelFormat iSurfaceFormat;
        
        RChunk* iSurfaceChunk;
        
        TInt8* iChunkBuffer0;
        
        TInt8* iChunkBuffer1;
        
        TVTSurfaceBuffer iSurfaceBuffer0;
        
        TVTSurfaceBuffer iSurfaceBuffer1;
        
        //CActiveCallBack* iSurfaceBufferAo0; 
        
        //CActiveCallBack* iSurfaceBufferAo1;
        
        CActiveCallBack* iCallBackTable[KVtEngMaxSurfaceBuffers];
        
        //NGA surface id
        TSurfaceId iSurfaceId;
        
        //NGA surface has created
        TBool	iSurfaceCreated;        
               
        // Buffer waiter instance
        CVtEngBufferWaiter* iBufferWaiter;              
        
        // Surface buffers
        TVtEngBuffer
            iSurfaceBuffers[ MDisplaySink::EFirstBitmap + KVtEngMaxSurfaceBuffers ];   
        
        TSglQue<TVtEngBuffer> iWaitingBuffers; // buffers waiting to be submitted
        TBool iSubmitPending;

        // Configuration
        TVtEngRenderConfigNGA iConfig;
        
        // Time stamp for surface update
        TTimeStamp iTimeStamp;
    };

#endif // CVTENGDRAWDSA_H
