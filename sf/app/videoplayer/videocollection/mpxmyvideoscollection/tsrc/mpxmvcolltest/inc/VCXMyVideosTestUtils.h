/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*/

#ifndef VCXMYVIDEOSTESTUTILS_H
#define VCXMYVIDEOSTESTUTILS_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <BAUTILS.H>

// CONSTANTS
const TInt KVcxTestLargeFile3GB = -1451435;

// MACROS

// FORWARD DECLARATIONS

// DATA TYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  CVCXMyVideosTestUtils
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
class CVCXMyVideosTestUtils : public CBase
{
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CVCXMyVideosTestUtils* NewL();

        /**
        * Destructor.
        */
        virtual ~CVCXMyVideosTestUtils();

    public: 

        enum TVcxTestVideoType
            {
            VcxTestVideo3Gp,               // Extension .3GP
            VcxTestVideoMpeg4,             // Extension .MP4
            VcxTestVideoWmv,                // Extension .WMV
            };        

    private:

        /**
        * C++ default constructor.
        */
        CVCXMyVideosTestUtils();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( );
        
    public: // New functions
        
        TInt EnsureFileIsNotInUse( const TPtrC& aFileName );

        /*
         * Creates a video file to the system.
         */
        void CreateVideoFileL( TVcxTestVideoType aVideoType, TDesC& aFileName, TInt aSize, TBool aUniqueName = false );

        /*
         * Creates video files to the system.
         */
        void CreateVideoFilesL( TVcxTestVideoType aVideoType, TDesC& aFileName, TInt aCount, RPointerArray<HBufC>& aFileArray );
        
    private:
        
        void GetVideoFile( TDes& aFileName, CVCXMyVideosTestUtils::TVcxTestVideoType aVideoType, const TDesC& aDrive );
        
    public: // Data

        RFs iFs;
        CFileMan* iFileMan;
};

#endif      // VCXMYVIDEOSTESTUTILS_H

// End of File
