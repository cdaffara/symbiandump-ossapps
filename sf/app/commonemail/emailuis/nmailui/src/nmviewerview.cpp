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
 * Description: Mail viewer implementation.
 *
 */

#include "nmuiheaders.h"

static const char *NMUI_MESSAGE_VIEWER_XML = ":/docml/nmmailviewer.docml";
static const char *NMUI_MESSAGE_VIEWER_VIEW = "nmailViewerView";
static const char *NMUI_MESSAGE_VIEWER_CONTENT = "content";
static const char *NMUI_MESSAGE_VIEWER_SCROLL_AREA = "viewerScrollArea";
static const char *NMUI_MESSAGE_VIEWER_SCROLL_AREA_CONTENTS = "viewerScrollAreaContents";
static const char *NMUI_MESSAGE_VIEWER_HEADER = "viewerHeader";
static const char *NMUI_MESSAGE_VIEWER_ATTALIST = "viewerAttaList";
static const char *NMUI_MESSAGE_VIEWER_SCROLL_WEB_VIEW = "viewerWebView";
static const int NmOrientationTimer = 100;
static const int NmHeaderMargin = 3;
static const int NmWhitePixmapSize = 10;
static const int NmProgressValueComplete = 100;
static const QString NmParamTextHeightSecondary = "hb-param-text-height-secondary";
static const QString NmHttpLinkScheme = "http";
static const QString NmHttpsLinkScheme = "https";
static const QString NmMailtoLinkScheme = "mailto";
static const QString NmImagePartInline = "inline";
static const QString NmImagePartAttachment = "attachment";

// Local constants
const qreal NmZoomFactor = 1.5;

/*!
	\class NmViewerView
	\brief Mail viewer class
*/

/*!
    Constructor
*/
NmViewerView::NmViewerView(
    NmApplication &application,
    NmUiStartParam* startParam,
    NmUiEngine &uiEngine,
    HbMainWindow *mainWindow,
    NmAttachmentManager &attaManager,
    bool toolbarEnabled,
    QGraphicsItem *parent)
:NmBaseView(startParam, application, parent),
mApplication(application),
mUiEngine(uiEngine),
mMainWindow(mainWindow),
mAttaManager(attaManager),
mToolbarEnabled(toolbarEnabled),
mMessage(NULL),
mScrollArea(NULL),
mViewerContent(NULL),
mWebView(NULL),
mHeaderWidget(NULL),
mMessagePartFetchingOperation(NULL),
mMessageFetchingOperation(NULL),
mDisplayingPlainText(false),
mDocumentLoader(NULL),
mScrollAreaContents(NULL),
mScreenSize(QSize(0,0)),
mWaitDialog(NULL),
webFrameloadingCompleted(false),
mLatestLoadingSize(QSize(0,0)),
mAttaIndexUnderFetch(NmNotFoundError),
mAttaWidget(NULL),
mViewReady(false),
mWaitNoteCancelled(false),
mErrorNote(NULL),
mAttachmentOpen(false)
{
    // Create documentloader
    mDocumentLoader = new NmUiDocumentLoader(mMainWindow);
    // Get screensize
    mScreenSize = mApplication.screenSize();
    // Connect external delete handling to uiengine signal
    connect(&mUiEngine,
            SIGNAL(messageDeleted(const NmId &, const NmId &, const NmId &)),
            this, SLOT(messageDeleted(const NmId &, const NmId &, const NmId &)),
            Qt::UniqueConnection);
    // Fetch message
    loadMessage();
    // Load view layout
    loadViewLayout();
}

/*!
    Destructor
*/
NmViewerView::~NmViewerView()
{
    // Delete opened temporary files.
    NmUtilities::deleteTempFiles(mTempFiles);
    delete mErrorNote;
    mErrorNote=NULL;
    delete mWebView;
    mWebView = NULL;
    delete mMessage;
    mMessage = NULL;
    delete mDocumentLoader;
    mDocumentLoader = NULL;
    mWidgetList.clear();
    delete mWaitDialog;
    mWaitDialog = NULL;
    // remove view from osbserving atta manager events
    mAttaManager.clearObserver();
    mAttaManager.cancelFetch();
}

/*!
    View is about to exit
*/
void NmViewerView::aboutToExitView()
{
    // View is about to exit, for safety, stop 
    // loading of content before closing the view
    if (mWebView){
        mAttaManager.cancelFetch();
        mWebView->stop();
        if (mWebView->page()){
            mWebView->page()->deleteLater();
        }
    }
}

/*!
    View layout loading from XML
*/
void NmViewerView::loadViewLayout()
{
    NM_FUNCTION;
    
    // Use document loader to load the view
    bool ok(false);
    setObjectName(QString(NMUI_MESSAGE_VIEWER_VIEW));
   QObjectList objectList;
   objectList.append(this);
   // Pass the view to documentloader. Document loader uses this view
   // when docml is parsed, instead of creating new view.
   // documentloader is created in constructor
   mDocumentLoader->setObjectTree(objectList);
   mWidgetList = mDocumentLoader->load(NMUI_MESSAGE_VIEWER_XML, &ok);

   if (ok)
   {
        // Create content and content layout
        // qobject_cast not work in this case, using reinterpret_cast
        mViewerContent = reinterpret_cast<HbWidget *>(
                mDocumentLoader->findObject(NMUI_MESSAGE_VIEWER_CONTENT));
        // Find scroll area
        mScrollArea = reinterpret_cast<HbScrollArea *>(
                mDocumentLoader->findObject(NMUI_MESSAGE_VIEWER_SCROLL_AREA));
        if (mScrollArea) {
            mScrollArea->setParentItem(this);
            mScrollArea->setScrollDirections(Qt::Vertical | Qt::Horizontal);
            connect(mScrollArea, SIGNAL(scrollPositionChanged(QPointF)),
                this, SLOT(contentScrollPositionChanged(QPointF)));

            // Get scroll area contents and set layout margins
            mScrollAreaContents = qobject_cast<HbWidget *>(
                    mDocumentLoader->findObject(NMUI_MESSAGE_VIEWER_SCROLL_AREA_CONTENTS));
            if (mScrollAreaContents) {
                QGraphicsLayout *layout = mScrollAreaContents->layout();
                if (layout){
                    layout->setContentsMargins(0,0,0,0);
                }
                // Set white pixmap to backgrounditem 
                QPixmap whitePixmap(NmWhitePixmapSize,NmWhitePixmapSize);
                whitePixmap.fill(Qt::white);
                QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem(whitePixmap);
                mScrollAreaContents->setBackgroundItem(pixmapItem);
            }

            // Load headerwidget
            mHeaderWidget = qobject_cast<NmViewerHeader *>(
                    mDocumentLoader->findObject(NMUI_MESSAGE_VIEWER_HEADER));
            if (mHeaderWidget) {
                mHeaderWidget->setView(this);
                mHeaderWidget->rescaleHeader(mScreenSize);
                mHeaderWidget->setMessage(mMessage);
                QPointF headerStartPos = mHeaderWidget->scenePos();
                mHeaderStartScenePos = QPointF(0,headerStartPos.y());
            }

            // Load webview
            mWebView = reinterpret_cast<NmMailViewerWK *>(
                    mDocumentLoader->findObject(QString(NMUI_MESSAGE_VIEWER_SCROLL_WEB_VIEW)));
            if (mWebView) {
                // Set auto load images and private browsing(no history) attributes
                QWebSettings *settings = mWebView->settings();
                if (settings) {
                    settings->setAttribute(QWebSettings::AutoLoadImages, true);
                    settings->setAttribute(QWebSettings::PrivateBrowsingEnabled, true);
                }
                QWebPage *page = mWebView->page();
                if (page) {
                    QWebFrame *frame = page->mainFrame();
                    if (frame) {
                        frame->setScrollBarPolicy(Qt::Vertical,Qt::ScrollBarAlwaysOff);
                        frame->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
                        frame->setTextSizeMultiplier(NmZoomFactor);
                        connect(mWebView->page()->mainFrame(),
                                SIGNAL(contentsSizeChanged(const QSize&)),
                            this, SLOT(scaleWebViewWhenLoading(const QSize&)));  
                    }
                }
            }
        }
    }
}
 
/*!
    Lazy loading when view layout has been loaded
*/
void NmViewerView::viewReady()
{
    if (!mViewReady){
        // Set mailbox name to title
        setMailboxName();
        // Create toolbar if needed
        if (mToolbarEnabled) {
            createToolBar();
        } else {
            // Connect options menu about to show to create options menu function
            QObject::connect(menu(), SIGNAL(aboutToShow()),
                    this, SLOT(createOptionsMenu())); 
            // Menu needs one dummy item so that aboutToShow signal is emitted.
            NmAction *dummy = new NmAction(0);
            menu()->addAction(dummy);
        }
                
        if (mHeaderWidget) {
            QPointF contentWidgetPos = mScrollArea->pos();
            qreal headerHeight = mHeaderWidget->geometry().height();
            if (mMainWindow->orientation() == Qt::Horizontal) {
                const QPointF pointToWebView(contentWidgetPos.x(), headerHeight+NmHeaderMargin);
                mScrollArea->scrollContentsTo(pointToWebView,0);
            }
        }
        
        // Run fetchmessage in queue
        QMetaObject::invokeMethod(this, "fetchMessage", Qt::QueuedConnection);
        // Set view ready
        mViewReady = true;
    }
}

/*!
    Function fecthes message data based on parameters
*/
void NmViewerView::loadMessage()
{
    NM_FUNCTION;
    NM_TIMESTAMP("Loading message starts.");
    
    if (mMessage) {
        delete mMessage;
        mMessage = NULL;
    }
    // Read start params and message object
    if (mStartParam){
        NmId mailboxId = mStartParam->mailboxId();
        NmId folderId = mStartParam->folderId();
        NmId msgId = mStartParam->messageId();
        mMessage = mUiEngine.message(mailboxId, folderId, msgId);
    }
}

/*!
    Function fecthes message data based on parameters. Returns false if message is available,
    true if message have to be fetched
*/
void NmViewerView::fetchMessage()
{
    NM_FUNCTION;
    
    if (mMessage) {
        NmId mailboxId = mStartParam->mailboxId();
        NmId folderId = mStartParam->folderId();
        NmId msgId = mStartParam->messageId();
        const NmMessagePart *body = mMessage->htmlBodyPart();
        if (!body) {
            // try plain to plain text part
            body = mMessage->plainTextBodyPart();
        }
        // try to fetch if body missing or fetched size is smaller than content size
        // if body missing it might mean that only header is fetched or message has no body
        if (!body || (body && (body->fetchedSize() < body->size()))) {
            // start fetching operation
            if (mMessageFetchingOperation && mMessageFetchingOperation->isRunning()) { 
                mMessageFetchingOperation->cancelOperation();
            }
            if (mMessagePartFetchingOperation && mMessagePartFetchingOperation->isRunning()) { 
                mMessagePartFetchingOperation->cancelOperation();
            }
            
            if(body)
                {
                mMessagePartFetchingOperation = mUiEngine.fetchMessagePart(mailboxId, folderId, msgId,body->partId());
                }
            else
                {
                mMessageFetchingOperation = mUiEngine.fetchMessage(mailboxId, folderId, msgId);
                }

            if (mMessageFetchingOperation) {
                connect(mMessageFetchingOperation,
                        SIGNAL(operationCompleted(int)),
                        this,
                        SLOT(messageFetched(int)));
                createAndShowWaitDialog();
            }
            
            if (mMessagePartFetchingOperation) {
                connect(mMessagePartFetchingOperation,
                        SIGNAL(operationCompleted(int)),
                        this,
                        SLOT(messageFetched(int)));
                createAndShowWaitDialog();
            }
            
        } else {
            // message is fetched
            setMessageData();
        }
    }
}

/*!
    This is signalled by mMessageFetchingOperation when the original message is fetched.
 */
void NmViewerView::messageFetched(int result)
{
    mWaitDialog->close();
    disconnect(mWaitDialog->mainWindow(), SIGNAL(orientationChanged(Qt::Orientation)),
                this, SLOT(orientationChanged(Qt::Orientation)));
    
    if (result == NmNoError && (mMessageFetchingOperation || mMessagePartFetchingOperation)) {
        if (mMessage) {
            delete mMessage;
            mMessage = NULL;
        }
        // Read start params and message object
        if (mStartParam) {
            NmId mailboxId = mStartParam->mailboxId();
            NmId folderId = mStartParam->folderId();
            NmId msgId = mStartParam->messageId();
            mMessage = mUiEngine.message(mailboxId, folderId, msgId);
        }
        setMessageData();
        // Update header message data
		if (mHeaderWidget){
        	mHeaderWidget->updateMessageData(mMessage);
		}
    }
}


/*!
    This is signalled by mWaitDialog when the note is cancelled
 */
void NmViewerView::waitNoteCancelled()
{
    if (!mWaitNoteCancelled) {
        if (mMessageFetchingOperation && mMessageFetchingOperation->isRunning()) { 
	        mMessageFetchingOperation->cancelOperation();
        }
        if (mMessagePartFetchingOperation && mMessagePartFetchingOperation->isRunning()) { 
            mMessagePartFetchingOperation->cancelOperation();
        }
        
        mWaitNoteCancelled = true;
        QMetaObject::invokeMethod(&mApplication, "prepareForPopView", Qt::QueuedConnection);
    }
}


/*!
    Function sets message data to web view and header
*/
void NmViewerView::setMessageData()
{
    NM_FUNCTION;
    
    // Connect to observe orientation change events
    connect(mApplication.mainWindow(), SIGNAL(orientationChanged(Qt::Orientation)),
                this, SLOT(orientationChanged(Qt::Orientation)));

    // Set page parameters
    QWebPage *page(NULL);
    if (mWebView){
       page = mWebView->page();
    }
    if (page){
        // Set custom network access manager for embedded image handling
        NmViewerViewNetManager &netMngr = mApplication.networkAccessManager();
        netMngr.setView(this);
        page->setNetworkAccessManager(&netMngr);
        QWebSettings *webSettings = page->settings();
        if (webSettings) {
            webSettings->setObjectCacheCapacities(0,0,0);
        }

        connect(page, SIGNAL(loadFinished(bool)),
                    this, SLOT(webFrameLoaded(bool)));
        page->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
        page->setContentEditable(false);
    }

  	// if everything is ok, set message to html viewer
    if (mMessage && mWebView && page) {
        // Set initial size of component and content before loading data
        mWebView->setPreferredWidth(mScreenSize.width());
        QRectF myGeometry = geometry();
        page->setViewportSize(myGeometry.size().toSize());
        //Set message data to html viewer.
        mWebView->setHtml(formatMessage());
        // Connect to link clicked
        QObject::connect(page, SIGNAL(linkClicked(const QUrl&)),
                this, SLOT(linkClicked(const QUrl&)));
        changeMessageReadStatus(true);
        setAttachmentList();
    }
}

/*!

*/
void NmViewerView::setAttachmentList()
{
    NM_FUNCTION;
    
    // Load headerwidget
    mAttaWidget = qobject_cast<NmAttachmentListWidget *>(
            mDocumentLoader->findObject(NMUI_MESSAGE_VIEWER_ATTALIST));
    if (mMessage && mAttaWidget) {
        // connect atta widget to listen progress value events
        QObject::connect(this, SIGNAL(progressValueChanged(int, int)),
                mAttaWidget, SLOT(setProgressBarValue(int, int)));

        // Set the text color of the attachment widget black and the background
        // color white.
        mAttaWidget->setTextColor(Qt::black);
        mAttaWidget->setBackgroundColor(Qt::white);

        // Set attawidget minimum & maximum size
        mAttaWidget->setMinimumWidth(mScreenSize.width());
        mAttaWidget->setMaximumWidth(mScreenSize.width());
        bool inserted(false);
        QList<NmMessagePart*> messageParts;
        mMessage->attachmentList(messageParts);
        for (int i = 0; i < messageParts.count();i++) {
            NmMessagePart *part = messageParts[i];
            // Inline images are not displayed in attachment list
            if (part && !isInlineImage(part)) {               
                QString fileName = part->attachmentName();
                // index starts from zero, next index is same as count
                int attaIndex = mAttaWidget->count();
                mAttaWidget->insertAttachment(attaIndex, fileName,
                                              NmUtilities::attachmentSizeString(part->size()));
                mAttaIdList.insert(attaIndex, part->partId());
                // check is part under fetch, add progress value to attachment widget
                if (mAttaManager.isFetching() && 
                        mAttaManager.partIdUnderFetch() == part->partId()) {
                    mAttaIndexUnderFetch = attaIndex;
                }
                inserted = true;
            }   
        }
        if (inserted) {
            QObject::connect(mAttaWidget, SIGNAL(itemActivated(int)),
                    this, SLOT(openAttachment(int)));
        }
        else {
            // No attachments, set height to 0
            mAttaWidget->setMinimumHeight(0);
            mAttaWidget->setMaximumHeight(0);
        }
        // set this as a observer for attachment manager
        mAttaManager.setObserver(this);
    }
}

/*!

*/
void NmViewerView::openAttachment(int index)
{
    NM_FUNCTION;
    if (index >= 0 && !mAttachmentOpen) {
        NmId attaId = mAttaIdList.at(index);
        // reload message to get updates part sizes
        loadMessage();
        QList<NmMessagePart*> messageParts;
        if (mMessage) {
            mMessage->attachmentList(messageParts);
             NmId mailboxId = mMessage->envelope().mailboxId();
             NmId folderId = mMessage->envelope().folderId();
             NmId messageId = mMessage->envelope().messageId();
             for (int i = 0; i < messageParts.count(); i++) {
                 // message part found have to found
                 // and its fetched size is smaller than size, then start part fetch
                 if (messageParts[i]->partId() == attaId &&
                     messageParts[i]->size() > messageParts[i]->fetchedSize()) {
                     // do not start if there's already ongoing fetch
                     if (mAttaIndexUnderFetch == NmNotFoundError) {
                         mAttaIndexUnderFetch = index;
                         mAttaManager.fetchAttachment(mailboxId, folderId,
                                                      messageId, attaId);
                     }
                 }
                 // attachment is fetched, open file
                 else if (messageParts[i]->partId() == attaId) {
                     mAttaManager.cancelFetch();
                     mAttachmentOpen = true;
                     QObject::disconnect(mAttaWidget, SIGNAL(itemActivated(int)),
                                         this, SLOT(openAttachment(int)));
					 // We need to fill the part before opening the attachment.
					 mUiEngine.contentToMessagePart(mailboxId, folderId, messageId, *(messageParts[i]));
					 int error = NmUtilities::openAttachment(messageParts[i],mTempFiles,this);
                     if (error==NmGeneralError){
                         mAttachmentOpen = false;
                         QObject::connect(mAttaWidget, SIGNAL(itemActivated(int)),
                                              this, SLOT(openAttachment(int)));
                         delete mErrorNote;
                         mErrorNote=NULL;
                         mErrorNote = NmUtilities::displayWarningNote(
                                 hbTrId("txt_mail_dialog_unable_to_open_attachment_file_ty"));
                     }
                 }
             }            
         }
    }
}

/*!
    Function formats message based on actual data
*/
QString NmViewerView::formatMessage()
{
    NM_FUNCTION;
    
    QString msg = "";
    if (mMessage) {
        NmMessagePart *html = mMessage->htmlBodyPart();
        if (html) {
            msg += formatHtmlMessage(html);
            // Remove DOCTYPE definition as it will break viewer layout
            if (msg.startsWith("<!DOCTYPE", Qt::CaseInsensitive)) {
                int doctypeTagEnd = msg.indexOf(">",0,Qt::CaseInsensitive);
                msg.remove(0,doctypeTagEnd+1);
            }     
        }
        else {
            NmMessagePart *plain = mMessage->plainTextBodyPart();
            if (plain) {
                msg += formatPlainTextMessage(plain);
            }
        }    
    }
    return msg;
}

/*!
    Function formats html message
*/
QString NmViewerView::formatHtmlMessage(NmMessagePart *html)
{    
    NM_FUNCTION;

    QString msg = "";
    if (html && mMessage) {
        NmId mailboxId = mMessage->envelope().mailboxId();
        NmId folderId = mMessage->envelope().folderId();
        NmId messageId = mMessage->envelope().messageId();
        QList<NmMessagePart*> parts;
        mMessage->attachmentList(parts);
        for (int i=0; i < parts.count(); i++) {
            NmMessagePart *child = parts[i];
            // Browse through embedded image parts and add those
            // the web view.
            bool isFetched = child->fetchedSize() >= child->size();
            // Read inline images to webkit memory if msg part content type IS image and
            // content disposition IS NOT marked to be attachment
            if(isInlineImage(child)) {
                QString contentId = child->contentId();
                if (isFetched) {
                    int ret = mUiEngine.contentToMessagePart(
                            mailboxId, folderId, messageId, *child);
                    if (ret == NmNoError) {
                      mWebView->addContent(contentId, QVariant::fromValue(child->binaryContent()), 
                              child->partId(), isFetched);
                    }
                }
                else {
                    mWebView->addContent(contentId, QVariant::fromValue(QByteArray()), 
                            child->partId(), isFetched);
                }
            }
        }
        int ret = mUiEngine.contentToMessagePart(mailboxId, folderId, messageId, *html);
        if (ret == NmNoError) {
         msg = html->textContent();
        }    
    }
    return msg;
}

/*!
    Function formats plain text message message
*/
QString NmViewerView::formatPlainTextMessage(NmMessagePart *plain)
{
    NM_FUNCTION;
      
    QString msg = "";
    if (plain && mMessage) {
        NmId mailboxId = mMessage->envelope().mailboxId();
        NmId folderId = mMessage->envelope().folderId();
        NmId messageId = mMessage->envelope().messageId();
        int ret = mUiEngine.contentToMessagePart(mailboxId, folderId,
                                                 messageId, *plain);
        if (ret == NmNoError) {
            QTextDocument document;
            // set font
            QFont currentFont = document.defaultFont();
            currentFont.setWeight(QFont::Normal);
            qreal secondarySize;
            bool found = style()->parameter(NmParamTextHeightSecondary, secondarySize);
            if (found) {
                HbFontSpec fontSpec(HbFontSpec::Secondary);
                fontSpec.setTextHeight(secondarySize);
                currentFont.setPixelSize(fontSpec.font().pixelSize());
            }
            document.setDefaultFont(currentFont);
            // convert to html with html and body tags
            // for background and text color
            document.setPlainText(plain->textContent());
            msg += "<html><body bgcolor=white text=black>";
            msg += document.toHtml();
            msg += "</body></html>";
            
            if (qApp->layoutDirection()==Qt::RightToLeft){
                // add right alignment to document css section
                QRegExp rx("(<style type=\"text/css\">)(.+)(</style>)", Qt::CaseInsensitive);
                rx.setMinimal(true);
                int pos = rx.indexIn(msg);
                if (pos > -1) {
                    QString newStr = rx.cap(1);
                    newStr.append(rx.cap(2));
                    newStr.append("p { text-align: right } ");
                    newStr.append(rx.cap(3));
                    msg.replace(rx, newStr);
                }
            }
        }    
    }
    mDisplayingPlainText=true;  
    return msg;
}


/*!
    Reloads the view contents with new given start parameters. Typically, this
    method is called when the view is already open and an external view
    activation occurs for this same view.

    \param startParam The new parameters.
*/
void NmViewerView::reloadViewContents(NmUiStartParam* startParam)
{
    NM_FUNCTION;

    // Check the validity of the start parameters. The message view cannot be
    // updated if the given message ID is invalid (zero).
    if (startParam && startParam->viewId() == NmUiViewMessageViewer &&
        startParam->messageId() != 0) {
        // Delete the existing start parameters and store the given parameters.
        delete mStartParam;
        mStartParam = startParam;

        // Reload the view using the new message information.
        setMailboxName();
        loadMessage(); // Updates mMessage.

        if (mHeaderWidget && mMessage) {
            // Update the sender, recipient and subject etc.
            mHeaderWidget->setMessage(mMessage);
        }

        setMessageData();        
    }
    else {
        NMLOG("NmViewerView::reloadViewContents(): Invalid start parameters!");

        // Since we will not take ownership, the given parameters need to be
        // deleted.
        delete startParam;
    }
}


/*!
    nmailViewId
*/
NmUiViewId NmViewerView::nmailViewId() const
{
    return NmUiViewMessageViewer;
}

/*!
    Scale web view width
*/
void NmViewerView::webFrameLoaded(bool loaded)
{
    if (loaded){
        webFrameloadingCompleted = true;
        // Scale web view after loading the
        // complete contents, including images
        QMetaObject::invokeMethod(this, "scaleWebViewWhenLoaded", Qt::QueuedConnection);       
    }
}

/*!
    Scale web view width when loading is ongoing
*/
void NmViewerView::scaleWebViewWhenLoading(const QSize &size)
{
    // Try to scale web view while mainframe is being loaded.
    // So that screen is scrollable even before images are fully loaded
    // First check that new size is different than previous, no need to react if
    // same size value is received more than once.
    if (size != mLatestLoadingSize) {
        if (!webFrameloadingCompleted && mWebView && mWebView->page() &&
            (size.width() > mScreenSize.width() || size.height() > geometry().height())) {
            int width = (int)size.width();
            int height = (int)size.height();
            // Set content (webview) width
            if (mDisplayingPlainText){
                mWebView->setPreferredWidth(geometry().width());           
            }
            else {
                mWebView->setPreferredWidth(width);
            }
            mWebView->setMinimumHeight(height);
        }
    }
    mLatestLoadingSize = size;
}

/*!
    Scale web view width when loading is completed
*/
void NmViewerView::scaleWebViewWhenLoaded()
{
    QRectF myGeometry = geometry();
    QWebPage *page = mWebView->page();
    if (mWebView && page) {
        page->setPreferredContentsSize(myGeometry.size().toSize());
        QSizeF contentSize = page->mainFrame()->contentsSize();
        mWebView->setPreferredSize(contentSize);
    }
    // Workaround for scrolling problem
    scene()->setProperty("overridingGesture",QVariant());
}

/*!
    Set new dimensions after orientation change.
*/
void NmViewerView::adjustViewDimensions()
{
    // Update current screensize
    mScreenSize = mApplication.screenSize();
    // Scale header to screen width
    if (mHeaderWidget){
        mHeaderWidget->rescaleHeader(mScreenSize);
    }
    if (mAttaWidget){
        // Set attawidget minimum & maximum size
        mAttaWidget->setMinimumWidth(mScreenSize.width());
        mAttaWidget->setMaximumWidth(mScreenSize.width());
    }    
    scaleWebViewWhenLoaded();

    if (mToolbarEnabled) {
		// Re-create toolbar in orientation switch
		createToolBar();
    }
}

/*!
   Screen orientation changed. Web view needs to be scaled when
   landscape <-> portrait switch occurs because text needs to
   be wrapped again.
*/
void NmViewerView::orientationChanged(Qt::Orientation orientation)
{
    Q_UNUSED(orientation);
    QTimer::singleShot(NmOrientationTimer, this, SLOT(adjustViewDimensions()));
}

/*!
   Link clicked callback
*/
void NmViewerView::linkClicked(const QUrl& link)
{
    NM_FUNCTION;
    
    if (link.scheme() == NmHttpLinkScheme ||
        link.scheme() == NmHttpsLinkScheme) {
        mAttaManager.cancelFetch();
        QDesktopServices::openUrl(link);
    } else if (link.scheme() == NmMailtoLinkScheme){
        mAttaManager.cancelFetch();

        NmUriParser uriParser;
        if (uriParser.extractData(link.toString())) {
            // Create start parameters. Address list ownership
            // is transferred to startparam object
            // The ownership of these is passed to NmApplication in launchEditorView()
            QList<NmAddress*> *toAddresses = NmUtilities::qstringListToNmAddressList(uriParser.toAddresses());
            QList<NmAddress*> *ccAddresses = NmUtilities::qstringListToNmAddressList(uriParser.ccAddresses());
            QList<NmAddress*> *bccAddresses = NmUtilities::qstringListToNmAddressList(uriParser.bccAddresses());
            
            QString* subject = new QString(uriParser.subject());
            QString* bodyText = new QString(uriParser.bodyText());
            
            NmUiStartParam* param = new NmUiStartParam(NmUiViewMessageEditor,
                                                       mStartParam->mailboxId(),
                                                       mStartParam->folderId(),
                                                       0, // message id
                                                       NmUiEditorMailto, // editor start mode
                                                       toAddresses,
                                                       NULL, // attachment list
                                                       false, // start as service
                                                       subject, // message subject
                                                       ccAddresses, // cc recipients
                                                       bccAddresses, //bcc recipients
                                                       bodyText // body text
                                                       );
            

            mApplication.enterNmUiView(param);
        }

    }    
}

/*!
   Function can be used to check whether mouse event has
   occured on top of header area.
*/
bool NmViewerView::eventOnTopOfHeaderArea(QGraphicsSceneMouseEvent *event)
{
    bool ret(false);
    if (event && mHeaderWidget) {
        QPointF lastReleasePoint = event->lastPos();
        QPointF contentWidgetPos = mScrollAreaContents->pos();
        int headerHeight = (int)mHeaderWidget->geometry().height();
        if (lastReleasePoint.y()<headerHeight+contentWidgetPos.y()) {
            ret=true;
        }
    }
    return ret;
}

/*!
    Get function for content widget web view.
*/
NmMailViewerWK* NmViewerView::webView()
{
    return mWebView;
}

/*!
    Get function for message being viewed
*/
NmMessage* NmViewerView::message()
{
    return mMessage;
}

/*!
   Function to set message read status
*/
void NmViewerView::changeMessageReadStatus(bool read)
{
    NM_FUNCTION;
    
    QList<const NmMessageEnvelope*> envelopeList;
    NmMessageEnvelope *envelope = &mMessage->envelope();
    QPointer<NmStoreEnvelopesOperation> op(NULL);
    if (envelope) {
        if ( read != envelope->isRead() ){
            if (read){
                envelope->setRead(true);
                envelopeList.append(envelope);
                op = mUiEngine.setEnvelopes(
                    mStartParam->mailboxId(),
                    mStartParam->folderId(),
                    MarkAsRead,
                    envelopeList);
            }
            else {
                envelope->setRead(false);
                envelopeList.append(envelope);
                op = mUiEngine.setEnvelopes(
                    mStartParam->mailboxId(),
                    mStartParam->folderId(),
                    MarkAsUnread,
                    envelopeList);
            }
        }
    }
}

/*!
    Set mailbox name to title
*/
void NmViewerView::setMailboxName()
{
    if (mStartParam){
        NmMailboxMetaData *meta = mUiEngine.mailboxById(mStartParam->mailboxId());
        if (meta) {
            setTitle(meta->name());
        }
    }
}

/*!
    contentScrollPositionChanged.
    Function reacts to scroll position change events and sets
    header to correct position
*/
void NmViewerView::contentScrollPositionChanged(const QPointF &newPosition)
{
    if (mWebView&&mHeaderWidget){
        QRectF webViewRect = mWebView->geometry();
        QTransform tr;
        qreal leftMovementThreshold(webViewRect.width()-mHeaderWidget->geometry().width());
        if (newPosition.x()<0) {
            tr.translate(webViewRect.topLeft().x() ,0);
        }
        else if (newPosition.x()>=0 && newPosition.x()<leftMovementThreshold) {
            tr.translate(mHeaderStartScenePos.x()+newPosition.x() ,0);
        }
        else {
            tr.translate(webViewRect.topLeft().x()+leftMovementThreshold ,0);
        }
        mHeaderWidget->setTransform(tr);
        if (mAttaWidget) {
            mAttaWidget->setTransform(tr);
        }
    }
    mLatestScrollPos = newPosition;
}

/*!
    createToolBar. Function asks menu commands from extension
    to be added to toolbar owned by the HbView.
*/
void NmViewerView::createToolBar()
{
    HbToolBar *tb = toolBar();
    NmUiExtensionManager &extMngr = mApplication.extManager();
    if (tb && &extMngr && mStartParam) {
        tb->clearActions();
        NmActionRequest request(this, NmActionToolbar, NmActionContextViewViewer,
                NmActionContextDataNone, mStartParam->mailboxId(), mStartParam->folderId() );
        QList<NmAction *> list;
        extMngr.getActions(request, list);
        for (int i = 0; i < list.count(); i++) {
            tb->addAction(list[i]);
        }
    }
}

/*!
    createOptionsMenu. Functions asks menu commands from extension
    to be added to options menu.
*/
void NmViewerView::createOptionsMenu()
{
	HbMenu *optionsMenu = menu();
	NmUiExtensionManager &extMngr = mApplication.extManager();
	if (optionsMenu && &extMngr && mStartParam) {
		optionsMenu->clearActions();
		NmActionRequest request(this, NmActionOptionsMenu, NmActionContextViewViewer,
				NmActionContextDataNone, mStartParam->mailboxId(), mStartParam->folderId() );

		QList<NmAction*> list;
		extMngr.getActions(request, list);
		for (int i=0;i<list.count();i++) {
			optionsMenu->addAction(list[i]);
		}
	}
}

/*!
    handleActionCommand. From NmActionObserver, extension manager calls this
    call to handle menu command in the UI.
*/
void NmViewerView::handleActionCommand(NmActionResponse &actionResponse)
{
    // Handle options menu or toolbar
    if (actionResponse.menuType() == NmActionOptionsMenu ||
    	actionResponse.menuType() == NmActionToolbar) {
        switch (actionResponse.responseCommand()) {
            case NmActionResponseCommandReply: {
                mAttaManager.cancelFetch();
                NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageEditor,
                    mStartParam->mailboxId(), mStartParam->folderId(),
                    mStartParam->messageId(), NmUiEditorReply);
                mApplication.enterNmUiView(startParam);
            }
            break;
            case NmActionResponseCommandReplyAll: {
                NM_TIMESTAMP("Reply All chose.");
                mAttaManager.cancelFetch();
                NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageEditor,
                    mStartParam->mailboxId(), mStartParam->folderId(),
                    mStartParam->messageId(), NmUiEditorReplyAll);
                mApplication.enterNmUiView(startParam);
            }
            break;
            case NmActionResponseCommandForward: {
                mAttaManager.cancelFetch();
                NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageEditor,
                    mStartParam->mailboxId(), mStartParam->folderId(),
                    mStartParam->messageId(), NmUiEditorForward);
                mApplication.enterNmUiView(startParam);
            }
            break;
            case NmActionResponseCommandDeleteMail: {
                NM_TIMESTAMP("Delete from viewer chose.");
                mAttaManager.cancelFetch();
                deleteMessage();
                }
            break;
            default:
                break;
        }
    }
}

/*!
    Deletes the currently open message
*/
void NmViewerView::deleteMessage()
{
    NM_FUNCTION;
    
    QList<NmId> messageList;
    messageList.append(mStartParam->messageId());  
    int err = mUiEngine.deleteMessages(mStartParam->mailboxId(),
                                       mStartParam->folderId(),
                                       messageList);
    messageList.clear();
}


/*!
    This is called when attachment fetch progress changes
*/
void NmViewerView::progressChanged(int value)
{
    if (mAttaIndexUnderFetch != NmNotFoundError) {
        // emit signal
        if (mAttaWidget && mAttaWidget->progressValue(mAttaIndexUnderFetch) < value) {
            progressValueChanged(mAttaIndexUnderFetch, value);
        }
    }
}


/*!
    This method is called when fetching an attachment is either completed or
    cancelled.

    \param result The result code (NmNoError if success).
*/
void NmViewerView::fetchCompleted(int result)
{
    if (mAttaWidget && mAttaIndexUnderFetch != NmNotFoundError) {
        if (result == NmNoError) {
            progressValueChanged(mAttaIndexUnderFetch, NmProgressValueComplete);
            openAttachment(mAttaIndexUnderFetch);
        } else {
            mAttaWidget->hideProgressBar(mAttaIndexUnderFetch);

            if (result == NmDiskFullError) {
                // Display an error message.
                delete mErrorNote;
                mErrorNote = NmUtilities::displayWarningNote(
                    hbTrId("txt_mail_dialog_downloading_canceled"));
            }

        }
    }

    mAttaIndexUnderFetch = NmNotFoundError;
}


/*!
    externalDelete. From NmUiEngine, handles viewer shutdown when current message is deleted.
*/
void NmViewerView::messageDeleted(const NmId &mailboxId, const NmId &folderId, const NmId &messageId)
{
    if ((mStartParam->viewId() == NmUiViewMessageViewer)
        && (mStartParam->mailboxId()== mailboxId)
        && (mStartParam->folderId()== folderId)
        && (mStartParam->messageId()== messageId)) {
        NM_TIMESTAMP("Message deleted.");
        mApplication.prepareForPopView();
    }
}

/*!
    Helper function for wait dialog creation.
*/
void NmViewerView::createAndShowWaitDialog()
{
    delete mWaitDialog;
    mWaitDialog = NULL;
    // Create new wait dialog and set it to me modal with dimmed background
    mWaitDialog = new HbProgressDialog(HbProgressDialog::WaitDialog);
    // Connect to observe orientation change events
    connect(mWaitDialog->mainWindow(), SIGNAL(orientationChanged(Qt::Orientation)),
                this, SLOT(orientationChanged(Qt::Orientation)));
    mWaitDialog->setModal(true);
    mWaitDialog->setBackgroundFaded(true);
    connect(mWaitDialog, SIGNAL(cancelled()), this, SLOT(waitNoteCancelled()));
    mWaitDialog->setText(hbTrId("txt_mail_dialog_loading_mail_content"));
    // Display wait dialog
    mWaitDialog->show(); 
}


/*!
    Helper function to determine whether message part is inline image.
*/
bool NmViewerView::isInlineImage(NmMessagePart* part)
{
    bool ret(false);
    if (part){
        // Check whether the message part is image
        if (part->contentType().trimmed().startsWith("image", Qt::CaseInsensitive)) {
            bool isMarkedInline(false);
            bool isMarkedAttachment(false);
            // Check whether the image has been marked correctly as an inline image
            if (part->contentDisposition().trimmed().startsWith(NmImagePartInline, Qt::CaseInsensitive)) {
                isMarkedInline=true;
            }            
            // Check whether the image has been marked correctly as an attachment
            else if (part->contentDisposition().trimmed().startsWith(NmImagePartAttachment, Qt::CaseInsensitive)) {
                isMarkedAttachment=true;
            }       
            // UI handles image as inline if it is correctly marked to be inline
            // or hasn't been marked at all with content disposition
            if (isMarkedInline || (!isMarkedInline && !isMarkedAttachment) ) {
                ret=true;
            }            
        }
    }
    return ret;
}

void NmViewerView::fileOpenCompleted(const QVariant&)
{
    mAttachmentOpen = false;
    QObject::connect(mAttaWidget, SIGNAL(itemActivated(int)),
                          this, SLOT(openAttachment(int)));
}

void NmViewerView::fileOpenError(int, const QString&)
{
    mAttachmentOpen = false;
    QObject::connect(mAttaWidget, SIGNAL(itemActivated(int)),
                          this, SLOT(openAttachment(int)));
}

