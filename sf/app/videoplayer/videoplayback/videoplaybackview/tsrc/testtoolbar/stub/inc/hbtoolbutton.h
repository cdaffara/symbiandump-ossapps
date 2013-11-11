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
* Description:  Implementation of HbToolButton
*
*/

// Version : %version:   1 %



#ifndef HbTOOLBUTTON_H_
#define HbTOOLBUTTON_H_

#include <hbwidget.h>

class HbAction;

class HbToolButton : public HbWidget
{
    Q_OBJECT

    public:
        HbToolButton( HbAction *action );
        virtual ~HbToolButton();

    public:
        void press();
        void release();
        bool isEnabled();

    signals:
        void pressed();
        void released();

    private:
        bool mEnabled;
        HbAction *mAction;
};

#endif /*HbTOOLBUTTON_H_*/

