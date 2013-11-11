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
* Description: 
*
*/

#ifndef GLXIMAGEDECODERWRAPPER_H
#define GLXIMAGEDECODERWRAPPER_H
#include <QObject>
#include <QtGlobal>
#include <QPixmap>
#ifdef BUILD_IMAGEWRAPPER
#define GLX_IMAGEWRAPPER_EXPORT Q_DECL_EXPORT
#else
#define GLX_IMAGEWRAPPER_EXPORT Q_DECL_IMPORT
#endif
class CGlxImageDecoder;
class GLX_IMAGEWRAPPER_EXPORT GlxImageDecoderWrapper : public QObject
{
    Q_OBJECT
public:
	/**
	*constructor
	*/
	GlxImageDecoderWrapper();
	~GlxImageDecoderWrapper();
	QSizeF decodeImage(QString sourcePath);
	QPixmap getPixmap();
	void resetDecoder();
	void decodedImageAvailable();
signals:
     void pixmapDecoded();
private:
	CGlxImageDecoder* imageDecoder;
};

#endif //GLXIMAGEDECODERWRAPPER_H
