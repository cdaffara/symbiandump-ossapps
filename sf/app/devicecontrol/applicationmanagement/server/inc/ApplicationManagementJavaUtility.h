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
 * Description:  Implementation of applicationmanagement components
 *
*/


#ifndef __APPLICATIONMANAGEMENTJAVAUTILITY_H__
#define __APPLICATIONMANAGEMENTJAVAUTILITY_H__

//  EXTERNAL INCLUDES
#include <e32base.h>
#include <javaregistryincludes.h>
using namespace Java;
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
    class CApplicationManagementJavaUtility :
        public CApplicationManagementUtility
        {

public:
        // Constructors and destructors

        static CApplicationManagementJavaUtility* NewL(); // Static constructor
        static CApplicationManagementJavaUtility* NewLC(); // Static constructor
        ~CApplicationManagementJavaUtility(); // Destructor

private:
        // Constructors and destructors

        CApplicationManagementJavaUtility(); // Default constructor
        void ConstructL(); // Second phase construct

public:
        // New methods

        TBool FindInstalledJavaUidL(const TUid &aUid) const;

        void InstalledSuiteUidsL(RArray <TUid>&);

        void JavaUidsL(RArray<TUid> &aUids);

        void RefreshJavaRegistryL();
        void GetInstalledMidletParametersL(
                TMidletParameters& aMidletParameters);

private:
        // Data

        CJavaRegistry* iJavaRegistry;
        };

    }

#endif      //  __ApplicationManagementJavaUtility_H__
