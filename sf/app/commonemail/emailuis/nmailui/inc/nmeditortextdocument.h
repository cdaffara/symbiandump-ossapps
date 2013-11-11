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


#ifndef NMEDITORTEXTDOCUMENT_H_
#define NMEDITORTEXTDOCUMENT_H_

#include <QTextDocument>

class QNetworkAccessManager;
class QNetworkReply;

class NmEditorTextDocument : public QTextDocument
{
    Q_OBJECT
public:
    NmEditorTextDocument(QNetworkAccessManager &manager);
    ~NmEditorTextDocument();

public slots:
    void replyFinished(QNetworkReply *reply);

protected:
    virtual QVariant loadResource(int type, const QUrl &name);

private:
    QNetworkAccessManager &mManager; // not owned
    QList<QNetworkReply*> mReplyList;
};

#endif /* NMEDITORTEXTDOCUMENT_H_ */
