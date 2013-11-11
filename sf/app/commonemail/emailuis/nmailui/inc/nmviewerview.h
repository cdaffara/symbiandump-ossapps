/*
 * Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef NMVIEWERVIEW_H_
#define NMVIEWERVIEW_H_

#include <QPointer>
#include <XQSharableFile.h>

#include "nmbaseview.h"
#include "nmactionobserver.h"
#include "nmattachmentfetchobserver.h"

class QWebPage;

class HbWidget;
class HbMainWindow;
class HbScrollArea;

class NmApplication;
class NmUiEngine;
class NmUiStartParam;
class NmMailViewerWK;
class NmMessage;
class NmViewerViewNetManager;
class NmViewerHeader;
class NmMailViewerWK;
class NmUiDocumentLoader;
class NmOperation;
class HbProgressDialog;
class NmAttachmentListWidget;
class NmAttachmentManager;
class HbMessageBox;
class NmMessagePart;

class NmViewerView : public NmBaseView, public NmActionObserver, public NmAttachmentFetchObserver
{
    Q_OBJECT
public:
    explicit NmViewerView(
				NmApplication &application,
				NmUiStartParam* startParam,
				NmUiEngine &uiEngine,
				HbMainWindow *mainWindow,
				NmAttachmentManager &attaManager,
				bool toolbar = false,
				QGraphicsItem *parent = NULL);
    ~NmViewerView();
    void reloadViewContents(NmUiStartParam* startParam);
    NmUiViewId nmailViewId() const;
    NmMailViewerWK* webView();
    NmMessage* message();
    void viewReady();
    void aboutToExitView();
    
signals:
    void progressValueChanged(int index, int value);

public slots:
    void orientationChanged(Qt::Orientation orientation);
    void adjustViewDimensions();
    void linkClicked(const QUrl& link);
    void contentScrollPositionChanged(const QPointF &newPosition);
    void fetchMessage();
    void openAttachment(int index);
	void createOptionsMenu();
	void fileOpenCompleted(const QVariant&);
	void fileOpenError(int, const QString&);
	

private slots:
    void setMessageData();
    void messageFetched(int result);
    void waitNoteCancelled();
    void webFrameLoaded(bool loaded);
    void scaleWebViewWhenLoading(const QSize &size);
    void scaleWebViewWhenLoaded();
    void messageDeleted(const NmId &mailboxId, const NmId &folderId, const NmId &messageId);

public: // From NmActionObserver
    void handleActionCommand(NmActionResponse &menuResponse);

public: // From NmAttachmentFetchObserver
    void progressChanged(int value);
    void fetchCompleted(int result);

private:
    void deleteMessage();
    void loadMessage();
    void loadViewLayout();
    QString formatMessage();
    QString formatHtmlMessage(NmMessagePart *html);
    QString formatPlainTextMessage(NmMessagePart *plain);
    bool eventOnTopOfHeaderArea(QGraphicsSceneMouseEvent *event);
    void changeMessageReadStatus(bool read);
    void setMailboxName();
    void createToolBar();
    void setAttachmentList();
    void createAndShowWaitDialog();
    bool isInlineImage(NmMessagePart* part);

private:
    NmApplication &mApplication;
    NmUiEngine &mUiEngine;
    HbMainWindow *mMainWindow;                       // Not owned
    NmAttachmentManager  &mAttaManager;
    bool mToolbarEnabled;					         // is toolbar or options menu in use
    NmMessage* mMessage;                             // Owned
    HbScrollArea *mScrollArea;                       // Not owned
    HbWidget *mViewerContent;                        // Not owned
    NmMailViewerWK *mWebView;                        // Not owned
    NmViewerHeader *mHeaderWidget;                   // Not owned
    NmAttachmentListWidget *mAttaListWidget;         // Not owned
    QPointF mHeaderStartScenePos;
    QPointer<NmOperation> mMessagePartFetchingOperation;    // Not owned
    QPointer<NmOperation> mMessageFetchingOperation; // Not owned 
    QPointF mLatestScrollPos;
    bool mDisplayingPlainText;
    QObjectList mWidgetList;
    NmUiDocumentLoader *mDocumentLoader;             // Owned
    HbWidget *mScrollAreaContents;                   // Not owned
    QSize mScreenSize;
    HbProgressDialog *mWaitDialog;                   // Owned
    bool webFrameloadingCompleted;
    QSize mLatestLoadingSize;
    QList<NmId> mAttaIdList;
    int mAttaIndexUnderFetch;
    NmAttachmentListWidget *mAttaWidget;             // Not owned
    bool mViewReady;
    bool mWaitNoteCancelled;
    HbMessageBox *mErrorNote;                       // Owned
    QList<QString> mTempFiles;
    bool mAttachmentOpen;
    XQSharableFile mOpenedAttachment;
};

#endif /* NMVIEWERVIEW_H_ */
