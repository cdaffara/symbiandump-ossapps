/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container for MPX Video File Details
*
*/

// Version : %version: 9 %




#ifndef __MPXVIDEOFILEDETAILS__
#define __MPXVIDEOFILEDETAILS__

// 
//  CLASS DECLARATION
//
NONSHARABLE_CLASS( CMPXVideoFileDetails ) : public CBase
{
    public:  
        //
        //  Constructors and destructor
        //
        static CMPXVideoFileDetails* NewL();
        
        //
        //  Destructor.
        //
        virtual ~CMPXVideoFileDetails();
        
        //
        //  Clear all file details
        //
        void ClearFileDetails();

    private:
        //
        //  By default Symbian 2nd phase constructor is private.
        //
        void ConstructL();

    public:    
        //
        // Data
        //
        HBufC*   iClipName;
        HBufC*   iTitle;
        HBufC*   iArtist;
        HBufC*   iMimeType;
        HBufC*   iDescription;
        HBufC*   iLocation;
        HBufC*   iCopyright;
        HBufC*   iLanguage;
        HBufC*   iKeywords;                


        TInt     iResolutionWidth;
        TInt     iResolutionHeight;
        TInt     iMaxVolume;
        TInt     iBitRate;
        TInt     iCreationTime;
        TInt     iModificationTime;
        
        TBool    iSeekable;
        TBool    iPausableStream;
        TBool    iLiveStream;
        TBool    iAudioEnabled;
        TBool    iVideoEnabled;
        TBool    iPartialPlayback;
        TBool    iDrmProtected;
        TUint32  iFourCCCode;

        TTimeIntervalMicroSeconds iDuration;
};

#endif      // __MPXVIDEOFILEDETAILS__  
            
// EOF
