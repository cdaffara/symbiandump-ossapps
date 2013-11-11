/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Image Transforms subsystem.
*
*/





#ifndef CVTIMAGESCALER_H
#define CVTIMAGESCALER_H

// INCLUDE FILES

#include <e32base.h>

// FORWARD DECLARATIONS

class CVtImageScalerImpl;
class CVtImage;
class CFbsBitmap;

// CLASS DECLARATIONS

/**
*  Image scaler observer interface.
*
*  @lib videosource.lib
*/
class MVtImageScalerObserver
    {
    public:
        /**
        * Callback method that is called by CVtImageScaler when scaling has
        * finished.
        * @param "aError" KErrNone if the scaling was done successfully, one
        * of the system wide error codes otherwise.
        * @return Pointer to newly created instance.
        */
        virtual void ScalingFinished( TInt aError ) = 0;
    };

/**
*  Image scaler.
*
*  @lib videosource.lib
*/
class CVtImageScaler : public CActive
    {
    public:

        /**
        * An enumeration within the CVtImageScaler namespace.
        * It provides a set of panic codes that may happen during
        * scaling process.
        */
        enum TPanics
            {
            /**
            * RotateL() is called while previous rotation process
            * has not yet been finished.
            */
            EBusy = 1
            };

        /**
        * An enumeration within the CVtImageScaler namespace.
        * It provides a set of supported scaling quality settings.
        */
        enum TQuality
            {
            /**
            * Nearest neigbour algorithm. Fast but poor image quality
            * when scale factor exceeds 2x.
            */
            ENearest,

            /**
            * Weighted average algorithm. Slower than Nearest Neighbour
            * but also better image quality when scaling at factor 2x or
            * higher.
            */
            EWeightedAverage,

            /**
            * Bilinear scaling algorithm. Slower than Weighted Average
            * but also better image quality.
            */
            EBilinear
            };

    public:

        /**
        * Creates new instance of CVtImageScaler.
        * @param "aObserver" Reference to instance observer.
        * @param "aQuality" Scaling quality. If this is omitted, then default
        * value of EWeightedAverage is used.
        * @param "aPriority" Active object priority.
        * @exception In error situation leaves with one of the system wide
        * error codes.
        * @return Pointer to newly created instance.
        */
        IMPORT_C static CVtImageScaler* NewL(
            MVtImageScalerObserver& aObserver,
            TQuality aQuality = EWeightedAverage,
            TPriority aPriority = EPriorityStandard );

        /**
        * C++ destructor.
        */
        IMPORT_C ~CVtImageScaler();

        /**
        * Sets scaling quality.
        * @param "aQuality" Scaling quality.
        * @exception In error situation leaves with one of the system wide
        * error codes.
        */
        IMPORT_C void SetQualityL( TQuality aQuality );

        /**
        * Scales image to new size.
        * @param "aSource" Source image that will be scaled.
        * @param "aTarget" Target image that will hold the scaled image. New
        * image's dimensions
        * will be taken from the size of target image.
        * @exception In error situation leaves with one of the system wide
        * error codes.
        */
        IMPORT_C void ScaleL( const CVtImage& aSource, CVtImage& aTarget );

        /**
        * Scales image to new size. Possible error during initialization is
        * returned via MVtImageScalerObserver.
        * @param "aSource" Source image that will be scaled.
        * @param "aTarget" Target image that will hold the scaled image. New
        * image's dimensions will be taken from the size of target image.
        */
        IMPORT_C void Scale( const CVtImage& aSource, CVtImage& aTarget );

        /**
        * Scales image to new size. This scale method is synchronous and
        * aTarget will contain scaled image of aSource on return. This method
        * may leave if aSource and/or aTarget are not suitable for scaling.
        * MVtImageScalerObserver::ScalingFinished is not called when this
        * method is used.
        * @param "aSource" Source image that will be scaled.
        * @param "aTarget" Target image that will hold the scaled image. New
        * image's dimensions will be taken from the size of target image.
        */
        IMPORT_C void ScaleNowL( const CVtImage& aSource, CVtImage& aTarget );

    private: // internal

        /**
        * C++ default constructor.
        */
        CVtImageScaler();

        /**
        * C++ constructor.
        * @param "aObserver" Reference to observer.
        * @param "aPriority" Active object priority.
        * @exception In error situation leaves with one of the system wide
        * error codes.
        */
        CVtImageScaler( MVtImageScalerObserver& aObserver, TPriority aPriority );

        /**
        * Second phase constructor.
        * @param "aQuality" Quality of the scaling.
        * @exception In error situation leaves with one of the system wide
        * error codes.
        */
        void ConstructL( TQuality aQuality );

        /**
        * Sets iStatus to KRequestPending and calls CActive::SetActive().
        */
        void Activate();

        /**
        * Signals this instance AO with given error code.
        * @param "aError" Signalling error code.
        */
        void Signal( TInt aError );

    private: // from CActive

        /**
        * This is defined in CActive. Check CActive for description.
        */
        void RunL();

        /**
        * This is defined in CActive. Check CActive for description.
        */
        void DoCancel();

        /**
        * Scale method called by the CAsyncCallBack.
        */
        static TInt StaticScale( TAny* aPtr );

        /**
        * Scale method called by the StaticScale.
        */
        void DoScale();

    private:

        // Scaling observer
        MVtImageScalerObserver& iObserver;

        // Active quality implementation
        CVtImageScalerImpl* iScalerImpl; // owned

        // Bitmap that will be used for locking global bitmap heap
        CFbsBitmap* iHeapLock; // owned

        // Asynccallback instance
        CAsyncCallBack* iAsyncCallBack; // owned

        // This is set to ETrue in DoCancel() method, and when CAsyncCallBack
        // gets executed, iCancelled is checked whether scaling should be done
        // or not.
        TBool iCancelled;
    };

#endif // CVTIMAGESCALER_H

// End of File


