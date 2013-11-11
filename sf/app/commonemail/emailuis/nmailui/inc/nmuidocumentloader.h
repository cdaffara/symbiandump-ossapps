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

#ifndef NMDOCLOADER_H_
#define NMDOCLOADER_H_

#include <QObject>
#include <QString>
#include <hbdocumentloader.h>

class HbMainWindow;

class NmUiDocumentLoader : public HbDocumentLoader
{
public:
    NmUiDocumentLoader(const HbMainWindow *window);
    ~NmUiDocumentLoader();
protected:
    QObject *createObject(const QString& type, const QString &name);

};

#endif /* NMDOCLOADER_H_ */
