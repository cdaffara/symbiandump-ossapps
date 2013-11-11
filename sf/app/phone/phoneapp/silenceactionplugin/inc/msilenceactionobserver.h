/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  A plug-in for silencing the rigning tones.
*
*/



#ifndef M_SILENCEACTIONOBSERVER_H
#define M_SILENCEACTIONOBSERVER_H

/**
* Silence action observer interface.
*
* @lib -
* @since S60 4.0
*/
NONSHARABLE_CLASS( MSilenceActionObserver )
    {
    public:
    
        /**
        * Notifies that ringing tone silence
        * action is completed.
        * 
        * @since S60 4.0
        */
        virtual void MuteActionCompleted() = 0;
    };
    
#endif // M_SILENCEACTIONOBSERVER_H
