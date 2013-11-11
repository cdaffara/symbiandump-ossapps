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
* Description: Message editor contents widget
*
*/

#ifndef NMEDITORCONTENT_H_
#define NMEDITORCONTENT_H_

#include <hbwidget.h>
#include "nmuiviewids.h"

class HbAnchorLayout;
class HbTextEdit;
class HbDocumentLoader;
class NmBaseViewScrollArea;
class NmMessage;
class NmMessageEnvelope;
class NmEditorHeader;
class NmEditorTextEdit;
class QNetworkAccessManager;
class NmApplication;

class NmEditorContent : public QObject
{
Q_OBJECT

public:
    NmEditorContent(QObject *parent,
                    HbDocumentLoader *documentLoader,
                    QNetworkAccessManager &manager,
                    NmApplication &application);

    virtual ~NmEditorContent();

    void setBodyContent(NmUiEditorStartMode editorStartMode,
                        const NmMessage *originalMessage,
                        const QString *signature);

    NmEditorTextEdit* editor() const;

    NmEditorHeader* header() const;
    
public:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    void createConnections();
    void removeEmbeddedImages(QString &bodyContent);
    void convertBodyStylesToDivision(QString &bodyContent);
    void createDivisionFromHead(QString &bodyContent);
    void convertBodyToDiv(QString &bodyContent, const QString &replacementString);
    void convertBGColorToStyle(QString &bodyContent);
    

signals:
    void setPlainText(const QString&);

    void setHtml(const QString&);

public slots:
    void setEditorContentHeight();
    void setEditorContentWidth();
    void ensureCursorVisibility();
    void repositHeader(const QPointF &scrollPosition);

private:
    NmEditorHeader *mHeader; // Not owned
    NmEditorTextEdit *mEditorWidget; // Not owned
    NmBaseViewScrollArea *mScrollArea; // Not owned
    HbWidget *mScrollAreaContents; // Not owned
    QPointF mScrollPosition;
    NmApplication &mApplication;
    bool mNeedForWidthAdjustment;
};

#endif /* NMEDITORCONTENT_H_ */
