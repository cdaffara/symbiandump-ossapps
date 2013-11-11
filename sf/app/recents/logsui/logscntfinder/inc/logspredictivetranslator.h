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

#ifndef LOGSPREDICTIVETRANSLATOR_H
#define LOGSPREDICTIVETRANSLATOR_H

#include <QObject>
#include <QChar>
#include <QList>

class HbKeymap;
class HbInputLanguage;

/**
 * predictive translator. Singelton
 *
 */
class LogsPredictiveTranslator : public QObject 
{

public: 

    static LogsPredictiveTranslator* instance();
    static void deleteInstance();
    
    ~LogsPredictiveTranslator();
    
    const QString translatePattern( const QString& pattern ) const;
    const QString translateText( const QString& text );
    
public: //abstracts
    
    virtual QStringList nameTokens( const QString& name ) const = 0;
    virtual QStringList patternTokens( const QString& pattern ) const = 0;
    virtual QString& trimPattern( QString& pattern, bool tailOnly = false ) const = 0;
    virtual int hasPatternSeparators( const QString& pattern ) const = 0;
    virtual const QChar translateChar( const QChar character, bool& ok ) const = 0;
    virtual int mib() const = 0;
    
protected:
    
    explicit LogsPredictiveTranslator( const HbInputLanguage& lang );
    bool encodes( const QString& sniplet );
    const QString translate( const QString& text, bool* ok = 0, int count = -1 ) const;
    
private:
    
    const LogsPredictiveTranslator& nameTranslator( const QString& name );
    
protected:
    
    const HbKeymap* mKeyMap;

private:
    
    LogsPredictiveTranslator* mNameTranslator;
    static LogsPredictiveTranslator* mInstance;
    friend class UT_LogsPredictiveTranslator;
    
};

#endif //LOGSPREDICTIVETRANSLATOR_H
