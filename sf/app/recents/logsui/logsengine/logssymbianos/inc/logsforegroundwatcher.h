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
#ifndef LOGSFOREGROUNDWATCHER_H
#define LOGSFOREGROUNDWATCHER_H


#include <QObject>
#include <coemain.h>

class LogsForegroundWatcher : public QObject, public MCoeForegroundObserver
{
    Q_OBJECT
    
public:

    LogsForegroundWatcher(QObject* parent = 0);
    virtual ~LogsForegroundWatcher();
    
    static void ensureBackgroundStartup();
    
signals:
    void gainingForeground();
    void losingForeground();

public: // from MCoeForegroundObserver
    void HandleGainingForeground();
    void HandleLosingForeground();
    
};

#endif //LOGSFOREGROUNDWATCHER_H
