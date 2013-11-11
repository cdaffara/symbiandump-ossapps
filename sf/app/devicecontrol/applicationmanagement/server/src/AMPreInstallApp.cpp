/*
 * Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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


#include <pathinfo.h>
#include <driveinfo.h>
#include <apmstd.h>  //for KMaxDataTypeLength
#include <sisregistrysession.h>
#include <sisregistrypackage.h>
#include <e32cmn.h> 
#include <utf.h>
#include <sisregistryentry.h>
#include <caf/caf.h>
#include <apgcli.h>
#include <SWInstDefs.h>

#include "AMPreInstallApp.h"
#include "debug.h"

using namespace NApplicationManagement;
CAMPreInstallApp* CAMPreInstallApp::NewL()
    {
    CAMPreInstallApp* self = CAMPreInstallApp::NewLC();
    CleanupStack::Pop(self);
    return self;

    }


CAMPreInstallApp* CAMPreInstallApp::NewLC()
    {
    CAMPreInstallApp *self = new (ELeave) CAMPreInstallApp();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;

    }
CAMPreInstallApp::CAMPreInstallApp()
    {
    }
CAMPreInstallApp::~CAMPreInstallApp()
    {
    delete iStorage;
    iFs.Close();
    #ifdef RD_MULTIPLE_DRIVE        
 		  delete iInstallDocPathArray;
    #endif    
    iPreInstalledAppParams.ResetAndDestroy();
    }
void CAMPreInstallApp::ConstructL()
    {
    iStorage = CDeliveryComponentStorage::NewL();
    User::LeaveIfError(iFs.Connect() );
    TPtrC mmcDrive(TParsePtrC( PathInfo::MemoryCardRootPath() ).Drive());
      iPreInstallPath.Append(mmcDrive);
      iPreInstallPath.Append(KPreInstallPath);
    }
void CAMPreInstallApp::GetPreInstalledAppsL(RPointerArray<
        TPreInstalledAppParams> &aPreInstalledAppParams)
    {

    MakeAllInstallPathsL();
    ListPreInstalledAppL();
    aPreInstalledAppParams = iPreInstalledAppParams;
    }


void CAMPreInstallApp::ListPreInstalledAppL(const TDesC& installDocPath)
    {

    RDEBUG("Clist_PreInstallAppUi: ListPreInstalledApp: <<<<");
    CDir* dir= NULL;
    TInt err = iFs.GetDir(installDocPath, KEntryAttNormal, ESortByName, dir);
   // _LIT(KInstallpath,"C:\\private\\10202dce\\");
    //TInt err = iFs.GetDir(KInstallpath, KEntryAttNormal, ESortByName, dir);
    RDEBUG_2("Clist_PreInstallAppUi: ListPreInstalledApp: err in getting dir list : %d",err);
    
    if (err==KErrNone)
        {
        CleanupStack::PushL(dir);
        RDEBUG_2("Clist_PreInstallAppUi: ListPreInstalledApp: no of dir's : %d",dir->Count());
        for (TInt i(0); i < dir->Count(); i++)
            {

	    RDEBUG_2("Clist_PreInstallAppUi: ListPreInstalledApp: <<<< dir entry %d", i);
            TEntry entry;
            entry = (*dir)[i];
            HBufC* pathAndName = HBufC::NewLC(installDocPath.Length()
                    + entry.iName.Length() );
            TPtr ptrPathAndName = pathAndName->Des();
            ptrPathAndName.Append(installDocPath);
            ptrPathAndName.Append(entry.iName);

            HBufC* mimeType = HBufC::NewLC(KMaxDataTypeLength);
            //Recognize     
            if (!RecognizeL(*pathAndName, mimeType))
                {
                CleanupStack::PopAndDestroy(mimeType);
                CleanupStack::PopAndDestroy(pathAndName);
                continue;
                }

#ifdef RD_MULTIPLE_DRIVE
            if (TParsePtrC(installDocPath).Path().CompareF(KPreInstallPath)
                    == 0)
#else
            if (installDocPath.CompareF(iPreInstallPath) == 0)
#endif
                {

		RDEBUG("Clist_PreInstallAppUi: ListPreInstalledApp: <<<< Entered installDocPath.CompareF(iPreInstallPath)");
                Swi::RSisRegistrySession regSession;
                CleanupClosePushL(regSession);
                User::LeaveIfError(regSession.Connect() );

                RArray<TUid> uids;
                CleanupClosePushL(uids);

                regSession.InstalledUidsL(uids);

                RFile temp;
                User::LeaveIfError(temp.Open(iFs, *pathAndName,
                        EFileShareReadersOnly | EFileRead) );
                CleanupClosePushL(temp);

                TUid appUid;
                TInt uidLen = sizeof(TInt32);
                TInt seekLen = sizeof(TInt32) + sizeof(TInt32);

                User::LeaveIfError(temp.Seek(ESeekStart, seekLen));

                TPckg<TInt32> uid1(appUid.iUid);
                User::LeaveIfError(temp.Read(uid1, uidLen));
                if (uid1.Length() != uidLen)
                    {
                    User::Leave(KErrUnderflow);
                    }

                //checking whether this is installed or not
                TBool installed = regSession.IsInstalledL(appUid);
                Swi::RSisRegistryEntry registryEntry;
                TInt regEntryError = registryEntry.Open(regSession, appUid);
		RDEBUG_2("Clist_PreInstallAppUi: ListPreInstalledApp: RegEntryError : %d",regEntryError);

		//User::LeaveIfError(regEntryError);
		if (regEntryError == KErrNone)
		    {
                CleanupClosePushL(registryEntry);
                TBool isPreInstalled = registryEntry.PreInstalledL();
               // TBool isPreInstalled = ETrue;

                RDEBUG_4("Clist_PreInstallAppUi: ListPreInstalledApp: UID is : '0x%X', installed:, preinstalled %d  %d",appUid,installed,isPreInstalled);

                for (TInt i(0); isPreInstalled && i < uids.Count(); i++)
                    {
                    if (appUid == uids[i])
                        {

			RDEBUG("Clist_PreInstallAppUi: appUid == uids[i]");
                        Swi::CSisRegistryPackage* sisRegistry = NULL;
                        TRAPD( err, sisRegistry = regSession.SidToPackageL( uids[i] ) ) ;
                        //User::LeaveIfError(err);
			if(err == KErrNone)
			{

			RDEBUG("Clist_PreInstallAppUi: err == KErrNone");
                        TPreInstalledAppParams *params =
                                new (ELeave) TPreInstalledAppParams;
                        params->iPreInstalledAppame.Copy(sisRegistry->Name());
                        params->iPreInstalledAppVendorName.Copy(
                                sisRegistry->Vendor());
                        params->iPreInstalledAppUid = sisRegistry->Uid();
                        params->iMimeType.Copy(*mimeType);
                        //Get version
                        TVersion version = registryEntry.VersionL();
                        TBuf8<KVersionLength> pkgDes;
                        pkgDes.AppendNum(version.iMajor);
                        pkgDes.Append(KLiteralPeriod);
                        pkgDes.AppendNum(version.iMinor);
                        params->iVersion.Copy(pkgDes);

                        iPreInstalledAppParams.AppendL(params);

                        RDEBUG_2("Clist_PreInstallAppUi: ListPreInstalledApp: Installed App UID is : '0x%X'",appUid);
                        RDEBUG_2("Clist_PreInstallAppUi: ListPreInstalledApp: Installed App Name is: %S",&(sisRegistry->Name()));
                        RDEBUG_2("Clist_PreInstallAppUi: ListPreInstalledApp: Installed App Vendor is: %S",&(sisRegistry->Vendor()));
                        RDEBUG_2("Clist_PreInstallAppUi: ListPreInstalledApp: Installed App UID is : '0x%X'",sisRegistry->Uid());
                        delete sisRegistry;
			}
                        continue;
                        }
                    }

                CleanupStack::PopAndDestroy(&registryEntry);
		    }
                CleanupStack::PopAndDestroy(&temp);
                CleanupStack::PopAndDestroy(&uids);
                CleanupStack::PopAndDestroy(&regSession);
                CleanupStack::PopAndDestroy(mimeType);
                CleanupStack::PopAndDestroy(pathAndName);
                }
            }
         CleanupStack::PopAndDestroy(dir);
        }
    }
void CAMPreInstallApp::ListPreInstalledAppL()
    {
    iPreInstalledAppParams.Reset();
#ifdef RD_MULTIPLE_DRIVE
    TInt count = iInstallDocPathArray->Count();
    for ( TInt index(0); index < count; index++ )
        {
        if ( (*iInstallDocPathArray)[index].Length()> 0 )
            {
            ListPreInstalledAppL( (*iInstallDocPathArray)[index] );
            }
        }
#else   
    if (iPreInstallDocumentPath.Length() > 0)
        {
        ListPreInstalledAppL(iPreInstallDocumentPath);
        //_LIT(KInstallpath,"C:\\private\\10202dce\\");
        //ListPreInstalledAppL(KInstallpath);
        }
#endif

    }
    
TBool CAMPreInstallApp::RecognizeL(TDesC& aPathAndName, HBufC* aMimeType)
    {
    TBool IsOK(ETrue);
    ContentAccess::CContent* pkgContent = ContentAccess::CContent::NewLC(
            aPathAndName, ContentAccess::EContentShareReadWrite);
    
    TPtr mimePtr(aMimeType->Des());
    pkgContent->GetStringAttribute(ContentAccess::EMimeType, mimePtr);

    HBufC8* tmpMime8 = HBufC8::NewLC(aMimeType->Length());
    tmpMime8->Des().Copy(*aMimeType);
    TPtr8 mimePtr8(tmpMime8->Des());

    if ((mimePtr8 != SwiUI::KSisxMimeType) && (mimePtr8
            != SwiUI::KSisMimeType) && (mimePtr8 != SwiUI::KPipMimeType))
        {
        IsOK = EFalse;
        }
    CleanupStack::PopAndDestroy(tmpMime8);
    CleanupStack::PopAndDestroy(pkgContent);
    return IsOK;

    }
void CAMPreInstallApp::MakeAllInstallPathsL()
    {
#ifdef RD_MULTIPLE_DRIVE        
    iInstallDocPathArray = new (ELeave) CDesCArrayFlat( 2 );
    TDriveList driveList;
    TInt driveCount = 0;
    //User::LeaveIfError(iFs.Connect() );
    User::LeaveIfError( DriveInfo::GetUserVisibleDrives(iFs, driveList,driveCount));
    TUint driveStatus = 0;
    for (TInt index(0); index < KMaxDrives; index++)
        {
        if (driveList[index])
            {
            User::LeaveIfError( DriveInfo::GetDriveStatus(iFs, index,
                    driveStatus));
            TFileName path;
            TDriveUnit driveUnit(index);
            if ( !(driveStatus & DriveInfo::EDriveRemote ))
                {
                if (driveStatus & DriveInfo::EDriveRemovable)
                    {
                    path.Zero();
                    path.Append(driveUnit.Name() );
                    path.Append(KPreInstallPath);
                    iInstallDocPathArray->AppendL(path);
                    }
                }
            }
        }
    //iFs.Close();
#else   
    TPtrC mmcDrive(TParsePtrC( PathInfo::MemoryCardRootPath() ).Drive());
    iPreInstallDocumentPath.Append(mmcDrive);
    iPreInstallDocumentPath.Append(KPreInstallPath);
#endif //RD_MULTIPLE_DRIVE 
    }
