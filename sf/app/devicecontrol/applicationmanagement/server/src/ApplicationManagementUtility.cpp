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


//	CLASS HEADER
#include "ApplicationManagementUtility.h"
#include "debug.h"
using namespace NApplicationManagement;

// CONSTRUCTION
CApplicationManagementUtility* CApplicationManagementUtility::NewL()
    {
    CApplicationManagementUtility* self =
            CApplicationManagementUtility::NewLC();
    CleanupStack::Pop();

    return self;
    }
TInt CApplicationManagementUtility::mHidden=0;
HbDialog* CApplicationManagementUtility::mCurrDlg=0;
TInt CApplicationManagementUtility::iSilentSession = 0;
TFileName CApplicationManagementUtility::iFileName(KNullDesC);

CApplicationManagementUtility* CApplicationManagementUtility::NewLC()
    {
    CApplicationManagementUtility* self = new( ELeave ) CApplicationManagementUtility();
    CleanupStack::PushL(self);

    self->ConstructL();
    return self;
    }

// Destructor (virtual by CBase)
CApplicationManagementUtility::~CApplicationManagementUtility()
    {
    }

// Default constructor
CApplicationManagementUtility::CApplicationManagementUtility()
    {
    }

// Second phase construct
void CApplicationManagementUtility::ConstructL()
    {
    }

TBool CApplicationManagementUtility::FindInstalledJavaUidL(const TUid& /*aUid*/) const
    {
    return EFalse;
    }

void CApplicationManagementUtility::InstalledSuiteUidsL(RArray <TUid>)
    {
    // do nothing
    }

void CApplicationManagementUtility::JavaUidsL(RArray<TUid>& /*aUids*/)
    {
    // do nothing
    }

void CApplicationManagementUtility::RefreshJavaRegistryL()
    {
    // do nothing
    }
void CApplicationManagementUtility::GetInstalledMidletParametersL(
        TMidletParameters& /*aMidletParameters*/)
    {
    RDEBUG( "CApplicationManagementUtility::GetInstalledMidletParametersL: Start");
    //do nothng
    RDEBUG( "CApplicationManagementUtility::GetInstalledMidletParametersL: end");
    }
void CApplicationManagementUtility::SetFileName(const TFileName& aFileName)
    {
	iFileName = aFileName;
    }
void CApplicationManagementUtility::GetFileName(TFileName& aFileName)
   {
	aFileName = iFileName;
   }
	
//  END OF FILE
