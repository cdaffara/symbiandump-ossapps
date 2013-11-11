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
* Description: Music Player collection container factory.
*
*/

#ifndef MPCOLLECTIONCONTAINERFACTORY_H
#define MPCOLLECTIONCONTAINERFACTORY_H

#include <QObject>

#include "mpmpxcollectionviewdefs.h"

class HbDocumentLoader;
class MpCollectionView;
class MpCollectionContainer;


class MpCollectionContainerFactory: public QObject
{
    Q_OBJECT

public:

    explicit MpCollectionContainerFactory( MpCollectionView *view, HbDocumentLoader *loader, QObject *parent = 0 );
    virtual ~MpCollectionContainerFactory();

    MpCollectionContainer *createContainer( TCollectionContext context);

private:

    void deleteCurrentContainer();

private:

    MpCollectionView        *mView;
    MpCollectionContainer   *mCurrentContainer;
    TCollectionContext      mCurrentContext;
    HbDocumentLoader        *mDocumentLoader;   // Not own

};

#endif  // MPCOLLECTIONCONTAINERFACTORY_H

