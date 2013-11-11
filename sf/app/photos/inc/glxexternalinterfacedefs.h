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

//glxexternalinterfacedefs.h
#ifndef GLXEXTERNALINTERFACEDEFS_H
#define GLXEXTERNALINTERFACEDEFS_H
//parameters to be passes by the external Applications wishing to use photos to show their images.
//Currently it will mainly be used by camera 
#include <QDateTime>
#include <QSize>

struct GlxInterfaceParams  
{
QString imageUri;
QDateTime timestamp;
QPixmap* image; 
bool isSelected;
QSize imageDimensions; // actual image size( w,h) 
};

typedef QList<GlxInterfaceParams> GlxExternalData;

#endif //GLXEXTERNALINTERFACEDEFS_H
