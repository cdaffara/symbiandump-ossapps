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
 * Description: Implementation of applicationmanagement components
 *
 */
#ifndef __APPLICATIONMANAGEMENTUTILITYFACTORY_H__
#define __APPLICATIONMANAGEMENTUTILITYFACTORY_H__

//  INTERNAL INCLUDES
#include "ApplicationManagementUtility.h"

namespace NApplicationManagement
    {

    //  CLASS DEFINITION
    /**
     *
     *   
     *
     */
    class ApplicationManagementUtilityFactory
    //: public CBase
        {

public:
        // New methods

        /**
         *	
         *	@param 
         *	@return 
         */
        static CApplicationManagementUtility* CreateManagementUtilityL();

        };

    }

#endif      //  __APPLICATIONMANAGEMENTUTILITYFACTORY_H__
