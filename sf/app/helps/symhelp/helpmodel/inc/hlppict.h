// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __HLPPICT_H__
#define __HLPPICT_H__

// System includes
#include <e32std.h>
#include <d32dbms.h>
#include <s32file.h>
#include <badesca.h>
#include <gdi.h> // for CPicture

// User includes
#include "HLPMODEL.H"

// Classes Referenced
class CHlpDatabase;

// Constants
const TUid KUidHelpImage = { 0x100068D4 };



//
// ----> CHlpPicture (header)
//
class CHlpPicture : public CPicture
	/**
	This class provides the means for restoring bitmaps from a multiple
	bitmap file, scaling them and drawing them on the screen.
	@publishedAll
	@class CHlpPicture
	@since 7.0
	@see CPicture, CStreamStore, TStreamId, CHlpModel,
				  CGraphicsContext, RWriteStream, CHlpDatabase
	*/
	{
//
public: // CONSTRUCT / DESTRUCT
//
	static CHlpPicture* NewLC(const CStreamStore& aStore, TStreamId aStreamId, CHlpDatabase& aDatabase, CHlpModel& aHlpModel);
	~CHlpPicture();
	void HandleHelpModelDestruction();

//
public: // FROM CPicture
//
	void Draw(CGraphicsContext& aGc, const TPoint& aTopLeft, const TRect& aClipRect, MGraphicsDeviceMap* aMap) const;
	void ExternalizeL(RWriteStream& aStream) const;
	void GetCropInTwips(TMargins& aMargins) const;
	void GetOriginalSizeInTwips(TSize& aSize) const;
	void SetScaleFactor(TInt aScaleFactorWidth, TInt aScaleFactorHeight);
	void SetCropInTwips(const TMargins& aMargins);
	TInt ScaleFactorWidth() const;
	TInt ScaleFactorHeight() const;

//
public: // Handles changes in zoom size
//
	void HandleZoomChangedL(THlpZoomState aNewZoom);

//
private: // INTERNAL CONSTRUCT
//
	CHlpPicture(CHlpDatabase& aDatabase, CHlpModel& aHlpModel);
	void ConstructL(const CStreamStore& aStore, TStreamId aStreamId);

//
private: // INTERNAL METHODS
//
	void RestoreL(const CStreamStore& aStore, TStreamId aStreamId, CHlpDatabase& aDatabase, THlpZoomState aZoomState);

//
private: // MEMBER DATA
//

	// Variable to represent the bitmap to be restored from the image table
	CFbsBitmap* iImage;

	// Reference to a 'CHlpDatabase' object
	CHlpDatabase& iDatabase;

	// Stores the ID of every image that is accessed
	TInt iImageId;

	// Pointer to a 'hlpmodel' object
	CHlpModel* iHlpModel;

	// Stores the number of bitmaps per picture
	TInt iImageCountForPicture;
	};



#endif
