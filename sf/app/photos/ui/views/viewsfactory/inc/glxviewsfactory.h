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

//Qt/Orbit Includes
class HbMainWindow;

//User defined forward Declarations
class GlxView;

//Includes
#include <qglobal.h>

#ifdef BUILD_GLXVIEWS
#define GLX_VIEW_EXPORT Q_DECL_EXPORT
#else
#define GLX_VIEW_EXPORT Q_DECL_IMPORT
#endif


class GLX_VIEW_EXPORT GlxViewsFactory
{
   
public :
    static GlxView * createView(qint32 viewId, HbMainWindow *window);
};

