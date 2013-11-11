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

#ifndef T_HSSHORTCUTSERVICE_H
#define T_HSSHORTCUTSERVICE_H

#include <QObject>
#include <QIcon>
#include <QSharedPointer>
#include <QMetaType>

#include "hsdomainmodel_global.h"

class QStateMachine;

class HsShortcutService : public QObject
{
    Q_OBJECT

public:
    ~HsShortcutService();
    static QSharedPointer<HsShortcutService> instance(QStateMachine *machine = 0);

    void executeCollectionAction(int shortcutId, QString collectionType);
    bool isItemShortcutWidget(int aItemId);

private:
    HsShortcutService(QStateMachine *aStateMachine, QObject *parent = 0);

    Q_DISABLE_COPY(HsShortcutService)


private:
};

Q_DECLARE_METATYPE(HsShortcutService *)

#endif // T_HSSHORTCUTSERVICE_H
