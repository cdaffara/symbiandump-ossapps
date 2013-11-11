/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
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

#include "cpkeyscreenview.h"
#include "cpkeyscreenmodel.h"
#include "cpkeyscreenconstants.h"
#include <hbdataform.h>
#include <QStringList>
#include <QDebug>
#include <QtCore/qobject.h>
#include <hbdataformmodel.h>
#include <cpsettingformitemdata.h>
#include <hbmessagebox.h>
#include <hbslider.h>
#include <hbstyleloader.h>

/*
 * Contructor 
 */
CpKeyScreenView::CpKeyScreenView(QGraphicsItem *parent) :
    CpBaseSettingView(0,parent),
    mScreenComboButton(0),
    mRotateCheckbox(0),
	mScreensaverCheckbox(0),
	mBrightSliderItem(0),
	mModel(0)
{   
    HbStyleLoader::registerFilePath(":/resources/hbslider_color.css");
    HbDataForm *form = qobject_cast<HbDataForm*> ( widget() );
    if (form) {
        // Valid range is:
        // 15 secs, 30 secs, 45 secs, 1 min, 2 mins
        mScreenLockValues.insert(KCpKeyscreenLock15s,hbTrId("txt_cp_setlabel_keys_screen_val_15_seconds"));
        mScreenLockValues.insert(KCpKeyscreenLock30s,hbTrId("txt_cp_setlabel_keys_screen_val_30_seconds"));
        mScreenLockValues.insert(KCpKeyscreenLock45s,hbTrId("txt_cp_setlabel_keys_screen_val_45_seconds"));
        mScreenLockValues.insert(KCpKeyscreenLock60s,hbTrId("txt_cp_setlabel_keys_screen_val_1_minute"));
        mScreenLockValues.insert(KCpKeyscreenLock120s,hbTrId("txt_cp_setlabel_keys_screen_val_2_minutes"));
        
        form->setHeading(hbTrId("txt_cp_subhead_keys_screen"));
        mModel = new CpKeyScreenModel();
        HbDataFormModel *model = new HbDataFormModel;

        if ( mModel->isKeyguardSupported() ) {
            makeScreenItem(*model);
        }
        if ( mModel->isScreensaverSupported() ) {
            makeScreensaverItem(*model);
        }        

        if ( mModel->isRotateSupported() ) {
            makeRotateItem(*model);
        }

        makeBrightnessItem(*model);

        form->setModel(model);
    }
}
/*
 * Create a key guard setting in setting form
 */
void CpKeyScreenView::makeScreenItem(HbDataFormModel& model)
{
    mScreenComboButton = new CpSettingFormItemData(HbDataFormModelItem::ComboBoxItem,
            hbTrId("txt_cp_setlabel_keys_screen_locked_after"));    
    
    HbDataForm * form = qobject_cast<HbDataForm*> ( widget() );
    if (form) {
        form->addConnection(
            mScreenComboButton,SIGNAL(currentIndexChanged(QString)),
            this,SLOT(screenValueChanged(QString)));    
    }
    model.appendDataFormItem(mScreenComboButton, model.invisibleRootItem());

    int period = mModel->keyguard();
    
    int selectedIndex(-1);    
    
    QMap<int,QString>::iterator it = mScreenLockValues.find(period);
    if (it == mScreenLockValues.end()) {
        mModel->setKeyguard(KCpKeyscreenLockDefault30s); // Set keyguard and backlight period to default
        selectedIndex = 1;
    }
    else {
        for (it = mScreenLockValues.begin();it != mScreenLockValues.end();++it) {
            selectedIndex++;
            if (it.key() == period) {
                break;
            }
        }
    }
    
    QStringList items = mScreenLockValues.values();
    mScreenComboButton->setContentWidgetData( QString("items"), items );
    mScreenComboButton->setContentWidgetData( QString("currentIndex"), selectedIndex);
    mScreenComboButton->setContentWidgetData("objectName", "screenComboButton");
}

/*
 * Create a rotate setting in setting form
 */
void CpKeyScreenView::makeRotateItem(HbDataFormModel& model)
{
    mRotateCheckbox = new CpSettingFormItemData(HbDataFormModelItem::CheckBoxItem, QString());
    HbDataForm * form = qobject_cast<HbDataForm*> ( widget() );
    if (form) {
        form->addConnection(mRotateCheckbox,SIGNAL(stateChanged(int)),this,SLOT(rotateValueChanged(int)));
    }
    model.appendDataFormItem(mRotateCheckbox, model.invisibleRootItem());

    mRotateCheckbox->setContentWidgetData( QString("text"), QVariant(hbTrId("txt_cp_list_autorotate_display")) );
    mRotateCheckbox->setData(HbDataFormModelItem::DescriptionRole, QString( hbTrId("txt_cp_info_rotate_the_display_content_automatical") ));
    Qt::CheckState state;
    if ( mModel->rotate() ){
        state = Qt::Checked;
    } else {
        state = Qt::Unchecked;
    }
    mRotateCheckbox->setContentWidgetData( QString("checkState"), state );
    mRotateCheckbox->setContentWidgetData("objectName", "rotateCheckbox");
}

/*
 * Create a screen saver setting in setting form
 */
void CpKeyScreenView::makeScreensaverItem(HbDataFormModel& model)
{
    mScreensaverCheckbox = new CpSettingFormItemData(HbDataFormModelItem::CheckBoxItem, QString());
    HbDataForm * form = qobject_cast<HbDataForm*> ( widget() );
    if (form) {
        form->addConnection(mScreensaverCheckbox,SIGNAL(stateChanged(int)),this,SLOT(screensaverValueChanged(int)));
    }
    model.appendDataFormItem(mScreensaverCheckbox, model.invisibleRootItem());

    mScreensaverCheckbox->setContentWidgetData( QString("text"), QVariant(hbTrId("txt_cp_list_screensaver")) );

    Qt::CheckState state;
    if ( mModel->screensaver() ){
        state = Qt::Checked;
    } else {
        state = Qt::Unchecked;
    }
    mScreensaverCheckbox->setContentWidgetData( QString("checkState"), state );
}

/*
 * Create a brightness setting in setting form
 */
void CpKeyScreenView::makeBrightnessItem(HbDataFormModel& model)
{
    mBrightSliderItem = new CpSettingFormItemData(HbDataFormModelItem::SliderItem, hbTrId("txt_cp_setlabel_brightness"));
    HbDataForm * form = qobject_cast<HbDataForm*> ( widget() );
    if (form) {
        form->addConnection(mBrightSliderItem,SIGNAL(valueChanged(int)),this,SLOT(brightValueChanged(int)));
    }
    model.appendDataFormItem(mBrightSliderItem, model.invisibleRootItem());

    QList<QVariant> sliderElements;
    sliderElements << QVariant(HbSlider::IncreaseElement) << QVariant(HbSlider::TrackElement)
                   << QVariant(HbSlider::DecreaseElement);
    mBrightSliderItem->setContentWidgetData("sliderElements",sliderElements);
    mBrightSliderItem->setContentWidgetData( QString("value"), mModel->brightness() );
    mBrightSliderItem->setContentWidgetData( QString("minimum"), 1 );
    mBrightSliderItem->setContentWidgetData( QString("maximum"), 5 );
    QMap< QString, QVariant > iconElements;
    iconElements.insert(QString("IncreaseElement") , QVariant("qtg_mono_vol_up"));
    iconElements.insert(QString("DecreaseElement"), QVariant("qtg_mono_vol_down") );
    mBrightSliderItem->setContentWidgetData( QString( "elementIcons" ), iconElements );
    mRotateCheckbox->setContentWidgetData("objectName", "brightSliderItem");
}

/*
 * Desctructor
 */
CpKeyScreenView::~CpKeyScreenView()
{
    delete mModel;
	mModel = 0;
}

/*
 * Private slots, invoke when screen key guard setting is changed
 */
void CpKeyScreenView::screenValueChanged(const QString &value)
{    
    for (QMap<int,QString>::iterator it = mScreenLockValues.begin();
         it != mScreenLockValues.end();++it) {
        if (it.value() == value) {
            mModel->setKeyguard(it.key());
            break;
        }
    }
}

/*
 * Private slots, invoke when rotate setting is changed
 */
void CpKeyScreenView::rotateValueChanged(int value)
{
    mModel->setRotate( value );
}

/*
 * Private slots, invoke when screen saver setting is changed
 */
void CpKeyScreenView::screensaverValueChanged(int value)
{
    mModel->setScreensaver( value );
}

/*
 * Private slots. invoke when bright setting is changed.
 */
void CpKeyScreenView::brightValueChanged(int value)
{
	mModel->setBrightness(value);
}



