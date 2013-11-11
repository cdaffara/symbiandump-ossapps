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
#ifndef SEARCHTESTACTIVITY_H_
#define SEARCHTESTACTIVITY_H_

#include <hbwidget.h>
#include <qgraphicslinearlayout.h>
#include <hbpushbutton.h>
#include <hblabel.h>
#include <hbLineedit.h>
class HbCheckBox;
class SearchTestActivity : public HbWidget
    {
Q_OBJECT
public:
    SearchTestActivity(QGraphicsItem *parent = 0);
    ~SearchTestActivity();

private:

    QGraphicsLinearLayout *mColumn1Layout;
    QGraphicsLinearLayout *mColumn2Layout;

    QGraphicsLinearLayout *mCheckBoxLayout;
    QGraphicsLinearLayout *mButtonLayout;

    QGraphicsLinearLayout *mMainLayout;

    HbLineEdit *mLineEdit;
    HbPushButton *mIndevice;
    HbPushButton *mOnline;

    QObject *mActivityManager;
    QList<HbCheckBox*> mselectioncheckbox;

private slots:
    void launchDeviceApp();
    void launchInternetApp();

    };

#endif /* SEARCHTESTACTIVITY_H_*/
