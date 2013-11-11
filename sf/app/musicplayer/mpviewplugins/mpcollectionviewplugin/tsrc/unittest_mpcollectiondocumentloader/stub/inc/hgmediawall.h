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
* Description: HgMediawall stub for testing MpCollectionDocumentLoader
*
*/

#ifndef HGMEDIAWALL_H
#define HGMEDIAWALL_H

//includes
#include <QGraphicsItem>
#include <QObject>

class HgMediawall : public QObject
{
    Q_OBJECT

public:

    explicit HgMediawall( QGraphicsItem *parent = 0 );
    virtual ~HgMediawall();

};

#endif    // HGMEDIAWALL_H
