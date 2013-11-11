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
#include <QDebug>
#include <smcmockclassincludes.h>
#include <hbmainwindow.h>
#include <QString>
#include <QGraphicsWidget>
#include <QFile>

#include "hbdocumentloader.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HbDocumentLoader::HbDocumentLoader
// -----------------------------------------------------------------------------
//
HbDocumentLoader::HbDocumentLoader(  )
    : d_ptr(0)
    {
    
    }


// -----------------------------------------------------------------------------
// HbDocumentLoader::HbDocumentLoader
// -----------------------------------------------------------------------------
//
HbDocumentLoader::HbDocumentLoader( 
        const HbMainWindow * window )
    : d_ptr(0)    
    {
    
    }


// -----------------------------------------------------------------------------
// HbDocumentLoader::~HbDocumentLoader
// -----------------------------------------------------------------------------
//
HbDocumentLoader::~HbDocumentLoader(  )
    {
    
    }


// -----------------------------------------------------------------------------
// HbDocumentLoader::load
// -----------------------------------------------------------------------------
//
QObjectList HbDocumentLoader::load( 
        const QString & fileName,
        const QString & section,
        bool * ok )
    {
    SMC_MOCK_METHOD3( QObjectList, const QString &, fileName, 
        const QString &, section, 
        bool *, ok )
    }


// -----------------------------------------------------------------------------
// HbDocumentLoader::load
// -----------------------------------------------------------------------------
//
QObjectList HbDocumentLoader::load( 
        QIODevice * device,
        const QString & section,
        bool * ok )
    {
    SMC_MOCK_METHOD3( QObjectList, QIODevice *, device, 
        const QString &, section, 
        bool *, ok )
    }

// -----------------------------------------------------------------------------
// HbDocumentLoader::load
// -----------------------------------------------------------------------------
//
QObjectList HbDocumentLoader::load( 
        const QString & fileName,
        bool * ok )
    {
    SMC_MOCK_METHOD2( QObjectList, const QString &, fileName, 
        bool *, ok )
    }

// -----------------------------------------------------------------------------
// HbDocumentLoader::load
// -----------------------------------------------------------------------------
//
QObjectList HbDocumentLoader::load( 
        QIODevice * device,
        bool * ok )
    {
    SMC_MOCK_METHOD2( QObjectList, QIODevice *, device, 
        bool *, ok )
    }


// -----------------------------------------------------------------------------
// HbDocumentLoader::findWidget
// -----------------------------------------------------------------------------
//
QGraphicsWidget * HbDocumentLoader::findWidget( 
        const QString & name ) const
    {
    SMC_MOCK_METHOD1( QGraphicsWidget *, const QString &, name )
    }


// -----------------------------------------------------------------------------
// HbDocumentLoader::findObject
// -----------------------------------------------------------------------------
//
QObject * HbDocumentLoader::findObject( 
        const QString & name ) const
    {
    SMC_MOCK_METHOD1( QObject *, const QString &, name )
    }


// -----------------------------------------------------------------------------
// HbDocumentLoader::setObjectTree
// -----------------------------------------------------------------------------
//
bool HbDocumentLoader::setObjectTree( 
        QObjectList roots )
    {
    SMC_MOCK_METHOD1( bool, QObjectList, roots )
    }


// -----------------------------------------------------------------------------
// HbDocumentLoader::reset
// -----------------------------------------------------------------------------
//
void HbDocumentLoader::reset(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbDocumentLoader::version
// -----------------------------------------------------------------------------
//
QString HbDocumentLoader::version(  )
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// HbDocumentLoader::createObject
// -----------------------------------------------------------------------------
//
QObject * HbDocumentLoader::createObject( 
        const QString & type,
        const QString & name )
    {
    SMC_MOCK_METHOD2( QObject *, const QString &, type, 
        const QString &, name )
    }


