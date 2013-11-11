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
* Description:  stub hbaction 
* 
*/

#ifndef HBACTION_H
#define HBACTION_H

#include <QObject>
#include <QString>
#include <qpointer.h>
#include <qaction.h>
#include "hbnamespace.h"
#include "hbicon.h"
#include "hbmenu.h"

#include "xqserviceutilstub.h"
#include "xqserviceproviderstub.h"

class  HbAction : public QAction
{
    Q_OBJECT

public:
    
    /**
     * Constructor
     */
    explicit HbAction(QObject *parent = 0) 
      : QAction(parent),
        mMenu(0)
    {
        Q_UNUSED(parent);
        initializeCount++;
        mTriggeredCount = 0;
    }
    
    /**
     * Contructor.
     */       
    explicit HbAction(const QString &txt, QObject *parent = 0) 
      : QAction(txt, parent), 
        mMenu(0)
    {
        Q_UNUSED(parent);
        initializeCount++;
        mTriggeredCount = 0;
    }
    
    /**
     * Contructor.
     */ 
    explicit HbAction(Hb::SoftKeyAction actType, QObject *parent = 0) 
      : QAction(parent),
        mMenu(0)
    {
        Q_UNUSED(parent);
        Q_UNUSED(actType);
        initializeCount++;
        mTriggeredCount = 0;
    }
    
    explicit HbAction(Hb::NavigationAction action, QObject* parent = 0) 
      : QAction(parent),
        mMenu(0)
    {
        Q_UNUSED(parent);
        Q_UNUSED(action);
        initializeCount++;
        mNavAction = action;
        mTriggeredCount = 0;
    }
    
    /**
     * destructor
     */
    virtual ~HbAction(){initializeCount--;}
    
    /**
     * icon
     */
    void setIcon(const HbIcon &icon) { mIcon = icon; }
    
    /**
     * icon
     */
    HbIcon icon() const { return mIcon; }
    
    /**
     * menu
     */
    void setMenu(HbMenu* menu) { mMenu = menu; }
    
    /**
     * menu
     */
    HbMenu *menu() const { return mMenu; }
    
    /**
     * Trigger stub
     */
    void trigger() 
    { 
        mTriggeredCount++;
        QAction::trigger();
    }
      
    
public: // data
    
    /**
     * counter to make sure alloc dealloc match
     */
    static int initializeCount;
    
    /**
     * Hb navigation action type.
     */
    static Hb::NavigationAction mNavAction;
    
    /**
     * Count how many times this was triggered.
     */
    int mTriggeredCount;
    
    /**
     * icon
     */
    HbIcon mIcon;
    
    /**
     * menu
     */
    HbMenu* mMenu;

};

#endif
