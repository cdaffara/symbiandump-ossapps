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





#ifndef CVTIMAGEROTATOR_H
#define CVTIMAGEROTATOR_H

// INCLUDE FILES

#include <e32base.h>

// FORWARD DECLARATIONS

class CVtImage;
class CFbsBitmap;
class CVtImageRotatorImpl;

// CLASS DECLARATIONS

/**
*  Image rotator observer interface.
*
*  @lib videosource.lib
*/
class MVtImageRotatorObserver
    {
    public:
        /**
        * Callback method that is called by CVtImageRotator when rotation
        * process has been finished.
        * @param "aError" KErrNone if the rotation was done successfully, one
        * of the system wide error codes otherwise.
        * @return Pointer to newly created instance.
        */
        virtual void RotationFinished( TInt aError ) = 0;
    };

/**
*  Image rotator.
*
*  @lib videosource.lib
*/
class CVtImageRotator : public CActive
    {
    public:
        /**
        * An enumeration within the CVtImageRotator namespace.
        * It provides a set of panic codes that may happen during
        * rotation process.
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
        * An enumeration within the CVtImageRotator namespace.
        * It provides a set of supported rotation and mirror angles.
        */
        enum TRotationAngle
            {
            /**
            * Uninitialized place holder. Giving this rotation angle
            * to RotateL() will make RotateL() leave with error code
            * KErrNotSupported.
            */
            ERotationNone = -1,

            /**
            * Rotates image 90 degrees in a clockwise direction.
            */
            E90DegreesClockwise,

            /**
            * Rotates image 180 degrees in a clockwise direction.
            * (flip & mirror)
            */
            E180DegreesClockwise,

            /**
            * Rotates image 270 degrees in a clockwise direction.
            */
            E270DegreesClockwise,

            /**
            * Mirrors the bitmap around the horizontal axis.
            */
            EMirrorHorizontalAxis,

            /**
            * Flips the image around the vertical axis.
            */
            EFlipVerticalAxis,
            };

    public:

        /**
        * Creates new instance of CVtImageRotator.
        * @param "aObserver" Reference to instance observer.
        * @param "aPriority" Active object priority.
        * @exception In error situation leaves with one of the system wide
        * error codes.
        * @return Pointer to newly created instance.
        */
        IMPORT_C static CVtImageRotator* NewL(
            MVtImageRotatorObserver& aObserver,
            TPriority aPriority = EPriorityLow );

        /**
        * C++ destructor.
        */
        IMPORT_C ~CVtImageRotator();

        /**
        * Rotates image to given angle.
        * @param "aSource" Source image that will be rotated.
        * @param "aTarget" Target image that will hold the rotated image.
        * @param "aAngle" Rotation angle.
        * @exception In error situation leaves with one of the system wide
        * error codes.
        */
        IMPORT_C void RotateL( const CVtImage& aSource, CVtImage& aTarget,
            const TRotationAngle& aAngle );

        /**
        * Rotates image to given angle. Possible error during initialization is
        * returned via MVtImageRotatorObserver.
        * @param "aSource" Source image that will be rotated.
        * @param "aTarget" Target image that will hold the rotated image.
        * @param "aAngle" Rotation angle.
        */
        IMPORT_C void Rotate( const CVtImage& aSource, CVtImage& aTarget,
            const TRotationAngle& aAngle );

    private: // internal

        /**
        * C++ default constructor.
        */
        CVtImageRotator();

        /**
        * C++ constructor.
        * @param "aObserver" Reference to observer.
        * @param "aPriority" Active object priority.
        * @exception In error situation leaves with one of the system wide
        * error codes.
        */
        CVtImageRotator( MVtImageRotatorObserver& aObserver, TPriority aPriority );

        /**
        * Second phase constructor.
        * @exception In error situation leaves with one of the system wide
        * error codes.
        */
        void ConstructL();

        /**
        * Creates new instance of rotator implementation if needed.
        * @param "aAngle" Used rotation angle.
        * @exception In error situation leaves with one of the system wide
        * error codes.
        */
        void CreateRotatorL( const TRotationAngle& aAngle );

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
        * Rotate method called by the CAsyncCallBack.
        */
        static TInt StaticRotate( TAny* aPtr );

        /**
        * Rotate method called by the StaticScale.
        */
        void DoRotate();

    private:

        // Scaling observer
        MVtImageRotatorObserver& iObserver;

        // Bitmap that will be used for locking global bitmap heap
        CFbsBitmap* iHeapLock; // owned

        // Rotator implementation
        CVtImageRotatorImpl* iRotatorImpl; // owned

        // Asynccallback instance
        CAsyncCallBack* iAsyncCallBack; // owned

        // This is set to ETrue in DoCancel() method, and when CAsyncCallBack
        // gets executed, iCancelled is checked whether scaling should be done
        // or not.
        TBool iCancelled;
    };

#endif // CVTIMAGEROTATOR_H

// End of File


