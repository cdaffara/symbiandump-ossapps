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
* Description:  Moc impl for image fetcher api.
*
*/


#include "xqaiwgetimageclient.h"
#include "hsimagegridwidget.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
XQAIWGetImageClient::XQAIWGetImageClient():
    mImageGrid(0)
{
    setProperty("testSetToFail", QVariant(true));
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
XQAIWGetImageClient::~XQAIWGetImageClient()
{
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void XQAIWGetImageClient::fetch()
{
    if (property("testSetToFail").value<bool>() == true)
    {
        imageSelectionCancelled();
    }
    else
    {
        imageSelected(QString(""));
    }
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void XQAIWGetImageClient::imageSelected(const QString& val)
{
    emit fetchCompleted(val);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void XQAIWGetImageClient::requestCompleted(const QVariant& val)
{
    Q_UNUSED(val)
}

void XQAIWGetImageClient::imageSelectionCancelled()
{
    emit fetchFailed(-1, QString()); //KErrNotFound
}
