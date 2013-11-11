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
* Description:   hbInstance stub class definition
*
*/

// Version : %version:  da1mmcf#8 %

#ifndef _HBINSTANCE_H
#define _HBINSTANCE_H

#include <QList>
#include <hbmainwindow.h>

// Get stubs included before they are searched from /epoc32/include/mw
#include "stub/inc/mpxviewpluginqt.h"
#include "stub/inc/xqpluginloader.h"
#include "stub/inc/xqplugininfo.h"
#include "stub/inc/hbview.h"

#define hbInstance (HbInstance::instance())

class QGraphicsWidget;

class HbInstance
{
public:
    static HbInstance *instance();

    QList<HbMainWindow *> allMainWindows() const;
    
    void removeView( QGraphicsWidget* y );

    void addView( QGraphicsWidget* x );
    
    void setCurrentView( HbView* x , bool animation );
    
private:
    HbInstance();
    ~HbInstance();
    
public:
    QList<HbMainWindow*>  mWindows;
};

#endif // STUBHBINSTANCE_H
