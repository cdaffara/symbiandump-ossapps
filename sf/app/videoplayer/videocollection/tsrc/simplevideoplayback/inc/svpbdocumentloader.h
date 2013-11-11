/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Document loader of the SimpleVideoPlayback MPX view plugin.
*
*/

#ifndef SVPBDOCUMENTLOADER_H_
#define SVPBDOCUMENTLOADER_H_

#include <hbdocumentloader.h>

class SvpbDocumentLoader : public HbDocumentLoader
{
public:
    SvpbDocumentLoader();

private: // from HbDocumentLoader
    QObject *createObject(const QString& type, const QString &name);

};

#endif /* SVPBDOCUMENTLOADER_H_ */
