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

#ifndef __CApplicationManagementUtility_H__
#define __CApplicationManagementUtility_H__

//  EXTERNAL INCLUDES
#include <e32base.h>
#include <hbdialog.h>
#include "ApplicationManagementCommon.h"
#include "javaregistryentry.h"


namespace NApplicationManagement
    {

    //  CLASS DEFINITION
    /**
     *
     *   
     *
     */

    class TMidletParameters
        {
public:
        TUid iMidletUid;
        TMidletnameName iMidletName;
        TMidletVendorName iMidletVenorName;
        TMidletVersion bufVersion;
        TDriveNumber iDrive;

        };
    class CApplicationManagementUtility : public CBase
        {

public:
        // Constructors and destructors

        static CApplicationManagementUtility* NewL(); // Static constructor
        static CApplicationManagementUtility* NewLC(); // Static constructor
        ~CApplicationManagementUtility(); // Destructor

protected:

        CApplicationManagementUtility(); // Default constructor

private:
        // Constructors and destructors

        void ConstructL(); // Second phase construct


public:
        // New methods

        virtual TBool FindInstalledJavaUidL(const TUid &aUid) const;

        virtual void InstalledSuiteUidsL(RArray <TUid>);

        virtual void JavaUidsL(RArray<TUid> &aUids);

        virtual void RefreshJavaRegistryL();
        virtual void GetInstalledMidletParametersL(
                TMidletParameters& aMidletParameters);

	static void SetFileName(const TFileName& aFileName);
	static void GetFileName(TFileName& aFileName);
        
public:
        static TInt mHidden;
        static HbDialog* mCurrDlg;
        static TInt iSilentSession;
        static TFileName iFileName;
        
        };

    }

#endif      //  __CApplicationManagementUtility_H__
