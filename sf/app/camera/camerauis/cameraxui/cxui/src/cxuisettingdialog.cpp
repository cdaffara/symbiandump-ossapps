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

#include <HbAction>
#include "cxutils.h"
#include "cxuisettingdialog.h"

CxuiSettingDialog::CxuiSettingDialog(QGraphicsItem *parent)
    : HbDialog(parent),
      mAction(NULL)
{
}

CxuiSettingDialog::~CxuiSettingDialog()
{
}

void CxuiSettingDialog::setStarterAction(HbAction *action)
{
    mAction = action;
}

void CxuiSettingDialog::showEvent(QShowEvent *event)
{
    emit dialogAboutToShow(mAction);
    HbDialog::showEvent(event);
}


void CxuiSettingDialog::closeEvent(QCloseEvent *event)
{
    HbDialog::closeEvent(event);
    emit dialogAboutToClose(mAction);
}

// end of file
