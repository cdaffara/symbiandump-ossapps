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
#include <QRegExp>
#include <hbinputkeymapfactory.h>
#include <hbinputkeymap.h>
#include <hbinputsettingproxy.h>

#include "logspredictive12keytranslator.h"
#include "logslogger.h"

const QChar ZeroSepar('0');
const int NotAssigned = -1;

// -----------------------------------------------------------------------------
// LogsPredictive12KeyTranslator::LogsPredictive12KeyTranslator()
// -----------------------------------------------------------------------------
//
LogsPredictive12KeyTranslator::LogsPredictive12KeyTranslator( 
                                                    const HbInputLanguage& lang ) 
    : LogsPredictiveTranslator( lang )
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsPredictive12KeyTranslator::\
LogsPredictive12KeyTranslator()" )
    LOGS_QDEBUG( "logs [FINDER] <- LogsPredictive12KeyTranslator::\
LogsPredictive12KeyTranslator()" )

}

// -----------------------------------------------------------------------------
// LogsPredictive12KeyTranslator::~LogsPredictive12KeyTranslator()
// -----------------------------------------------------------------------------
//
LogsPredictive12KeyTranslator::~LogsPredictive12KeyTranslator()
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsPredictive12KeyTranslator::\
~LogsPredictive12KeyTranslator()" )
    LOGS_QDEBUG( "logs [FINDER] <- LogsPredictive12KeyTranslator::\
~LogsPredictive12KeyTranslator()" )
    
}

// -----------------------------------------------------------------------------
// LogsPredictive12KeyTranslator::patternTokens()
// -----------------------------------------------------------------------------
//
QStringList LogsPredictive12KeyTranslator::patternTokens( const QString& pattern ) const
{
        
    LOGS_QDEBUG( "logs [FINDER] -> LogsPredictive12KeyTranslator::\
patternTokens()" )
    LOGS_QDEBUG_2( "logs [FINDER] pattern ", pattern );
    QString car;
    QString cdr;
    
    QStringList target;
    splitPattern( pattern, car, cdr );
    if ( car.length() ) {
        target.append( car );
        if ( cdr.length() ) {
            target.append( cdr );
        }
    }
    LOGS_QDEBUG( "logs [FINDER] <- LogsPredictive12KeyTranslator::\
patternTokens()" )
    return target;
}

// -----------------------------------------------------------------------------
// LogsPredictive12KeyTranslator::splitPattern()
// -----------------------------------------------------------------------------
//
void LogsPredictive12KeyTranslator::splitPattern( const QString& pattern, 
                                                  QString& car, QString& cdr ) const
{
    car = "";
    cdr = "";
    
    QChar current;
    QChar previous;
    int splitStart = NotAssigned;
    int splitEnd = NotAssigned;
    int index = 0;
    while( splitEnd == NotAssigned && index < pattern.length() ) {
        current = pattern[index];
        splitStart = splitStart == NotAssigned &&
                    ( previous != ZeroSepar && previous != QChar() ) && 
                    current == ZeroSepar ? 
                        index : splitStart;
        splitEnd = splitStart != NotAssigned && 
                   previous == ZeroSepar && 
                   current != ZeroSepar ?
                      index : splitEnd;
        previous = current;
        index++;
    }
    
    if ( splitStart != NotAssigned && splitEnd != NotAssigned ) {
        car = pattern.left( splitStart );
        cdr = pattern.right( pattern.length() - splitEnd );  
    } else {
        car = pattern; 
    }
}

// -----------------------------------------------------------------------------
// LogsPredictive12KeyTranslator::trimPattern()
// -----------------------------------------------------------------------------
//
QString& LogsPredictive12KeyTranslator::trimPattern( QString& pattern, 
                                                     bool tailOnly ) const
{
    QRegExp lead("^0*");//remove leading zeros
    QRegExp trail("0*$");//remove trailing zeros
    
    if ( pattern.length() ) {
        if ( !tailOnly ) {
            pattern.remove( lead );
        }
        
        pattern.remove( trail );
        
        if( !pattern.length() ) {
            pattern += ZeroSepar;
        }
    }
    return pattern;
}


// -----------------------------------------------------------------------------
// LogsPredictive12KeyTranslator::hasPatternSeparators()
// -----------------------------------------------------------------------------
//
int LogsPredictive12KeyTranslator::hasPatternSeparators( 
        const QString& pattern ) const
{
    return pattern.count( ZeroSepar );

}

// -----------------------------------------------------------------------------
// LogsPredictive12KeyTranslator::translateChar()
// -----------------------------------------------------------------------------
//
const QChar LogsPredictive12KeyTranslator::translateChar( 
        const QChar character ) const
{
    const HbMappedKey* mappedKey = 0;
    if ( mKeyMap ) {
        mappedKey = mKeyMap->keyForCharacter( HbKeyboardVirtual12Key, character );
    }
    return mappedKey ? mappedKey->keycode : QChar();
}


