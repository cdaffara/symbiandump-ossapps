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

#include "nmuiengineheaders.h"

/*!
    \brief Constructor
    Constructor of NmOperation
 */
NmOperation::NmOperation()
: mProgress(0),
  mIsRunning(true)
{
    NM_FUNCTION;
    
    QMetaObject::invokeMethod(this, "runAsyncOperation", Qt::QueuedConnection);
}

/*!
    \brief Destructor
 */
NmOperation::~NmOperation()
{
    NM_FUNCTION;
    
    while (!mPreliminaryOperations.isEmpty()) {
        QPointer<NmOperation> operation = mPreliminaryOperations.takeLast();
        if (operation && operation->isRunning()) {
            operation->cancelOperation();
        }
    }
}

/*!
    \brief Tells whether the operation is running or not
 */
bool NmOperation::isRunning() const
{
    NM_FUNCTION;
    
    return mIsRunning;
}

/*!
    \brief Adds a "preliminary operation" which needs to end until this operation can start.
 */
void NmOperation::addPreliminaryOperation(NmOperation *operation)
{
    NM_FUNCTION;
    
    // if the preliminary operation is already completed
    // the input parameter can be null
    if (operation && operation->isRunning()) {
        connect(operation, SIGNAL(operationCompleted()), this,
            SLOT(handlePreliminaryOperationFinished()));
    
        connect(operation, SIGNAL(operationCancelled()), this,
            SLOT(handlePreliminaryOperationFinished()));
    
        mPreliminaryOperations.append(operation);
    }
}

/*!
    \brief Slot, complete
    The performer of the asynchronous function call should use this slot when
    the operation is completed, this will emit the actionCompleted signal * 
    \param result Result from operation
 */
void NmOperation::completeOperation(int result)
{
    NM_FUNCTION;
    
    mIsRunning = false;
    // Operation is completed, emit the signal
    doCompleteOperation();
    emit this->operationCompleted(result);
    QMetaObject::invokeMethod(this, "deleteOperation", Qt::QueuedConnection);
}

/*!
    \brief Slot, cancel
    The observer of the asynchronous function call should use this slot if it
    wants to cancel the ongoing operation, it will emit the actionCancelled signal
 */
void NmOperation::cancelOperation()
{
    NM_FUNCTION;
    
    mIsRunning = false;
    // Operation is canceled, emit the signal
    this->doCancelOperation();
    emit this->operationCancelled();
    QMetaObject::invokeMethod(this, "deleteOperation", Qt::QueuedConnection);
}
/*!
    \brief Slot, update progress
    The performer of the asynchronous function call should use this slot when
    updating the operation progress, this will emit the progressChanged signal
 */
void NmOperation::updateOperationProgress(int progress)
{
    NM_FUNCTION;
    
    mProgress = progress;
    this->doUpdateOperationProgress();
    emit this->operationProgressChanged(mProgress);
}

/*!
    \brief Slot, run the operation if no preliminary operations are running.
    Calls the pure virtual doRunAsyncOperation() of the derived class if there are no preliminary
    operations running. Operation will be deleted after it has compeleted or cancelled and the control
    returns to the event loop.
 */
void NmOperation::runAsyncOperation()
{
    NM_FUNCTION;
    
    int count = mPreliminaryOperations.count();
    int ready = 0;
    // go through preliminary operations
    for (int i = 0; i < count; ++i) {
        if (!mPreliminaryOperations[i] || !mPreliminaryOperations[i]->isRunning()) {
            ready++;
        }
    }
    // when all preliminary operations are either completed or cancelled
    // start this one
    if (count == ready) {
        this->doRunAsyncOperation();
    }
}

/*!
    \brief Slot, update progress
    This is signalled by a preliminary operation when its operation is completed or cancelled. 
    Do not call runAsyncOperation immediately but let the signal be handled by other slots first. 
 */
void NmOperation::handlePreliminaryOperationFinished()
{
    NM_FUNCTION;
    
    QMetaObject::invokeMethod(this, "runAsyncOperation", Qt::QueuedConnection);
}

/*!
    \brief Virtual function to be implemented by subclasses
    This function is called from the completeAction before the signal is
    emitted. Derived class can override this is some special actions
    are needed.
 */ 
void NmOperation::doCompleteOperation()
{
    NM_FUNCTION;
}

/*!
    \brief Virtual function to be implemented by subclasses
    This function is called from the cancelAction after the trigger timer
    and before the signal is emitted. Derived class can override this
    and put its own cancellation operations here.
 */
void NmOperation::doCancelOperation()
{
    NM_FUNCTION;
}

/*!
    \brief Virtual function to be implemented by subclasses
    This function is called from the updateProgress after the progress
    value has been updated and before the signal is emitted. Derived class
    can override this.
 */
void NmOperation::doUpdateOperationProgress()
{
    NM_FUNCTION;
}

/*!
    \brief Private slot for operation deleting
    This is signalled when operation is completed or cancelled. 
    Delete happens when the control returns to the event loop.
 */
void NmOperation::deleteOperation()
{
    NM_FUNCTION;
    
    this->deleteLater();   
}

