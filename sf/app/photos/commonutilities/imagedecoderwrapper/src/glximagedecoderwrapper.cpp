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
#include "glximagedecoder.h"
#include "glximagedecoderwrapper.h"

// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// constructor.
// ---------------------------------------------------------------------------
//

GlxImageDecoderWrapper::GlxImageDecoderWrapper()
{
	imageDecoder = CGlxImageDecoder::NewL(this);
}


// ---------------------------------------------------------------------------
// destructor.
// ---------------------------------------------------------------------------
//
GlxImageDecoderWrapper::~GlxImageDecoderWrapper()
{
	imageDecoder->Cancel();
	delete imageDecoder;
}

// ---------------------------------------------------------------------------
// decodeImage.
// ---------------------------------------------------------------------------
//
QSizeF GlxImageDecoderWrapper::decodeImage(QString sourcePath)
{
	return imageDecoder->DecodeImageL(sourcePath);
}

QPixmap GlxImageDecoderWrapper::getPixmap()
{
	return imageDecoder->GetPixmap();
}

void GlxImageDecoderWrapper::resetDecoder()
{
	imageDecoder->ResetDecoder();
}

void GlxImageDecoderWrapper::decodedImageAvailable()
{
	emit pixmapDecoded();
}
