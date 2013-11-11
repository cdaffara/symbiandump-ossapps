/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SConPcdUtility implementation
*
*/


//  CLASS HEADER
#include <swi/sisregistryentry.h>
#include <swi/sisregistrysession.h>
#include <swi/sisregistrypackage.h>
#include <stringresourcereader.h>
// #include <WidgetRegistryClient.h>
#include <javaregistryincludes.h>
#include <appversion.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <sconftp.rsg>              // Resource to be read header 

using namespace Java;

#include "debug.h"
#include "sconpcdutility.h"
#include "sconconmltask.h"

// localized "unknown vendor".
_LIT( KSConResourceName, "z:\\Resource\\sconftp.rsc" );



//  METHODS
//----------------------------------------------------------------------------
// void SConPcdUtility::ProcessListInstalledAppsL( CSConTask*& aTask )
//----------------------------------------------------------------------------
//
void SConPcdUtility::ProcessListInstalledAppsL( CSConTask*& aTask )
    {
    TRACE_FUNC_ENTRY;
    
    LOGGER_WRITE_1("iAllApps: %d",(TInt)aTask->iListAppsParams->iAllApps);
    LOGGER_WRITE8_1("driveList: %S", &aTask->iListAppsParams->iDriveList);
    
    AppendInstalledSisL( *aTask->iListAppsParams );
    AppendInstalledJavaL( *aTask->iListAppsParams );
    AppendInstalledWidgetsL( *aTask->iListAppsParams );
    
    TRACE_FUNC_EXIT;
    }

//----------------------------------------------------------------------------
// void SConPcdUtility::AppendInstalledSisL( RPointerArray<CSConInstApp> &aApps )
// Appends installed sis packages and augmentations to aApps array.
//----------------------------------------------------------------------------
//
void SConPcdUtility::AppendInstalledSisL( CSConListInstApps& aListInstApps )
    {
    TRACE_FUNC_ENTRY;
    // Get installed applications from sis registry
    Swi::RSisRegistrySession sisRegistry;
    CleanupClosePushL( sisRegistry );
    User::LeaveIfError( sisRegistry.Connect() );
    
    RArray<TUid> uids;
    CleanupClosePushL(uids);
    sisRegistry.InstalledUidsL( uids );
    
    
    //Read package information
    for( TInt i = 0; i < uids.Count(); i++ )
        {
        Swi::RSisRegistryEntry entry;
        CleanupClosePushL(entry);
        User::LeaveIfError( entry.Open( sisRegistry, uids[i] ) );
        
        TBool showIt( EFalse );
        if ( aListInstApps.iAllApps )
            {
            // show all apps -param defined, exlude stubs on ROM
            if ( !entry.IsInRomL() )
                {
                showIt = ETrue;
                }
            }
        else
            {
            // show if installed one of the specified drives
            // don't exlude stubs on ROM
            showIt = IsInstalledToSelectedDrive( aListInstApps.iDriveList,
                                                 entry.InstalledDrivesL() );
            }
        
        // Only show if not in rom
        if ( showIt && entry.IsPresentL() )
            {    
            // Add the created object to the list
            LOGGER_WRITE_1( "SConPcdUtility::ProcessListInstalledAppsL : add pkg, index %d", i );
            CSConInstApp* app = new (ELeave) CSConInstApp();
            CleanupStack::PushL( app );
            
            HBufC* temp = entry.PackageNameL();
            TPtrC tempPtr = temp->Des();
            LOGGER_WRITE_1("PackageNameL: %S", &tempPtr);
            CleanupStack::PushL( temp );
            if ( temp->Length() > app->iName.MaxLength() )
            	{
            	User::Leave( KErrTooBig );
            	}
            app->iName.Copy( *temp );
            CleanupStack::PopAndDestroy( temp );
            temp = NULL;
            
            temp = entry.UniqueVendorNameL();
            CleanupStack::PushL( temp );
            if ( temp->Length() > app->iVendor.MaxLength() )
            	{
            	User::Leave( KErrTooBig );
            	}
            app->iVendor.Copy( *temp );
            CleanupStack::PopAndDestroy( temp );
            temp = NULL;
            
            app->iVersion.Copy( entry.VersionL().Name() );
            app->iType = ESisApplication;
            app->iSize = entry.SizeL();
            app->iUid = entry.UidL();
            
            User::LeaveIfError( aListInstApps.iApps.Append( app ) );
            CleanupStack::Pop( app );
            }
        
        // Get possible augmentations
        RPointerArray<Swi::CSisRegistryPackage> augPackages;
        CleanupResetAndDestroyPushL( augPackages );
        entry.AugmentationsL( augPackages );
        for ( TInt j( 0 ); j < augPackages.Count(); j++ )
            {
            Swi::RSisRegistryEntry augmentationEntry;
            CleanupClosePushL( augmentationEntry );
            augmentationEntry.OpenL( sisRegistry, *augPackages[j] );
            
            TBool showIt( EFalse );
            if ( aListInstApps.iAllApps )
                {
                // show all apps -param defined, exlude stubs on ROM
                if ( !augmentationEntry.IsInRomL() )
                    {
                    showIt = ETrue;
                    }
                }
            else
                {
                // show if installed one of the specified drives
                // don't exlude stubs on ROM
                showIt = IsInstalledToSelectedDrive( aListInstApps.iDriveList,
                                        augmentationEntry.InstalledDrivesL() );
                }
            
            // Only show if not in rom
            if ( showIt && augmentationEntry.IsPresentL() )
                {
                CSConInstApp* augApp = new (ELeave) CSConInstApp();
                CleanupStack::PushL( augApp );
                augApp->iName.Copy( augPackages[j]->Name() );
                
                HBufC* temp = entry.PackageNameL();
                CleanupStack::PushL( temp );
                if ( temp->Length() > augApp->iParentName.MaxLength() )
                	{
                	User::Leave( KErrTooBig );
                	}
                augApp->iParentName.Copy( *temp );
                CleanupStack::PopAndDestroy( temp );
                temp = NULL;
                
                augApp->iVendor.Copy( augPackages[j]->Vendor() );
                augApp->iVersion.Copy( augmentationEntry.VersionL().Name() );
                augApp->iType = ESisAugmentation;
                augApp->iSize = augmentationEntry.SizeL();
                augApp->iUid = augmentationEntry.UidL();
                
                LOGGER_WRITE_1( "SConPcdUtility::ProcessListInstalledAppsL : add augmentation, index: %d", j );
                LOGGER_WRITE_1( "SConPcdUtility::ProcessListInstalledAppsL : add augmentation, basepkg: %d", i );
                TInt augindex = augPackages[j]->Index();
                LOGGER_WRITE_1( "SConPcdUtility::ProcessListInstalledAppsL : augindex: %d",augindex );
                User::LeaveIfError( aListInstApps.iApps.Append( augApp ) );
                CleanupStack::Pop( augApp );   
                }
            CleanupStack::PopAndDestroy( &augmentationEntry );
            }  
        CleanupStack::PopAndDestroy( &augPackages ); 
        CleanupStack::PopAndDestroy( &entry );
        }
    
    CleanupStack::PopAndDestroy(&uids);
    CleanupStack::PopAndDestroy(&sisRegistry);
    TRACE_FUNC_EXIT;
    }

//----------------------------------------------------------------------------
// void SConPcdUtility::AppendInstalledJavaL( RPointerArray<CSConInstApp> &aApps )
// Appends installed java packages to aApps array.
//----------------------------------------------------------------------------
//
void SConPcdUtility::AppendInstalledJavaL( CSConListInstApps& aListInstApps )
    {
    TRACE_FUNC_ENTRY;
    CJavaRegistry* javaRegistry = CJavaRegistry::NewLC( );
    RArray<TUid> packageUids;
    CleanupClosePushL( packageUids );
    javaRegistry->GetRegistryEntryUidsL( packageUids );
    LOGGER_WRITE_1("packageUids.Count(): %d", packageUids.Count());
    for (TInt i=0; i<packageUids.Count(); i++ )
        {
        LOGGER_WRITE_1("RegistryEntryL: %d",i);
        LOGGER_WRITE_1("handle entry uid: 0x%08X",packageUids[i].iUid);
        CJavaRegistryEntry* entry = javaRegistry->RegistryEntryL( packageUids[i] );
        if ( entry )
            {
            CleanupStack::PushL( entry );
            if ( entry->Type() >= EGeneralPackage && entry->Type() < EGeneralApplication )
                {
                // entry was correct type
                CJavaRegistryPackageEntry* packageEntry = ( CJavaRegistryPackageEntry* ) entry;
                
                // check do we need to filter it out
                TBool showIt( EFalse );
                if ( aListInstApps.iAllApps )
                    {
                    showIt = ETrue;
                    }
                else
                    {
                    TDriveNumber drive = packageEntry->Drive();
                    if ( aListInstApps.iDriveList.Length() > drive
                            && aListInstApps.iDriveList[ drive ] )
                        {
                        showIt = ETrue;
                        }
                    }
                
                if ( showIt )
                    {
                    CSConInstApp* app = new (ELeave) CSConInstApp();
                    CleanupStack::PushL( app );
                    // Get Uid, name, type, vendor
                    app->iUid =  packageEntry->Uid();
                    app->iName.Copy ( packageEntry->Name() );
                    LOGGER_WRITE_1( "Name: %S", &app->iName );
                    app->iType = EJavaApplication;
                    
                    // Get version
                    TAppVersion midletVersion( packageEntry->Version() );
                    TVersion verType(midletVersion.iMajor, midletVersion.iMinor, midletVersion.iBuild);    
                    app->iVersion.Copy( verType.Name() );
                    
                    // Get vendor
                    if ( entry->NumberOfCertificateChains() > 0 && packageEntry->Vendor().Length() > 0 )
                        {
                        app->iVendor.Copy( packageEntry->Vendor() );
                        }
                    else
                        {
                        // untrusted applications do not have certificates.
                        // if the application has a certificate, it is installed either as 
                        // trusted or not installed at all.
                        
                        // unknown vendor
                        TFileName myFileName( KSConResourceName );
                        CStringResourceReader* resReader = CStringResourceReader::NewL( myFileName );
                        TPtrC bufUnknownSuplier;
                        bufUnknownSuplier.Set( resReader->ReadResourceString( R_SECON_UNKNOWN_SUPPLIER ) );
                        
                        app->iVendor.Copy( bufUnknownSuplier );
                        
                        delete resReader;
                        }
                    
                    // Get size
                    app->iSize = packageEntry->UsedUserDiskSpace();
                    
                    User::LeaveIfError( aListInstApps.iApps.Append( app ) );
                    CleanupStack::Pop( app );
                    }
                }
            CleanupStack::PopAndDestroy( entry );
            }
        }
    
    CleanupStack::PopAndDestroy( &packageUids );
    CleanupStack::PopAndDestroy( javaRegistry );
    TRACE_FUNC_EXIT;
    }

//----------------------------------------------------------------------------
// void SConPcdUtility::AppendInstalledWidgetsL( RPointerArray<CSConInstApp> &aApps )
// Appends installed widgets to aApps array.
//----------------------------------------------------------------------------
//
void SConPcdUtility::AppendInstalledWidgetsL( CSConListInstApps& aListInstApps )
    {
    // Commented out as WidgetRegistry will be removed from MCL.
    // TODO: Check how Widgets should be listed on future.
    /*
    TRACE_FUNC_ENTRY;
    RWidgetRegistryClientSession widgetSession;
    CleanupClosePushL( widgetSession );
    User::LeaveIfError( widgetSession.Connect() );
    
    // Get the list of installed widgets
    RWidgetInfoArray widgetInfoArr;
    CleanupClosePushL( widgetInfoArr );
    widgetSession.InstalledWidgetsL( widgetInfoArr );
    TFileName bundleId;
    for ( TInt i = 0; i < widgetInfoArr.Count(); i++ )
        {
        CWidgetInfo *item = widgetInfoArr[i];
        CleanupStack::PushL( item );
        
        
        TBool showIt( EFalse );
        if ( aListInstApps.iAllApps )
            {
            // show all apps -param defined
            showIt = ETrue;
            }
        else
            {
            TDriveUnit locationDrive = item->iDriveName->Des();
            // show if installed one of the specified drives
            if (  aListInstApps.iDriveList.Length() > locationDrive
                    && aListInstApps.iDriveList[locationDrive] )
                {
                showIt = ETrue;
                }
            }
        
        if ( showIt )
            {
            
            CSConInstApp* app = new (ELeave) CSConInstApp();
            CleanupStack::PushL( app );
            app->iName.Copy( *(item->iBundleName) );
            app->iType = EWidgetApplication;
            app->iSize = item->iFileSize;
            app->iUid = item->iUid;
            
            CWidgetPropertyValue* propValue = widgetSession.GetWidgetPropertyValueL(
                    item->iUid, EBundleVersion );
            if ( propValue && propValue->iType == EWidgetPropTypeString )
                {
                app->iVersion.Copy( *(propValue->iValue.s) );
                }
            delete propValue;
            propValue = NULL;
            bundleId.Zero();
            widgetSession.GetWidgetBundleId( item->iUid, bundleId );
            app->iWidgetBundleId = bundleId.AllocL();
            
            User::LeaveIfError( aListInstApps.iApps.Append( app ) );
            CleanupStack::Pop( app ); // ownership transferred, do not delete
            }
        CleanupStack::PopAndDestroy( item );
        }
    User::LeaveIfError( widgetSession.Disconnect() );
    
    CleanupStack::PopAndDestroy( &widgetInfoArr );
    CleanupStack::PopAndDestroy( &widgetSession );
    TRACE_FUNC_EXIT;
    */
    }

// ---------------------------------------------------------
// SConPcdUtility::IsInstalledToSelectedDrive
// Solve highest drive from aInstalledDrives and check
// if that drive is selected
// ---------------------------------------------------------
//
TBool SConPcdUtility::IsInstalledToSelectedDrive( const TDriveList& aSelectedDriveList, TUint aInstalledDrives )
    {
    TInt locationDrive;
    if( aInstalledDrives )
         {
         // Select the highest drive as location drive
         TInt drive = EDriveA;
         while( aInstalledDrives >>= 1 )
             {
             drive++;
             }
         locationDrive = drive;
         }
     else
         {
         // No installed files, select C: as location drive
         locationDrive = EDriveC;
         }
    
    if ( aSelectedDriveList.Length() > locationDrive && aSelectedDriveList[locationDrive] )
        {
        LOGGER_WRITE("SConPcdUtility::IsInstalledToSelectedDrive : return ETrue");
        return ETrue;
        }
    else
        {
        LOGGER_WRITE("SConPcdUtility::IsInstalledToSelectedDrive : return EFalse");
        return EFalse;
        }
    }
//  END OF FILE
