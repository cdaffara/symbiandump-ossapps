/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Declaration for TMMCScBkupDriveFilter
*     
*
*/

#ifndef __TMMCSCBKUPDRIVEFILTER_H__
#define __TMMCSCBKUPDRIVEFILTER_H__

// System includes
#include <e32std.h>
#include <f32file.h>

/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(TMMCScBkupDriveFilter)
    {
    public:

        /**
        * C++ default constructor
        */
        inline TMMCScBkupDriveFilter()
            { Reset(); }

        /**
        * C++ default constructor
        */
        inline TMMCScBkupDriveFilter( const TDriveList& aPrimaryList )
            : iPrimaryDriveList( aPrimaryList ), iHaveSecondaryList( EFalse )
            { Reset(); }

    public: // API

        /**
        *
        */
        void Reset();

        /**
        *
        */
        void SetPrimaryDriveFilter( const TDriveList& aDriveList );

        /**
        *
        */
        void SetSecondaryDriveFilter( const TDriveList& aDriveList );

        /**
        *
        */
        TBool NextValidDrive(TDriveNumber& aDrive);

        /**
        *
        */
        TDriveNumber CurrentDrive() const;

    private: // Internal methods

        /**
        *
        */
        TBool FindValidDrive( const TDriveList& aList, TDriveNumber& aDrive, TBool aOneAttemptOnly = EFalse );

    private: // Member data

        /**
        *
        */
        TInt iCurrentDrive;
        
        /**
        *
        */
        TDriveList iPrimaryDriveList;
        
        /**
        *
        */
        TDriveList iSecondaryDriveList;

        /**
        *
        */
        TBool iHaveSecondaryList;
    };




#endif // __TMMCSCBKUPDRIVEFILTER_H__

//End of File
