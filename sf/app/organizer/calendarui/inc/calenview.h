/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Baseclass for creating customised views
 *
*/

#ifndef CALENVIEW_H
#define CALENVIEW_H

// INCLUDES
#include <hbview.h>


/**
 * CCalenView is the base class for
 * creating customised calendar views.
 */
class  CalenView  : public HbView
    {
    public:  // New functions
        
        virtual void doPopulation() = 0;
        virtual void populationComplete() = 0;
        
    };

#endif  // CALENVIEW_H

// End of file
