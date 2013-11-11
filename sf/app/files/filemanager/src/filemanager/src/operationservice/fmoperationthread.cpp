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
*     The source file of the operation thread of file manager
*
*/

#include "fmoperationthread.h"
#include "fmbackuprestorehandler.h"

#include <QDir>
#include <QStack>

/*
 * Constructs the operation thread with \a parent.
 */
FmOperationThread::FmOperationThread( QObject *parent ) : QThread( parent ),
                mStop( false), mOperationBase( 0 )
{
    setPriority( LowestPriority );
}

/*
 * Destructs the operation thread.
 */
FmOperationThread::~FmOperationThread()
{
}

/*
 * Prepare some conditions before starts the operation.
 * Returns the error id.
 * \a operationBase the operation to be prepared.
 */
int FmOperationThread::prepareOperationAndStart( FmOperationBase* operationBase )
{    
    if ( isRunning() ) {
         return FmErrAlreadyStarted;
    }
    mOperationBase = operationBase;
    int ret = mOperationBase->prepare();
    if ( ret == FmErrNone ) {
        mStop = false;
        start();    
    } 
    return ret; 
}

/* Stops the current thread.
 * Caused by user interaction.
 */
void FmOperationThread::stop()
{
    mStop = true;
}

/*
 * reimp
 */
void FmOperationThread::run()
{    
    mOperationBase->start( &mStop );
}

