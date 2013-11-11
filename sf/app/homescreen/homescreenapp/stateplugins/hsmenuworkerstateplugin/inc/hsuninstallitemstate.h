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

#ifndef HSUNINSTALLITEMSTATE_H
#define HSUNINSTALLITEMSTATE_H

#include <QState>
#include <QSharedPointer>

#include "hsmenustates_global.h"
HS_STATES_TEST_CLASS(MenuStatesTest)

class QAction;
class HbDocumentLoader;
class HsDialogController;
class HbAction;
class HbMessageBox;
class HsShortcutService;
class HsMenuService;
class HbDialog;
class CaNotifier;
class CaEntry;

class HsUninstallItemState: public QState
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsUninstallItemState(QState *parent = 0);

    virtual ~HsUninstallItemState();

protected:

    void onEntry(QEvent *event);

private slots:

    void uninstallItem();

    void cleanUp();

public slots:

    void uninstallFailed(int error);

signals:

    void exit();

private:

    void construct();

    bool getApplicationsNames(QString &componentName,
        QStringList &applicationsNames,
        QString &confirmationMessage);

    void createSimpleUninstallMessage(bool isJava=false);
    void createUninstallJavaMessage();
    HsDialogController *prepareDialog(HbDialog *dialog);
    QString loadProperSection(QStringList &applicationsNames,
            QString &detailsMessage,
            HbDocumentLoader &loader);
    void prepareApplicationListLabel( QStringList & applicationsNames,
            HbDocumentLoader & loader);
    void prepareDialogLabel(
            QString &componentName, HbDocumentLoader &loader);

private:

    QSharedPointer<CaEntry> mEntry;

};

#endif //HSUNINSTALLITEMSTATE_H
