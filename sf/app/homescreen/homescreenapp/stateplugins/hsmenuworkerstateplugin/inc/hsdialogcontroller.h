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
 
#ifndef HS_DIALOG_CONTROLLER_H
#define HS_DIALOG_CONTROLLER_H

#include <QObject>
#include <QPair>
#include <QMap>

#include "hsmenudialogfactory.h"
#include "hsmenuentryremovedhandler.h"

class QAction;
class QString;
class HbDialog;

class HsDialogController: public QObject
{
    Q_OBJECT
public:
    HsDialogController(
            const QString &message,
            HsMenuDialogFactory::Options =
                 HsMenuDialogFactory::OkCancel);

    HsDialogController(HbDialog *dialog, int acceptActionIndex,
                       int rejectActionIndex);

    ~HsDialogController();

    void openDialog(int entryId = 0);


signals:
    void acceptActionTriggered(QAction* action);
    void rejectActionTriggered(QAction* action);
    void dialogCompleted();

public slots:
    void dismissDialog();

private slots:
    void onDialogFinished(HbAction* action);
private:
    HbDialog *mDialog;

    const int mAcceptActionIndex;
    const int mRejectActionIndex;
};

#endif
