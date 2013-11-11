/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Stub declaration for CAlfCompositionSource
*
*/

// Version : %version: 1 %



// This file defines the API for .dll

#ifndef __ALFCOMPOSITIONUTILITY_H__
#define __ALFCOMPOSITIONUTILITY_H__

//  Include Files

#include <e32base.h>	// CBase
#include <e32std.h>	 // TBuf


class RWindow;


//  Class Definitions

class CAlfCompositionSource : public CBase
    {

public:

    static CAlfCompositionSource* NewL( RWindow& aClientWindow );
    
    ~CAlfCompositionSource();

    /** 
    * Enables / disables alpha on composition target 
    **/
    TInt EnableAlpha( TBool aEnable = ETrue );
    
    };

#endif // #define __ALFCOMPOSITIONUTILITY_H__

// EOF
