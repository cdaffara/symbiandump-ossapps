/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Screensaver label.
*
*/

#ifndef SNSRLABEL_H
#define SNSRLABEL_H

#include <hblabel.h>

#include "snsrutils_global.h"
#include "snsrtest_global.h"

SCREENSAVER_TEST_CLASS(T_SnsrUtils)

class SNSRUTILS_EXPORT SnsrLabel : public HbLabel
{

    Q_OBJECT
    
public:
    
    enum TextColorType
    {
        ThemedColorForActiveMode,
        FixedColorForPowerSaveMode // = Qt::white
    };
    
public:
    
    SnsrLabel(QGraphicsItem *parent = 0);
    SnsrLabel(const QString &displayText, QGraphicsItem *parent = 0);
    ~SnsrLabel();
    
public:
    
    void setTextColorType(const TextColorType &colorType);
    
protected:

    virtual void changeEvent(QEvent *event);

private:

    void setThemedTextColor();
 
private:
    
    TextColorType mTextColorType;

    SCREENSAVER_TEST_FRIEND_CLASS(T_SnsrUtils)

};

#endif // SNSRLABEL_H
