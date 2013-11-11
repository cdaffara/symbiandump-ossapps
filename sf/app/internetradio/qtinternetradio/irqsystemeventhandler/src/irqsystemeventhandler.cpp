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
*/

#include "irqsystemeventhandler.h"
#include "irqsystemeventhandler_p.h"

IRQSystemEventHandler::IRQSystemEventHandler():d_ptr(new IRQSystemEventHandlerPrivate(this))
{   
    d_ptr->init(); 
}

IRQSystemEventHandler::~IRQSystemEventHandler()
{
    delete d_ptr;
}

qint64 IRQSystemEventHandler::diskCriticalLevel() const
{
    return d_ptr->diskCriticalLevel();
}

void IRQSystemEventHandler::cancel()
{
    d_ptr->cancel();
}

void IRQSystemEventHandler::start()
{
    d_ptr->start();
}

bool IRQSystemEventHandler::isBelowCriticalLevel(const qint64 aCriticalLevel)
{
    return d_ptr->isBelowCriticalLevel(aCriticalLevel);
}
 


