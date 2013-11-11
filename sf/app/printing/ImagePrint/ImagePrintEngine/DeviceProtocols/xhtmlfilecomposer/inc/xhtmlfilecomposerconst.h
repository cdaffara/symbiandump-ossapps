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
* Description:  Contains the constant definitions used for composing XHTML file.
*
*/


#ifndef XHTMLFILECOMPOSERCONST_H
#define XHTMLFILECOMPOSERCONST_H

/**
 * @name Constants
 */
//@{
/** Frame ratio defined for all image frames
 *
 * Paper is used maximally. That is, the image frames at certain layout
 * are calculated to fill the whole page what ever an aspect ratio of a frame will be.\n
 * It is wanted to use defined aspect ratio for all images at all layouts. It is more
 * effectual to perform an image at reasonable aspect ratio despiting of the unused space
 * left on page.
 */
const TReal KFrameRatio = 1.3325;

/** Relation between inches and millimieters.
 * @note 25.4 is the amount to multiply inch to get millimeters.
 */
const TReal KInchDivider = 25.4;

/** DPI (dots per inch) divider to convert the image size in pixels to the same unit as the paper is.*/
const TInt KDpiDivider = 150;
//@}

/**
 * @ref Constants
 */
//@{
/** Maximum size of template file */
const TInt KXFCMaxFileSize = 2048;

/** Maximum length for XHTML value to be set in final XHTML file */
const TInt KXFCMaxSettingLen = 110;

/** Maximum length for template element for XHTML block before assigning */
const TInt KXFCMaxBlockLen = 250;
//@}


/**
 * @ref Constants
 */
//@{
/** Degree value for image orientation*/
const TInt  KXFC270Deg = 270;
const TInt  KXFCZeroDeg = 0;
const TInt  KXFC90Deg = 90;
const TInt  KXFC180Deg = 180;
//@}

/**
 * @name Literal constants
 */
//@{
/// Default path for the XHTML-file
_LIT( KXFCDefaultResultPath, "c:\\system\\data\\xhtml\\" );

/// Mime type identifier for JPEG
_LIT8( KMimeJpeg, "image/jpeg" );

_LIT8(KMimeXhtml, "application/xhtml+xml");

/// Characters
_LIT8( KPros8, "%" );
_LIT8( KUnderScore8, "_" );
_LIT( KPros, "%" );
_LIT( KUnderScore, "_" );
_LIT8( KBackSlash, "\\" );
_LIT8( KSlash, "/" );

/// URL encode values
_LIT8( KAmpUrlEncoded, "%26"   );
_LIT8( KSemiColonUrlEncoded, "%3b" );
_LIT8( KSlashUrlEncoded, "%2f" );
_LIT8( KQuestionMarkUrlEncoded, "%3f" );
_LIT8( KAtUrlEncoded, "%40" );
_LIT8( KEqualsUrlEncoded, "%3d" );
_LIT8( KPlusUrlEncoded, "%2b" );
_LIT8( KDollarUrlEncoded, "%24" );
_LIT8( KCommaUrlEncoded, "%2c" );
_LIT8( KAposUrlEncoded, "%27" );
_LIT8( KLeftBracketUrlEncoded, "%5b" );
_LIT8( KRightBracketUrlEncoded, "%5d" );
_LIT8( KHashUrlEncoded, "%23" );
_LIT8( KBackSlashUrlEncoded, "%5c" );
_LIT8( KColonUrlEncoded, "%3a" );

//@}

/** @typedef 	typedef TBuf8<KXFCMaxSettingLen> TXhtmlSetting
 *  @brief 		Type definition for XHTML setting value string */
typedef TBuf8<KXFCMaxSettingLen> TXhtmlSetting;

/** @typedef 	typedef TBuf8<KXFCMaxBlockLen> TBlock
 *  @brief 		Type definition for XHTML element block */
typedef TBuf8<KXFCMaxBlockLen> TBlock;

/** @typedef 	typedef TBuf8<KMaxFileName> TFileName8
 *  @brief 		Type definition for 8-bit filename buffer */
typedef TBuf8<KMaxFileName> TFileName8;


/**
* @class 	TSizeReal
* @brief	A type class to store a size (width, height) with floating values.
*
*/
class TSizeReal
{
	public:
		//@{
		/// Constructors
		inline TSizeReal(){iWidth = 0; iHeight = 0;}
		inline TSizeReal(const TSize &a){iWidth = a.iWidth; iHeight = a.iHeight;}
		inline TSizeReal(const TReal aW, const TReal aH){iWidth = aW; iHeight = aH;}
		inline TSizeReal(const TInt aW, const TInt aH){iWidth = aW; iHeight = aH;}
		inline TSizeReal(const TReal aW, const TInt aH){iWidth = aW; iHeight = aH;}
		inline TSizeReal(const TInt aW, const TReal aH){iWidth = aW; iHeight = aH;}
		//@}

		inline void SetSize(const TReal aW, const TReal aH){iWidth = aW; iHeight = aH;}
		inline void SetSize(const TInt aW, const TInt aH){iWidth = aW; iHeight = aH;}
		inline void SetSize(const TReal aW, const TInt aH){iWidth = aW; iHeight = aH;}
		inline void SetSize(const TInt aW, const TReal aH){iWidth = aW; iHeight = aH;}

		// Arithmetic operators
		inline TSizeReal operator*(const TSizeReal& a) const
		{TSizeReal x; x.iWidth = iWidth * a.iWidth; x.iHeight = iHeight * a.iHeight; return x;}

		inline TSizeReal operator*(const TReal& a) const
		{TSizeReal x; x.iWidth = iWidth * a; x.iHeight = iHeight * a; return x;}

		inline TSizeReal operator/(const TReal& a) const
		{TSizeReal x; x.iWidth = iWidth / a; x.iHeight = iHeight / a; return x;}

		inline TSizeReal operator/(const TSizeReal& a) const
		{TSizeReal x; x.iWidth = iWidth / a.iWidth; x.iHeight = iHeight / a.iHeight; return x;}

		inline TSizeReal operator-(const TReal& a) const
		{TSizeReal x; x.iWidth = iWidth - a; x.iHeight = iHeight - a; return x;}

		inline TSizeReal operator-(const TSizeReal& a) const
		{TSizeReal x; x.iWidth = iWidth - a.iWidth; x.iHeight = iHeight - a.iHeight; return x;}

		inline TSizeReal operator+(const TReal& a) const
		{TSizeReal x; x.iWidth = iWidth + a; x.iHeight = iHeight + a; return x;}

		inline TSizeReal operator+(const TSizeReal& a) const
		{TSizeReal x; x.iWidth = iWidth + a.iWidth; x.iHeight = iHeight + a.iHeight; return x;}

		inline TSizeReal operator-=(const TReal& a)
		{iWidth = iWidth - a; iHeight = iHeight - a; return *this;}

		inline TSizeReal operator-=(const TSizeReal& a)
		{iWidth = iWidth - a.iWidth; iHeight = iHeight - a.iHeight; return *this;}

		inline TSizeReal operator+=(const TReal& a)
		{iWidth = iWidth + a; iHeight = iHeight + a; return *this;}

		inline TSizeReal operator+=(const TSizeReal& a)
		{iWidth = iWidth + a.iWidth; iHeight = iHeight + a.iHeight; return *this;}

		inline TSizeReal operator*=(const TReal& a)
		{iWidth = iWidth * a; iHeight = iHeight * a; return *this;}

		inline TSizeReal operator*=(const TSizeReal& a)
		{iWidth = iWidth * a.iWidth; iHeight = iHeight * a.iHeight; return *this;}

		inline TSizeReal operator/=(const TReal& a)
		{iWidth = iWidth / a; iHeight = iHeight / a; return *this;}

		inline TSizeReal operator/=(const TSizeReal& a)
		{iWidth = iWidth / a.iWidth; iHeight = iHeight / a.iHeight; return *this;}

		// Compare operators
		inline TBool operator==(const TSizeReal& a) const
		{if((iWidth == a.iWidth) && (iHeight == a.iHeight))return ETrue;
		 else return EFalse;}

		inline TBool operator!=(const TSizeReal& a) const
		{if((iWidth != a.iWidth) && (iHeight != a.iHeight))return ETrue;
		 else return EFalse;}

		inline TBool operator<=(const TSizeReal& a) const
		{if((iWidth <= a.iWidth) && (iHeight <= a.iHeight))return ETrue;
		 else return EFalse;}

		inline TBool operator>=(const TSizeReal& a) const
		{if((iWidth >= a.iWidth) && (iHeight >= a.iHeight))return ETrue;
		 else return EFalse;}

		inline TBool operator<(const TSizeReal& a) const
		{if((iWidth < a.iWidth) && (iHeight < a.iHeight))return ETrue;
		 else return EFalse;}

		inline TBool operator>(const TSizeReal& a) const
		{if((iWidth > a.iWidth) && (iHeight > a.iHeight))return ETrue;
		 else return EFalse;}

		inline TBool operator==(const TReal& a) const
		{if((iWidth == a) && (iHeight == a))return ETrue;
		 else return EFalse;}

		inline TBool operator!=(const TReal& a) const
		{if((iWidth != a) && (iHeight != a))return ETrue;
		 else return EFalse;}

		inline TBool operator<=(const TReal& a) const
		{if((iWidth <= a) && (iHeight <= a))return ETrue;
		 else return EFalse;}

		inline TBool operator>=(const TReal& a) const
		{if((iWidth >= a) && (iHeight >= a))return ETrue;
		 else return EFalse;}

		inline TBool operator<(const TReal& a) const
		{if((iWidth < a) && (iHeight < a))return ETrue;
		 else return EFalse;}

		inline TBool operator>(const TReal& a) const
		{if((iWidth > a) && (iHeight > a))return ETrue;
		 else return EFalse;}

		// Change width to height and vice versa
		inline void Reverse()
		{TSizeReal x(iHeight, iWidth); iWidth = x.iWidth; iHeight = x.iHeight;}

		// Get the ratio of of width vs. height
		inline TReal Ratio()
		{return iWidth/iHeight;}

	public:
		TReal iWidth;
		TReal iHeight;
};

#endif // XHTMLFILECOMPOSERCONST_H

//  End of File
