/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Clock widget
*
*/

#ifndef HSCLOCKWIDGETTYPE_H
#define HSCLOCKWIDGETTYPE_H

#include <QObject>
#include <QString>
#include <hstest_global.h>

HOMESCREEN_TEST_CLASS(TestClockWidget)

class CEnvironmentChangeNotifier;
class HsClockSettingsNotifier : public QObject
{
    Q_OBJECT
    
public:

    HsClockSettingsNotifier(QObject *parent = 0);
    ~HsClockSettingsNotifier();

    QString clockFormat() const;
    QString timeFormat() const;
    
    static TInt EnvironmentChanged( TAny *aObj );
    
signals:
    void settingsChanged(QString clockFormat, QString timeFormat);
        
private: 
    void createObserver();
    QString clockFormatString()const;
    QString timeFormatString()const;
    
private slots:
    void onSettingsChanged();

private:
    CEnvironmentChangeNotifier *mDateTimeNotifier;
    
    QString mClockFormat;
    QString mTimeFormat;
    
    Q_DISABLE_COPY(HsClockSettingsNotifier)

    HOMESCREEN_TEST_FRIEND_CLASS(TestClockWidget)

};

#endif
