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
* Description:  video playlist utility class stub for unit testing
*
*/

// Version : %version:  1 %

#ifndef __VIDEOPLAYLISTUTILITY_H
#define __VIDEOPLAYLISTUTILITY_H

class CMPXMedia;

#include <e32base.h>
#include <mediarecognizer.h>

class CVideoPlaylistUtility : public CBase
{
    public:
    		CVideoPlaylistUtility();
    		~CVideoPlaylistUtility();
                                     
        static CVideoPlaylistUtility* NewL();                                     

        void GetFileInfoL(  RFile& aFile, 
                            CMediaRecognizer::TMediaType aMediaType,
                            TBool& aSingleLink, TBool& aLocalFile );
                            
        void GetFileInfoL( const TDesC& aFileName, 
                           CMediaRecognizer::TMediaType aMediaType,
                           TBool& aSingleLink, 
                           TBool& aLocalFile);                           
                           
        TPtrC GetLinkLC();
        
        TBool IsSeekable();
        
        CMPXMedia* GetPlayListL( TInt aAccessPointId );

                                     
};
#endif /*__VIDEOPLAYLISTUTILITY_H*/
