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
#ifndef CXUISETTINGSLIDER_H_
#define CXUISETTINGSLIDER_H_

#include <hbslider.h>
#include "cxuisettingsinfo.h"

class CxeEngine;
class CxeSettings;

class CxuiSettingSlider : public HbSlider
{
    Q_OBJECT
    Q_PROPERTY(QString settingId READ settingId WRITE setSettingId)

public:
    explicit CxuiSettingSlider(QGraphicsItem *parent, CxeEngine *engine);

    QString settingId() const;
    void setSettingId(const QString &id);
    void init(CxUiSettings::SliderParams *data);

signals:
    void selectionCommitted();

public slots:
    void handleValueChanged(int);
    void handleClose();
    void handleSelectionAccepted();

private:
    Q_DISABLE_COPY(CxuiSettingSlider)

    QString mSettingId;
    CxeEngine *mEngine;
    CxeSettings *mSettings;
    qreal mOriginalValue;
    qreal mSliderScaleValue;
};

#endif /* CXUISETTINGSLIDER_H_ */
