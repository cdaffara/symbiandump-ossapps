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
* Description: Internet Radio App Insecptor
*
*/

#ifndef IRAPPINSPECTOR_H_
#define IRAPPINSPECTOR_H_

// System includes
#include <QObject>
#include <xqsettingskey.h>

// User includes

// Forward declarations
class XQSettingsManager;
class QVariant;

Q_ENUMS(IrServiceCmd::IrRunningStatus)
    
class IrAppInspector : public QObject
{
    Q_OBJECT

public:
    explicit IrAppInspector(QObject *aParent = 0);
    virtual ~IrAppInspector();

    bool isIrRunning();

    bool startInspectingIrRunningStatus();
    void stopInspectingIrRunningStatus();

    enum IrRunningStatus
    {
        StartingUp = 0,
        Exiting    = 1
    };
       
signals:
    void irRunningStatusChanged(IrAppInspector::IrRunningStatus aNewStatus);

private slots:
    void handleIrExit(const XQSettingsKey &aKey);
    void handleIrRunningStatusChanged(const XQSettingsKey &aKey, const QVariant &aValue);

private:
    XQSettingsManager *mSettingsManager; // For getting and P&S notifications
    XQSettingsKey      mIrRunningStatusKey;
    
    Q_DISABLE_COPY(IrAppInspector)    
};

#endif /* IRAPPINSPECTOR_H_ */
