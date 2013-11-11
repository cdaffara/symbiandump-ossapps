/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  YUV format transcoder.
*
*/


#ifndef TVTYUVCONVERTER_H
#define TVTYUVCONVERTER_H

#include <e32base.h>

/**
 *  Yuv format transcoder.
 *
 *  Implements conversion between different YUV formats.
 *
 *  @lib VTImageConverter.lib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( VtYuvConverter )
    {
public:

    /**
     * Converts from 4:2:0 planar source image to 4:2:2 interleaved image.
     *
     * @since S60 v3.1
     * @param aSource Constant descriptor containing source image data.
     * @param aSize Size of the image in pixels.
     * @param aTarget Modifiable descriptor for target image data.
     * @param aStide Number of bytes per scanline in target buffer.
     * @return KErrNone if conversion was successful, KErrUnderflow if result
     * does not fit in target buffer or KErrArgument if source buffer's size
     * does not match with the given source size in pixels.
     */
    static TInt ConvertFrom420PlanarTo422Interleaved(
        const TDesC8& aSource,
        const TSize& aSize,
        TDes8& aTarget,
        TInt aStride );

private:

    /**
     * Constructor.
     */
    VtYuvConverter();

    /**
     * Copy constructor.
     */
    VtYuvConverter( const VtYuvConverter& );

    /**
     * Returns image size in bytes for 420 planar image that has given pixel
     * size.
     */
    static TInt ByteSize420Planar( const TSize& aSizeInPixels );

    /**
     * Returns image size in bytes for 422 interleaved image that has given
     * pixel size.
     */
    static TInt ByteSize422Interleaved( const TSize& aSizeInPixels );
    };

#endif // TVTYUVCONVERTER_H
