/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SConPcdUtility header file
*
*/


#ifndef __CSCONPCDUTILITY_H__
#define __CSCONPCDUTILITY_H__


class CSConTask;
class CSConInstApp;
class CSConListInstApps;
//============================================================
// Class TSConPcdUtility declaration
//============================================================  
NONSHARABLE_CLASS ( SConPcdUtility )
    {
    public:     // Methods
        
		static void ProcessListInstalledAppsL( CSConTask*& aTask );
        
    private:    // Methods
    	static void AppendInstalledSisL( CSConListInstApps& aListInstApps );
    	static void AppendInstalledJavaL( CSConListInstApps& aListInstApps );
    	static void AppendInstalledWidgetsL( CSConListInstApps& aListInstApps );
        
        /**
         * Checks if package is installed to selected drive
         * @param aSelectedDriveList selected drives
         * @param aInstalledDrives package drivemask
         * @return ETrue if package is installed to one of the drives on aDriveList
         */
        static TBool IsInstalledToSelectedDrive( const TDriveList& aSelectedDriveList, TUint aInstalledDrives );
        
    };

#endif      //  __CSCONPCDUTILITY_H__
