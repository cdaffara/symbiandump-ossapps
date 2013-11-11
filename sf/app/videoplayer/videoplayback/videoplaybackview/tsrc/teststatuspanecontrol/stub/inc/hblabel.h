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
* Description:  Stub Implementation of HbLabel
*
*/

// Version : %version:  2 %



#ifndef HBLABEL_H_
#define HBLABEL_H_

#include <hbwidget.h>


class HbLabel : public HbWidget
{
    Q_OBJECT

    public:
        HbLabel();
        virtual ~HbLabel();

    public:
        void setVisible( bool visible );       
        bool isVisible();
        void setPlainText( QString string );

    public:
        bool mVisible;
        QString mString;
};

#endif /*HBLABEL_H_*/

