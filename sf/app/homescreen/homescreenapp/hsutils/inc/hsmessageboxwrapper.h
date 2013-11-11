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

#ifndef HSMESSAGEBOXWRAPPER_H
#define HSMESSAGEBOXWRAPPER_H

#include <HbWidget>

#include "hsutils_global.h"

#include "hstest_global.h"
HOMESCREEN_TEST_CLASS(t_hsUtils)

class HbAction;
class HbMessageBox;
struct HsMessageBoxWrapperImpl;

class HSUTILS_EXPORT HsMessageBoxWrapper : public QObject
{
    Q_OBJECT

public:
    HsMessageBoxWrapper(QObject *parent=0);
    ~HsMessageBoxWrapper();

    void setHeader(const QString &header);
    void setQueryText(const QString &queryText);
    void close();

signals:
    void accepted();
    void rejected();
public slots:
    void show();

private:
    Q_DISABLE_COPY(HsMessageBoxWrapper)

private slots:
    void onDialogClosed(int action);
 
private:
    HsMessageBoxWrapperImpl *mImpl;
    HbMessageBox *mBox;

    HOMESCREEN_TEST_FRIEND_CLASS(t_hsUtils)
};

#endif // HSPAGEINDICATOR_H
