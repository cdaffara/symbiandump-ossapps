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

#ifndef HSPAGEVISUAL_H
#define HSPAGEVISUAL_H


#include <HbWidget>
#include "hsdomainmodel_global.h"
#include "hstest_global.h"


class HsPageTouchArea;

HOMESCREEN_TEST_CLASS(TestHsDomainModel)

class HSDOMAINMODEL_EXPORT HsPageVisual : public HbWidget
{
    Q_OBJECT
    Q_PROPERTY(int pageIndex READ pageIndex)

public:
    HsPageVisual(QGraphicsItem *parent = 0);
    ~HsPageVisual();
   
    int pageIndex() const;

    void setGeometry(const QRectF &rect);

private:
    Q_DISABLE_COPY(HsPageVisual)
    void setupTouchArea();

private:
    HsPageTouchArea *mTouchArea;
    HOMESCREEN_TEST_FRIEND_CLASS(TestHsDomainModel)
};

#endif
