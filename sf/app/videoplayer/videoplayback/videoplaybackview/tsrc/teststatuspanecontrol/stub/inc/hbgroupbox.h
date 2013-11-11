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
* Description:  Stub Implementation of HbGroupBox
*
*/

// Version : %version:   1 %



#ifndef HBGROUPBOX_H_
#define HBGROUPBOX_H_

#include <hbwidget.h>


class HbGroupBox : public HbWidget
{
    Q_OBJECT

    public:
        HbGroupBox();
        virtual ~HbGroupBox();

    public:
        void setVisible( bool visible );       
        bool isVisible();
        void setHeading( QString string );

    public:
        bool mVisible;
        QString mString;
};

#endif /*HBGROUPBOX_H_*/

