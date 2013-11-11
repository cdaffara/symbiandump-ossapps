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
*     The header file of the operation thread of file manager
*
*/

#ifndef FMOPERATIONTHREAD_H
#define FMOPERATIONTHREAD_H

#include "fmoperationbase.h"
#include "fmcommon.h"

#include <QString>
#include <QStringList>

#include <QThread>

class FmOperationThread : public QThread
{
    Q_OBJECT
public:
    FmOperationThread( QObject *parent );
    ~FmOperationThread();
    int prepareOperationAndStart( FmOperationBase* operationBase );    
    void stop();    

protected:
    void run();

private:
    volatile bool mStop;
    FmOperationBase* mOperationBase;
    QString mErrString;
};

#endif
