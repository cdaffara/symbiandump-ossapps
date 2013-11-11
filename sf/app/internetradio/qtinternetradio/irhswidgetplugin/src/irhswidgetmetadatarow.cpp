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
* Description:  Internet Radio home screen widget
*
*/
// System includes
#include <QGraphicsLinearLayout>
#include <HbLabel>
#include <HbMarqueeItem>
#include <HbIconAnimationManager>
#include <HbIconAnimationDefinition>
#include <HbColorScheme>
#include <HbStyleLoader>
#include <HbTapGesture>
#include <HbEvent>

// User includes
#include "irhswidgetmetadatarow.h"
#include "irhswidgetuiloader.h"
#include "irqlogger.h"

// Constants
static const QString KIrHsWidgetMetaDataRowDocML       = ":/resource/irhswidgetmetadatarow.docml";
static const QString KIrHsWidgetMetaDataRowContainer   = "metadatarow_layout";
static const QString KIrHsWidgetMetaDataLayout         = "metadata_layout";
static const QString KIrHsWidgetMetaDataLabel          = "metadata_label";
static const QString KNmHsWidgetControlLayout          = "control_layout";
static const QString KNmHsWidgetControlLabel           = "control_label";

static const QString KPlayButtonIcon          = "qtg_mono_play";
static const QString KStopButtonIcon          = "qtg_mono_stop";
static const QString KMetaDataColor           = "qtc_hs_list_item_title_normal";
static const QString KControlLabelDataColor   = "qtc_hs_list_item_title_normal";
static const QString KIrHsWidgetCss           = ":/resource/irhswidget_color.css";

static const QString KLoadingAnimationPrefix  = "qtg_anim_loading_";
static const QString KLoadingIconName         = "LoadingAnimation";

static const int KIrHsWidgetContentsMargin  = 0;
static const int KLoadingFramePostfixMin    = 1;  // loading animation frame name postfix (min value) defined in layout guide.
static const int KLoadingFramePostfixMax    = 10; // loading animation frame name postfix (max value) defined in layout guide.
static const int KLoadingAnimationDuration  = 100; // duration for frames to construct a animation, unit : ms

IrHsWidgetMetaDataRow::IrHsWidgetMetaDataRow(QGraphicsItem *aParent, Qt::WindowFlags aFlags) :
    HbWidget(aParent, aFlags), 
    mMetaData(NULL), 
    mControlLayout(NULL),
    mControlLabel(NULL)
{
    LOG_METHOD;
    HbStyleLoader::registerFilePath(KIrHsWidgetCss);
    
    loadUi();
    createLoadingIcon();
    grabGesture(Qt::TapGesture);
}


IrHsWidgetMetaDataRow::~IrHsWidgetMetaDataRow()
{
    LOG_METHOD;
    HbStyleLoader::unregisterFilePath(KIrHsWidgetCss);
}

void IrHsWidgetMetaDataRow::loadUi()
{
    LOG_METHOD;
    IRHsWidgetUiLoader loader;
    loader.load(KIrHsWidgetMetaDataRowDocML);

    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
    layout->setContentsMargins(KIrHsWidgetContentsMargin, KIrHsWidgetContentsMargin,
        KIrHsWidgetContentsMargin, KIrHsWidgetContentsMargin);
    layout->setSpacing(KIrHsWidgetContentsMargin);
    setLayout(layout);

    QGraphicsWidget *container = loader.findWidget(KIrHsWidgetMetaDataRowContainer);
    layout->addItem(container);   
                      
    mMetaData = static_cast<HbMarqueeItem*> (loader.findWidget(KIrHsWidgetMetaDataLabel));
    mMetaData->setTextColor(HbColorScheme::color(KMetaDataColor));  
    mMetaData->setLoopCount(-1);
    
    mControlLayout = static_cast<HbLabel*> (loader.findWidget(KNmHsWidgetControlLayout));
    mControlLabel  = static_cast<HbLabel*> (loader.findWidget(KNmHsWidgetControlLabel));
}

void IrHsWidgetMetaDataRow::setMetaData(const QString &aMetaData)
{
    LOG_METHOD;
    LOG_FORMAT("aMetaData = %s", STRING2CHAR(aMetaData));
    if (mMetaData->text() != aMetaData)
    {
        mMetaData->setText(aMetaData);
        if (aMetaData.isEmpty())
        {
            mMetaData->stopAnimation();        
        }
        else
        {
            mMetaData->startAnimation();
        }
    }    
}

void IrHsWidgetMetaDataRow::startMetaDataMarquee()
{    
    LOG_METHOD;
    if (!mMetaData->text().isEmpty())
    {
        mMetaData->startAnimation();        
    }      
}
    
void IrHsWidgetMetaDataRow::stopMetaDataMarquee()
{
    LOG_METHOD;
    mMetaData->stopAnimation(); 
}   

void IrHsWidgetMetaDataRow::setPlayIcon()
{
    LOG_METHOD;
    mControlLabel->setIcon(KPlayButtonIcon);
}
    
void IrHsWidgetMetaDataRow::setStopIcon()
{
    LOG_METHOD;
    mControlLabel->setIcon(KStopButtonIcon);
}
    
void IrHsWidgetMetaDataRow::setLoadingIcon()
{
    LOG_METHOD;
    mControlLabel->setIcon(mLoadingIcon);
}    
    
void IrHsWidgetMetaDataRow::createLoadingIcon()
{
    LOG_METHOD;
    HbIconAnimationManager *animationManager = HbIconAnimationManager::global();
    HbIconAnimationDefinition animationDefinition;
    HbIconAnimationDefinition::AnimationFrame animationFrame;
    QList<HbIconAnimationDefinition::AnimationFrame> animationFrameList;
    
    QString animationFrameIconName;
    for (int i = KLoadingFramePostfixMin; i <= KLoadingFramePostfixMax; i++)
    {
        animationFrame.duration = KLoadingAnimationDuration;
        animationFrameIconName.clear();
        animationFrameIconName.append(KLoadingAnimationPrefix);
        animationFrameIconName.append(animationFrameIconName.number(i));
        animationFrame.iconName = animationFrameIconName;
        animationFrameList.append(animationFrame);
    }
    animationDefinition.setPlayMode(HbIconAnimationDefinition::Loop);
    animationDefinition.setFrameList(animationFrameList);
    animationManager->addDefinition(KLoadingIconName, animationDefinition);

    mLoadingIcon.setIconName(KLoadingIconName);        
}        

void IrHsWidgetMetaDataRow::gestureEvent(QGestureEvent *aEvent)
{
    LOG_METHOD;
    HbTapGesture *tapGesture = qobject_cast<HbTapGesture *>(aEvent->gesture(Qt::TapGesture));
    if (!tapGesture)
    {
        return;
    }
    
    if (Qt::GestureFinished == tapGesture->state()
        && HbTapGesture::Tap == tapGesture->tapStyleHint())
    {
        QPointF tapScenePoint = tapGesture->scenePosition();
        QPointF tapLocalPoint = mControlLayout->sceneTransform().inverted().map(tapScenePoint);
        if (mControlLayout->contains(tapLocalPoint))
        {
            LOG("emit controlAreaClicked()");
            emit controlAreaClicked();
        }
        else
        {
            LOG("emit metaDataAreaClicked()");
            emit metaDataAreaClicked();
        }
    }
}

//Implemented to receive theme change event
//@param QEvent, The change events to be received  
//
void IrHsWidgetMetaDataRow::changeEvent(QEvent *event)
{
    LOG_METHOD;
    if (HbEvent::ThemeChanged == event->type())
    {
        // get the text color from theme and 
        // set it to meta data and control label
        mMetaData->setTextColor(HbColorScheme::color(KMetaDataColor));
    }
    
    HbWidget::changeEvent(event);
}
