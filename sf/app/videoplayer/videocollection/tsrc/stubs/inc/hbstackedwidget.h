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
* Description:  stub HbStackedWidget for testing video list selection dialog
* 
*/

#ifndef HBSTACKEDWIDGET
#define HBSTACKEDWIDGET

#include <QGraphicsWidget>
#include <qpointer.h>
#include "hbwidget.h"

class HbStackedWidget : public HbWidget
{
    Q_OBJECT
public:
    /**
     * constructor
     */
    HbStackedWidget(QGraphicsItem *parent = 0) : HbWidget(parent), mCurrentWidget(0){initializeCount++;}
    
    /**
     * destructor
     */
    ~HbStackedWidget(){initializeCount--; delete mCurrentWidget;}
    
    /**
     * dummey member
     */
    int addWidget(QObject *widget){mCurrentWidget = widget; return 0;}
    
    /**
     * dummy member
     */
    QObject* currentWidget(){ return mCurrentWidget;}
    
    /**
     * dummy member
     */
    QObject *removeAt(int index){Q_UNUSED(index); QObject *tmp = mCurrentWidget; mCurrentWidget = 0;   return tmp;}
    
    /**
     * counter to make sure alloc dealloc match
     */
    static int initializeCount;
    
    QPointer<QObject> mCurrentWidget;

};

#endif
