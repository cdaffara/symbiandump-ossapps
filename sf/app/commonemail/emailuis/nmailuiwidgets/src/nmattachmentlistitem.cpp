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
* Description: Attachment list item widget
*
*/

#include "nmailuiwidgetsheaders.h"

static const QString FILE_PATH_WIDGETML = ":nmattachmentlistitem.widgetml";
static const QString FILE_PATH_CSS = ":nmattachmentlistitem.css";
static const QString LIST_ITEM_BG_FRAME_NORMAL ("qtg_fr_list_normal");
static const QString LIST_ITEM_BG_FRAME_PRESSED("qtg_fr_list_pressed");

static const int PROGRESSBAR_MIN = 0; 
static const int PROGRESSBAR_MAX = 100;
static const int PROGRESSBAR_HIDE_COUNTDOWN = 500; // Milliseconds


/*!
    @nmailuiwidgets

    \class NmAttachmentListItem

    \brief  NmAttachmentListItem is a widget used for displaying a single
            attachment with the attachment file size and download progress in
            the attachment list.
*/


/*!
    Constructs a new NmAttachmentListItem with \a parent.
*/
NmAttachmentListItem::NmAttachmentListItem(QGraphicsItem *parent)
: HbWidget( parent ),
  mFileNameText(NULL),
  mFileSizeText(NULL),
  mProgressBar(NULL),
  mBackground(NULL)
{
    NM_FUNCTION;

    init();
	
    // Informs GestureFramework that NmAttachmentListItem widget is interested 
    // in Tap and TapAndHold gestures.
    grabGesture(Qt::TapGesture);
    HbEffect::add("mailAttachmentWidget", "listviewitem_press", "pressed");
    HbEffect::add("mailAttachmentWidget", "listviewitem_release", "released");
}


/*!
    Destructor.
*/
NmAttachmentListItem::~NmAttachmentListItem()
{
    NM_FUNCTION;
    
    HbStyleLoader::unregisterFilePath(FILE_PATH_WIDGETML);
    HbStyleLoader::unregisterFilePath(FILE_PATH_CSS);
}


/*!
    Setter for items text color override. This method can be used if the theme
    background is not used and text needs to be shown with a different color.

    \param color The new test color.
*/
void NmAttachmentListItem::setTextColor(const QColor color)
{
    NM_FUNCTION;
    
    mTextColor = color;

    if (mTextColor.isValid() && mFileNameText) {
        mFileNameText->setTextColor(mTextColor);
    }
}


/*!
    Sets the background color. This method can be used to override the
    default background color set by the current theme. Because of the effect,
    the implementation "smoke and mirrors" but it works.

    \param color The new background color.
*/
void NmAttachmentListItem::setBackgroundColor(const QColor color)
{
    NM_FUNCTION;

    if (!color.isValid()) {
        // Invalid color!
        return;
    }

    mBackgroundColor = color;

    // The effect color is the opposite color of the background color.
    QColor effectColor(qAbs(color.red() - 255),
                       qAbs(color.green() - 255),
                       qAbs(color.blue() - 255));
    
    QPalette palette = QApplication::palette();
    QBrush brush(effectColor);
    palette.setBrush(QPalette::Window, effectColor);
    setPalette(palette);
    setAutoFillBackground(true);

    if (mBackground) {
        // Set the background color for the background item.
        brush.setColor(mBackgroundColor);
        palette.setBrush(QPalette::Window, brush);
        mBackground->setPalette(palette);
        mBackground->setAutoFillBackground(true);
        mBackground->frameDrawer().setFrameType(HbFrameDrawer::OnePiece);
    }

    repolish();
}


/*!
    Sets the text to be displayed in the file name item.

    \param filename The filename.
*/
void NmAttachmentListItem::setFileNameText(const QString &fileName)
{
    NM_FUNCTION;
    
    if (mFileNameText) {
        if (mTextColor.isValid()) {
            mFileNameText->setTextColor(mTextColor);
        }

        mFileNameText->setTextWrapping(Hb::TextNoWrap);
        mFileNameText->setText(fileName);    
    }
}


/*!
    Sets the value of the text for displaying the attachment file size.
    
    \param fileSize The file size as a string.
*/
void NmAttachmentListItem::setFileSizeText(const QString &fileSize)
{
    NM_FUNCTION;
    
    if (mFileSizeText) {
        if (mTextColor.isValid()) {
            mFileSizeText->setTextColor(mTextColor);
        }

        mFileSizeText->setTextWrapping(Hb::TextNoWrap);
        mFileSizeText->setText(fileSize);
    }
}


/*!
    Set the download progress bar value (0 - 100 %). If the given value is 0,
    the progress bar is hidden.

    \param value The progress bar value.
*/
void NmAttachmentListItem::setProgressBarValue(const int value)
{
    NM_FUNCTION;
    
    // First check if the value is 0 or below -> hide progressbar.
    if (0 >= value) {
        removeProgressBar();
        return;
    }

    if (!mProgressBar) {
        mProgressBar = new HbProgressBar(this); 
        mProgressBar->setObjectName("attachmentlistitem_progress");
        mProgressBar->setRange(PROGRESSBAR_MIN,PROGRESSBAR_MAX);
        HbStyle::setItemName(mProgressBar, "progressbar");
        repolish();
    }

    mProgressBar->setProgressValue(value);
    
    // Start hiding the count down.
    if (PROGRESSBAR_MAX <= value){
        hideProgressBar();
    }
}


/*!
    \return The value of the download progress bar.
*/
int NmAttachmentListItem::progressBarValue() const
{
    NM_FUNCTION;
    
    int ret = 0;

    if (mProgressBar) {
        ret = mProgressBar->progressValue();
    }

    return ret;
}


/*!
    Hides the progress bar. Used if the download is cancelled before 100
    percent is reached.
*/
void NmAttachmentListItem::hideProgressBar()
{
    NM_FUNCTION;
    
    QTimer::singleShot(PROGRESSBAR_HIDE_COUNTDOWN, this, SLOT(removeProgressBar()));
}


/*!
    Displays a visual effect according to the current pressed state.

    \param pressed True, if the item was pressed.
*/
void NmAttachmentListItem::setPressed(bool pressed)
{
    if (pressed) {
        setProperty("state", "pressed");
        mBackground->frameDrawer().setFrameGraphicsName(LIST_ITEM_BG_FRAME_PRESSED);
        HbEffect::cancel(mBackground, "released");
        HbEffect::start(mBackground, "mailAttachmentWidget", "pressed");
    }
    else {
        setProperty("state", "normal");
        mBackground->frameDrawer().setFrameGraphicsName(LIST_ITEM_BG_FRAME_NORMAL);
        HbEffect::cancel(mBackground, "pressed");
        HbEffect::start(mBackground, "mailAttachmentWidget", "released");
    }    
}


/*!
    Handles the given gesture event.

    \param event The event to handle.
*/
void NmAttachmentListItem::gestureEvent(QGestureEvent *event)
{
    NM_FUNCTION;
    
    if (HbTapGesture *tap = qobject_cast<HbTapGesture *>(event->gesture(Qt::TapGesture))) {
        switch (tap->tapStyleHint()) {
            case HbTapGesture::Tap: {
                Qt::GestureState state = tap->state();
                HbInstantFeedback::play(HbFeedback::Basic);
                setPressed(true);

                if (state == Qt::GestureFinished) {
                    emit itemActivated();
                    setPressed(false);
                }
                else if (state == Qt::GestureCanceled) {
                    setPressed(false);
                }

                break;
            }
            case HbTapGesture::TapAndHold: {
                emit itemLongPressed(event->mapToGraphicsScene(tap->position()));
                setPressed(false);    
                break;
            }
        } // switch
    }
    else {
        // Let the HbWidget handle the event.
        HbWidget::gestureEvent(event);
    }
}


/*!
    Hides and deletes the download progress bar.
*/
void NmAttachmentListItem::removeProgressBar()
{
    NM_FUNCTION;
    
    if (mProgressBar) {
        HbStyle::setItemName(mProgressBar, "");
        mProgressBar->deleteLater();
        mProgressBar = 0;
        repolish();
    }
}


/*!
    Initializes the widget.
*/
void NmAttachmentListItem::init( )
{
    NM_FUNCTION;
    
    constructUi();

    // Set the default flag values.
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsSelectable);
}


/*!
    Constructs the UI, sets style item names etc.
*/
void NmAttachmentListItem::constructUi()
{
    NM_FUNCTION;
    
    // Background
    QScopedPointer<HbFrameItem> background(new HbFrameItem(this));
    background->frameDrawer().setFrameGraphicsName(LIST_ITEM_BG_FRAME_NORMAL);
    background->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
    setBackgroundItem(background.data());

    // The ownership was transferred to the base class.
    mBackground = background.take();
    
    // Construct the default UI.
    HbStyleLoader::registerFilePath(FILE_PATH_WIDGETML);
    HbStyleLoader::registerFilePath(FILE_PATH_CSS);
    
    QScopedPointer<HbTextItem> fileNameText(new HbTextItem(this));
    fileNameText->setObjectName("nmattachmentlistitem_filenametext");
    HbStyle::setItemName(fileNameText.data(), "filename");
    fileNameText->setElideMode(Qt::ElideRight);
    
    QScopedPointer<HbTextItem> fileSizeText(new HbTextItem(this));
    fileSizeText->setObjectName("nmattachmentlistitem_filenamesize");
    HbStyle::setItemName(fileSizeText.data(), "filesize");
    fileSizeText->setElideMode(Qt::ElideNone);
    
    // The ownership was transferred to this instance.
    mFileSizeText = fileSizeText.take(); 
    mFileNameText = fileNameText.take();
}



// End of file.
