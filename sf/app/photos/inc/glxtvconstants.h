/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Definition of TV Out constants
*
*/



/**
 * @internal reviewed 24/08/2007 by D Holland
 */


#ifndef __GLXTVCONSTANTS_H__
#define __GLXTVCONSTANTS_H__

/** The TV change type notifications */
enum TTvChangeType
    {
    ETvConnectionChanged = 0,
    ETvDisplayIsVisible,
    ETvDisplayNotVisible,
    EDisconnected
    };

/** TV size constants */
namespace glxTvOut
    {
    // Constants TV Size Values
    const TInt KGlxTvAspectNormal   = 0;
    const TInt KGlxTvAspectWide     = 1;
    
    //For tv display buffer
    const TInt KGlxTvOutWidth 	    = 640;
    const TInt KGlxTvOutHeight      = 480;
    const TInt KGlxTvOutWidthWide 	= 640;
    const TInt KGlxTvOutHeightWide  = 360;
    }

#endif //__GLXTVCONSTANTS_H__