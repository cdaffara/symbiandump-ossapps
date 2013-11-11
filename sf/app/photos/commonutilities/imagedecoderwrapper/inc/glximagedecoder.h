/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/


#ifndef GLXIMAGEDECODER_H
#define GLXIMAGEDECODER_H
#include <QPixmap>
#include <f32file.h> 
#include "glximagedecoderwrapper.h"

class GlxImageDecoderWrapper;
class CImageDecoder;
class CFbsBitmap;
const TReal KTargetSize = 1000000;
//if any image is converted to Pixmap with an dimension > 2048 
//the conversion will fail
const TInt KMaxDimensionLimit = 2000;
class CGlxImageDecoder : public CActive
{
public:
	static CGlxImageDecoder* NewL(GlxImageDecoderWrapper* decoderWrapper);
	~CGlxImageDecoder();
    QSizeF DecodeImageL(QString aSourceFileName);
	QPixmap GetPixmap();
	void ResetDecoder();
public:    
    /**
     * Standard Active object functions
     * */
    virtual void RunL();
    virtual void DoCancel();
private:
    /*
     * Constructor
     * */
    CGlxImageDecoder();
	/*
     *Second-phase constuction 
     * */
    void ConstructL(GlxImageDecoderWrapper* decoderWrapper);

    /*
     * Checks if the mimetype needs recalculations
     * @returnType false if jpeg
     * true otheriwise 
     */
    TBool DoesMimeTypeNeedsRecalculateL(QString aSourceFileName);
    
    /*
     * Does the recalculation and returns back the correct size
     * @returntype Size of the decoded bitmap
     */
    TSize ReCalculateSizeL(QString aSourceFileName, TSize aDestSize);
    
private:
	GlxImageDecoderWrapper* iDecoderWrapper;
	/*Specifies the Decoder */
    CImageDecoder*          iImageDecoder; // decoder from ICL API
	/*Contains the Bitmap generated*/
    CFbsBitmap* iBitmap;
	/*Contains the bitmap in QT format8*/
	QPixmap iDecodedPixmap;
	/*A handle to a file server session.*/ 
    RFs iFs;

};
#endif //GLXIMAGEDECODER_H
