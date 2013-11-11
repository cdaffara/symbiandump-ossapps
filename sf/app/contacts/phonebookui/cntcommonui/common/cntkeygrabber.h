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

#ifndef CNTKEYGRABBER_H
#define CNTKEYGRABBER_H

#include <QObject>

#include "cntglobal.h"

class HbMainWindow;
class XQAiwRequest;
class XQKeyCapture;

class QTPBK_EXPORT CntKeyGrabber : public QObject
{
    Q_OBJECT
    friend class TestCntKeyGrabber;

public:
    CntKeyGrabber(HbMainWindow *window, QObject *parent = NULL);
    ~CntKeyGrabber();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    
private:
    HbMainWindow    *mMainWindow;
    XQAiwRequest    *mRequest;
    XQKeyCapture    *mKeyCapture;

};

#endif // CNTKEYGRABBER_H
