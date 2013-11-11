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
 * 
 * Description:
 *     The header file of filemanager messagebox
 */

#ifndef FMMESSAGEBOX_H
#define FMMESSAGEBOX_H

#include <QEventLoop>
#include <hbmessagebox.h>
#include <hbaction.h>

class FmMessageBox : public QObject
{
Q_OBJECT
public:
    FmMessageBox()
    {
    }
    ~FmMessageBox()
    {
    }
    void information( const QString &informationText,
        HbMessageBox::StandardButtons buttons, bool isBlocking = false );

    bool question( const QString &questionText,
        HbMessageBox::StandardButtons buttons );
    
    void warning( const QString &warningText,
        HbMessageBox::StandardButtons buttons, bool isBlocking = false );
    
public slots:
    void dialogClosed(HbAction *action);
private:
    QEventLoop mEventLoop;
    bool mRet;
};

#endif
