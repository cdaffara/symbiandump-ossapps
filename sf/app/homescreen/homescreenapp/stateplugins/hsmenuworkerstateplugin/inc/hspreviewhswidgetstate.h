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


#ifndef HSPREVIEWHSWIDGETSTATE_H
#define HSPREVIEWHSWIDGETSTATE_H

#include <QState>
#include <QVariant>

#include "hsmenustates_global.h"
#include "hsmenuentryremovedhandler.h"

HS_STATES_TEST_CLASS(MenuStatesTest)

class QAction;
class HbAction;
class CaEntry;
class HbDialog;
class HsWidgetHost;
class CaNotifier;
class HbScrollArea;
class HbMessageBox;

class HsPreviewHSWidgetState : public QState
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsPreviewHSWidgetState(QState *parent = 0);

    virtual ~HsPreviewHSWidgetState();

private slots:

    void addToHomeScreen();

    void removeWidget();
    
    void onEntry(QEvent *event);

    void cleanUp();

signals:
    void exit();

private:
    void showMessageWidgetCorrupted();
    HbDialog* buildPreviewDialog(const CaEntry& entry) const;

private:

    int mEntryId;
    
    QVariant mToken;
    
    QString mUri;
};

#endif //HSPREVIEWHSWIDGETSTATE_H
