/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef LOADGEN_PHOTOCAPTURE_H
#define LOADGEN_PHOTOCAPTURE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <ecam.h>  // CCamera, MCameraObserver

#include "loadgen_loadbase.h"
#include "loadgen_loadattributes.h"


// FORWARD DECLARATIONS
class CCameraManager;


// CLASS DECLARATIONS

class CPhotoCapture : public CLoadBase
    {
public:
    static CPhotoCapture* NewL(TPhotoCaptureAttributes& aAttributes, TInt aReferenceNumber);
    virtual ~CPhotoCapture();
        
private: // Constructors
    CPhotoCapture(TPhotoCaptureAttributes& aAttributes, TInt aReferenceNumber);
    void ConstructL();  

public:  // New methods
    virtual void Resume();
    virtual void Suspend();
    virtual void SetPriority();
    virtual void Close();
    virtual TPtrC Description();
    inline TPhotoCaptureAttributes& Attributes() { return iAttributes; }
	inline void SetAttributes(const TPhotoCaptureAttributes& aAttributes) { iAttributes = aAttributes; }


public:  // New static methods
    static TInt ThreadFunction(TAny* aThreadArg);
    //static void SetImagesReady(TInt aImages);
    //static TInt ImagesReady();

private:  // New static methods
    static void GenerateLoad(TPhotoCaptureAttributes& aAttributes);
    static void DoHeaveStuff(TInt aMode);

private: // Data
    TPhotoCaptureAttributes iAttributes;    
    RThread                 iThread;
    //static TInt             iImagesReady;
    };
 


 
class CCameraManager : public CActive, public MCameraObserver
    {
    public:
        
        static CCameraManager* NewL(TPhotoCaptureAttributes& aAttrs);        
        virtual ~CCameraManager();
 
    private:
        
        CCameraManager(TPhotoCaptureAttributes& aAttrs);
        void ConstructL();
 
    private:
        /**
         * From MCameraObserver:
         */         
        virtual void ReserveComplete(TInt aError);
        virtual void PowerOnComplete(TInt aError);
        virtual void ViewFinderFrameReady(CFbsBitmap& aFrame);
        virtual void ImageReady(CFbsBitmap* aBitmap,
                HBufC8* aData, TInt aError);
        virtual void FrameBufferReady(MFrameBuffer* aFrameBuffer,
                TInt aError);
    
        static TInt PeriodicTimerCallBack(TAny* aAny);
    public:
        
        /**
         * Reserves the camera.
         */
        void ReserveCameraL();
        
        /**
         * Takes a picture.
         */
        void CapturePhotoL();
        
        TInt NumberOfPictures();
                                        
        /**
         * From CActive:
         */
        void RunL();
        void DoCancel();
 
    private:
        
        enum TState
            {
            ENotReady,
            ECameraReserved,
            EIdle,
            ECapture
            };

        CPeriodic*                  iPeriodicTimer;
        TPhotoCaptureAttributes&    iAttributes;
        CCamera*                    iCamera;
        TState                      iState;
        TInt                        iNumOfPics;
        TCameraInfo                 iCameraInfo;
        TInt                        iCurrentCamera;
    };


#endif
