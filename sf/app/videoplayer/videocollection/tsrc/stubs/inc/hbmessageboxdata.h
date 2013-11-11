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
* Description:  HbMessageBox class declaration.
*
*/


#ifndef HBMESSAGEBOXDATA_H
#define HBMESSAGEBOXDATA_H

class HbMessageBoxData
{
public:
    static void reset()
    {
        mQuestionReturnValue = false;
        mLatestTxt = "";
        mWarningCallCount = 0;
        mInformationCallCount = 0;
        mType = -1;
        mAttribute = -1;
        mOpenCallCount = 0;
        mShowCallCount = 0;
    }
    
public: // data
    static bool mQuestionReturnValue;
    static QString mLatestTxt;
    static int mWarningCallCount;
    static int mInformationCallCount;
    static int mType;
    static int mAttribute;
    static int mOpenCallCount;
    static int mShowCallCount;
};

#endif // HBMESSAGEBOXDATA_H_
