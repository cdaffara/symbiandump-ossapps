/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Menu Dialog
*
*/


#ifndef HSMENUDIALOG_H
#define HSMENUDIALOG_H

#include <QScopedPointer>

class QStringList;
class HbAction;
class HbMessageBox;

#include "hsutils_global.h"

class HSUTILS_EXPORT HsMenuDialogFactory
{
public:
    enum Options { OkCancel, Close, DeleteCancel };

    virtual HbMessageBox *create(const QString &text,
                                 Options options = OkCancel) const;
    virtual ~HsMenuDialogFactory();
    static int acceptActionIndex();
    static int rejectActionIndex();
private:
    void setUpActions(HbMessageBox *box,
                      const QStringList &translationIdentifiers) const;
};

#endif

