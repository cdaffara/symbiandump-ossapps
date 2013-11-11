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


#ifndef MIDLEPRINTEVENTREGISTERIF_H
#define MIDLEPRINTEVENTREGISTERIF_H

#include <e32def.h>

class MIdlePrintEventObserver;

/**
 *
 * Interface to use the idle print event functionality of Image Print Engine
 *
 */
class MIdlePrintEventRegisterIF
    {	
	public:		// Abstract methods

        
        /**
         *  Registers class to IdlePrintEventObserver. After registration
         *  the class receives idle events
         *  
         *  @param aIdleObserver class that wants to register
         *
         */
        virtual void RegisterObserver( MIdlePrintEventObserver* aIdleObserver ) = 0;
        
        /**
         *  Unregisters class from IdlePrintEventObserver. After unregistration
         *  the class stops receiving idle events. Class is also deleted from observer
         *  array.
         *  
         *  @param aIdleObserver class that wants to unregister
         *
         */        
        virtual void UnRegisterObserver( MIdlePrintEventObserver* aIdleObserver ) = 0;
    };

#endif // MIDLEPRINTEVENTREGISTERIF_H

//  End of File
