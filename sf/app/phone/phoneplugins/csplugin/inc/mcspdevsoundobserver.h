/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Observer for audio stream start up.
*
*/


#ifndef MCSPDEVSOUNDOBSERVER_H
#define MCSPDEVSOUNDOBSERVER_H

/**
 *  Observer interface for knowing when audio streams have been
 *  successfully ramped up.
 *
 */
class MCSPDevSoundObserver
    {

public:

    /**
     * Notification that Mic has been activated successfully
     *     
     */
    virtual void MicActivatedSuccessfully() = 0;
    
    /**
     * Notification that speaker has been activated successfully
     *
     */   
    virtual void SpeakerActivatedSuccessfully() = 0;
    
    /**
     * Notification that Mic initialisation failed.
     *
     */
    virtual void MicActivationFailed() = 0;
    
    /**
     * Notification that Speaker activation failed.
     *
     */
    virtual void SpeakerActivationFailed() = 0;

    };


#endif // MCSPDEVSOUNDOBSERVER_H
