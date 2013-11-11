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
 * Description: Menu All Applications state.
 *
 */

#ifndef HSICONSIDLELOADER_H_
#define HSICONSIDLELOADER_H_

#include <QObject>
#include <QSize>
#include "hsmenuservice_global.h"

class QTimer;
class HsMenuItemModel;


class HsIconsIdleLoader: public QObject
{
    HS_SERVICE_TEST_FRIEND_CLASS(MenuServiceTest)
    
    Q_OBJECT
public:
    
    HsIconsIdleLoader(HsMenuItemModel *model, QObject *parent = 0);
    ~HsIconsIdleLoader();
    
private slots:
    void processWhenIdle();
    
private:
    HsMenuItemModel *mModel;
    QTimer *mTimer;
    const QSizeF mIconSize;
    int mOutStandingIconToLoad;
};

#endif /* HSICONSIDLELOADER_H_ */
