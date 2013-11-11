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
* Description:  Implementation of HbToolBar
*
*/

// Version : %version:  1 %



#ifndef HBTOOLBAR_H_
#define HBTOOLBAR_H_

#include <hbwidget.h>

class HbAction;
class QGraphicsLayout;
class QGraphicsLinearLayout;

class HbToolBar : public HbWidget
{
    Q_OBJECT

    public:
        HbToolBar();
        virtual ~HbToolBar();

    public:
        void setVisible( bool visible );
        void setEnabled( bool enabled );
        void setOrientation( int orientation );
        void clearActions();
        void addAction( HbAction *action );
        QGraphicsLayout* layout();

    public:
        bool mVisible;
        bool mEnabled;
        QGraphicsLinearLayout *mLayout;
};

#endif /*HBTOOLBAR_H_*/

