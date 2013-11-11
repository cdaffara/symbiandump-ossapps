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
* Description:  Homescreen document loader class.
*
*/

#ifndef HSDOCUMENTLOADER_H
#define HSDOCUMENTLOADER_H

#include <hbdocumentloader.h>
#include "hstest_global.h"

HOMESCREEN_TEST_CLASS(TestHsDomainModel)

class HsDocumentLoader : public HbDocumentLoader
{
public:
    HsDocumentLoader();
    virtual ~HsDocumentLoader();

public:
    QObject *createObject(const QString &type, const QString &name);

private:

    Q_DISABLE_COPY(HsDocumentLoader)

    HOMESCREEN_TEST_FRIEND_CLASS(TestHsDomainModel)
};

#endif
