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
* Description:  Thumbnail Manager Stub
 *
*/

// Version : %version: 1 %

#ifndef TNMGR_STUB_H_
#define TNMGR_STUB_H_

class CFbsBitmap;
class MThumbnailData;
//class MThumbnailManagerObserver;

typedef TInt TThumbnailRequestId;

class MThumbnailManagerObserver
    {
public:

    virtual void ThumbnailPreviewReady( MThumbnailData& aThumbnail,
                                        TThumbnailRequestId aId ) = 0;

    virtual void ThumbnailReady( TInt aError, MThumbnailData& aThumbnail,
                                 TThumbnailRequestId aId ) = 0;
};


class CThumbnailObjectSource : public CBase
    {
public:

    CThumbnailObjectSource( CFbsBitmap*  aBitmap, const TDesC& aUri );
    virtual ~CThumbnailObjectSource();    
    static CThumbnailObjectSource* NewL( CFbsBitmap*  aBitmap, const TDesC& aUri );
};



class CThumbnailManager : public CBase
{
    
public:
    
    CThumbnailManager( MThumbnailManagerObserver& aObserver );
    virtual ~CThumbnailManager();
    static CThumbnailManager* NewL( MThumbnailManagerObserver& aObserver ); 
    
    TThumbnailRequestId SetThumbnailL( CThumbnailObjectSource& source );   
    
public:
    MThumbnailManagerObserver&   iObserver;
};


#endif /* TNMGR_STUB_H_ */
