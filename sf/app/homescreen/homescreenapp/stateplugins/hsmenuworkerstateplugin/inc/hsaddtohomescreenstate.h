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
* Description:  Menu Application Library state.
 *
*/


#ifndef HSADDSHORTCUTSTATE_H
#define HSADDSHORTCUTSTATE_H

#include <QScopedPointer>
#include <QState>
#include <QVariant>

#include "hsmenustates_global.h"
#include "hsapp_defs.h"

HS_STATES_TEST_CLASS(MenuStatesTest)

class QAction;
class HbAction;
class HbMessageBox;
class CaEntry;
class CaNotifier;
class HsContentService;
class HsDialogController;

class HsAddToHomeScreenState: public  QState
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsAddToHomeScreenState(QState *parent = 0);

    virtual ~HsAddToHomeScreenState();

private slots:

    void onEntry(QEvent *event);

    void cleanUp();

    void openHomeScreen();

    void removeWidget();

signals:

    void exit();

private:

    void showMessageWidgetCorrupted();

    bool addWidget(HsContentService &service, const QString &uri);

    bool addShortcut(HsContentService &contentService);

    bool addApplication(HsContentService &contentService, CaEntry &entryId);

    void logActionResult(QString operationName, int entryId,
                         bool operationSucceded);

private:

    // keep path in memory so it wont
    QString mLibraryPath;

    int mEntryId;
    
    Hs::HsMenuMode mMenuMode;
    
    QVariant mToken;

};

#endif //HSADDSHORTCUTSTATE_H
