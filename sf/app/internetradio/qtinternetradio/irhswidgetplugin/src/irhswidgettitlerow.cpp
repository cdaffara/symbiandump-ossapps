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
#include <QSettings>
#include <QPixmap>
#include <HbLabel>
#include <HbColorScheme>
#include <HbStyleLoader>
#include <HbTapGesture>
#include <HbEvent>

// User includes
#include "irhswidgettitlerow.h"
#include "irservicedef.h"
#include "irhswidgetuiloader.h"
#include "irqlogger.h"

// Constants
static const QString KIrHsWidgetTitleRowDocML       = ":/resource/irhswidgettitlerow.docml";
static const QString KIrHsWidgetTitleRowContainer   = "titlerow_layout";
static const QString KIrHsWidgetStationLogo         = "station_logo";
static const QString KNmHsWidgetStationName         = "station_name";

static const QString KStationNameColor      = "qtc_hs_list_item_title_normal";
static const QString KDefaultStationLogo    = "qtg_large_internet_radio";

static const int KIrHsWidgetContentsMargin  = 0;
static const int KIrHsWidgetLogoSize        = 50; // hs widget logo size

IrHsWidgetTitleRow::IrHsWidgetTitleRow(QGraphicsItem *aParent, Qt::WindowFlags aFlags) :
    HbWidget(aParent, aFlags), 
    mStationLogo(NULL), 
    mStationName(NULL)
{
    LOG_METHOD;
    loadUi();
    grabGesture(Qt::TapGesture);
}

IrHsWidgetTitleRow::~IrHsWidgetTitleRow()
{
    LOG_METHOD;
}

void IrHsWidgetTitleRow::loadUi()
{
    LOG_METHOD;
    IRHsWidgetUiLoader loader;
    loader.load(KIrHsWidgetTitleRowDocML);

    //Create layout
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);

    layout->setContentsMargins(KIrHsWidgetContentsMargin, KIrHsWidgetContentsMargin,
        KIrHsWidgetContentsMargin, KIrHsWidgetContentsMargin);
    layout->setSpacing(KIrHsWidgetContentsMargin);
    setLayout(layout);
    
    QGraphicsWidget *container = loader.findWidget(KIrHsWidgetTitleRowContainer);
    layout->addItem(container);

    mStationLogo = static_cast<HbLabel*> (loader.findWidget(KIrHsWidgetStationLogo));
    mStationName = static_cast<HbLabel*> (loader.findWidget(KNmHsWidgetStationName));
    mStationName->setTextColor(HbColorScheme::color(KStationNameColor));         
}

void IrHsWidgetTitleRow::setDefaultTitle()
{
    LOG_METHOD;
#ifdef SUBTITLE_STR_BY_LOCID 
    mStationName->setPlainText(hbTrId("txt_ir_list_internet_radio"));
#else
    mStationName->setPlainText(hbTrId("Internet Radio"));    
#endif
}

void IrHsWidgetTitleRow::setStationName(const QString& aStationName)
{
    LOG_METHOD;
    LOG_FORMAT("aStationName = %s", STRING2CHAR(aStationName));
    if (mStationName->plainText() != aStationName)
    {
        mStationName->setPlainText(aStationName);
    }   
}
    
void IrHsWidgetTitleRow::loadStationLogo()
{
    LOG_METHOD;
    QSettings settings(KIrSettingOrganization, KIrSettingApplication);
    if (settings.value(KIrSettingStationLogo).canConvert<QPixmap>())
    {
        QPixmap logoPixmap = settings.value(KIrSettingStationLogo).value<QPixmap>();
        QPixmap newLogoPixmap = 
             logoPixmap.scaled(QSize(KIrHsWidgetLogoSize,KIrHsWidgetLogoSize),Qt::KeepAspectRatio);
        QIcon logoQIcon(newLogoPixmap);
        HbIcon logoHbIcon(logoQIcon); 
        LOG("station logo loaded succesfully");
        mStationLogo->setIcon(logoHbIcon);
    }  
}

void IrHsWidgetTitleRow::setDefaultLogo()
{
    LOG_METHOD;
    mStationLogo->setIcon(KDefaultStationLogo);
}

void IrHsWidgetTitleRow::gestureEvent(QGestureEvent *aEvent)
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
        LOG("emit titleRowClicked()");
        emit titleRowClicked();
    }
}

//Implemented to receive theme change event
//@param QEvent, The change events to be received  
//
void IrHsWidgetTitleRow::changeEvent(QEvent *event)
{
    LOG_METHOD;
    if (HbEvent::ThemeChanged == event->type())
    {
        // get the text color from theme and 
        // set it to station name label
        mStationName->setTextColor(HbColorScheme::color(KStationNameColor));
    }
    
    HbWidget::changeEvent(event);
}
