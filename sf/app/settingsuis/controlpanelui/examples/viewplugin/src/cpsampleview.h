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
#ifndef	CPSAMPLEVIEW_H
#define	CPSAMPLEVIEW_H

#include <cpbasesettingview.h>

class HbDataFormModelItem;
class CpSettingFormItemData;

class CpSampleView : public CpBaseSettingView
{
    Q_OBJECT
public:
    explicit CpSampleView(QGraphicsItem *parent = 0);
    virtual ~CpSampleView();
private slots:
    //need handling your member's value change
    void sliderValueChanged(int value);
    void checkBoxStateChanged(int state);
private:
    HbDataFormModelItem   *mGroupItem;
    CpSettingFormItemData *mSliderItem;
    CpSettingFormItemData *mCheckBoxItem;
};
#endif//	CPSAMPLEVIEW_H
