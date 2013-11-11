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


#ifndef MPRINTPREVIEWOBSERVER_H
#define MPRINTPREVIEWOBSERVER_H

#include <e32std.h>

/**
 *
 *  Observer class that will get notification message when the print preview
 *  is ready
 *
 */
class MPrintPreviewObserver
    {
    public:
        
        /**
         *  Gets called when the print server has finished the generation
         *  of preview bitmap
         *
         *  @param aHandle      Handle to the FBS bitmap
         */
        virtual void PreviewBitmapReady( TInt aHandle, 
                                         TBool aPaperLandscape ) = 0;

        virtual TSize PaperSizeLandscape() = 0;

        virtual TSize PaperSizePortrait() = 0;

        virtual TPoint PaperPositionLanscape() = 0;

        virtual TPoint PaperPositionPortrait() = 0;

    };

#endif  // MPRINTPREVIEWOBSERVER_H

//  End of File
