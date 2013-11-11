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

#ifndef VIDEOOPERATORSERVICE_PDATA_H
#define VIDEOOPERATORSERVICE_PDATA_H

#include <qlist.h>
#include <qstring.h>

class VideoOperatorServicePrivateData
{
public: // methods
    static void reset()
    {

    }
    
public: // data
    
    static bool mLoadReturnValue;
    static QString mTitleReturnValue;
    static QString mIconResourceReturnValue;
    static int mLaunchServiceCallCount;
    
};

#endif /* VIDEOOPERATORSERVICE_PDATA_H */
