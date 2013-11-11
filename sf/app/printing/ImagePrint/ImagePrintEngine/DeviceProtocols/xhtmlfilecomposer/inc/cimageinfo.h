/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains the declarations for miscellanous image information.
*
*/


#ifndef CIMAGEINFO_H
#define CIMAGEINFO_H

#include "xhtmlfilecomposerconst.h"

/**
* @class 		CImageInfo
* @brief		A simple class for miscellaneous information of an image to be printed.
*
* This class contains information of the image, required in several phases of the process.
* The data included is not all required for composing XHTML file but also for other use of the
* image data from the printing job creation to the end of job printing.
*/
class CImageInfo : public CBase
{
	public:
		IMPORT_C CImageInfo();
		IMPORT_C CImageInfo(const CImageInfo& a);
		IMPORT_C ~CImageInfo();

		// statics
		IMPORT_C static CImageInfo* NewL();
		IMPORT_C static CImageInfo* NewL(const CImageInfo& a);
		IMPORT_C static CImageInfo* NewLC();
		IMPORT_C static CImageInfo* NewLC(const CImageInfo& a);


		IMPORT_C static void ConvertToUTF8L(const TDesC& aSource, TDes8& aResult);

		IMPORT_C static void ConvertToUnicodeL(const TDesC8& aSource, TDes& aResult);

		IMPORT_C static void AppendL(HBufC8* aWhere, const TDesC8& aWhat);
		
		IMPORT_C static void UriLog(const TDesC8& aUri);
		IMPORT_C static void UriLog(const TDesC& aUri);
		IMPORT_C static void UriLog(const TDesC8& aUri, TDes8& aConvert);
		IMPORT_C static void UriLog(const TDesC& aUri, TDes& aConvert);

		// non-statics
		IMPORT_C void EncodeUriL(const TDesC16& aDecodedUri, TDes8& aEncodedUri);
		IMPORT_C void EncodeUriL(const TDesC8& aDecodedUri, TDes8& aEncodedUri);

		IMPORT_C void SetUri(const TDesC8& aUri, const TBool aEncoded=EFalse);
		IMPORT_C void SetUriL(const TDesC& aUri);

		IMPORT_C void GetUri(TDes8& aUri) const;
		IMPORT_C void GetUriL(TDes& aUri) const;
		IMPORT_C void GetEncodedUriL(TDes8& aUri);
		IMPORT_C TBool CompareUri(const TDesC& aUri);
		IMPORT_C TBool CompareUri(const TDesC8& aUri);

		IMPORT_C void SetFilePath(const TDesC8& aFilePath);
		IMPORT_C void SetFilePathL(const TDesC& aFilePath);

		IMPORT_C void GetFilePath(TDes8& aFilePath) const;
		IMPORT_C void GetFilePathL(TDes& aFilePath) const;
		IMPORT_C TBool CompareFilePath(const TDesC& aFilePath);
		IMPORT_C TBool CompareFilePath(const TDesC8& aFilePath);

		IMPORT_C void SetId(const TInt aId);
		IMPORT_C TInt Id() const;

		IMPORT_C void SetCopies(const TInt aCopies);
		IMPORT_C TInt Copies() const;

		IMPORT_C void SetIndex(const TInt aIndex);
		IMPORT_C TInt Index() const;


	protected:
		IMPORT_C void ConstructL();

	private:
		IMPORT_C void EncodeFileNameL(TDes8& aString);

		/** @var 	TFileName8 iUri
		 *  @brief 	Image's shared uri used as a reference to the image. */
		TFileName8 iUri;
		TBool iEncoded;

		/** @var 	TFileName8 iFilePath
		 *  @brief	File location in local file system otherway zero length. */
		TFileName8 iFilePath;

		/** @var 	TInt iId
		 *  @brief 	Id of shared image. */
		TInt iId;

		/**  @var 	TInt iCopies
		 *  @brief 	Number of copies to print of this image */
		TInt iCopies;

		/** @var 	TInt iIndex
		 *  @brief 	Index for mapping the number of copies to correct image */
		TInt iIndex;
};

#endif // CIMAGEINFO_H

//  End of File
