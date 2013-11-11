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

#ifndef AMPREINSTALLAPP_H_
#define AMPREINSTALLAPP_H_

#include <e32base.h>
#include <badesca.h> 
#include <f32file.h>
#include "ApplicationManagementCommon.h"
#include "amstorage.h"


_LIT(KPreInstallPath, "\\private\\10202dce\\");

namespace NApplicationManagement
    {
    
    class TPreInstalledAppParams
        {
public:
        TUid iPreInstalledAppUid;
#if 1
       TPreInstalledAppName iPreInstalledAppame;
       TPreInstalledAppVendorName iPreInstalledAppVendorName;
       TPreInstalledVersion iVersion;
       TMimeType iMimeType;
#endif
        //TPreInstalledVersion bufVersion;
#if 0
        HBufC* iPreInstalledAppame;
        HBufC* iPreInstalledAppVendorName;
#endif
        };
    
    class CAMPreInstallApp : public CBase
        {

public:
        ~CAMPreInstallApp();
        static CAMPreInstallApp* NewL();
        static CAMPreInstallApp* NewLC();
        void ListPreInstalledAppL(const TDesC& installDocPath);
        void ListPreInstalledAppL();
        TBool RecognizeL(TDesC& aPathAndName, HBufC* aMimeType);
        void MakeAllInstallPathsL();
        void GetPreInstalledAppsL(RPointerArray<TPreInstalledAppParams> &aPreInstalledAppParams);
        void Listallapps();
private:
        CAMPreInstallApp();

        void ConstructL();

private:

        TFileName iPreInstallPath; // Path where to scan for pre-install files
#ifdef RD_MULTIPLE_DRIVE        
        CDesCArray* iInstallDocPathArray;
#else        
        TFileName iPreInstallDocumentPath; // Path where to scan for pre-install files  
#endif // RD_MULTIPLE_DRIVE

        RFs iFs;
        CDeliveryComponentStorage *iStorage;
        RPointerArray<TPreInstalledAppParams> iPreInstalledAppParams;

        };



    }

#endif /*AMPREINSTALLAPP_H_*/
