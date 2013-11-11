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
* Description: FM Radio widget profile reader
*
*/

#ifndef RADIOHSWIDGETPROFILEREADER_H_
#define RADIOHSWIDGETPROFILEREADER_H_

// System includes
#include <QObject>
#include <QVariant>

// Forward declarations
class RadioHsWidget;
class XQSettingsManager;
class XQSettingsKey;

class RadioHsWidgetProfileReader : public QObject
{
Q_OBJECT

public:
    explicit RadioHsWidgetProfileReader(RadioHsWidget *parent = 0);
    virtual ~RadioHsWidgetProfileReader();

    void startMonitoringRadioRunningStatus();
    
    bool isInOfflineMode();

public slots:
    void handleDeletedItem(const XQSettingsKey &key);
    void handleChanges(const XQSettingsKey &key, const QVariant &value);
    
private:
    void radioRunningStatus(const QVariant &value);
    
private: // data
    /*!
       Stores the reference to parent. This is used to call parents
       functions when notifications are received.
     */
    RadioHsWidget &mParent;
    /*! For getting values and notifications of changes from P&S keys. */
    XQSettingsManager *mSettingsManager;
    /*!
       Stores internally the status of radio application read from the P&S
       key.
      */
    int mRadioStatus;
};

#endif /* RADIOHSWIDGETPROFILEREADER_H_ */
