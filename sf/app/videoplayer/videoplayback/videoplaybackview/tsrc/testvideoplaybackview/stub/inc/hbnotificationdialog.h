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
* Description:  stub hbnotificationdialog
*
*/

// Version : %version:   1 %



#ifndef HBNOTIFICATIONDIALOG_H
#define HBNOTIFICATIONDIALOG_H

#include "hbdialog.h"

class HbNotificationDialog : public HbDialog
{
    Q_OBJECT

    public:
        HbNotificationDialog(){mNotifConstructCount++;}
        virtual ~HbNotificationDialog(){};
        void setTitle(const QString &title)
        {
            Q_UNUSED(title);
        }

   static int mNotifConstructCount;

};


#endif /*HBNOTIFICATIONDIALOG_H*/

