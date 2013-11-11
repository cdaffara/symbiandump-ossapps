/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef DIALERWIDGETENGINE_H_
#define DIALERWIDGETENGINE_H_

#include <QObject>
#include <xqsettingsmanager.h>

class DialerWidgetEngine : public QObject    
    {
    Q_OBJECT
public:
    DialerWidgetEngine();
    virtual ~DialerWidgetEngine();
    bool initialize();
    int missedCallsCount();
    
public slots:
    void valueChanged(const XQSettingsKey &key, const QVariant &value);
signals:
    void exceptionOccured(const int& err);
    void missedCallsCountChanged(const int& missedCallsCount);
private:

private:
    XQSettingsManager * m_settingsManager;
    int m_missedCalls;
    };

#endif /* DIALERWIDGETENGINE_H_ */

