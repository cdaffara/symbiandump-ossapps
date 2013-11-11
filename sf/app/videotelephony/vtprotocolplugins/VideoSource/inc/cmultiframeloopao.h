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
* Description:  Video Source subsystem.
*
*/


#ifndef C_MULTIFRAMELOOPAO_H
#define C_MULTIFRAMELOOPAO_H

// INCLUDE FILES
#include <e32base.h>

// FORWARD DECLARATIONS
class CMultiframeProvider;

// MACROS

/**
* @lib videosource.lib
* Handles data looping in multi frame sharing case.
*/
NONSHARABLE_CLASS( CMultiframeloopAO ) : public CActive
    {
    public: // Constructors and destructor

        /**
        * Constructor.
        * @param "aRequester" instance to requester
        */
        CMultiframeloopAO( CMultiframeProvider* aRequester );

        /**
        * Destructor.
        */
        ~CMultiframeloopAO();
        
        /**
        * Decreases number of free bitmaps
        * @param "aCount" count how many bitmaps to be removed
        */
        void RemoveFreeBitmaps( TInt aCount );
        
        /**
        * Increases number of free bitmaps
        * @param "aCount" count how many bitmaps to be increased
        */
        void SetFreeBitmaps( TInt aCount );
        
        /**
        * Cancels looping and completes request in correct thread.
        */
        void CancelDataLoop();

    public: // New functions

        /**
        * Activates looping and completes request in correct thread. 
        */
        void Signal( );
        
       	/**
        * Sets frame count.
        * @param "aCount" frame count
        */
        void SetFrameCount( TInt aCount );

    private:    // Functions from base classes

        /**
        * From CActive. See CActive for description.
        */
        void RunL();

        /**
        * From CActive. See CActive for description.
        */
        void DoCancel();

    private:    // Data

        // Pointer to requester.
        CMultiframeProvider* iRequester; // not owned
        
        // Index of frame to be handled
        TInt iIndex;
        
        // Total nuber of frames
        TInt iFramecount;
        
        // Number of free bitmaps 
        TInt iFreeBMCount;
        
        // Id of thread whose active scheduler this instance is added into.
        TThreadId iThreadId;
        
        // A flag to indecate loop ao must be stopped
        TBool iRequireStopping;
    };
#endif //C_MULTIFRAMELOOPAO_H

// End of File 
