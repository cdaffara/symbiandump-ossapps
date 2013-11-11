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
#ifndef CPSAMPLEGROUP_H
#define CPSAMPLEGROUP_H

#include <cpsettingformitemdata.h>

class CpSettingFormItemData;
class CpItemDataHelper;

class CpSampleGroup : public CpSettingFormItemData
{
    Q_OBJECT
public:
    explicit CpSampleGroup(CpItemDataHelper &itemDataHelper);
    ~CpSampleGroup();

private slots:
    //need handling your member's value change
    void sliderValueChanged(int value);
    void checkBoxStateChanged(int state);

private:
    CpSettingFormItemData *mSliderItem;
    CpSettingFormItemData *mCheckBoxItem;
};

#endif // CPSAMPLEGROUP_H
