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

//	CLASS HEADER

#include <e32property.h>
#include <javadomainpskeys.h>
#include <javaregistryincludes.h>
#include <appversion.h>
#include "ApplicationManagementJavaUtility.h"
#include "debug.h"
//  INTERNAL INCLUDES


using namespace NApplicationManagement;

//----------------------------------------------------------------------------
// 
//----------------------------------------------------------------------------
// CONSTRUCTION
CApplicationManagementJavaUtility* CApplicationManagementJavaUtility::NewL()
    {
    CApplicationManagementJavaUtility* self =
            CApplicationManagementJavaUtility::NewLC();
    CleanupStack::Pop();

    return self;
    }

//----------------------------------------------------------------------------
// 
//----------------------------------------------------------------------------
//
CApplicationManagementJavaUtility* CApplicationManagementJavaUtility::NewLC()
    {
    CApplicationManagementJavaUtility* self = new( ELeave ) CApplicationManagementJavaUtility();
    CleanupStack::PushL(self);

    self->ConstructL();
    return self;
    }

//----------------------------------------------------------------------------
// 
//----------------------------------------------------------------------------
// Destructor (virtual by CBase)
CApplicationManagementJavaUtility::~CApplicationManagementJavaUtility()
    {

    if (iJavaRegistry)
        {
        delete iJavaRegistry;
        }
    }

//----------------------------------------------------------------------------
// 
//----------------------------------------------------------------------------
// Default constructor
CApplicationManagementJavaUtility::CApplicationManagementJavaUtility()
    {
    }

//----------------------------------------------------------------------------
// 
//----------------------------------------------------------------------------
// Second phase construct
void CApplicationManagementJavaUtility::ConstructL()
    {
    RefreshJavaRegistryL();
    }

//  METHODS

//----------------------------------------------------------------------------
// 
//----------------------------------------------------------------------------
//
TBool CApplicationManagementJavaUtility::FindInstalledJavaUidL(
        const TUid &aUid) const
    {
    TBool ret;
    ret = iJavaRegistry->RegistryEntryExistsL(aUid);
    return ret;

    }

//----------------------------------------------------------------------------
// 
//----------------------------------------------------------------------------
//
void CApplicationManagementJavaUtility::InstalledSuiteUidsL(
        RArray <TUid>& aUids)
    {
    iJavaRegistry->GetRegistryEntryUidsL(aUids);
    }

//----------------------------------------------------------------------------
// 
//----------------------------------------------------------------------------
//
void CApplicationManagementJavaUtility::JavaUidsL(RArray<TUid> &aUids)
    {
    iJavaRegistry->GetRegistryEntryUidsL(aUids);
    }

//----------------------------------------------------------------------------
// 
//----------------------------------------------------------------------------
//
void CApplicationManagementJavaUtility::RefreshJavaRegistryL()
    {
    if (iJavaRegistry)
        {
        delete iJavaRegistry;
        iJavaRegistry = NULL;
        }

    iJavaRegistry = CJavaRegistry::NewL();
    }
void CApplicationManagementJavaUtility::GetInstalledMidletParametersL(
        TMidletParameters& aMidletParameters)
    {
    RDEBUG( "CApplicationManagementJavaUtility::GetInstalledMidletParametersL: Start");
    TInt suiteUid = 0;
    // Get UID for the latest installed Midlet suite
    // KPSUidJavaLatestInstallation = 0x10282567
    RProperty::Get(KUidSystemCategory, KPSUidJavaLatestInstallation, suiteUid);

    if ( !suiteUid)
        {
        User::Leave(KErrNotFound);
        }

    RArray<TUid> uids;
    CleanupClosePushL(uids);

    Java::CJavaRegistry* javaRegistry;
    javaRegistry = Java::CJavaRegistry::NewLC();

    Java::CJavaRegistryEntry* regEntry =
            javaRegistry->RegistryEntryL(TUid::Uid(suiteUid) );

    if (regEntry)
        {
        CleanupStack::PushL(regEntry);
        Java::TJavaRegistryEntryType entryType = regEntry->Type();

        if ( (entryType >= Java::EGeneralPackage ) && (entryType
                < Java::EGeneralApplication))
            {
            //package entry
            Java::CJavaRegistryPackageEntry * regPackageEntry =
                    static_cast<Java::CJavaRegistryPackageEntry*>(regEntry );
            regPackageEntry->GetEmbeddedEntries(uids);

            aMidletParameters.iMidletUid = uids[0]; // conidered the first application from the suite
            aMidletParameters.iMidletName.Copy(regPackageEntry->Name());
            aMidletParameters.iMidletVenorName.Copy(regPackageEntry->Vendor());

            TAppVersion version;
            version = regPackageEntry->Version();
            _LIT8(KDot,".");
            aMidletParameters.bufVersion.Num(version.iMajor);
            aMidletParameters.bufVersion.Append(KDot);
            aMidletParameters.bufVersion.AppendNum(version.iMinor);
            }
        else
            {
            //application entry
            uids.AppendL(regEntry->Uid() );
            aMidletParameters.iMidletUid = regEntry->Uid();
            aMidletParameters.iMidletName.Copy(regEntry->Name());
            //How to get Vendor& version name for java application
            aMidletParameters.iMidletVenorName.Copy(KNullDesC8);
            TAppVersion version(0,0,0);
            _LIT8(KDot,".");
            aMidletParameters.bufVersion.Num(version.iMajor);
            aMidletParameters.bufVersion.Append(KDot);
            aMidletParameters.bufVersion.AppendNum(version.iMinor);
            }
        CleanupStack::PopAndDestroy(regEntry);
        }
    CleanupStack::PopAndDestroy(javaRegistry);
    CleanupStack::PopAndDestroy( &uids);
    RDEBUG( "CApplicationManagementJavaUtility::GetInstalledMidletParametersL: End");
    }
//  END OF FILE
