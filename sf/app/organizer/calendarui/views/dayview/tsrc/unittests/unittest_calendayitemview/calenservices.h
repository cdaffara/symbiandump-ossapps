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
 * Description:
 *
 */

#ifndef CALENSERVICES_H
#define CALENSERVICES_H

#include "calencontext.h"

extern quint32 SELECTED_COMMAND;

/*!
 Mocked class MCalenServices
 */
class MCalenServices
{
public:
    MCalenServices() {
        
    }
    
    ~MCalenServices() {
        
    }
    
    MCalenContext Context() {return MCalenContext();}
    
    bool IssueCommandL( quint32 aCommand  ) 
    {
        SELECTED_COMMAND = aCommand;

        return true;
    }
};

#endif // CALENSERVICES_H
