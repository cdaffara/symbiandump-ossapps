/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MPHONERINGINGTONEOBSERVER_H
#define MPHONERINGINGTONEOBSERVER_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
*  Observer interface for ringing tone playing
*
* @since Series 60 3.1
*/
class MPhoneRingingToneObserver
    {
    public:  
        /**
        * Observer the ringing tone playing
        * @param aPlayingAudio ETrue is ringing tone audio is playing.
        * @param aPlayingVideo ETrue is ringing tone video is playing.
        */
        virtual void RingingTonePlaying( 
            TBool aPlayingAudio,
            TBool aPlayingVideo ) = 0;
    };

#endif      // MPHONERINGINGTONEOBSERVER_H   
            
// End of File
