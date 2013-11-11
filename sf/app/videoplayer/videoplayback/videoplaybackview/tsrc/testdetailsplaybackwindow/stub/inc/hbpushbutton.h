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
* Description:  Implementation of HbPushButton
*
*/

// Version : %version:  2 %



#ifndef HBPUSHBUTTON_H_
#define HBPUSHBUTTON_H

#include <hbwidget.h>
#include <hbstyle.h>

class QGraphicsItem;
class HbFrameDrawer;

class HbPushButton : public HbWidget
{
    Q_OBJECT

    public:
        HbPushButton();
        virtual ~HbPushButton();

    public:
        void setEnabled( bool enabled );
        void setVisible( bool visible );
        void setSelected( bool selected );
        bool isEnabled();
        bool isVisible();
        bool isSelected();
        QGraphicsItem* primitive( HbStyle::Primitive primitive ) const;
        void press();
        void release();
        HbFrameDrawer* frameBackground();
        void setFrameBackground( HbFrameDrawer *drawer );

    signals:
        void pressed();
        void released();

    public:
        bool mVisible;
        bool mEnabled;
        bool mSelected;

        QGraphicsItem *mBackground;
        HbFrameDrawer *mFrameDrawer;
};

#endif /*HBPUSHBUTTON_H*/

