/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Conversion utility
*
*/


#ifndef _CONVERSIONUTIL_H_
#define _CONVERSIONUTIL_H_

#include <favouritesitem.h>

class COMADSFolderObject;
class CVBookmarkConverter;
class CFolderItem;

class CBookmarkConversionUtil : public CBase
{
public:
    static CBookmarkConversionUtil* NewL();
    static CBookmarkConversionUtil* NewLC();
    
    ~CBookmarkConversionUtil();
    
    void FavouritesItemToBufferL( const CFavouritesItem& aItem,
        CBufFlat& aBuffer );
    
    void RssItemToBufferL( const CFolderItem& aItem,
        CBufFlat& aBuffer);
    
    TInt BufferToFavouritesItemL( CFavouritesItem::TType aType,
        CBufFlat& aBuffer, CFavouritesItem& aItem );
        
private:

    void ConstructL();

    COMADSFolderObject* iOmaConverter;
    CVBookmarkConverter* iVBookmarkConverter;
};

#endif
