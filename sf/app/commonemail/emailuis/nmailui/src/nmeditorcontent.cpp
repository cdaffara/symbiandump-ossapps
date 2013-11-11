/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "nmuiheaders.h"

// Layout
static const char *NMUI_EDITOR_BODY = "BodyTextEdit";
static const char *NMUI_EDITOR_SCROLL_AREA = "scrollArea";
static const char *NMUI_EDITOR_SCROLL_AREA_CONTENTS = "scrollAreaContents";

// Regular expression for selecting img tags with "cid" in the mail.
static const char *NMUI_EDITOR_REMOVE_EMBD_IMAGES_REG = 
    "(<img[^<]+(src\\s*=\\s*)(.{0,1}cid)([^<]+)(>\\s*|/>\\s*|></img>\\s*))";
	
/*!
    Constructor
*/
NmEditorContent::NmEditorContent(QObject *parent,
                                 HbDocumentLoader *documentLoader,
                                 QNetworkAccessManager &manager,
                                 NmApplication &application) :
    QObject(parent),
    mHeader(NULL),
    mEditorWidget(NULL),
    mScrollArea(NULL),
    mScrollAreaContents(NULL),
    mApplication(application),
    mNeedForWidthAdjustment(false)
{
    NM_FUNCTION;

    // Construct container for the header widgets
    mHeader = new NmEditorHeader(this, application, documentLoader);

    // Get pointer to body text area handling widget
    mEditorWidget = qobject_cast<NmEditorTextEdit *>(documentLoader->findWidget(NMUI_EDITOR_BODY));

    // Enable the emoticons.
    mEditorWidget->setSmileysEnabled(true);

    // Set body editor to use NmEditorTextDocument
    NmEditorTextDocument *textDocument = new NmEditorTextDocument(manager);
    mEditorWidget->setDocument(textDocument); 
    textDocument->setParent(mEditorWidget); // ownership changes

    mScrollArea = qobject_cast<NmBaseViewScrollArea *>
        (documentLoader->findWidget(NMUI_EDITOR_SCROLL_AREA));
    
    mScrollArea->setScrollDirections(Qt::Vertical | Qt::Horizontal);
    mScrollArea->setClampingStyle(HbScrollArea::BounceBackClamping);
        
    // Enable style picker menu item.
    mEditorWidget->setFormatDialog(new HbFormatDialog());

    mScrollAreaContents =
        qobject_cast<HbWidget *>(documentLoader->findWidget(NMUI_EDITOR_SCROLL_AREA_CONTENTS));
    
    // Create signal slot connections
    createConnections();

    // The following line is necessary in terms of being able to add emoticons
    // (smileys) to an empty document (mail content). Otherwise the private
    // pointer of the QTextDocument which the smiley engine has is NULL and
    // inserting a smiley will lead to an error.
    mEditorWidget->setPlainText("");
    
    QCoreApplication::instance()->installEventFilter(this); // see eventFilter()
}

/*!
    Destructor
*/
NmEditorContent::~NmEditorContent()
{
    NM_FUNCTION;

    QCoreApplication::instance()->removeEventFilter(this); // see eventFilter()
}

/*!
    Sets the body content. If reply envelopw is present, reply header is generated and set to 
    editor. Reply envelope ownership is not transferred here.
 */
void NmEditorContent::setBodyContent(NmUiEditorStartMode editorStartMode,
                                     const NmMessage *originalMessage, 
                                     const QString *signature)
{
    NM_FUNCTION;
    
    QString bodyContent;
    
    // first insert the signature
    if (signature) {
        bodyContent.append("<html><body><br><br>");
        bodyContent.append(*signature);
        bodyContent.append("<br></body></html>");
    }
    
	QTextCursor cursor(mEditorWidget->document());
    
    // Create the "reply" header (also for forward message)
	// sets the font color of the reply header and the original body text to black
    if ((editorStartMode == NmUiEditorReply || editorStartMode == NmUiEditorReplyAll || 
        editorStartMode == NmUiEditorForward) && originalMessage) {
		bodyContent.append(QString("<style type=\"text/css\">* { color: black; }</style>"));
        bodyContent.append(NmUtilities::createReplyHeader(originalMessage->envelope()));
    }
    
    // Check which part is present. Html or plain text part. We use the original message parts.
    const NmMessagePart *htmlPart = NULL;
    const NmMessagePart *plainPart = NULL;
    if (originalMessage) {
        htmlPart = originalMessage->htmlBodyPart();
        plainPart = originalMessage->plainTextBodyPart();
    }
 
    if (htmlPart) {
        QString bodyText(htmlPart->textContent());
        if (editorStartMode == NmUiEditorReply || editorStartMode == NmUiEditorReplyAll || 
                editorStartMode == NmUiEditorForward) {
            convertBodyStylesToDivision(bodyText);
        }
        
        if(editorStartMode == NmUiEditorReply || editorStartMode == NmUiEditorReplyAll ) {
            removeEmbeddedImages(bodyText);
        }
        bodyContent.append(bodyText);
    }
    else if (plainPart) {
        // Plain text part was present, set it to HbTextEdit as HTML
        bodyContent.append(QString("<html><body><p>"));
        bodyContent.append(plainPart->textContent());
        bodyContent.append(QString("</p></body></html>"));
    }
    cursor.insertHtml(bodyContent);

    // Update of the body width is done when next contentChanged signal comes from the body.
    mNeedForWidthAdjustment = true;
	cursor.clearSelection();
	cursor.setPosition(0);
	cursor.insertHtml(QString("<html><body></body></html>"));
	mEditorWidget->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
	QMetaObject::invokeMethod(this, "ensureCursorVisibility", Qt::QueuedConnection);
}  

/*!
   This method creates all needed signal-slot connections
 */
void NmEditorContent::createConnections()
{
    NM_FUNCTION;
    
    // Signal for setting HbTextEdit widgets html content
    connect(this, SIGNAL(setHtml(QString)),
            mEditorWidget, SLOT(setHtml(QString)), Qt::QueuedConnection);

    // Signal for setting HbTextEdit widgets plain text content
    connect(this, SIGNAL(setPlainText(QString)),
            mEditorWidget, SLOT(setPlainText(QString)), Qt::QueuedConnection);

    // Inform text edit widget that header height has been changed
    connect(mHeader, SIGNAL(headerHeightChanged(int)), this, SLOT(setEditorContentHeight()),
        Qt::QueuedConnection);

    // we are interested in the document's height changes
    connect(mEditorWidget->document()->documentLayout(), SIGNAL(documentSizeChanged(QSizeF)), this,
        SLOT(setEditorContentHeight()), Qt::QueuedConnection);

    // listen to the parent's (NmEditorView) size changes which happen eg. when VKB is opened/closed
    connect(parent(), SIGNAL(sizeChanged()), this, SLOT(ensureCursorVisibility()),
        Qt::QueuedConnection);

    // Listen scroll posion change signals for header reposition.
    connect(mScrollArea, SIGNAL(scrollPositionChanged(QPointF)),
       		this, SLOT(repositHeader(QPointF)));
    
    // Listen content change signal for body widget width adjustment.
    connect(mEditorWidget->document(), SIGNAL(contentsChanged()), this, 
        SLOT(setEditorContentWidth()), Qt::QueuedConnection);
}

/*!
    Return pointer to the email body text edit widget
 */
NmEditorTextEdit *NmEditorContent::editor() const
{
    NM_FUNCTION;
    
    return mEditorWidget;
}

/*!
    Return pointer to the header widget
 */
NmEditorHeader *NmEditorContent::header() const
{
    NM_FUNCTION;
    
    return mHeader;
}

/*!
    This slot is called when header widget height has been changed. Function performs
    the repositioning of the body field and resizing of the editor and content area.
 */
void NmEditorContent::setEditorContentHeight()
{
    NM_FUNCTION;
    
    HbStyle* style = mScrollAreaContents->style();
    
    if (style) {
        // the height of the margin between the title bar and the header
        qreal topMargin = 0;
        style->parameter("hb-param-margin-gene-top", topMargin);
        
        // header height
        qreal headerHeight = mHeader->headerHeight();
    
        // body area editor's document height with margins added
        qreal documentHeightAndMargins = mEditorWidget->document()->size().height() +
            (mEditorWidget->document()->documentMargin() * 2);
    
        // chrome height
        qreal chromeHeight = 0;
        style->parameter("hb-param-widget-chrome-height", chromeHeight);
        
        // screen height
        qreal screenHeight = mApplication.screenSize().height();
    
        // set min size for the body area so that at least the screen area is always filled
        qreal bodyAreaMinSize = screenHeight - chromeHeight - topMargin - headerHeight;
        
        qreal bodyAreaSize = fmax(bodyAreaMinSize, documentHeightAndMargins);
        
        mScrollAreaContents->setPreferredHeight(topMargin + headerHeight + bodyAreaSize);
    }
}

/*!
    This slot is used to set width for the body field.
    For some reason HbTextEdit does not set it's width, so we need to se it here.
    This slot can be removed if HbTextEdit is fixed.
 */
void NmEditorContent::setEditorContentWidth()
{
    NM_FUNCTION;
    
    if (mNeedForWidthAdjustment) {
        mNeedForWidthAdjustment = false;
        mScrollAreaContents->setPreferredWidth(mEditorWidget->document()->idealWidth());
    }
}

/*!
    This slot is called when the cursor visibility has to be ensured ie. the scroll position is 
    adjusted so that the cursor can be seen. For defining exatcly what area is ensured the 
    same algorithm that HbAbstractEditPrivate::ensurePositionVisible() uses is used here. 
*/
void NmEditorContent::ensureCursorVisibility()
{
    NM_FUNCTION;
    
    QGraphicsItem *focused = NULL;
    QRectF localRect; // set to empty
    
    // find out which widget has the input focus and get the cursor position ractangle
    if (mHeader->toEdit() && mHeader->toEdit()->hasInputFocus()) {
        focused = mHeader->toEdit();
        localRect = mHeader->toEdit()->rectForCursorPosition();
    }
    else if (mHeader->ccEdit() && mHeader->ccEdit()->hasInputFocus()) {
        focused = mHeader->ccEdit();
        localRect = mHeader->ccEdit()->rectForCursorPosition();
    }
    else if (mHeader->bccEdit() && mHeader->bccEdit()->hasInputFocus()) {
        focused = mHeader->bccEdit();
        localRect = mHeader->bccEdit()->rectForCursorPosition();
    }
    else if (mHeader->subjectEdit() && mHeader->subjectEdit()->hasInputFocus()) {
        focused = mHeader->subjectEdit();
        localRect = mHeader->subjectEdit()->rectForCursorPosition();
    }
    else if (mEditorWidget->hasInputFocus()) {
        focused = mEditorWidget;
        localRect = mEditorWidget->rectForCursorPosition();
    }

    // ensure that the cursor position is visible
    if (focused && !localRect.isEmpty()) {
        QRectF rect = focused->mapRectToItem(mScrollAreaContents, localRect);
        mScrollArea->ensureVisible(rect.center(), rect.width(), rect.height() / 2 );
    }
}
/*!
    Removes embedded images from the message body
 */
void NmEditorContent::removeEmbeddedImages(QString &bodyContent)
{
    NM_FUNCTION;
 
    QRegExp regExp(NMUI_EDITOR_REMOVE_EMBD_IMAGES_REG, Qt::CaseInsensitive);
    bodyContent.remove(regExp);
}

/*!
    This slot is called when scroll position has been changed.
    Function create translation object which is used to set new position for
    header so that header stays visible when body is scrolled horizontally.
 */
void NmEditorContent::repositHeader(const QPointF &scrollPosition)
{
    NM_FUNCTION;
    
    // Get the layout's left margin
    qreal margin = 0;
    HbInstance::instance()->style()->parameter("hb-param-margin-gene-left", margin);

    // Calculate header width. (Screen width minus left and right margins.
    qreal headerWidth = mApplication.screenSize().width() - margin - margin;

    // Create translation object for header position adjustment.
    QRectF editorBodyRect = mEditorWidget->geometry();
    QTransform tr;

    qreal bodyWidth = editorBodyRect.width();
    if ( bodyWidth < mApplication.screenSize().width() ) {
    	    bodyWidth = mApplication.screenSize().width();
    }

    qreal leftMovementThreshold( bodyWidth - headerWidth);

    if (scrollPosition.x() < 0) {
        // Left side positioning. Allow left side baunch effect.
        tr.translate(editorBodyRect.topLeft().x() - margin ,0);
    }
    else if (scrollPosition.x() >= 0 && scrollPosition.x() < leftMovementThreshold) {
        // Middle area positioning
        tr.translate(scrollPosition.x() ,0);
    }
    else {
        // Right side positioning. Allow right side baunch effect.
        tr.translate(editorBodyRect.topLeft().x() + leftMovementThreshold - margin ,0);
    }
    // Call header to perform the translation which moves hader to new position.
    mHeader->repositHeader(tr);
}

/*!
    Removes body bgcolor tag from content and creates DIV from it.
 */
void NmEditorContent::convertBodyStylesToDivision(QString &bodyContent)
{
    NM_FUNCTION;
    convertBGColorToStyle(bodyContent);
  
    // Make division from the head style body tag
    createDivisionFromHead(bodyContent);
}

/*!
 *  Creates div.reply from the html header styles (if there is any) and applies this 
 *  style to the message by surrounding the message with the div.reply element.
 *  
 *  Example:
 *  <html><head>
 *  <style type="text/css">
 *  body {
 *  color: #FF0000;
 *  background-color: #ffffff }
 *  h3{ color: blue }
 *  </style></head>
 *  <body>
 *  <h3>This is bigger text.</h3>
 *  This is the body text.
 *  </body></html>
 *  
 *      ---->
 *      
 *  <html><head>
 *  <style type="text/css">
 *  div.reply {
 *  color: #FF0000;
 *  background-color: #ffffff }
 *  h3 { color: blue }
 *  </style></head>
 *  <body>
 *  <div class="reply">
 *  <h3>This is bigger text.</h3>
 *  This is the body text.
 *  </div></body></html> 
 *  
 */
void NmEditorContent::createDivisionFromHead(QString &bodyContent)
{
    NM_FUNCTION;
    
    // Regular expression string for searching <head> part that contains <style>
    QString bodyStyleDefined(
        "<head"
        "(\\s)*"    // 0-* white spaces, browsers seem to accept this too.
        ">"
        "(.)*"      // 0-* any characters
        "<style"
        "(.)*"      // 0-* any characters
        "</head"
        "(\\s)*"    // 0-* white spaces
        ">");
    QRegExp bodyStyleDefinedRegExp(bodyStyleDefined, Qt::CaseInsensitive);
    bodyStyleDefinedRegExp.setMinimal(true);
    
    QString bodyStartReplacement("<body>\n<div");
    
    if (bodyContent.contains(bodyStyleDefinedRegExp)) {
        QString headPartString = bodyStyleDefinedRegExp.cap(0);
        QString headBodyStyleString("body(\\s)*");
        QRegExp bodyStyleReplacementRegExp(headBodyStyleString, Qt::CaseInsensitive);
        
        if (headPartString.contains(bodyStyleReplacementRegExp)) {
            headPartString.replace(bodyStyleReplacementRegExp, "div.reply ");
            bodyContent.replace(bodyStyleDefinedRegExp, headPartString);
            
            bodyStartReplacement = "<body>\n<div class=\"reply\"";
        }
    }
    
    convertBodyToDiv(bodyContent, bodyStartReplacement);
}

/*!
 *  Renames <body...></body> to <div...></div> and creates new <body> element.
 *  
 *  Example:
 *  <body style="background: #00ff00; color: red; border: solid">
 *  This is the body text.
 *  </body>
 *
 *      ---->
 *  
 *  <body>    
 *  <div style="background: #00ff00; color: red; border: solid">
 *  This is the body text.
 *  </div>
 *  </body>
 */
void NmEditorContent::convertBodyToDiv(QString &bodyContent, const QString &replacementString)
{
    NM_FUNCTION;
    
    QString bodyStart("<body");
    QString bodyEnd("</body>");
    QString bodyEndReplacement("</div></body>");
    QRegExp bodyStartRegExp(bodyStart, Qt::CaseInsensitive);
    QRegExp bodyEndRegExp(bodyEnd, Qt::CaseInsensitive);
    
    bodyContent.replace(bodyStartRegExp, replacementString);
    bodyContent.replace(bodyEndRegExp, bodyEndReplacement);    
}

/*!
 *  Converts "body bgcolor=#color" attribute to "body style=background: #color".
 *  
 *  Example:
 *  <body bgcolor="blue"></body> 
 *  
 *              ----->
 *  
 *  <body style="background:blue"></div></body>
 */
void NmEditorContent::convertBGColorToStyle(QString &bodyContent)
{
    NM_FUNCTION;
    
    QString bgColorInBodyFetchString("<body"
        "[^<]+"             // 1...* any character except '<'
        "(bgcolor"
        "(\\s|)"            // White space or nothing
        "="
        "(\\s|)"            // White space or nothing
        "(\"|)"             // '"' or nothing
        "(#|))"             // '#' or nothing
        "*>");              // 0...* Any character + >
    QRegExp bgColorInBodyRegExp(bgColorInBodyFetchString, Qt::CaseInsensitive);
    
    if (bodyContent.contains(bgColorInBodyRegExp)) {
        // There can be only one meaningful bgcolor, the first one.
        QString bgColorString = bgColorInBodyRegExp.cap(0);
        
        // Extract the color code from the string
        QString colorCode(bgColorString);
        QRegExp removeBeginningRegExp(
            "<body"
            "[^<]+"         // 1...* any character except '<'
            "bgcolor"
            "(\\s|)"        // White space or nothing
            "="
            "(\\s|)"        // White space or nothing
            "(\"|)"         // '"' or nothing
            "(#|)",         // '#' or nothing
            Qt::CaseInsensitive);
        
        colorCode.remove(removeBeginningRegExp);
        
        QRegExp removeEndRegExp(""
            "((\"|\\s)"     // '"' or white space
            "([^<]*)"       // 0...* any characters except '<'
            ">)"            // '>'           
            "|>");          // ... or nothing before this, just '>'
        
        colorCode.remove(removeEndRegExp);
        
        QString plainBgColorFetchString("bgcolor"
            "(\\s|)*"       // 0...* White space or nothing
            "="             
            "(\\s|)*"       // 0...* White space or nothing
            "[^\\s]+"       // 1...* any character except white space
            "(\\s|(?=>))"); // White space or follower by '>'
        
        QString bgColorReplacement("style=\"background: #"+colorCode+"\" ");
        QRegExp plainBgColorFetchRegExp(plainBgColorFetchString, Qt::CaseInsensitive);
        
        // Create temporary string for bgcolor
        bgColorString.replace(plainBgColorFetchRegExp, bgColorReplacement);
        
        // Copy the contents of the temporary string to the message
        bodyContent.replace(bgColorInBodyRegExp, bgColorString);
    }    
}

/*!
 *  Listen to the input method events (eg. cursor position changes, preedit text changes etc.) and 
 *  invoke ensureCursorVisibility. Qt FW does not automatically ensure the visiblity of a text in a
 *  edit widget in our custom scroll area. Filtering events is the only way to get events of the
 *  preedit text changes.
 */
bool NmEditorContent::eventFilter(QObject *obj, QEvent *event)
{
    // let the FW handle the event first
    bool ret = QObject::eventFilter(obj, event);
    
    if (event && event->type() == QEvent::InputMethod) {
        QMetaObject::invokeMethod(this, "ensureCursorVisibility", Qt::QueuedConnection);
    }
    
    return ret;
}

