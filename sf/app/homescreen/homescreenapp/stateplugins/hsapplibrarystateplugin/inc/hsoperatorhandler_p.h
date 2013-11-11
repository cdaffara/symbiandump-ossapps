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
 * Description: Hs Operator Handler Private.
 *
 */
#ifndef HSOPERATORHANDLER_P_H_
#define HSOPERATORHANDLER_P_H_

#ifdef QT_EXTENSIONS

#include <QObject>
#include <XQSettingsManager>
#include <HbIcon>
#include "hsmenustates_global.h"

class CaEntry;

class HsOperatorHandlerPrivate: public QObject
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    enum HsStoreType {
        NoneType,
        UrlType,
        ApplicationType
    };

    HsOperatorHandlerPrivate(QObject *parent = 0);
    virtual ~HsOperatorHandlerPrivate();
    HbIcon icon();
    QString text();
    void action();
    bool oviStorePresent();
    bool operatorStorePresent();
    bool operatorStoreFirst();

private:
    bool storeValue(XQSettingsManager *crManager, int storePresenceKey);
    QVariant getVariantFromKey(
            XQSettingsManager *crManager,
            int crKey,
            XQSettingsManager::Type type);
    HsStoreType operatorStoreType(XQSettingsManager *crManager);

    void readCentralRepository();
    HbIcon createIcon(XQSettingsManager *crManager);
    QString operatorStoreTitle(XQSettingsManager *crManager);
    CaEntry *createAppEntry(XQSettingsManager *crManager);
    CaEntry *createUrlEntry(XQSettingsManager *crManager);
    bool operatorStoreFirst(XQSettingsManager *crManager);

private:
    bool mOviStorePresent;
    bool mOperatorStorePresent;
    HsStoreType mStoreType;
    CaEntry *mOperatorEntry;
    HbIcon mIcon;
    QString mOperatorStoreTitle;
    bool mOperatorStoreFirst;

};

#endif

#endif /* HSOPERATORHANDLER_P_H_ */

