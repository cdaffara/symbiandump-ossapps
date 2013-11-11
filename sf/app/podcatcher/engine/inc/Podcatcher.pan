/*
* Copyright (c) 2010 Sebastian Brannstrom
*
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* EmbedDev AB - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#ifndef __PODCATCHER_PAN__
#define __PODCATCHER_PAN__

enum TPodcatcherPanics
    {
    EPodcatcherPanicDB = 1,
    EPodcatcherPanicAlreadyActive,
    EPodcatcherPanicFeedEngineState,
    EPodcatcherPanicFeedView
    };


inline void Panic(TPodcatcherPanics aReason)
    {
    _LIT(applicationName,"Podcatcher");
    User::Panic(applicationName, aReason);
    }

#endif
