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

#ifndef MTONEPLAYINGWATCHER_H
#define MTONEPLAYINGWATCHER_H

#include <e32def.h>

/*
 * this class is used to watch tone playing event, inherited by ToneFetcherEnginePrivate
 */
class MTonePlayingWatcher
    {
    public:
        // handle preview event
        virtual void HandlePreviewEvent( TInt event ) = 0;
    };

#endif /* MTONEPLAYINGWATCHER_H */
