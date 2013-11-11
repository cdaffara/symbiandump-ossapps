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
#include <QObject>
#include <QLocale>
#include <QHash>
#include <hbinputkeymapfactory.h>
#include <hbinputsettingproxy.h>
#include <QStringList>
#include <QTextCodec>

#include "logspredictivetranslator.h"
#include "logspredictivelatin12keytranslator.h"
#include "logspredictivethai12keytranslator.h"
#include "logslogger.h"

LogsPredictiveTranslator* LogsPredictiveTranslator::mInstance = 0;


// -----------------------------------------------------------------------------
// LogsPredictiveTranslator::instance()
// -----------------------------------------------------------------------------
//
LogsPredictiveTranslator* LogsPredictiveTranslator::instance()
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsPredictiveTranslator::\
instance()" )
    if ( !mInstance ) {
        HbInputLanguage lang = 
                HbInputSettingProxy::instance()->globalInputLanguage();
        LOGS_QDEBUG_2( "logs [FINDER] Input lang is %d", 
                       lang.language() )
        switch( lang.language() ) {
            case QLocale::Thai:
                mInstance = new LogsPredictiveThai12KeyTranslator( lang );
                break;
            default:
                mInstance = new LogsPredictiveLatin12KeyTranslator( lang );
                break;
            }
    }
    LOGS_QDEBUG( "logs [FINDER] <- LogsPredictiveTranslator::\
instance()" )
    return mInstance;
}

// -----------------------------------------------------------------------------
// LogsPredictiveTranslator::deleteInstance()
// -----------------------------------------------------------------------------
//
void LogsPredictiveTranslator::deleteInstance()
{
    delete mInstance;
    mInstance = 0;
}


// -----------------------------------------------------------------------------
// LogsPredictiveTranslator::LogsPredictiveTranslator()
// -----------------------------------------------------------------------------
//
LogsPredictiveTranslator::LogsPredictiveTranslator( const HbInputLanguage& lang )
    : mNameTranslator(0)

{
    LOGS_QDEBUG( "logs [FINDER] -> LogsPredictiveTranslator::\
LogsPredictiveTranslator()" )

    mKeyMap = HbKeymapFactory::instance()->keymap( lang.language(), 
                                                   lang.variant() );

    LOGS_QDEBUG( "logs [FINDER] <- LogsPredictiveTranslator::\
LogsPredictiveTranslator()" )
}


// -----------------------------------------------------------------------------
// LogsPredictiveTranslator::~LogsPredictiveTranslator()
// -----------------------------------------------------------------------------
//
LogsPredictiveTranslator::~LogsPredictiveTranslator()
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsPredictiveTranslator::\
~LogsPredictiveTranslator()" )
    mInstance = 0;
    mKeyMap = 0;
    delete mNameTranslator;
    mNameTranslator = 0;
    LOGS_QDEBUG( "logs [FINDER] <- LogsPredictiveTranslator::\
~LogsPredictiveTranslator()" )
    
}

// -----------------------------------------------------------------------------
// LogsPredictiveTranslator::translateText()
// -----------------------------------------------------------------------------
//
const QString LogsPredictiveTranslator::translateText( 
                                                  const QString& text )
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsPredictiveTranslator::translateText()" )
    LOGS_QDEBUG_2( "logs [FINDER] text ", text );
            
    bool ok;
    
    QString result = translate( text, &ok );
    if ( !ok ) {
        LOGS_QDEBUG( "logs [FINDER] Text is not in input language" )
        result = nameTranslator( text ).translate( text );
    }
    
    LOGS_QDEBUG( "logs [FINDER] <- LogsPredictiveTranslator::translateText()" )
    return result;
}

// -----------------------------------------------------------------------------
// LogsPredictiveTranslator::nameTranslator()
// -----------------------------------------------------------------------------
//
const LogsPredictiveTranslator& LogsPredictiveTranslator::nameTranslator( 
                                                   const QString& name )
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsPredictiveTranslator::nameTranslator()" )
    
    const LogsPredictiveTranslator* nameTranslator = this;
    if ( mNameTranslator && mNameTranslator->encodes( name ) ) {
        LOGS_QDEBUG( "logs [FINDER] <- LogsPredictiveTranslator::\
nameTranslator() - use current" )
        nameTranslator = mNameTranslator;
    } else { 
        delete mNameTranslator;
        mNameTranslator = new LogsPredictiveLatin12KeyTranslator();
        if( mNameTranslator->mKeyMap && 
            mNameTranslator->encodes( name ) ) {
            nameTranslator = mNameTranslator;
            LOGS_QDEBUG( "logs [FINDER] <- LogsPredictiveTranslator::\
nameTranslator() - use latin" )
            return *nameTranslator;
        }
        delete mNameTranslator;
        mNameTranslator = new LogsPredictiveThai12KeyTranslator();
        if( mNameTranslator->mKeyMap && 
            mNameTranslator->encodes( name ) ) {
            nameTranslator = mNameTranslator;
            LOGS_QDEBUG( "logs [FINDER] <- LogsPredictiveTranslator::\
nameTranslator() - use thai" )
            return *nameTranslator;
        }
        delete mNameTranslator;
        mNameTranslator = 0;
        LOGS_QDEBUG( "logs [FINDER] <- LogsPredictiveTranslator::\
nameTranslator() - use global input language" )

    }
    
        
    return *nameTranslator;
    
}

// -----------------------------------------------------------------------------
// LogsPredictiveTranslator::encodes()
// -----------------------------------------------------------------------------
//
bool LogsPredictiveTranslator::encodes( const QString& sniplet )
{
    bool ok;
    translate( sniplet, &ok );
    return ok;
}

// -----------------------------------------------------------------------------
// LogsPredictiveTranslator::translatePattern()
// -----------------------------------------------------------------------------
//
const QString LogsPredictiveTranslator::translatePattern( 
                                                   const QString& pattern ) const
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsPredictiveTranslator::translatePattern()" )
    QString result = translate( pattern );
    
    if ( !result.length() ) {
        result = pattern;
    }
    LOGS_QDEBUG( "logs [FINDER] <- LogsPredictiveTranslator::translatePattern()" )
    return result;
}

// -----------------------------------------------------------------------------
// LogsPredictiveTranslator::translate()
// -----------------------------------------------------------------------------
//
const QString LogsPredictiveTranslator::translate( const QString& text,
                                                   bool* ok, int count ) const
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsPredictiveTranslator::translate()" )
    LOGS_QDEBUG_2( "logs [FINDER] text ", text );
    QString result;
    bool isok = ok ? *ok : true;
    
    count = count == -1 ? text.length() : count;
    const QChar* content = text.data();
    int index = 0;
    while( index < count && isok ) {
        QChar ch = translateChar( *content++, isok );
        if ( !ch.isNull() ) {
            result.append( ch );
        }
        index++;
    }
    
    if ( ok ) {
        *ok = isok;
    }
    LOGS_QDEBUG( "logs [FINDER] <- LogsPredictiveTranslator::translate()" )
    return result;
}


