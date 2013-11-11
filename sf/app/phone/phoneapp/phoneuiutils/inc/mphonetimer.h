/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Mixin class for phone timer.
*
*/


#ifndef __MPHONETIMER_H
#define __MPHONETIMER_H

/**
*  Mixin class for phone timer
*/
class MPhoneTimer
    {

    public: // New functions
        
        /**
        * Destructor
        */
        virtual ~MPhoneTimer(){};

        /**
        * This function is called after on timeout
        */
        virtual void HandleTimeOutL() = 0;
    };

#endif      // __MPHONETIMER_H   
            
// End of File
