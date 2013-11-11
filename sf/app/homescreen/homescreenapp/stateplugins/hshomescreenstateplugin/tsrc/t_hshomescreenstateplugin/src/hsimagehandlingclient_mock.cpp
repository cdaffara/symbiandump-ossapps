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
* Description:  Mocked Photos image handling client.
*
*/


#include "hsimagehandlingclient.h"
#include <QVariant>

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
#ifdef Q_OS_SYMBIAN
HsImageHandlingClient::HsImageHandlingClient(QObject *parent)
    : QObject(parent),
      mReq(0)
#else
HsImageHandlingClient::HsImageHandlingClient(QObject *parent)
    : QObject(parent)
#endif
{
    setProperty("testSetToFail", QVariant(true));
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
HsImageHandlingClient::~HsImageHandlingClient()
{
#ifdef Q_OS_SYMBIAN
    delete mReq;
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsImageHandlingClient::fetch()
{
    if (property("testSetToFail").value<bool>() == true)
    {
        handleError(-1,"KErrNotFound");
    }
    else
    {
         handleOk("");
    }
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsImageHandlingClient::edit(const QString &imagePath)
{
    Q_UNUSED(imagePath);

    if (property("testSetToFail").value<bool>() == true)
    {
        handleEditorError(-1,"KErrNotFound");
    }
    else
    {
        handleEditorOk("");
    }    
}

// Aiw request responses
void HsImageHandlingClient::handleOk(const QVariant& result)
{
    emit fetchCompleted(result.toString());
}

void HsImageHandlingClient::handleError(int errorCode, const QString& errorMessage)
{
    Q_UNUSED(errorCode);
    Q_UNUSED(errorMessage);
    emit fetchFailed(-1, "KErrNotFound");
}

void HsImageHandlingClient::handleEditorOk(const QVariant& result)
{
    Q_UNUSED(result)
    emit editorCompleted();
}

void HsImageHandlingClient::handleEditorError(int errorCode,
                                             const QString& errorMessage)
{
    emit editorFailed(errorCode, errorMessage);
}
