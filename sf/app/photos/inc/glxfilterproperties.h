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
* Description:   Represents a set of filter properties
*
*/



#ifndef T_GLXFILTERPROPERTIES_H
#define T_GLXFILTERPROPERTIES_H

// INCLUDES
#include <glxmediaid.h>
#include <glxfiltergeneraldefs.h>

// FORWARD DECLARATIONS
class CMPXCollectionPath;

// CONSTANTS

// CLASS DECLARATION


class TGlxFilterProperties 
	{
public:
	inline TGlxFilterProperties () :
				iSortOrder(EGlxFilterSortOrderNotUsed),
				iSortDirection(EGlxFilterSortDirectionNotUsed),
				iIncludeCameraAlbum(ETrue),
				iMinCount(0),
				iContainsItem(0),
				iExcludeAnimation(EFalse),
				iLastCaptureDate(EFalse),
				iMaxCount(KMaxTUint),
				iThumbnailLoadability(0,0),
				iPath(NULL),
				iItemType(EGlxFilterVideoAndImages),
				iPromoteSystemItems(EFalse),
				iOrigin(EGlxFilterOriginNotUsed),
				iUri(NULL),
				iStartDate(0),
				iEndDate(0),
				iNoDRM(EFalse),
				iMimeType(NULL)
				  {}

public:
	TGlxFilterSortOrder iSortOrder;
	TGlxFilterSortDirection iSortDirection;
	TBool iIncludeCameraAlbum;
	TInt iMinCount;
	TGlxMediaId iContainsItem;
	TBool iExcludeAnimation;
	TBool iLastCaptureDate;
	TInt iMaxCount;
	TSize iThumbnailLoadability;
	CMPXCollectionPath* iPath; 
	TGlxFilterItemType iItemType;
	TBool iPromoteSystemItems;
	TGlxFilterOrigin iOrigin;
	const TDesC* iUri;
	TTime iStartDate;
	TTime iEndDate;
	TBool iNoDRM;
	const TDesC* iMimeType;
	};


#endif // T_GLXFILTERPROPERTIES_H
