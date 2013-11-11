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
* Description:  stub hbmessagebox
*
*/

// Version : %version:   1 %



#ifndef HBMESSAGEBOX_H
#define HBMESSAGEBOX_H

#include "hbdialog.h"

class HbMessageBox : public HbDialog
{
    Q_OBJECT
    
    public:
    
    enum MessageBoxType {
        MessageTypeWarning
    };
    enum StandardButton {
        NoButton           = 0x00000000,
    };
    
    HbMessageBox(const QString &txt, int type)
    {
        Q_UNUSED(txt);
        Q_UNUSED(type);
        mMessageBConstructCount++;
    }
    virtual ~HbMessageBox()
    {
        
    }
    void setStandardButtons(int value)
    {
     Q_UNUSED(value);
    }
    
    static int mMessageBConstructCount;
};

#endif /*HBMESSAGEBOX_H*/

