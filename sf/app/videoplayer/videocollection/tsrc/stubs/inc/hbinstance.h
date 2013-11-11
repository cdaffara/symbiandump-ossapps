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
* Description:  stub hbinstance
* 
*/

#ifndef HBINSTANCE_H
#define HBINSTANCE_H


#include "hbmainwindow.h"
#include "hbglobal.h"

#define hbInstance (HbInstance::instance())

class HbInstance
{
public:
    
    /**
     * returns static instance
     */
    static HbInstance *instance()
    {
        static HbInstance __instance;
        return &__instance;
    }
    
    /**
     * returns list of windowses 
     */
    QList<HbMainWindow*> allMainWindows()
    {
        return mWindowses;
    }
            
    /**
     * list of windowses 
     */ 
    QList<HbMainWindow*> mWindowses;
    
private:
    /**
     * no copy contructor
     */
    Q_DISABLE_COPY(HbInstance)
    
    /**
     * default constuctor
     */
    HbInstance(){};
    
    /**
     * destructor
     */
    ~HbInstance(){};
};

#endif
