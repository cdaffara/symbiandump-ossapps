/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/



#ifndef CONFORMANCECHECKER_H
#define CONFORMANCECHECKER_H


//  INCLUDES
#include <e32base.h>
#include <e32std.h>

#include <gmxmldocument.h>
#include <gmxmlelement.h>


// CONSTANTS
_LIT(KMetaTag,"meta");
_LIT(KPercent,"%");
_LIT(KPx,"px");
_LIT(KMs,"ms");
_LIT(KImage,"Image");
_LIT(KText,"Text");

const TInt KUSAsciiMinCode = 0;
const TInt KUSAsciiMaxCode = 127;

//Type for the version of the Conformance Document
enum TMmsSmilVersion
	{
	EMmsSmil_v2_0=2,
	EMmsSmil_v3_0
	};


enum TFlags
	{
	EAllowVideoTag=1,				//Allowing  video tag as media
	EAllowAllAttributes=2,			//Not checking the attribute name of the elements
	EAllowNonMilliseconds=4,		//Allows other  timing specifications then 'ms' (e.g. 's')
	EAllowNonUsAscii=8,				//Allows other  than us-ascii values for src attribute
									//for media elements
	EAllowMixedRegionDimensions=16,	//Allows mixed region dimensions (pixel and percent)
	EAllowSeqTag=32,				//Allows one <seq> tag right after the <body> tag
	EAllowAnyRegionNames=64			//Allows any name for the regions
	};



// CLASS DECLARATION

/**
*  Class for checking the the content of a DOM tree against the Conformance Document.
*  (Checking legacy content.)
*
*  @lib smileng.lib
*  @since 2.0
*/
class CConformanceChecker : public CBase
	{

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
		* @param aStrict - value of the checking mode flag
        */
        IMPORT_C static CConformanceChecker* NewL();

        /**
        * Destructor.
        */
        ~CConformanceChecker();

    public: // New functions

        /**
	    * Checks if the DOM tree's content is legacy content or not.
        * @param aXmldoc - the root of the DOM tree
		* @aVersion - the version of the Conformance Document
		* @aFlags - the flags to gide the checking (combination of TFlags values)
        * @return - true if the content is legacy
				  - false  if the content is not legacy
        */
        TBool Check(CMDXMLDocument* aXmldoc, TMmsSmilVersion aVersion, TInt aFlags);

    private:

		/** Checks the structure of the head element: allowed to contain only layout and meta element
		* @param aHead - the root of head subtree
		* @param aStrict - flag for checking mode (true strict checking, false loose checking)
		* @return  - true - if head has legacy structure
		*		   - false - otherwise
		*/
		TBool CheckHeadStructure(CMDXMLNode* aHead);


		/** Checks the structure of the body element:
								- allowed to contain only par elements
		*						(one including seq element right after the body is allowed)
		* @param aBody - the root of body subtree
		* @return  - true - if body has legacy structure
		*		   - false - otherwise
		*/
		TBool CheckBodyStructure(CMDXMLNode* aBody);


		/** Checks the content of the layout element
		*						- root-layout and max. 2 region elements are allowed
		* @param aLayout - the root of layout subtree
		* @return  - true - if layout has legacy content
		*		   - false - otherwise
		*/
		TBool CheckLayoutContent(CMDXMLNode* aLayout);


		/** Checks the content of the body element
		* @param aBody - the root of body subtree
		* @return - true - if body has legacy content
		*		  - false - otherwise
		*/
		TBool CheckBodyContent(CMDXMLNode* aBody);


		/** Checks the content of the par element
		* @param aPar - the root of par subtree
		* @return  - true - if par has legacy content
		*		   - false - otherwise
		*/
		TBool CheckParContent(CMDXMLNode* aPar);


		/** Checks if  a descriptor is ending with 'ms'
		* @param aValue - value to check
		* @return  - true if the ending is ms
		*		   - false otherwise
		*/

		TBool IsInMilliseconds(TDesC& aValue) const;


		/** Checks if the given value is ending with percent or pixel
		* and if this is conforming to the already set pixel and percent values.
		* @param aValue - value to check,
		*				- should be the value of 'width', 'height', 'top', 'left'
		*				   attributes for a region
		* @param aPercent - true if the value should end with percent
		* @param aPixel - true if the value should be in pixel
		* @return  true - if the value is not mixed (regarding aPercent,aPixel)
		*		   false - if mixed
		*/
		TBool CheckMixedRegionAttribute(TDesC& aValue,TBool& aPercent,TBool& aPixel) const;

		/** Checks if the given value is exactly "Image" or "Text" and if it
		* is duplicated (checking the transmitted boolean parameters).
		* @param aValue - value to check,
		* @param aFoundImage - true if the "Image" value will be duplicate
		* @param aFoundText - true if the "Text" value will be duplicate
		* @return  true - if the value is "Image" or "Text" and it's not duplicated
		*		   false - otherwise
		*/
		TBool CheckRegionNames(TDesC& aValue,TBool& aFoundImage,TBool& aFoundText) const;


    private:    // Data


		TInt iFlags; //contains the flags to guide the checking
    };

#endif      // CONFORMANCECHECKER_H

// End of File
