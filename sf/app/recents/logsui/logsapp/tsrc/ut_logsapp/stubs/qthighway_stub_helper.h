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

/**
* Helper class to control logcli stub behavior
*
*/ 
class QtHighwayStubHelper{
    public:
        static void reset();
        static QString service();
        static QString message();
        static bool utilToBackground();
        static void setIsService(bool isService);
};
