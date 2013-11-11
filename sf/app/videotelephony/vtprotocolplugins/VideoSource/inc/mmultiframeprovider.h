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
* Description:  Video Source subsystem.
*
*/


#ifndef M_MULTIFRAMEPROVIDER_H
#define M_MULTIFRAMEPROVIDER_H

// INCLUDE FILES
#include <e32base.h>

// FORWARD DECLARATIONS
class CVtImageBitmap;

// ============================ TImageShareDataBuffer ===============================
/**
*	Share buffer class
* @lib videosource.lib
*/
NONSHARABLE_CLASS( TImageShareDataBuffer)
	{
	public :
			CVtImageBitmap* iBitmap;
			TBool iIsBitmapFree;		
	};

 // General Settings image type
 enum TGeneralSettingsImageType
 			{
      EGSStillImage,
      EGSDefaultStillImage
      };

// ============================ MMultiframeprovider ===============================
/**
*	Multiframe observer API
* Interface for receiving data and acknowledgements from multiframe provider.
* @lib videosource.lib
*/
class MMultiframeprovider
	{
	public:
	/**
  * Indicates when initialization of multiframeprovider has finished
  */
	virtual void InitializeReady() = 0;
	
	/**
  * Refresh YUV data
  * @param "aYUVBitMap" reference new YUV data.
  */
	virtual void RefreshYUVData( TImageShareDataBuffer& aYUVBitMap ) = 0;
	
	/**
  * Refresh VF data
  * @param "aVFBitMap" reference new VF data.
  */
	virtual void RefreshViewFinder( TImageShareDataBuffer& aVFBitMap ) = 0;
	
	/**
  * Notifies if error occures in multiframeprovider
  * @param "aError" symbian wide error code.
  */
	virtual void NotifyImageHandlingError( TInt aError ) = 0;
		
	};
#endif //M_MULTIFRAMEPROVIDER_H

// End of File 
