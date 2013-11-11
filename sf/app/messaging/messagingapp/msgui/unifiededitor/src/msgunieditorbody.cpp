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

// INCLUDES
#include <HbTextEdit>
#include <HbTextItem>
#include <HbFrameItem>
#include <HbFrameDrawer>
#include <HbIconItem>
#include <HbAction>
#include <HbMainWindow>
#include <HbDeviceProfile>
#include <QImageReader>
#include <QFileInfo>
#include <MsgMediaResolver.h>
#include <MsgImageInfo.h>
#include <HbIconAnimationManager>
#include <HbIconAnimator>
#include <HbIcon>
#include <QGraphicsLinearLayout>
#include <HbNotificationDialog>

#include <csmsaccount.h>
#include <smutset.h>
#include <xqaiwrequest.h>
#include <xqrequestinfo.h>
#include <xqsharablefile.h>
#include <xqappmgr.h>
#include <xqconversions.h>
#include <hbmessagebox.h>
#include <hbcolorscheme.h>
#include <QColor>
// USER INCLUDES
#include "msgunieditorbody.h"
#include "UniEditorGenUtils.h"
#include "UniSendingSettings.h"
#include "msgunieditormonitor.h"

#include "mmsconformancecheck.h"
#include "unieditorpluginloader.h"
#include "unieditorplugininterface.h"
#include "convergedmessage.h"
#include "msgmediautil.h"
#include "msgunieditorpixmapwidget.h"
#include "msgunieditoraudiowidget.h"

// Constants
const QString BACKGROUND_FRAME("qtg_fr_messaging_char_count");
const QString CHAR_COUNTER_COLOR("qtc_messaging_char_count");

const int KShowCounterLimit = 10;
const int BYTES_TO_KBYTES_FACTOR = 1024; 

//Localized Constants for item specific menu
#define LOC_TITLE   hbTrId("txt_messaging_title_messaging")
#define LOC_UNABLE_TO_ADD_CONTENT hbTrId("txt_messaging_dpopinfo_unable_to_add_more_content")
#define LOC_UNABLE_TO_ATTACH_ITEM hbTrId("txt_messaging_dpopinfo_unable_to_attach_item_avai")
#define LOC_PROCESSING hbTrId("txt_messaging_formlabel_loading")
#define LOC_HINT_TEXT hbTrId("txt_messaging_formlabel_enter_message_here")
#define LOC_ATTACHED_PHOTO_SIZE hbTrId("txt_messaging_dpopinfo_attached_photo_size_is_l1")

const QString ANIMATION_ICON("qtg_anim_loading");
const QString ANIMATION_FILE(":/qtg_anim_loading.axml");
// LOCAL FUNCTIONS

//---------------------------------------------------------------
// showInsertFailureNote
// @return fullPath of unified editor's temporary dir
//---------------------------------------------------------------
void showInsertFailureNote()
{
    int availableSize =
            (MsgUnifiedEditorMonitor::maxMmsSize() - MsgUnifiedEditorMonitor::messageSize())
            /BYTES_TO_KBYTES_FACTOR;
    QString displayStr = QString(LOC_UNABLE_TO_ATTACH_ITEM)
            .arg(availableSize);

    HbMessageBox::information(displayStr, 0, 0, HbMessageBox::Ok);
}


MsgUnifiedEditorBody::MsgUnifiedEditorBody( QGraphicsItem *parent ) :
MsgUnifiedEditorBaseWidget(parent),
mHasImage(false),
mHasAudio(false),
mTextEdit(0),
mPixmapItem(0),
mAudioItem(0),
mImageSize(0),
mAudioSize(0),
mVideoSize(0),
mProcessImageOperation(0),
mMediaResolver(0),
mImageInfo(0),
mProcessingWidget(0),
mDraftMessage(false),
mIsImageResizing(false)
{
    mTextEdit = new HbTextEdit(this);
    mTextEdit->setPlaceholderText(LOC_HINT_TEXT);
    mTextEdit->setFontSpec(HbFontSpec(HbFontSpec::Secondary));
    HbStyle::setItemName(mTextEdit,"textEdit");
    connect(mTextEdit, SIGNAL(contentsChanged()), this, SLOT(onTextChanged()));

    mMmsConformanceCheck = new MmsConformanceCheck;
    
    mCharCounter = new HbTextItem(this);
    HbStyle::setItemName(mCharCounter, "charCounter");
    mCharCounter->setZValue(1.5);

    QColor color = HbColorScheme::color(CHAR_COUNTER_COLOR);
    mCharCounter->setTextColor(color);

    mBackgroundItem = new HbFrameItem(this);
    HbStyle::setItemName(mBackgroundItem, "charCounterFrame");

    mBackgroundItem->frameDrawer().setFrameType(HbFrameDrawer::ThreePiecesHorizontal);
    mBackgroundItem->frameDrawer().setFillWholeRect(true);
    
    mBackgroundItem->frameDrawer().setFrameGraphicsName(BACKGROUND_FRAME);
    
    mPluginLoader = new UniEditorPluginLoader(this);

    mPluginInterface =
                        mPluginLoader->getUniEditorPlugin(ConvergedMessage::Sms);    
    
    TRAP_IGNORE(
    CSmsSettings* settings = CSmsSettings::NewLC();
    CSmsAccount* account = CSmsAccount::NewLC();
    account->LoadSettingsL( *settings );

    if( settings->CharacterSet() == TSmsDataCodingScheme::ESmsAlphabetUCS2)
    {
    mCharSupportType = TUniSendingSettings::EUniCharSupportFull;
    }
    else
    {
    mCharSupportType = TUniSendingSettings::EUniCharSupportReduced;
    }
    
    CleanupStack::PopAndDestroy( account );
    CleanupStack::PopAndDestroy( settings );                
    );

    mCharCounter->setVisible(false);
    mBackgroundItem->setVisible(false);
}

MsgUnifiedEditorBody::~MsgUnifiedEditorBody()
{
    delete mMmsConformanceCheck;
    delete mProcessImageOperation;
    delete mMediaResolver;
    delete mImageInfo;
}

QString MsgUnifiedEditorBody::text()
{
    return mTextEdit->toPlainText();
}

void MsgUnifiedEditorBody::setImage(QString& imagefile, bool draftMessage)
    {
    // do nothing if filepath is empty
    if (imagefile.isEmpty())
        {
        return;
        }

    mDraftMessage = draftMessage;
    if (!mImageInfo)
        {
        setImage(true);

        mImageFile = imagefile;
        if (mPixmapItem)
            {
            mPixmapItem->setParent(NULL);
            delete mPixmapItem;
            mPixmapItem = NULL;
            mImageSize = 0;
            }

        int error = KErrNone;
        if (!mDraftMessage)
            {
        // if image is in draft, no need to resize it because it is resized already
            if (!mProcessImageOperation)
                {
                TRAP(error,mProcessImageOperation =
                        CUniEditorProcessImageOperation::NewL(*this));
                }
            if (!mMediaResolver && error == KErrNone)
                {
                TRAP(error,mMediaResolver = CMsgMediaResolver::NewL());
                }

            if (error == KErrNone)
                {
                mMediaResolver->SetCharacterSetRecognition(EFalse);
                HBufC *name = XQConversions::qStringToS60Desc(imagefile);
                RFile file;
                TRAP(error, file = mMediaResolver->FileHandleL(*name));
                if (error == KErrNone)
                    {
                    TRAP(error,mImageInfo = static_cast<CMsgImageInfo*>
                            (mMediaResolver->CreateMediaInfoL(file)));
                    if (error == KErrNone)
                        {
                        TRAP(error, mMediaResolver->ParseInfoDetailsL(
                                        mImageInfo, file));
                        }
                    file.Close();
                    }
                delete name;
                }
            }
        if (error == KErrNone && !mDraftMessage)
            {
            mSavedImageFile = imagefile;
            startResizeAnimation();
            mIsImageResizing = true;
            mProcessImageOperation->Process(mImageInfo);
            }
        else
            {
            if (mImageInfo)
                {
                delete mImageInfo;
                mImageInfo = NULL;
                }
            mSavedImageFile.clear();
            handleSetImage();
            }

        }
    }

void MsgUnifiedEditorBody::handleSetImage()
{   
    //check for insert conformance
    if(EInsertSuccess != mMmsConformanceCheck->checkModeForInsert(mImageFile))
        return;
    
    int msgSize = messageSize();
    QFileInfo fileinfo(mImageFile);
    int imageSize = fileinfo.size() + KEstimatedMimeHeaderSize;
    if ( (imageSize + msgSize) <= MsgUnifiedEditorMonitor::maxMmsSize())
    {
        mImageSize = imageSize;
    }
    else
    {
        mImageFile.clear();
        setImage(false);
        //Show appropriate note and leave
        showInsertFailureNote();
        return;
    }

    mPixmapItem = new MsgUnifiedEditorPixmapWidget(this);
    mPixmapItem->hide();
    HbStyle::setItemName(mPixmapItem, "pixmap");
    mPixmapItem->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    mPixmapItem->populate(mImageFile);
    connect(mPixmapItem,SIGNAL(remove()),this,SLOT(removeMedia()));
    
    // repolish the body widget
    this->repolish();

    // emit signal to indicate addition of image
    emit contentChanged();
}

void MsgUnifiedEditorBody::setAudio(QString& audiofile)
{
    // do nothing if filepath is empty
    if(audiofile.isEmpty())
    {
        return;
    }

    //check for insert conformance
    if(EInsertSuccess != mMmsConformanceCheck->checkModeForInsert(audiofile))
        return;

    setAudio(true);
    mAudioFile = audiofile;
    if(mAudioItem)
    {
        mAudioItem->setParent(NULL);
        delete mAudioItem;
        mAudioItem = NULL;
        mAudioSize = 0;
    }

    int msgSize = messageSize();
    QFileInfo fileinfo(mAudioFile);
    int audioSize = fileinfo.size() + KEstimatedMimeHeaderSize;
    if((audioSize + msgSize) <= MsgUnifiedEditorMonitor::maxMmsSize() )
    {
    	mAudioSize = audioSize;
    }
    else
    {
    	mAudioFile.clear();
    	setAudio(false);
    	//Show appropriate note and leave
    	showInsertFailureNote();
    	return;
    }    

    mAudioItem = new MsgUniFiedEditorAudioWidget(this);
    mAudioItem->hide();
    HbStyle::setItemName(mAudioItem,"audioItem");
    mAudioItem->populate(mAudioFile);
    connect(mAudioItem,SIGNAL(remove()),this,SLOT(removeMedia()));

    // repolish the body widget
    this->repolish();

    // emit signal to indicate addition of audio
    emit contentChanged();
}

void MsgUnifiedEditorBody::setText(QString& text)
{
    if(!text.isEmpty())
    {    
        mTextEdit->setPlainText(text);
    }
}

const QStringList MsgUnifiedEditorBody::mediaContent()
{
    QStringList mediaFiles;
    // Pick mImageFile only if mSavedImageFile is empty 
    // as it means we are not in middle of resizing
    if (mHasImage && mSavedImageFile.isEmpty()) 
        mediaFiles << mImageFile;
    if(mHasAudio)
        mediaFiles << mAudioFile;

    return mediaFiles;
}

QSizeF MsgUnifiedEditorBody::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    QSizeF szHint = HbWidget::sizeHint(which,constraint);

    HbMainWindow* mMainWindow = this->mainWindow();    
    if(!mMainWindow)
    {
        return szHint;
    }

    qreal leftMargin = 0.0;
    qreal rightMargin = 0.0;
    qreal chromeHeight = 0.0;
    qreal toolbarHeight = 0.0;
    qreal bodyItemSpacing = 0.0;
    
    style()->parameter("hb-param-margin-gene-left",leftMargin);
    style()->parameter("hb-param-margin-gene-right",rightMargin);
    style()->parameter("hb-param-widget-chrome-height",chromeHeight);
    style()->parameter("hb-param-widget-toolbar-height",toolbarHeight);
    style()->parameter("hb-param-margin-gene-middle-vertical",bodyItemSpacing);

    HbDeviceProfile prf = HbDeviceProfile::profile(mMainWindow);
    qreal unt = prf.unitValue();

    // Note: With NGA, the orientation change does not cause HW to switch orientation.
    // So, the HW resolution also remains unchanged. We need to swap values of width &
    // height to map portrait resolution to landscape resolution.
    qreal maxWidth = 0.0, maxHeight = 0.0;
    
    // Landscape
    if(mMainWindow->orientation() == Qt::Horizontal)
        {
        maxHeight = mMainWindow->width()-chromeHeight-toolbarHeight;
        maxWidth = (mMainWindow->height()-leftMargin-rightMargin-(2*unt))/2;
        }
    else
    {
        maxWidth = mMainWindow->width()-leftMargin-rightMargin;
        maxHeight = mMainWindow->height()-chromeHeight-toolbarHeight;
    }
    szHint.setHeight(maxHeight);
    
    // add space for audio
    if(mAudioItem)
    {
		if(mMainWindow->orientation() == Qt::Horizontal)
		{
			mAudioItem->setStretched(true);
		}
		else
		{
			mAudioItem->setStretched(false);
		}
        QSizeF audioSize = mAudioItem->effectiveSizeHint(which, constraint);
        szHint.rheight() += (audioSize.height() + bodyItemSpacing);
        mAudioItem->show();
    }
    
    if(mPixmapItem || mProcessingWidget)
        {
        QSizeF imageSize(0.0,0.0);
        QSizeF defaultImageSize(QImageReader(mImageFile).size());

        if(!defaultImageSize.isEmpty())
            {
            imageSize.setWidth(maxWidth);
            if(mMainWindow->orientation() == Qt::Vertical)
            {
                szHint.rheight() += bodyItemSpacing;
            }

            qreal newHeight = 0.0;
            if(defaultImageSize.width() <= imageSize.width())
            {
                // resize not needed
                newHeight = qMin(defaultImageSize.height(), maxHeight);
                imageSize.setHeight(newHeight);
            }
            else
            {
                // keep aspect-ratio and resize
                newHeight = imageSize.width()*(defaultImageSize.height()/defaultImageSize.width());
                newHeight = qMin(newHeight,maxHeight);
                imageSize.setHeight(newHeight);
            }

            if(newHeight == maxHeight)
            {
                qreal newWidth = defaultImageSize.width()*newHeight/defaultImageSize.height();
                imageSize.setWidth(newWidth);
            }

            if(mPixmapItem)
            {
            mPixmapItem->setPreferredSize(imageSize);
            mPixmapItem->setSize(imageSize);
            if(mMainWindow->orientation() == Qt::Horizontal)
            {
                QPointF currPos = mPixmapItem->pos();
                currPos.setX(leftMargin+((maxWidth-imageSize.width())/2));
                mPixmapItem->setPos(currPos);
            }
            mPixmapItem->show();
            }
            
            if(mProcessingWidget)
            {
                imageSize.setHeight(mProcessingWidget->preferredHeight());
                mProcessingWidget->setPreferredSize(imageSize);
                mProcessingWidget->show();
            }
            szHint.rheight() += imageSize.height();
        }
    }
    

    mTextEdit->setMinimumHeight(maxHeight);
    szHint.rheight() += bodyItemSpacing;
    return szHint;
}

void MsgUnifiedEditorBody::removeMedia()
{
    QObject* senderObject = sender();
    
    MsgUnifiedEditorPixmapWidget* pixmap = 
        qobject_cast<MsgUnifiedEditorPixmapWidget*>(senderObject);

    MsgUniFiedEditorAudioWidget* audio = 
        qobject_cast<MsgUniFiedEditorAudioWidget*>(senderObject);
    
    if(pixmap) //remove image
    {
        mImageFile.clear();
        if(mPixmapItem)
        {
            mPixmapItem->setParent(NULL);
            delete mPixmapItem;
            mPixmapItem = NULL;
        }
        setImage(false);
        mImageSize = 0;
    }
    else if(audio)//remove audio item
        {
            mAudioFile.clear();
            if(mAudioItem)
            {
                mAudioItem->setParent(NULL);
                delete mAudioItem;
                mAudioItem = NULL;
            }
            setAudio(false);
            mAudioSize = 0;
        }

    this->repolish();

	emit contentChanged();
}

bool MsgUnifiedEditorBody::hasImage()
{
    return mHasImage;
}

void MsgUnifiedEditorBody::setImage(bool image)
{
    mHasImage = image;
}

bool MsgUnifiedEditorBody::hasAudio()
{
    return mHasAudio;
}

void MsgUnifiedEditorBody::setAudio(bool audio)
{
    mHasAudio = audio;
}

int MsgUnifiedEditorBody::bodySize()
{
	int bodysize = 0;
	
	if( mImageSize || mTextEdit->toPlainText().size() || 
	    mAudioSize || mVideoSize )
	{
	     
	    UniEditorGenUtils* genUtils = q_check_ptr(new UniEditorGenUtils);
	    
        bodysize +=  mImageSize + mAudioSize + mVideoSize +
            genUtils->UTF8Size(mTextEdit->toPlainText()) +
            KEstimatedMimeHeaderSize + KEstimatedMmsSmilHeaderSize;
        delete genUtils;
	}
	return bodysize;
}

int MsgUnifiedEditorBody::messageSize()
{
    int estimatedMediaSize = bodySize();
    if(!estimatedMediaSize)
    {
        // This is the first media content to be inserted
        estimatedMediaSize = KEstimatedMmsSmilHeaderSize;
    }
    
    return estimatedMediaSize + MsgUnifiedEditorMonitor::subjectSize() + MsgUnifiedEditorMonitor::containerSize();
}

void MsgUnifiedEditorBody::onTextChanged()
{   
    QString string = text();

    if( string.size() > mPrevBuffer.size() &&
        MsgUnifiedEditorMonitor::messageType() == ConvergedMessage::Mms )
    {
        // reject any text input if mms size limit is reached
        int futureSize = bodySize() +
                MsgUnifiedEditorMonitor::containerSize() + MsgUnifiedEditorMonitor::subjectSize();
        if(futureSize > MsgUnifiedEditorMonitor::maxMmsSize())
        {
            mTextEdit->setPlainText(mPrevBuffer);
            HbMessageBox::information(LOC_UNABLE_TO_ADD_CONTENT, 0, 0, HbMessageBox::Ok);
            mTextEdit->setCursorPosition(mPrevBuffer.length());
            return;
        }
        else if(!mPrevBuffer.isEmpty())
        {
            //Save the previous buffer
            mPrevBuffer = string;
            // emit signal to indicate change in content
            emit contentChanged();
            return;
        }
    }

    //Check done for optimization
    //Only if content is deleted we need to call encodingsettings again
    if (mPrevBuffer.isEmpty() || string.size() <= mPrevBuffer.size())
    {
        mPluginInterface->setEncodingSettings(EFalse, ESmsEncodingNone,
            mCharSupportType);
    }

    TInt numOfRemainingChars;
    TInt numOfPDUs;
    TBool unicodeMode;
    TSmsEncoding alternativeEncodingType;
    mPluginInterface->getNumPDUs(string, numOfRemainingChars, numOfPDUs,
        unicodeMode, alternativeEncodingType);

    //Save the unicode value returned
    mUnicode = unicodeMode;
    //Save the old buffer
    mPrevBuffer = string;
    
    // emit signal to indicate change in content
    emit contentChanged();
    
    if(MsgUnifiedEditorMonitor::messageType() == ConvergedMessage::Sms)
    {
        //Set char counter value
        QString display = QString("%1(%2)").arg(numOfRemainingChars).arg(
            numOfPDUs);
        mCharCounter->setText(display);
        
        if (numOfPDUs > 1 || numOfRemainingChars <= KShowCounterLimit)
        {
            mCharCounter->setVisible(true);
            mBackgroundItem->setVisible(true);
        }
        else
        {
            mCharCounter->setVisible(false);
            mBackgroundItem->setVisible(false);
        }
    }
}

void MsgUnifiedEditorBody::EditorOperationEvent(
    TUniEditorProcessImageOperationEvent aEvent, TFileName& aFileName)
{
    delete mImageInfo;
    mImageInfo = NULL; 

    if (aEvent == EUniEditorProcessImageOperationComplete && 
            aFileName.Length() > 0)
    {
        mImageFile = XQConversions::s60DescToQString(aFileName);
        
        QSize modifiedSize(QImageReader(mImageFile).size());
        QString information = LOC_ATTACHED_PHOTO_SIZE.arg(modifiedSize.width()).arg(modifiedSize.height());
        HbNotificationDialog::launchDialog(information);        
    }
    else
    {
        mImageFile = mSavedImageFile;
    }
    mSavedImageFile.clear();

   // image resize is complete. reset the image resize flag
    mIsImageResizing = false;
    //handle the processed image from ProcessImage Operation
    handleSetImage();
    
    stopResizeAnimation();
}

void MsgUnifiedEditorBody::startResizeAnimation()
{
    // emit signal to indicate disable the send tool button.
    emit enableSendButton(false);
    
	
    QGraphicsLinearLayout* processingLayout = new QGraphicsLinearLayout(Qt::Vertical);
    
    mProcessingWidget = new HbWidget(this);
    HbStyle::setItemName(mProcessingWidget,"pixmap"); 
    mProcessingWidget->hide();
    mProcessingWidget->setLayout(processingLayout);
    
    HbTextItem* processingText = new HbTextItem(LOC_PROCESSING,mProcessingWidget);
    processingText->setFontSpec(HbFontSpec(HbFontSpec::Secondary));
    processingText->setAlignment(Qt::AlignCenter);
    processingLayout->addItem(processingText);
    
    HbIconItem* animationItem = new HbIconItem(ANIMATION_ICON,mProcessingWidget);
    animationItem->setAlignment(Qt::AlignHCenter);
    processingLayout->addItem(animationItem);
    
    HbIconAnimator& iconAnimator = animationItem->animator();
    HbIconAnimationManager* iconAnimationManager = HbIconAnimationManager::global();
    iconAnimationManager->addDefinitionFile(ANIMATION_FILE);
    
    iconAnimator.startAnimation();
    
    this->repolish();    
}

void MsgUnifiedEditorBody::stopResizeAnimation()
{
    if(mProcessingWidget)
    {
        delete mProcessingWidget;
        mProcessingWidget = NULL;
    }
	
}

// ---------------------------------------------------------
// MsgUnifiedEditorBody::isUnicode
// ---------------------------------------------------------
//
bool MsgUnifiedEditorBody::isUnicode()
{
    return mUnicode;
}

// ---------------------------------------------------------
// MsgUnifiedEditorBody::disableCharCounter
// ---------------------------------------------------------
//
void MsgUnifiedEditorBody::disableCharCounter()
{
    mCharCounter->setVisible(false);
    mBackgroundItem->setVisible(false);
}

// ---------------------------------------------------------
// MsgUnifiedEditorBody::enableCharCounter
// ---------------------------------------------------------
//
void MsgUnifiedEditorBody::enableCharCounter()
    {
    mPluginInterface->setEncodingSettings(EFalse, ESmsEncodingNone,
            mCharSupportType);

    TInt numOfRemainingChars;
    TInt numOfPDUs;
    TBool unicodeMode;
    TSmsEncoding alternativeEncodingType;
    QString string = mTextEdit->toPlainText();
    mPluginInterface->getNumPDUs(string,
            numOfRemainingChars, numOfPDUs, unicodeMode,
            alternativeEncodingType);

    //Save the unicode value returned
    mUnicode = unicodeMode;

    //Set char counter value
    QString display = QString("%1(%2)").arg(numOfRemainingChars).arg(
            numOfPDUs);
    mCharCounter->setText(display);

    if (numOfPDUs > 1 || numOfRemainingChars <= KShowCounterLimit)
        {
        mCharCounter->setVisible(true);
        mBackgroundItem->setVisible(true);
        }
    else
        {
        mCharCounter->setVisible(false);
        mBackgroundItem->setVisible(false);
        }

    }

//---------------------------------------------------------------
// MsgUnifiedEditorBody :: setFocus
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorBody::setFocus()
{
    mTextEdit->setFocus(Qt::MouseFocusReason);
}

//---------------------------------------------------------------
// MsgUnifiedEditorBody :: removeMediaContent
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorBody::removeMediaContent(QString fileName)
{
    if(!(mImageFile.compare(fileName))) //remove image
    {
        mImageFile.clear();
        if(mPixmapItem)
        {
            mPixmapItem->setParent(NULL);
            delete mPixmapItem;
            mPixmapItem = NULL;
        }
        setImage(false);
        mImageSize = 0;
    }
    else if(!(mAudioFile.compare(fileName)))//remove audio item
    {
        mAudioFile.clear();
        if(mAudioItem)
        {
            mAudioItem->setParent(NULL);
            delete mAudioItem;
            mAudioItem = NULL;
        }
        setAudio(false);
        mAudioSize = 0;
    }

    this->repolish();

    emit contentChanged();
}
// EOF
