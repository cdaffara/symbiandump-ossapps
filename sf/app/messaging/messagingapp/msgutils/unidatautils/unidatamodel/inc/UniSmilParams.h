/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*  Reads default parameters from resource file for composing MMS SMIL
*
*
*/


#ifndef __UNISMILPARAMS_H
#define __UNISMILPARAMS_H

//  INCLUDES
#include <e32base.h>
#include <barsc.h>
#include <f32file.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  TUniSmilComposeParam
*  Class to hold general SMIL composing parameters
*
*  @lib     UniDataModel.lib
*  @since   3.1
*/
class TUniSmilComposeParam
    {
public:
    /**
     *  Default constructor
     */
    inline TUniSmilComposeParam();

    /**
     *  Copy constructor
     *  @param   aValues     Source region parameter object.
     */
    inline TUniSmilComposeParam(const TUniSmilComposeParam& aValues);

public: // data
    TInt iRootLayoutCompose;    // See r_root_layout_compose
                                // explanation in rss file.

    TInt iDisplayWidth;         // From resources
    TInt iDisplayHeight;        // From resources

    TBool iComposeXmlns;        // From resources
    TInt iSlideDuration;        // From resources
    TInt iTopPadding;           // From resources
    TInt iRegionPadding;        // From resources
    TInt iBottomPadding;        // From resources
    };


/**
*  TUniSmilRegionParam
*  Class to hold SMIL region attribute values.
*  Only supports attributes mentioned in SMIL confomance document.
*
*  @lib     UniDataModel.lib
*  @since   3.1
*/

class TUniSmilRegionParam
    {
public:
    /**
     *  Default constructor
     */
    inline TUniSmilRegionParam();

    /**
     *  Copy constructor
     *  @param   aValues     Source region parameter object.
     */
    inline TUniSmilRegionParam(const TUniSmilRegionParam& aValues);

    /**
    *  destructor.
    */
    inline ~TUniSmilRegionParam();

    /**
    *  Get region width.
    *  @return  Width of the region.
    */
    inline TInt Width();

    /**
    *  Get region height.
    *  @return  Height of the region.
    */
    inline TInt Height();

    /**
    *  Get region top.
    *  @return  Top co-ordinate of the region.
    */
    inline TInt Top();

    /**
    *  Get region left.
    *  @return  Left co-ordinate of the region.
    */
    inline TInt Left();

    /**
    *  Set region size.
    *  @param   aSize
    */
    inline void SetSize(const TSize& aSize);

    /**
    *  Set top left corner co-ordinates of the region.
    *  @param   aTl     Top-Left corner co-ordinates of region.
    */
    inline void SetTopLeft(const TPoint& aTl);

public: // Percent values
    TInt    iTopPercent;
    TInt    iLeftPercent;
    TInt    iWidthPercent;
    TInt    iHeightPercent;
    HBufC   *iId;
    HBufC   *iFit;
    // Pixel values
    TSize   iMaxSize;
    TSize   iMinSize;
    TSize   iSize;
    TPoint  iTopLeft;

private:

    };


/**
*  CUniSmilParams
*  Reads MMS SMIL composing default parameters from resources.
*
*  @lib     UniDataModel.lib
*  @since   3.1
*/
class CUniSmilParams : public CBase
    {
public:  // Constructors and destructor

    /**
    *  Two-phased constructor.
    *  @param   aSession     File system session handle to be used.
    */
    IMPORT_C static CUniSmilParams* NewL( const RFs& aSession );
        
    /**
    *  Two-phased constructor with cleanup stack.
    *  @param   aSession     File system session handle to be used.
    */
    IMPORT_C static CUniSmilParams* NewLC( const RFs& aSession );
        
    /**
    *  Destructor.
    */
    virtual ~CUniSmilParams();

public: // New functions

protected:  // Functions from base classes


private:

    /**
    *  By default Symbian OS constructor is private.
    */
    void ConstructL();

    /**
    *  C++ default constructor is private.
    *  @param   aSession     File system session handle to be used.
    */
    CUniSmilParams( const RFs& aSession );

    /**
    *  Reads the region string and integer constants
    *  from the resource file.
    */
    void ReadResourcesL();

    /**
    *  Opens the resource file for reading
    *  @param   aResFile     Resource file handle to be used.
    */
    void OpenResourceFileL( RResourceFile& aResFile );

    /**
    *  Reads general composing params from the reousrce file.
    *  @param   aResFile     Resource file handle to be used.
    */
    void ReadComposeParamResourcesL(
            const RResourceFile& aResFile );

    /**
    *  Reads image region defaults from the resource file.
    */
    void ReadImageRegionResourcesL(
            const RResourceFile& resourceFile );

    /**
    *  Reads text region defaults from the resource file.
    *  @param   aResFile     Resource file handle to be used.
    */
    void ReadTextRegionResourcesL(
            const RResourceFile& resourceFile );


private:    // Data
    const RFs&              iSession;

public:    // Data
    // General compose parameters
    TUniSmilComposeParam    *iComposeParams;
    TUniSmilRegionParam     *iImageRegionParams;
    TUniSmilRegionParam     *iTextRegionParams;

    };

#include <UniSmilParams.inl>

#endif // __UNISMILPARAMS_H

// End of File
