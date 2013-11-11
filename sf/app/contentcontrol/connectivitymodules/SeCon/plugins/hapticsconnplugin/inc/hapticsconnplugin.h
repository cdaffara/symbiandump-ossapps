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
* Description:  Haptics OBEX connection plugin.
*
*/


#ifndef C_HAPTICSCONNPLUGIN_H
#define C_HAPTICSCONNPLUGIN_H

#include <obexserver.h>
#include <SrcsInterface.h>

class CHapticsMsgHandler;

class CHapticsConnPlugin : public CSrcsInterface
    {
    public:
        /**
         * 2-phased constructor.
         */
        static CHapticsConnPlugin* NewL();

        /**
         * Destructor.
         */
        virtual ~CHapticsConnPlugin();
        
        /**
         * From CSrcsInterface.
         * @see CSrcsInterface.
         */
        TBool IsOBEXActive();
        
    private:
        /**
         * 2nd phase constructor.
         */    
        void ConstructL();
        
        /**
         * Constructor.
         */
        CHapticsConnPlugin();
        
    private:
        /**
         * Component to handle messages between file systen and 
         * haptics server.
         * Own.
         */
        CHapticsMsgHandler* iMsgHandler;
    };

#endif // C_HAPTICSCONNPLUGIN_H