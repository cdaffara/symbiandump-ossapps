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

#ifndef HBSTUB_HELPER_H
#define HBSTUB_HELPER_H


/**
* Helper class to control logsdbconnector stub behavior
*
*/ 
class HbStubHelper 
{
    public:
        static void reset();
        static QString notificationDialogTxt();
        static void stringUtilDigitConversion(bool enabled);
};

#endif
