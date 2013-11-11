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
* Description:  hbgroupbox stub
*
*/

#ifndef HBGROUPBOX_H
#define HBGROUPBOX_H

#include <hbwidget.h>

class HbGroupBox : public HbWidget
{
    Q_OBJECT

public:
    HbGroupBox(QGraphicsItem *parent = 0) : HbWidget(parent), mText(QString()) {};
    HbGroupBox( const QString &titleText, QGraphicsItem *parent = 0 ) : HbWidget(parent) {
        mText = titleText;
    }
    ~HbGroupBox() {}

    void setHeading(const QString &text) {
        mText = text;
    }
    
    QString heading() const {
        return mText;
    }
	
    QString mText;
};

#endif  //HbGroupBox_H

