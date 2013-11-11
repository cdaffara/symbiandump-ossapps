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
 *     
 */

#ifndef MTONESELECTIONWATCHER_H
#define MTONESELECTIONWATCHER_H

#include <e32def.h>
#include <e32cmn.h> 

/*
 * this class is used to watch MDE system change, inherited by ToneFetcherEnginePrivate
 */
class MToneSelectionWatcher
    {
    public:
        // handle mde session error event
        virtual void HandleMdeSessionError( TInt aError ) = 0;
        
        // handle mde session open event
        virtual void HandleMdeSessionOpened() = 0;
        
        // handle query error event        
        virtual void HandleQueryError( TInt aError ) = 0;
        
        // handle query complete event
        virtual void HandleQueryComplete( RPointerArray<TDesC>& ) = 0;
        
        // handle object changed event
        virtual void HandleObjectChanged() = 0;
    };

#endif /* MTONESELECTIONWATCHER_H */
