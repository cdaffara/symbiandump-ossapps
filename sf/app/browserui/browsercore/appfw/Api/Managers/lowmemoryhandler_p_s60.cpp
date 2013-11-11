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


#include "lowmemoryhandler_p.h"
#include "lowmemoryhandler.h"

LowMemoryHandlerPrivate::LowMemoryHandlerPrivate()
    : CActive(EPriorityStandard)
    , iInitialized(EFalse)
{
    CActiveScheduler::Add(this);
    TInt err = iNotifier.Create();
    // TODO: handle err
}

LowMemoryHandlerPrivate::~LowMemoryHandlerPrivate()
{
    Cancel();
    iNotifier.Close();
}

void LowMemoryHandlerPrivate::start()
{
    if (IsActive())
        return;
		
    TInt err = iNotifier.Logon(iStatus);
    // TODO: handle err
    SetActive();
}
    
void LowMemoryHandlerPrivate::stop()
{
    Cancel();
}

void LowMemoryHandlerPrivate::DoCancel()
{
    iNotifier.LogonCancel();
}

void LowMemoryHandlerPrivate::RunL()
{
    if (iInitialized == EFalse) {
        iInitialized = ETrue;
    } else {
        TInt changes = iStatus.Int();

        Q_Q(LowMemoryHandler);

        if (changes & EChangesOutOfMemory) {
            emit(q->outOfMemory());
        } else if (changes & EChangesLowMemory) {
            emit(q->lowMemory());
        }
    }
        
    User::LeaveIfError(iNotifier.Logon(iStatus));
    SetActive();	
}

TInt LowMemoryHandlerPrivate::RunError()
{
    iNotifier.Close();
    return KErrNone;
}
