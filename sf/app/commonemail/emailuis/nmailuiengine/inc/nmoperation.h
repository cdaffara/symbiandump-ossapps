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

#ifndef NMOPERATION_H_
#define NMOPERATION_H_

#include <QObject>
#include "nmuienginedef.h"


class NMUIENGINE_EXPORT NmOperation: public QObject
{
    Q_OBJECT

public:
    NmOperation();  
    bool isRunning() const;
    void addPreliminaryOperation(NmOperation *operation);
    
signals:
    void operationProgressChanged(int progress);
    void operationCompleted(int result = 0);
    void operationCancelled();

public slots:
    void completeOperation(int result);
    void cancelOperation();
    void updateOperationProgress(int progress);

protected slots:
    virtual void runAsyncOperation();
    virtual void handlePreliminaryOperationFinished();

private slots:
    void deleteOperation();
    
protected:
    virtual ~NmOperation();  
    virtual void doCompleteOperation();
    virtual void doCancelOperation();
    virtual void doUpdateOperationProgress();
    virtual void doRunAsyncOperation() = 0;

    
private:
    int mProgress;
    bool mIsRunning;
    QList<NmOperation *> mPreliminaryOperations;
};

#endif /* NMOPERATION_H_ */
