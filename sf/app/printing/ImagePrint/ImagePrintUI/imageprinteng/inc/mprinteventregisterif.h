/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MPRINTEVENTREGISTERIF_H
#define MPRINTEVENTREGISTERIF_H

#include <e32def.h>

class MIdlePrintEventObserver;
class MPrintJobStatus;

/**
 *
 * Interface to use the print event functionality of Image Print Engine
 *
 */
class MPrintEventRegisterIF
    {
	public:		// Abstract methods


        /**
         *  Registers class to PrintEventObserver. After registration
         *  the class receives events
         *
         *  @param aObserver class that wants to register
         *
         */
        virtual void RegisterObserver( MPrintJobStatus* aObserver ) = 0;

        /**
         *  Unregisters class from PrintEventObserver. After unregistration
         *  the class stops receiving events. Class is also deleted from observer
         *  array.
         *
         *  @param aIdleObserver class that wants to unregister
         *
         */
        virtual void UnRegisterObserver( MPrintJobStatus* aObserver ) = 0;
    };

#endif  // MPRINTEVENTREGISTERIF_H

//  End of File
