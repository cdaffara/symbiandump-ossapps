/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#ifndef CPREVIEWINFO_H
#define CPREVIEWINFO_H

#include <e32def.h>
#include <e32std.h>

#include "tpreviewimageinfo.h"

/**
 *
 *  Holds information of the preview of a single template
 *
 */
class CPreviewInfo : public CBase
    {
    public: // Constructors and destructors

	    /**
		 *  Constructor
         *
         *  @param aCapabilityUid   Uid of the template
         *  @param aPaperLandscape  Is paper drawn to landscape or not
		 */
		CPreviewInfo( TInt aCapabilityUid, TBool aPaperLandscape );

		/**
         *  Destructor
         */
        ~CPreviewInfo();

    public: // data

        // Paper in landscape format or not..
		TBool iPaperLandscape;

        // Template capability UID
        TInt iUid;

        // Used for sticker templates; one image per page, no matter
        // how many image positions there will be per sheet
        TBool iOneImagePerPage;

        // Index of a background bitmap
        TInt iBckgIndex;

        // Pointers to the image information in the template
        RPointerArray<TPreviewImageInfo> iImageInfo;
    };

#endif //  CPREVIEWINFO_H

//  End of File
