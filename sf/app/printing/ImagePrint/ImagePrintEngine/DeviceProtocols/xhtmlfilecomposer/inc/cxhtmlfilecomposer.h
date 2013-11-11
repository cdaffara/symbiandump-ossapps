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
* Description:  Defines the CXhtmlFileComposer class.
*
*/


#ifndef CXHTMLFILECOMPOSER_H
#define CXHTMLFILECOMPOSER_H

#include <e32base.h>
#include <f32file.h>
#include <e32math.h>

#include "xfctemplates.h"
#include "xhtmlfilecomposerconst.h"
#include "cimageinfo.h"

/**
* @class 		CXhtmlFileComposer
* @brief	Implements the printing job compose to a XHTML file.
*/

/**
* @example ExampleOfCreateXhtmlFile.cpp
* This example
*	@li creates a list of images to be included in XHTML-file as CImageInfo classes
*	@li creates a new instance of this class
*	@li calls the CreateXhtmlFileL method to create the physical file
*/

class CXhtmlFileComposer : public CBase
	{
		public:

			/**
			* @enum 	TMeasureUnit
			*
			* The measure type is used in XHTML file for describing the unit for dimensions like
			* page, frame and image sizes, frame positions at the page etc.
			* Depending of the paper size type the two units of the measure is used: inches and millimeters.
			*/
			enum TMeasureUnit
			{
				EUAInches, 		///<@brief Inch unit identifier.
				EUAMillis  		///<@brief Millimeter unit identifier.
			};

		public:

			/**  		NewL
			* @brief	Creates a new instance of XHTML file composer.
			* @param	aPath	The path the XHTML file will be located.\n
			*					If @a aPath is empty, the @ref KXFPDefaultResultPath is used to open the file
			*					server session and to create path. Otherway the file name given in method
			*					@ref CreateXhtmlFileL can be the name of the file without path.
			* @return	Pointer to the instance.
			*/
			IMPORT_C static CXhtmlFileComposer* NewL(const TDesC& aPath);

			/**  		NewLC
			* @brief	Creates a new instance of XHTML file composer and leaves it in the cleanup stack.
			* @param	aPath	The path the XHTML file will be located.\n
			*					If @a aPath is empty, the @ref KXFPDefaultResultPath is used to open the file
			*					server session and to create path. Otherway the file name given in method
			*					@ref CreateXhtmlFileL can be the name of the file without path.
			* @return	Pointer to the instance.
			*/
			IMPORT_C static CXhtmlFileComposer* NewLC(const TDesC& aPath);

			/**  		~CXhtmlFileComposer
			* @brief	Destructor.
			*/
			IMPORT_C ~CXhtmlFileComposer();

			/**			CreateXhtmlFileL
			* @brief	Writes the XHTML-Print document of the job according to the given parameters.
			* @param	aImages 	Array of the images to write in file
			* @param	aLayout 	Jobs' <em>Number up</em> setting
			* @param	aPaperSize 	Jobs' <em>Media size</em> setting
			* @param	aQuality	Jobs' <em>Quality</em> setting <b>not implemented</b>
			* @param	aXhtmlPrintFile The name of the file to generate. The file name can be given
			*						without the path, when the file is located in the @a aPath given when the
			*						instance of this class is created. The @a aXhtmlPrintFile can be also given
			*						with the path, but then the path must already exist.
			* @param	aPageCount	Returns the number of pages to print
			*/
			IMPORT_C void CreateXhtmlFileL(const RArray<CImageInfo>& aImages, const TInt aLayout,
								  const TInt aPaperSize, const TInt aQuality,
								  const TDesC& aXhtmlPrintFile, TInt& aPageCount, TBool aDataFile = EFalse);


		protected:

			/**  		ConstructL
			* @brief	Symbian second phase constructor.
			* @param	aPath	The path the XHTML file will be located.\n
			*					If @a aPath is empty, the @ref KXFPDefaultResultPath is used to open the file
			*					server session and the path is created. This ensures the argument @a aXhtmlPrintFile
			*					of the method @ref CreateXhtmlFileL can be given without path.
			*/
			void ConstructL(const TDesC& aPath);


		private:

			/**  		CXhtmlFileComposer
			* @brief	C++ constructor
			*/
			CXhtmlFileComposer();

			/**  		UnitToPixel
			* @brief	Converts current unit to pixels.
			* @param	aValue Inch/millimeter value
			* @return	Pixel value
			*/
			IMPORT_C TReal UnitToPixel(TReal aValue);

			/**  		UnitToPixel
			* @brief	Converts current unit to pixels.
			* @param	aValue Inch/millimeter value
			* @return	Pixel value
			*/
			IMPORT_C TSizeReal UnitToPixel(TSizeReal aValue);

			/**  		PixelToUnit
			* @brief	Converts pixels to current unit.
			* @param	aValue pixel value
			* @return	Inch/millimeter value
			*/
			IMPORT_C TReal PixelToUnit(TReal aValue);

			/**  		PixelToUnit
			* @brief	Converts pixels to current unit.
			* @param	aValue pixel value
			* @return	Inch/millimeter value
			*/
			IMPORT_C TSizeReal PixelToUnit(TSizeReal aValue);

			/**   		ReadTemplateL
			* @brief	Reads the template file in iFileBuf
			*/
			void ReadTemplateFileL();

			/**   		GetRatioL
			* @brief	Counts the croping and scaling of the image
			* @param	aNewSize 	Cropped and scaled size for the image
			* @param	aImageSize 	Full string to define the margin in XHTML file
			*/
			void GetRatioL(TSizeReal &aNewSize, TSizeReal aImageSize);

			/**   		ComposeUnitL
			* @brief	Converts the template to current unit (millimeters or inches) in member buffer.
			*/
			void ComposeUnitL();

			/**   		ComposeTemplateL
			* @brief	Replaces the template with string value in member buffer
			* @param	aStr	String to convert for
			* @param	aTemplate	Template to convert
			*/
			void ComposeTemplateL(const TDesC8 &aStr, const TDesC8 &aTemplate);

			/**   		ComposeTemplateL
			* @brief	Replaces the template with number value in member buffer
			* @param	aValue Number to convert for
			* @param	aTemplate Template to convert
			*/
			void ComposeTemplateL(const TInt aValue, const TDesC8 &aTemplate);

			/**   		ComposeFileL
			* @brief	Composes the data in the template in member buffer
			* @param	aImageName Image name and path
			* @param	aImgSize Image size
			*/
			void ComposeFileL(const TDes8 &aImageName, TSizeReal aImgSize, TBool aDataFile = EFalse);

			/**   		PageData
			* @brief	Collects the paper size related data to page arguments
			* @param	aPaperSize Media size
			*/
			void PageData(TInt aPaperSize);

			/**   		LayoutData
			* @brief	Collects the layout related data to image frame arguments
			* @param	aLayout Layout
			* @param	aPositions Array of the image positions on the page
			*/
			void LayoutData(TInt aLayout, RArray<TSizeReal> &aPositions);

			/**   		FinalizeBufferL
			* @brief	Trims the member buffer tail. Replacing the template strings from
			*			the buffer makes the data length in the buffer vary and generates
			*			rubbish in the end of buffer.
			* @return	Pointer to the buffer data.
			*/
			TPtrC8 FinalizeBufferL();

			/**   		SetPageTypeL
			* @brief	Creates a new XHTML div for page. Last/only one must be always @c page_last.
			*/
			void SetPageTypeL();

			/**   		GetTemplateStringL
			* @brief	Reads the requested element's template for the XHTML file.
			* @param	aStart Start marker tag for the element
			* @param	aEnd End marker tag for the element
			* @param	aTemplate Reference to the template element read
			* @return	Pointer to the template element read
			*/
			TPtrC8 GetTemplateStringL(const TDesC8& aStart, const TDesC8& aEnd, TDes8& aTemplate);

			/**   		AddTemplateStringL
			* @brief	Adds the template element in XHTML buffer
			* @param	const TDesC8& The template element to add
			* @param	const TDesC8& A marker tag to where to add the element
			*/
			void AddTemplateStringL(const TDesC8& aTemplate,const TDesC8& aTag);

			/**   		GetImageSize
			* @brief	Returns the size in pixels of the image file
			* @param	CImageInfo Image's info
			* @return	Image size
			*/
			TSizeReal GetImageSizeL(CImageInfo aImage);

			/**   		AddPageL
			* @brief	Adds the new page element in XHTML template if last one is full
			* @param	aCurrentImage Index of the current image
			* @param	aPositionIndex Reference to the index of the image position at page
			* @param	aPageCount Reference to the page counter
			* @return	True if page added, False if not.
			*/
			TBool AddPageL(TInt aCurrentImage, TInt& aPositionIndex, TInt& aPageCount);

			/**   		CreateMatrix
			* @brief	Creates a list of positions at page for the grid of images.
			* @param	aRows Number of rows at one page
			* @param	aColumns Number of columns at one page
			* @param	aPortrait Orientation of the paper vs. images
			* @param	aPositions Reference to the array of the positions
			*/
			void CreateMatrix(TInt aRows, TInt aColumns, RArray<TSizeReal>& aPositions);

			/**   		GetImageNameL
			* @brief	Returns the name of the image for IMG tag.
			* @param	aImageInfo 	Image details
			* @param	aImageName	Reference to the image name
			*/
			void GetImageNameL(CImageInfo aImageInfo, TDes8 &aImageName, TBool aDataFile = EFalse);

			/**   		SizeAndOrientationL
			* @brief	Returns the size and orientation of the image.
			* @param	aImageInfo	Image details
			* @param	aImageSize	Reference to the image size
			* @param	aFramePortrait Returns True if image is portrait, otherway False
			*/
			void SizeAndOrientationL(const CImageInfo aImageInfo, TSizeReal& aImageSize, TBool& aFramePortrait);

			/**   		ComposePositionL
			* @brief	Composes the position string identifier values in XHTML file.
			* @param	aPositionIndex index of the position to compose
			* @param	aPositions The array of the positions
			*/
			void ComposePositionL(TInt aPositionIndex, RArray<TSizeReal>& aPositions);

			/**   		FormatReal
			* @brief	Composes the position string identifier values in XHTML file.
			* @param	aNumber float number to convert
			* @param	aString Converted string type of "%f<<units>>"
			*/
			void FormatReal(TRealX aNumber, TDes8& aString);

		private:

			/** @var TSizeReal iPaperSize
			 *  Paper size to print */
			TSizeReal iPaperSize;

			/** @var TInt iPaperType
			 *  Measurement unit (millimeters or inches) in XHTML file */
			TInt iPaperType;

			/** @var TReal iPageMargin
			 *  Marginals at the page */
			TReal iPageMargin;

			/** @var TSizeReal iImageFrame
			 *  Frame to contain an image */
			TSizeReal iImageFrame;

			/** @var HBufC8* iFileBuf
			 *  Buffer to read the template file */
			HBufC8* iFileBuf;

			/** @var RFs iFsSession
			 *  File server session */
			RFs iFsSession;

			/** @var TInt iImagesPerPage
			 * Number of the images at one page */
			TInt iImagesPerPage;

			/** @var TBool iImagePortrait
			 * The orientation of the image. TRUE if portrait, FALSE if landscape */
			TBool iImagePortrait;

			/** @var TBool iPagePortrait
			 * The orientation of the page. TRUE if portrait, FALSE if landscape */
			TBool iPagePortrait;

			/** @var TBlock iPageTemplate
			 * Page element template of the XHTML file */
			TBlock iPageTemplate;

			/** @var TBlock iFrameTemplate
			 *  Frame element template of the XHTML file */
			TBlock iFrameTemplate;

			/** @var TBlock iPositionTemplate
			 *  Image position element template of the XHTML file */
			TBlock iPositionTemplate;

			/** @var TBlock iImageTemplate
			 *  Image element template of the XHTML file */
			TBlock iImageTemplate;
	};

#endif // CXHTMLFILECOMPOSER_H

//  End of File
