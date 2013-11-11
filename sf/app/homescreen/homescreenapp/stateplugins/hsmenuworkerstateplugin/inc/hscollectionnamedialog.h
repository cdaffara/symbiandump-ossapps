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

#ifndef HS_INPUT_DIALOG_H
#define HS_INPUT_DIALOG_H

#include <hbinputdialog.h>

#include "hsmenustates_global.h"
HS_STATES_TEST_CLASS(MenuStatesTest)

class HsCollectionNameDialog: public HbInputDialog
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsCollectionNameDialog(const int &itemId = 0);

    virtual ~HsCollectionNameDialog();

    void open(QObject* receiver, const char* member);

    QString uniqueCollectionName() const;

protected:
    void closeEvent( QCloseEvent * event );
private:

    void makeConnect();

    void makeDisconnect();

    QString suggestedCollectionName(const QString &name);

    QString generateUniqueCollectionName(const QString &name) const;

private slots:

    void onTextChanged(const QString &text);

    void onContentsChanged();

private:

    /*
     * Collections names list from data model.
     */
    QStringList mOtherCollectionsNames;

    /*
     * Already suggested names.
     */
    QStringList mSuggestedNames;
};

#endif /* HS_INPUT_DIALOG_H */
