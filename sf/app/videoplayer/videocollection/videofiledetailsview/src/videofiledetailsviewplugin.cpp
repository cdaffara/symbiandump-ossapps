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
* Description:   VideoCollectionViewPlugin class implementation
*
*/

// Version : %version: 76.1.9 %

// INCLUDE FILES
#include <qcoreapplication.h>
#include <xqserviceutil.h>
#include <xqplugin.h>
#include <hbview.h>
#include <hbinstance.h>
#include <hbstackedwidget.h>
#include <hbmarqueeitem.h>
#include <hbpushbutton.h>
#include <hbaction.h>
#include <qabstractitemmodel.h>
#include <hbmessagebox.h>
#include <hblistwidget.h>
#include <hblistwidgetitem.h>
#include <hblistviewitem.h>
#include <hbparameterlengthlimiter.h>
#include <hbtextitem.h>
#include <hbscrollbar.h>
#include <cmath>
#include <thumbnailmanager_qt.h>
#include <shareui.h>

#include "videocollectionclient.h"
#include "videofiledetailsviewplugin.h"
#include "videocollectioncommon.h"
#include "mpxhbvideocommondefs.h"
#include "videocollectionwrapper.h"
#include "videoproxymodelgeneric.h"
#include "videoservices.h"
#include "videodetailslabel.h"
#include "videocollectiontrace.h"

// Object names.
const char* const VIDEO_DETAILS_OBJECT_NAME_THUMBLABEL          = "vc:FileDetailsThumbnailLabel";
const char* const VIDEO_DETAILS_OBJECT_NAME_DELETE_VIDEO        = "vc:FileDetailsMessageBoxDeleteVideo";
const char* const VIDEO_DETAILS_OBJECT_NAME_MESSAGE_BOX_WARNING = "vc:FileDetailsMessageBoxWarning";
const char* const VIDEO_DETAILS_OBJECT_NAME_DELETE_ACTION       = "vc:FileDetailsDelete";
const char* const VIDEO_DETAILS_OBJECT_NAME_NAVKEY_BACK         = "vc:FileDetailsNavKeyBack";
const char* const VIDEO_DETAILS_OBJECT_NAME_TITLE_ANIM          = "vc:FileDetailsTitleAnim";

// Docml constants.
const char* const VIDEO_DETAILS_DOCML             = ":/xml/videofiledetails.docml";
const char* const VIDEO_DETAILS_PORTRAIT          = "portrait";
const char* const VIDEO_DETAILS_LANDSCAPE         = "landscape";
const char* const VIDEO_DETAILS_VIEW              = "videofiledetailsview";
const char* const VIDEO_DETAILS_TITLE             = "mLblTitle";
const char* const VIDEO_DETAILS_THUMBNAIL         = "mDetailsLabel";
const char* const VIDEO_DETAILS_BUTTON            = "mButton";
const char* const VIDEO_DETAILS_MENUACTION_DELETE = "mOptionsDelete";
const char* const VIDEO_DETAILS_LISTWIDGET        = "mDetailsList";

// Default thumbnail.
const char* const VIDEO_DETAILS_GFX_DEFAULT       = "qtg_large_video";

const int VIDEO_DETAILS_SECONDARY_TEXT_ROW_COUNT  = 255;

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoFileDetailsViewPlugin::VideoFileDetailsViewPlugin()
    : mModel( 0 )
    , mVideoServices( 0 )
    , mActivated( false )
    , mIsService( false )
    , mVideoId( TMPXItemId::InvalidId() )
    , mDeletedIndex( -1 )
    , mPreviousOrietation( Qt::Vertical )
    , mNavKeyBackAction( 0 )
    , mTitleAnim( 0 )
    , mThumbLabel( 0 )
    , mThumbnailManager( 0 )
    , mCollectionWrapper( VideoCollectionWrapper::instance() )
    , mShareUi(0)
{
	FUNC_LOG;
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoFileDetailsViewPlugin::~VideoFileDetailsViewPlugin()
{
	FUNC_LOG;
	destroyView();
}

// ---------------------------------------------------------------------------
// Create view
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::createView()
{
	FUNC_LOG;
	mLoader.reset();

	mActivated = false;

	bool ok = false;

	//Load the details view docml first
	mLoader.load(VIDEO_DETAILS_DOCML, &ok);

	if(!ok)
	{
	    ERROR(-1, "VideoFileDetailsViewPlugin::createView() failed to load docml.");
		return;
	}

	//Load portrait section by default as only vertical orientation is currently supported by videoplayer
	mLoader.load(VIDEO_DETAILS_DOCML, VIDEO_DETAILS_PORTRAIT, &ok);

	if(!ok)
	{
	    ERROR(-1, "VideoFileDetailsViewPlugin::createView() failed to load portrait view.");
		return;
	}

	mModel = mCollectionWrapper.getAllVideosModel();

	if (!mModel)
		{
	    ERROR(-1, "VideoFileDetailsViewPlugin::createView() get model failed.");
		return;
		}

	mIsService = XQServiceUtil::isService();

	// no deallocation needed for this since
	// stackedwidget takes ownership
	mTitleAnim = new HbMarqueeItem;
	HbFontSpec spec = mTitleAnim->fontSpec();
    spec.setRole( HbFontSpec::Primary );
    mTitleAnim->setFontSpec( spec );
    mTitleAnim->setLoopCount(-1);
    mTitleAnim->setObjectName(VIDEO_DETAILS_OBJECT_NAME_TITLE_ANIM);

	connect(mModel->sourceModel(),
			SIGNAL(shortDetailsReady(TMPXItemId)),
			this, SLOT(shortDetailsReadySlot(TMPXItemId)));

	connect(mModel->sourceModel(),
			SIGNAL(fullVideoDetailsReady(QVariant&)),
			this, SLOT(fullDetailsReadySlot(QVariant&)));

	connect(mModel,
			SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
			this, SLOT(rowsRemovedSlot(const QModelIndex&, int, int)));

	// Setup thumbnail widget.
	HbStackedWidget* thumbWidget = findWidget<HbStackedWidget>(VIDEO_DETAILS_THUMBNAIL);
	if(!thumbWidget)
	{
	    ERROR(-1, "VideoFileDetailsViewPlugin::createView() failed to load thumbnail widget.");
	    return;
	}

	// no deallocation needed for this since
	// stackedwidget takes ownership
	mThumbLabel = new VideoDetailsLabel;
	mThumbLabel->setAlignment(Qt::AlignCenter);
	mThumbLabel->setObjectName(VIDEO_DETAILS_OBJECT_NAME_THUMBLABEL);

	connect(mThumbLabel, SIGNAL(clicked(bool)), this, SLOT(startPlaybackSlot()));

	thumbWidget->addWidget(mThumbLabel);

	// Load details title.
	HbStackedWidget* title = findObject<HbStackedWidget>(VIDEO_DETAILS_TITLE);
	if(!title)
	{
	    ERROR(-1, "VideoFileDetailsViewPlugin::createView() failed to load title.");
	    return;
	}
	title->addWidget(mTitleAnim);

	// Load delete action.
	HbAction* deleteAction = findObject<HbAction>(VIDEO_DETAILS_MENUACTION_DELETE);
    if(!deleteAction)
    {
        ERROR(-1, "VideoFileDetailsViewPlugin::createView() failed to delete action.");
        return;
    }
    deleteAction->setObjectName(VIDEO_DETAILS_OBJECT_NAME_DELETE_ACTION);

	if (mIsService)
	{
		deleteAction->setVisible(false);
	}
	else
	{
		connect(deleteAction, SIGNAL(triggered(bool)), this, SLOT(deleteVideoSlot()));
	}

	// Create navigation keys.
	mNavKeyBackAction = new HbAction(Hb::BackNaviAction);
	mNavKeyBackAction->setObjectName(VIDEO_DETAILS_OBJECT_NAME_NAVKEY_BACK);

	if (!mThumbnailManager)
	{
		mThumbnailManager = new ThumbnailManager();
	}

	connect(mThumbnailManager, SIGNAL(thumbnailReady(QPixmap,void*,int,int)),
			this, SLOT(thumbnailReadySlot(QPixmap,void*,int,int)));

    HbListWidget* list = findWidget<HbListWidget>(VIDEO_DETAILS_LISTWIDGET);
    if(!list)
    {
        ERROR(-1, "VideoFileDetailsViewPlugin::activateView() failed to load details list widget.");
        return;
    }

    list->setEnabledAnimations(HbAbstractItemView::None);
    
    list->setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAsNeeded);
    
    HbScrollBar* bar = list->verticalScrollBar();
    
    if(bar)
    {
        bar->setInteractive(true);
    }
}

// ---------------------------------------------------------------------------
// Destroy view
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::destroyView()
{
	FUNC_LOG;
    if (mActivated)
    {
        deactivateView();
    }

    if(mVideoServices)
    {
    	mVideoServices->decreaseReferenceCount();
    	mVideoServices = 0;
    }

    delete mNavKeyBackAction;
    mNavKeyBackAction = 0;

    delete mThumbnailManager;
    mThumbnailManager = 0;

    disconnect();
    mLoader.reset();
}

// ---------------------------------------------------------------------------
// Activate view
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::activateView()
{
	FUNC_LOG;
	if (!mActivated)
    {
        HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);

        HbView *currentView = mainWnd->currentView();
        if(currentView && mNavKeyBackAction)
        {
        	if (connect(mNavKeyBackAction, SIGNAL(triggered()), this, SLOT(back())), Qt::UniqueConnection)
        	{
        		currentView->setNavigationAction(mNavKeyBackAction);
        	}
        	else
        	{
        	    ERROR(-1, "VideoFileDetailsViewPlugin::activateView() failed to connect navkey signal.");
        		return;
        	}
        }

        mainWnd->setOrientation(Qt::Vertical, false);

        // following if is for the future implementations where we should support
        // also landscape configuration.
        Qt::Orientation orientation = mainWnd->orientation();
        if ( (orientation == Qt::Vertical) && (orientation != mPreviousOrietation) )
        {
    		mPreviousOrietation = orientation;
        	mLoader.load(VIDEO_DETAILS_DOCML, VIDEO_DETAILS_PORTRAIT);
        }
        else if ( (orientation == Qt::Horizontal) && (orientation != mPreviousOrietation) )
        {
    		mPreviousOrietation = orientation;
    		mLoader.load(VIDEO_DETAILS_DOCML, VIDEO_DETAILS_LANDSCAPE);
        }

    	if (mIsService && !mVideoServices)
    	{
    		mVideoServices = VideoServices::instance();

			if (!mVideoServices)
			{
			    ERROR(-1, "VideoFileDetailsViewPlugin::activateView() failed to get video services instance.");
				return;
			}
    	}

		VideoServices::TVideoService service = VideoServices::ENoService;
		if (mIsService && mVideoServices)
		{
		    service = mVideoServices->currentService();

            HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);
            mainWnd->currentView()->setTitle(mVideoServices->contextTitle());
		}

        HbPushButton* button = findWidget<HbPushButton>(VIDEO_DETAILS_BUTTON);
		if(!button)
		{
		    ERROR(-1, "VideoFileDetailsViewPlugin::activateView() failed to load details button.");
		    return;
		}

	    // Fix the size of the thumbnail, as that needs to be in 16:9
	    qreal width = button->size().width();
	    qreal height = width * 9 / 16;

	    HbStackedWidget* thumbWidget = findWidget<HbStackedWidget>(VIDEO_DETAILS_THUMBNAIL);
	    thumbWidget->setPreferredWidth(width);
	    thumbWidget->setPreferredHeight(height);

		if (service == VideoServices::EUriFetcher)
		{
			HbIcon icon = HbIcon("qtg_mono_attach");
			button->setIcon(icon);

            connect(button, SIGNAL(clicked(bool)), this, SLOT(getFileUri()), Qt::UniqueConnection);
            connect(this, SIGNAL(fileUri(const QString&)), mVideoServices, SLOT(itemSelected(const QString&)), Qt::UniqueConnection);
		}
		else
		{
			connect(button, SIGNAL(clicked(bool)), this, SLOT(sendVideoSlot()), Qt::UniqueConnection);

			HbIcon icon = HbIcon("qtg_mono_share");
			button->setIcon(icon);
    	}

        connect(mainWnd,
                SIGNAL(orientationChanged(Qt::Orientation)),
                this, SLOT(orientationChange(Qt::Orientation)),
                Qt::UniqueConnection);

        connect(&mCollectionWrapper,
               SIGNAL(asyncStatus(int, QVariant&)),
               this, SLOT(handleErrorSlot(int, QVariant&)),
               Qt::UniqueConnection);

        // setup title size in order for animation to be enabled if needed
        mTitleAnim->setMinimumWidth(hbInstance->allMainWindows().value(0)->width()-50);
        mTitleAnim->adjustSize();

        mActivated = true;
    }
}

// ---------------------------------------------------------------------------
// Deactivate view
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::deactivateView()
{
	FUNC_LOG;
    if ( mActivated )
    {
        mVideoId = TMPXItemId::InvalidId();

        mDeletedIndex = -1;  // set index as invalid.

        HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);

        mainWnd->unsetOrientation();

        HbView *currentView = mainWnd->currentView();
        if(currentView)
        {
       		currentView->setNavigationAction(0);
        }

        disconnect(mNavKeyBackAction, SIGNAL(triggered()), this, SLOT(back()));

        disconnect(mainWnd, SIGNAL( orientationChanged( Qt::Orientation ) ),
             this, SLOT( orientationChange( Qt::Orientation ) ));

        disconnect(&mCollectionWrapper,
                       SIGNAL(asyncStatus(int, QVariant&)),
                       this, SLOT(handleErrorSlot(int, QVariant&)));

        mTitleAnim->stopAnimation();
        mTitleAnim->setText("");

        mActivated = false;

    	mThumbLabel->setIcon(HbIcon());

   		HbPushButton* button = findWidget<HbPushButton>(VIDEO_DETAILS_BUTTON);

    	if (mIsService)
    	{
			disconnect(button, SIGNAL(clicked(bool)), this, SLOT(getFileUri()));
			disconnect(this, SIGNAL(fileUri(const QString&)), mVideoServices, SLOT(itemSelected(const QString&)));
    	}
		else
		{
			disconnect(button, SIGNAL(clicked(bool)), this, SLOT(sendVideoSlot()));
		}
    }
    delete mShareUi;
    mShareUi = 0;
}

// ---------------------------------------------------------------------------
// Get view
// ---------------------------------------------------------------------------
//
QGraphicsWidget* VideoFileDetailsViewPlugin::getView()
{
	FUNC_LOG;
    return mLoader.findWidget(VIDEO_DETAILS_VIEW);
}

// ---------------------------------------------------------------------------
// Slot: Orientation change
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::orientationChange( Qt::Orientation orientation )
{
	FUNC_LOG;
    if (orientation == Qt::Vertical)
    {
    	mLoader.load(VIDEO_DETAILS_DOCML, VIDEO_DETAILS_PORTRAIT);
    }

    else if (orientation == Qt::Horizontal)
    {
       	mLoader.load(VIDEO_DETAILS_DOCML, VIDEO_DETAILS_LANDSCAPE);
    }
    mTitleAnim->adjustSize();
    mTitleAnim->startAnimation();
}

// ---------------------------------------------------------------------------
// Slot: back
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::back()
{
	FUNC_LOG;
    if (mActivated)
    {
        emit command( MpxHbVideoCommon::ActivateCollectionView );
    }
}

// ---------------------------------------------------------------------------
// Slot: shortDetailsReadySlot
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::shortDetailsReadySlot(TMPXItemId id)
{
	FUNC_LOG;
    // first clear all details, so that the view doesn't display the old data.
    findWidget<HbListWidget>(VIDEO_DETAILS_LISTWIDGET)->clear();

    QModelIndex modelIndex = mModel->indexOfId(id);

    mVideoId = id;

    QVariant variant = mModel->data(modelIndex, VideoCollectionCommon::KeyTitle);

    if (variant.isValid() && mTitleAnim)
    {
        mTitleAnim->setText(variant.toString());
    }
    startFetchingThumbnail();
}

// ---------------------------------------------------------------------------
// Slot: fullDetailsReadySlot
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::fullDetailsReadySlot(QVariant& variant)
{
	FUNC_LOG;
    using namespace VideoCollectionCommon;

    int detailCount = sizeof(VideoDetailLabelKeys) / sizeof(int);

    QMap<QString, QVariant> metadata = variant.toMap();

    HbListWidget* list = findWidget<HbListWidget>(VIDEO_DETAILS_LISTWIDGET);
    if(!list)
    {
        ERROR(-1, "VideoFileDetailsViewPlugin::activateView() failed to load details list widget.");
        return;
    }

    if(list->count())
    {
        list->clear();
    }

    HbListViewItem *prototype = list->listItemPrototype();
    prototype->setSecondaryTextRowCount(1, VIDEO_DETAILS_SECONDARY_TEXT_ROW_COUNT);

    for(int i = 0; i< detailCount; i++) {
        if (metadata.contains(VideoDetailLabelKeys[i]))
        {
            HbListWidgetItem* listWidgetItem = new HbListWidgetItem();
            listWidgetItem->setText( hbTrId(VideoDetailLabels[i]));
            listWidgetItem->setSecondaryText(metadata[VideoDetailLabelKeys[i]].toString());
            list->addItem(listWidgetItem);
            
            // following is required to change wrapping-mode for the second row
            HbListViewItem *viewItem = static_cast<HbListViewItem*>(list->viewItem(list->row(listWidgetItem)));
            if(viewItem)
            {
                HbTextItem *secondLine = static_cast<HbTextItem*>(viewItem->primitive("text-2"));
                if(secondLine)
                {
                    secondLine->setTextWrapping(Hb::TextWrapAnywhere);
                }
            }
        }
    }

    // start title animation
    mTitleAnim->startAnimation();
}

// ---------------------------------------------------------------------------
// Slot: getFileUri
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::getFileUri()
{
	FUNC_LOG;
	if (mVideoId != TMPXItemId::InvalidId())
    {
        QModelIndex modelIndex = mModel->indexOfId(mVideoId);
		QVariant variant = mModel->data(modelIndex, VideoCollectionCommon::KeyFilePath);
		if (variant.isValid())
		{
			QString itemPath = variant.value<QString>();
    		emit(fileUri(itemPath));
		}
    }
}

// ---------------------------------------------------------------------------
// Slot: startPlaybackSlot
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::startPlaybackSlot()
{
	FUNC_LOG;
	if (mVideoId != TMPXItemId::InvalidId())
	{
    	mModel->openItem(mVideoId);
	}
}

// ---------------------------------------------------------------------------
// Slot: sendVideo
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::sendVideoSlot()
{
	FUNC_LOG;

    if(mVideoId != TMPXItemId::InvalidId())
    {
        if(!mShareUi)
        {
            mShareUi = new ShareUi();
        }
        QModelIndex modelIndex = mModel->indexOfId(mVideoId);
        QVariant variant = mModel->data(modelIndex, VideoCollectionCommon::KeyFilePath);
        if(variant.isValid())
        {
            QStringList fileList;
            fileList.append(variant.toString());
            mShareUi->send(fileList, true);
        }
    }
}

// ---------------------------------------------------------------------------
// Slot: deleteVideo
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::deleteVideoSlot()
{
	FUNC_LOG;
	if (mVideoId != TMPXItemId::InvalidId())
        {
		QModelIndex modelIndex = mModel->indexOfId(mVideoId);
		QVariant variant = mModel->data(modelIndex, VideoCollectionCommon::KeyTitle);

        if (variant.isValid())
        {
        	QString text = HbParameterLengthLimiter(hbTrId("txt_videos_info_do_you_want_to_delete_1")).
        			arg(variant.toString());

            HbMessageBox *messageBox = new HbMessageBox(text, HbMessageBox::MessageTypeQuestion);
            messageBox->setAttribute(Qt::WA_DeleteOnClose);
            messageBox->setStandardButtons(HbMessageBox::Yes | HbMessageBox::No);
            messageBox->setObjectName(VIDEO_DETAILS_OBJECT_NAME_DELETE_VIDEO);
            messageBox->open(this, SLOT(deleteVideoDialogFinished(int)));
        }
    }
}

// ---------------------------------------------------------------------------
// deleteVideoDialogFinished
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::deleteVideoDialogFinished(int action)
{
    FUNC_LOG;
    QModelIndex modelIndex = mModel->indexOfId(mVideoId);
    if(action == HbMessageBox::Yes && modelIndex.isValid())
    {
        deleteItem(modelIndex);
    }
}

// ---------------------------------------------------------------------------
// deleteItem
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::deleteItem(QModelIndex index)
{
	FUNC_LOG;
    mDeletedIndex = index.row();

    QModelIndexList list;
    list.append(index);
    mModel->deleteItems(list);
}

// ---------------------------------------------------------------------------
// Slot: rowsRemovedSlot
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::rowsRemovedSlot(const QModelIndex& parent,
                                                 int first, int last)
{
	FUNC_LOG;
	Q_UNUSED(parent);

	if(mActivated && mDeletedIndex > -1 &&
       (mDeletedIndex >= first && mDeletedIndex <= last))
    {
        // item is withing the group of removed items, deactivate view
        emit command(MpxHbVideoCommon::ActivateCollectionView);
    }
}

// -------------------------------------------------------------------------------------------------
// Slot: handleErrorSlot()
// error signal received from collectionwrapper
// -------------------------------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::handleErrorSlot(int errorCode, QVariant &additional)
{
	FUNC_LOG;
    QString msg("");
    if(errorCode == VideoCollectionCommon::statusSingleDeleteFail)
    {
        QString format = hbTrId("txt_videos_info_unable_to_delete_1_it_is_current");
        if(additional.isValid())
        {
           msg = HbParameterLengthLimiter(format).arg(additional.toString());
        }
    }
    if(msg.count() > 0)
    {
        // show msg box if there's something to show
        HbMessageBox *messageBox = new HbMessageBox(msg, HbMessageBox::MessageTypeWarning);
        messageBox->setAttribute(Qt::WA_DeleteOnClose);
        messageBox->setObjectName(VIDEO_DETAILS_OBJECT_NAME_MESSAGE_BOX_WARNING);
        messageBox->show();
    }
}

// ---------------------------------------------------------------------------
// Slot: thumbnailReadySlot
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::thumbnailReadySlot(QPixmap pixmap,
        void * clientData, int id, int errorCode)
{
	FUNC_LOG;
    Q_UNUSED(clientData);
    Q_UNUSED(id);

	HbStackedWidget* thumbWidget = findWidget<HbStackedWidget>(VIDEO_DETAILS_THUMBNAIL);
	if(!thumbWidget)
	{
	    ERROR(-1, "VideoFileDetailsViewPlugin::thumbnailReadySlot() failed to load thumbnail widget.");
	    return;
	}

	QSize size(thumbWidget->size().toSize());

	if (!errorCode)
	{
		QImage sourceImage = pixmap.toImage();

		if ((sourceImage.size().height() != size.height()) || (sourceImage.size().width() != size.width()))
		{
			// Smooth scaling is very expensive (size^2). Therefore we reduce the size
			// to 2x of the destination size and using fast transformation before doing final smooth scaling.
			if (sourceImage.size().width() > (4*size.width()) || sourceImage.size().height() > (4*size.height()))
			{
				QSize intermediate_size = QSize( size.width() * 2, size.height() * 2 );
				sourceImage = sourceImage.scaled(intermediate_size, Qt::KeepAspectRatioByExpanding, Qt::FastTransformation );
			}
			sourceImage = sourceImage.scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
		}

		int difference(0);
		QRect rect = thumbWidget->rect().toRect();

		if(sourceImage.width() > size.width())
		{
			difference = sourceImage.width() - size.width();
			difference = (difference/2)+1;
			rect.moveLeft(rect.left()+difference);
		}
		else if(sourceImage.height() > size.height())
		{
			difference = sourceImage.height() - size.height();
			difference = (difference/2)+1;
			rect.moveBottom(rect.bottom()+difference);
		}

		sourceImage = sourceImage.copy(rect);
		QImage resultImage = QImage(sourceImage.size(), QImage::Format_ARGB32_Premultiplied);

		QPainter painter(&resultImage);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
		painter.fillRect(resultImage.rect(), Qt::transparent);
		painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
		painter.drawPixmap( (int)(sourceImage.width() - playIcon().width())/2,
				           (int)(sourceImage.height() - playIcon().height())/2,
				            playIcon() );
		painter.setCompositionMode(QPainter::CompositionMode_Screen);
        painter.drawImage(0, 0, sourceImage);
		painter.end();

		HbIcon compsedIcon(QPixmap::fromImage(resultImage));
	    mThumbLabel->setIcon(compsedIcon);
	}
    else
    {
        ERROR(errorCode, "VideoFileDetailsViewPlugin::thumbnailReadySlot() tbn fetch failed.");
		mThumbLabel->setIcon(HbIcon(VIDEO_DETAILS_GFX_DEFAULT));
	}
}

// ---------------------------------------------------------------------------
// startFetchingThumbnail
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::startFetchingThumbnail()
{
	FUNC_LOG;
    int tnId = -1;

    if(mModel && mThumbnailManager)
    {
    	QModelIndex modelIndex = mModel->indexOfId(mVideoId);
		QVariant variant = mModel->data(modelIndex, VideoCollectionCommon::KeyFilePath);
        if(variant.isValid())
        {
            mThumbnailManager->setThumbnailSize(ThumbnailManager::ThumbnailLarge);
            tnId = mThumbnailManager->getThumbnail(variant.toString(), 0, 5000);
        }
    }

    if(tnId == -1)
    {
        ERROR(-1, "VideoFileDetailsViewPlugin::startFetchingThumbnail() starting the fetch failed.");
    	mThumbLabel->setIcon(HbIcon(VIDEO_DETAILS_GFX_DEFAULT));
    }
}

// ---------------------------------------------------------------------------
// playIcon
// ---------------------------------------------------------------------------
//
const QPixmap &VideoFileDetailsViewPlugin::playIcon()
{
	FUNC_LOG;
    // Check if we have already the icon.
    if(!mPlayIcon.isNull())
    {
        return mPlayIcon;
    }

    // Compose the icon.
    HbIcon play =        HbIcon("qtg_mono_play");
    HbIcon topLeft =     HbIcon("qtg_fr_popup_trans_tl");
    HbIcon top =         HbIcon("qtg_fr_popup_trans_t");
    HbIcon topRight =    HbIcon("qtg_fr_popup_trans_tr");
    HbIcon left =        HbIcon("qtg_fr_popup_trans_l");
    HbIcon center =      HbIcon("qtg_fr_popup_trans_c");
    HbIcon right =       HbIcon("qtg_fr_popup_trans_r");
    HbIcon bottomLeft =  HbIcon("qtg_fr_popup_trans_bl");
    HbIcon bottom =      HbIcon("qtg_fr_popup_trans_b");
    HbIcon bottomRight = HbIcon("qtg_fr_popup_trans_br");

    int width = topLeft.width() + top.width() + topRight.width();
    int height = topLeft.height() + center.height() + bottomLeft.height();

    mPlayIcon = QPixmap(width, height);

    QPainter painter(&mPlayIcon);
    painter.fillRect(mPlayIcon.rect(), Qt::white);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    int x = 0;
    int y = 0;

    // Draw top
    painter.drawPixmap(QPoint(x, y), topLeft.pixmap());
    x += left.width();

    painter.drawPixmap(QPoint(x, y), top.pixmap());
    x += top.width();

    painter.drawPixmap(QPoint(x, y), topRight.pixmap());
    y += top.height();

    // Draw center
    x = 0;
    painter.drawPixmap(QPoint(x, y), left.pixmap());
    x += left.width();

    painter.drawPixmap(QPoint(x, y), center.pixmap());
    x += center.width();

    painter.drawPixmap(QPoint(x, y), right.pixmap());
    y += center.height();

    // Draw bottom
    x = 0;
    painter.drawPixmap(QPoint(x, y), bottomLeft.pixmap());
    x += left.width();

    painter.drawPixmap(QPoint(x, y), bottom.pixmap());
    x += top.width();

    painter.drawPixmap(QPoint(x, y), bottomRight.pixmap());

    // Draw play icon
    play.setSize(mPlayIcon.size());
    play.setColor(Qt::white);
    painter.drawPixmap(mPlayIcon.rect(), play.pixmap());
    painter.end();

    return mPlayIcon;
}

// ---------------------------------------------------------------------------
// findWidget
// ---------------------------------------------------------------------------
//
template<class T>
T* VideoFileDetailsViewPlugin::findWidget(QString name)
{
	FUNC_LOG;
    return qobject_cast<T *>(mLoader.findWidget(name));
}

// ---------------------------------------------------------------------------
// findObject
// ---------------------------------------------------------------------------
//
template<class T>
T* VideoFileDetailsViewPlugin::findObject(QString name)
{
	FUNC_LOG;
    return qobject_cast<T *>(mLoader.findObject(name));
}

XQ_EXPORT_PLUGIN2( videofiledetailsview, VideoFileDetailsViewPlugin );

// End of file
