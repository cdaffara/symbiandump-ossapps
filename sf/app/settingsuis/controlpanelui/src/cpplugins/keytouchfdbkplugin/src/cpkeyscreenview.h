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
#ifndef	CPKEYSCREENVIEW_H
#define	CPKEYSCREENVIEW_H

#include <QMap>
#include <cpbasesettingview.h>
#include <hbdataformmodelitem.h>

class HbDataFormModelItem;
class HbDataFormModel;
class CpSettingFormItemData;
class CpKeyScreenModel;
class QPersistentModelIndex;
class QVariant;

class CpKeyScreenView : public CpBaseSettingView
{
    Q_OBJECT
public:
    explicit CpKeyScreenView(QGraphicsItem *parent = 0);
    virtual ~CpKeyScreenView();

private:
    void makeScreenItem(HbDataFormModel& model);
    void makeRotateItem(HbDataFormModel& model);
    void makeBrightnessItem(HbDataFormModel& model);
    void makeScreensaverItem(HbDataFormModel& model);
    
private slots:
    void screenValueChanged(const QString&);
    void rotateValueChanged(int value);
    void screensaverValueChanged(int value);
    void brightValueChanged(int value);

private:
    CpSettingFormItemData *mScreenComboButton;
    CpSettingFormItemData *mRotateCheckbox;
    CpSettingFormItemData *mScreensaverCheckbox;
	CpSettingFormItemData *mBrightSliderItem;
	CpKeyScreenModel* mModel;
	
	QMap<int,QString> mScreenLockValues;
};
#endif//	CPKEYSCREENVIEW_H
