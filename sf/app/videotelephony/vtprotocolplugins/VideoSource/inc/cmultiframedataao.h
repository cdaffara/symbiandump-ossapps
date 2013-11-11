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

#ifndef C_MULTIFRAMEDATAAO_H
#define C_MULTIFRAMEDATAAO_H

// INCLUDE FILES
#include <e32base.h>
#include "mmultiframeprovider.h"
// FORWARD DECLARATIONS
class CMultiframeProvider;

// MACROS
	
/**
*  @lib videosource.lib
*/
// ============================ CMultiframeDataAO ===============================
NONSHARABLE_CLASS( CMultiframeDataAO ) : public CTimer
    {
    public: // Constructors and destructor

        /**
        * Constructor.
        * @param "aRequester" instance to requester
        */
        CMultiframeDataAO( CMultiframeProvider* aRequester );

        /**
        * Destructor.
        */
        ~CMultiframeDataAO();

    public: // New functions
				
				/**
        * Sets observer.
        * @param "aObserever" instance to observer
        */
				void SetObserver( MMultiframeprovider* aObserver );
					
        /**
        * Activates CTimer
        * @param "aUpdateTime" expiration time
        */
        void Activate( TTimeIntervalMicroSeconds aUpdateTime, TBool aInit, TBool aVFUpdate );
        
       	/**
        * Updates local buffer.
        * @param "aLocalBuffer" local buffer
        */
        void LocalBuffer( CVtImageBitmap* aLocalBuffer );
        
        /**
        * Updates remote buffer.
        * @param "aRemoteBuffer" remote buffer
        */
        void RemoteBuffer ( CVtImageBitmap* aRemoteBuffer );
        
        /**
        * Cancels looping and completes request in correct thread.
        */        
        void CancelTimeLoop();

    private:    // Functions from base classes

        /**
        * From CActive. See CActive for description.
        */
        void RunL();

    private:    // Data

        // Pointer to observer.
        MMultiframeprovider* iObserver; // not owned
        
        // Pointer to requester
        CMultiframeProvider* iRequester;
        
        // For updating ViewFinder buffer
        TImageShareDataBuffer iLocalBuffer;
        // For updating remote buffer
        TImageShareDataBuffer iRemoteBuffer;
        
        // For updating remote buffer if activated when active
        TImageShareDataBuffer iRemoteSecondBuffer;
        
        // For updating ViewFinder buffer if activated when active
        TImageShareDataBuffer iLocalSecondBuffer;
        
        // Initialization flag
        TBool iInit;
        
        // Delta calc ref point
       	TTimeIntervalMicroSeconds iTime;
        
        // VF update flag
        TBool iVFUpdate;
        
        // Id of thread whose active scheduler this instance is added into.
        TThreadId iThreadId;
        
        // A flag to indecate loop ao must be stopped
        TBool iRequireStopping;
    };

#endif //C_MULTIFRAMEDATAAO_H

//  End of File





