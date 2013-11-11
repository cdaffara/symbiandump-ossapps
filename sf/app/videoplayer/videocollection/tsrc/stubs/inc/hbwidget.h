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
* Description:  stub hbwidget
* 
*/

#ifndef HBWIDGET_H
#define HBWIDGET_H

#include <QGraphicsWidget>
#include <hbnamespace.h>
#include <hbglobal.h>


class HbWidget : public QGraphicsWidget
{
    Q_OBJECT
public:
    HbWidget(QGraphicsItem *parent = 0) : QGraphicsWidget(parent){initializeCount++;}
    
    virtual ~HbWidget(){initializeCount--;}
    
    void clearActions() 
    {
        // Note actions().clear() does not work as actions() returns a copy of the actions list.
        QList<QAction*> act = actions();
        QAction* temp;
        foreach(temp, act) {
            this->removeAction(temp);
        }
    }
    
    static int initializeCount;
};

#endif
