/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#ifndef __LOWMEMORYHANDLERPRIVATE_H__
#define __LOWMEMORYHANDLERPRIVATE_H__

#include <QObject>

#ifdef Q_OS_SYMBIAN
#include <e32base.h>
#include <e32std.h>
#endif

class LowMemoryHandler;

class LowMemoryHandlerPrivate
#ifdef Q_OS_SYMBIAN
    : public CActive
#endif
{
    Q_DECLARE_PUBLIC(LowMemoryHandler)

public:
    LowMemoryHandlerPrivate();
    virtual ~LowMemoryHandlerPrivate();
    
    void start();
    void stop();

    LowMemoryHandler* q_ptr;

private:
#ifdef Q_OS_SYMBIAN
    void DoCancel();
    void RunL();
    TInt RunError();

    RChangeNotifier iNotifier;
    TBool iInitialized;
#endif
};

#endif
