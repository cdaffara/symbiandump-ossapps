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

#ifndef _HBINSTANCE_H
#define _HBINSTANCE_H

#include <QList>

// Get stubs included before they are searched from /epoc32/include/mw
#include "stub/inc/hbview.h"

#define hbInstance (HbInstance::instance())

class QGraphicsWidget;

class HbInstance
{
public:
    static HbInstance *instance();

    HbInstance* primaryWindow() const;

    QList<HbInstance *> allMainWindows() const;
    
    void removeView( QGraphicsWidget* y );

    void addView( QGraphicsWidget* x );
    
    void setCurrentView( HbView* x , bool animation );
    
private:
    HbInstance();
    ~HbInstance();
};

#endif // STUBHBINSTANCE_H
