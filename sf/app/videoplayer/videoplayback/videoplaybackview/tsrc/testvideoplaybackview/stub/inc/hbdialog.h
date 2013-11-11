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
* Description:  stub hbdialog
*
*/

// Version : %version:   1 %



#ifndef HBDIALOG_H
#define HBDIALOG_H

#include <QObject>

class HbDialog : public QObject
{
    Q_OBJECT

    signals:
        void aboutToClose();
    
    public:
        HbDialog() {}
        virtual ~HbDialog() {}
        void setAttribute( int value )
        {
            Q_UNUSED(value);
        }
        void setDismissPolicy( int value )
        {
            Q_UNUSED(value);
        }
        void setTimeout( int value )
        {
            Q_UNUSED(value);
        }
        void show()
        {
            emit this->aboutToClose();
            delete this;
        }
        
        
};

#endif /*HBDIALOG_H*/

