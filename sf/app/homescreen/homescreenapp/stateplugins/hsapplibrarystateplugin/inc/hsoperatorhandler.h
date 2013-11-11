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
 * Description: Hs Operator Handler.
 *
 */

#ifndef HSOPERATORHANDLER_H_
#define HSOPERATORHANDLER_H_

#include <QObject>
#include "hbicon.h"
#include "hsmenustates_global.h"

class HsOperatorHandlerPrivate;
class HbAction;

class HsOperatorHandler: public QObject
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:
    HsOperatorHandler(QObject *parent = 0);
    virtual ~HsOperatorHandler();
    bool operatorStorePresent();
    bool oviStorePresent();
    bool operatorStoreFirst();
    HbAction *prepareOperatorStoreAction(
            HbAction *const operatorAction);
    QString text();

private:
    HbIcon icon();

private slots:
    void action();

private:
    HsOperatorHandlerPrivate *const m_q;
};

#endif /* HSOPERATORHANDLER_H_ */
