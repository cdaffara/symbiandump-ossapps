/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    UString converter implementation class that converts symbian data 
*                types to UString types
*
*/




#ifndef GLXUSTRINGCONVERTER_H
#define GLXUSTRINGCONVERTER_H                           

#include "glxmedia.h"

namespace osncore
    {
    //template <class T> class AlfPtrVector;  
    class UString;
    }
using namespace osncore;

// ================================================
// DO NOT USE THIS CLASS IF NOT GLXDATAPROVIDER.DLL
// ================================================
//
/**
 * CGlxUStringConverter class that converts the symbian types to UString type
 */

NONSHARABLE_CLASS(CGlxUStringConverter): public CBase
{
public:

    /**
     * Constructor
     * @param aMedia The media object
     */
   
    CGlxUStringConverter();
    
    /**
     * Two phase construction
     * @param aMedia The media object
     */ 
    IMPORT_C static CGlxUStringConverter* NewL();
    
    /**
     * Two phase construction
     * @param aMedia The media object
     */
    static CGlxUStringConverter* NewLC();

    /**
     * The function that converts symbian types to UString
     * @param aMedia The media object
     * @param aAttribute The attribute used to extract the data to be converted
     * @param aFormatString Holds the information regarding the format in which data
     * should be.
     */
	IMPORT_C void AsStringL(const TGlxMedia& aMedia, const TMPXAttribute& aAttribute, TInt aFormatString, HBufC*& aString ) const;
	
private:
    
    ~CGlxUStringConverter();

	/**
     * Returns the item size in the required format
     */
	void GetFormattedItemSizeL(const CGlxMedia& media, HBufC*& aString) const;
	
	/**
     * Returns the item location in the required format
     * @param aCoordinate holds the coordinate information
     */
	void GetFormatLocationL( const TCoordinate& aCoordinate, HBufC*& aString )const;
	
	/**
     * Returns the item duration in the required format
     * @param aDuration holds the duration information
     */
	void GetFormattedDurationL( const TReal& aDuration, HBufC*& aString ) const;
	
	/**
     * Returns the item resolution in the required format
     * @param aSize holds the resolution size information
     */
	void GetFormatResolutionL( const TSize& aSize, HBufC*& aString ) const;
	
private:

   
};

#endif //GLXUSTRINGCONVERTER_H
