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

#ifndef CXUIDOCUMENTLOADER_H_
#define CXUIDOCUMENTLOADER_H_

#include <hbdocumentloader.h>

class CxeEngine;

class CxuiDocumentLoader : public HbDocumentLoader
{
public:
    explicit CxuiDocumentLoader(CxeEngine *engine = NULL);

protected:
    QObject *createObject(const QString& type, const QString &name);

private:
    CxeEngine *mEngine;
};

#endif /* CXUIDOCUMENTLOADER_H_ */
