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
* Description: Screensaver custom document loader.
*
*/

#ifndef SNSRDOCUMENTLOADER_H
#define SNSRDOCUMENTLOADER_H

#include <hbdocumentloader.h>

#include "snsrtest_global.h"

SCREENSAVER_TEST_CLASS(T_SnsrBigClockScreensaverPlugin)

class SnsrDocumentLoader : public HbDocumentLoader
{

private:

    virtual QObject *createObject(const QString &type, const QString &name);

    SCREENSAVER_TEST_FRIEND_CLASS(T_SnsrBigClockScreensaverPlugin)
    
};

#endif // SNSRDOCUMENTLOADER_H
