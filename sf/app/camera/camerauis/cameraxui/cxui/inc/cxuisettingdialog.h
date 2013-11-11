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
#ifndef CXUISETTINGDIALOG_H
#define CXUISETTINGDIALOG_H

#include <QObject>
#include <HbDialog>

class HbAction;

class CxuiSettingDialog : public HbDialog
{
    Q_OBJECT
public:
    explicit CxuiSettingDialog(QGraphicsItem *parent = NULL);
    ~CxuiSettingDialog();

public:
    void setStarterAction(HbAction *action);

signals:
    void dialogAboutToShow(HbAction *action);
    void dialogAboutToClose(HbAction *action);

protected:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    HbAction *mAction;
};


#endif // CXUISETTINGDIALOG_H
