/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   unit test factory function
*
*/



/**
 * @internal reviewed 5/02/2007 by Alex Birkett
 */
 
//  CLASS HEADER
#include "t_glxzoomstatepublisher.h"

//  EXTERNAL INCLUDES
#include <CEUnitTestSuite.h>

EXPORT_C MEUnitTest* CreateTestSuiteL()
    {
    return T_GlxZoomStatePublisher::NewL();
    }

//  END OF FILE
