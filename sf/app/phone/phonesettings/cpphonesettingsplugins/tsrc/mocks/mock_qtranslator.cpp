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
#include <qcoreapplication.h>
#include <smcmockclassincludes.h>
#include "qtranslator.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// QTranslator::QTranslator
// -----------------------------------------------------------------------------
//
QTranslator::QTranslator( 
        QObject * parent )
    :
    QObject( parent )
    {
    
    }


// -----------------------------------------------------------------------------
// QTranslator::~QTranslator
// -----------------------------------------------------------------------------
//
QTranslator::~QTranslator(  )
    {
    if (QCoreApplication::instance()) 
        {
        QCoreApplication::removeTranslator(this);
        }
    }


// -----------------------------------------------------------------------------
// QTranslator::translate
// -----------------------------------------------------------------------------
//
QString QTranslator::translate( 
        const char * aContext,
        const char * sourceText,
        const char * disambiguation ) const
    {
    SMC_MOCK_METHOD3( QString, const char *, aContext, 
        const char *, sourceText, 
        const char *, disambiguation )
    }


// -----------------------------------------------------------------------------
// QTranslator::translate
// -----------------------------------------------------------------------------
//
QString QTranslator::translate( 
        const char * aContext,
        const char * sourceText,
        const char * disambiguation,
        int n ) const
    {
    SMC_MOCK_METHOD4( QString, const char *, aContext, 
        const char *, sourceText, 
        const char *, disambiguation, 
        int, n )
    }


// -----------------------------------------------------------------------------
// QTranslator::isEmpty
// -----------------------------------------------------------------------------
//
bool QTranslator::isEmpty(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// QTranslator::load
// -----------------------------------------------------------------------------
//
bool QTranslator::load( 
        const QString & filename,
        const QString & directory,
        const QString & search_delimiters,
        const QString & suffix )
    {
    SMC_MOCK_METHOD4( bool, const QString &, filename, 
        const QString &, directory, 
        const QString &, search_delimiters, 
        const QString &, suffix )
    }


// -----------------------------------------------------------------------------
// QTranslator::load
// -----------------------------------------------------------------------------
//
bool QTranslator::load( 
        const uchar * data,
        int len )
    {
    SMC_MOCK_METHOD2( bool, const uchar *, data, 
        int, len )
    }
