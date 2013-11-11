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

#ifndef  CXESETTINGSSTOREDESKTOP_H_
#define  CXESETTINGSSTOREDESKTOP_H_

//  Include Files
#include <QHash>
#include <QVariant>
#include <QMetaType>
#include "cxesettingsstore.h"
#include "cxeerror.h"

/*!
* CxeSettingsStoreDesktop class implements CxeSettingsStore.
*/
class CxeSettingsStoreDesktop : public QObject,
                                public CxeSettingsStore
{
    Q_OBJECT

public:

    CxeSettingsStoreDesktop();
    ~CxeSettingsStoreDesktop();

public: // from base class
    void reset();
    CxeError::Id get(const QString &key, QVariant &value);
    void startMonitoring(long int uid, unsigned long int key, Cxe::SettingKeyType type, QVariant &value);
    CxeError::Id set(const QString &key,const QVariant newValue);

    QHash<QString, QVariantList> loadVariationSettings(QList<QString> &settingKeys);

private:
    void initSettings();

private:
    QHash<QString, QVariant> mSettings;

};

#endif  // CXESETTINGSSTOREDESKTOP_H_

