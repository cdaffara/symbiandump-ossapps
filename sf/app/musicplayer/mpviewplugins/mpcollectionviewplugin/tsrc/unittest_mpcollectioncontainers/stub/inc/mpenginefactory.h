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
* Description: MpEngineFactory stub for testing mpplaybackview
*
*/

#ifndef MPENGINEFACTORY_H
#define MPENGINEFACTORY_H

#include <hbglobal.h>
#include "mpengine.h"

class MpPlaybackData;

class MpEngineFactory
{
    
public:
    // Stub functions
private:
    explicit MpEngineFactory();

public:
    virtual ~MpEngineFactory();
    static MpEngineFactory * instance(); 
    static MpEngine * sharedEngine();
    
private:
    Q_DISABLE_COPY( MpEngineFactory )
    
    MpEngine *mSharedEngine;
  
};


#endif // MPENGINEFACTORY_H
