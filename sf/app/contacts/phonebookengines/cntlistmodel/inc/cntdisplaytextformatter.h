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

#ifndef CNTDISPLAYTEXTFORMATTER_H_
#define CNTDISPLAYTEXTFORMATTER_H_

#include <QObject>
#include <qcontactfilter.h>
QTM_USE_NAMESPACE

#define TAG_START "<span style=\"background-color:%1;color:%2\">"
#define TAG_END   "</span>"
    
class CntDisplayTextFormatter
{
public:
    virtual ~CntDisplayTextFormatter(){}
    virtual QString formattedText( const QString aText, const QContactFilter& aCriteria ) = 0;
};

class CntDummyDisplayTextFormatter : public QObject, public CntDisplayTextFormatter
{
    Q_OBJECT
public:
    CntDummyDisplayTextFormatter(){}
    ~CntDummyDisplayTextFormatter(){}
    
    inline QString formattedText( const QString aText, const QContactFilter& aCriteria )
    {
        Q_UNUSED( aCriteria );
        return aText;
    }
};

class CntHTMLDisplayTextFormatter : public QObject, public CntDisplayTextFormatter
{
    Q_OBJECT

public:
    CntHTMLDisplayTextFormatter();
    virtual ~CntHTMLDisplayTextFormatter();
    
    /*!
     * Format given text with applied filter. Not that only following filter is supported:
     * 
     * QContactDetailFilter filter;
     * filter.setDetailDefinitionName( QContactDisplayLabel::DefinitionName );
     * filter.setMatchFlags( QContactDetailFilter::MatchStartsWith );
     * 
     * \param aText Buffer where to format
     * \param aCriteria Applied filter
     */
    QString formattedText( const QString aText, const QContactFilter& aCriteria );
    
    /*!
     * Insert tag to given text leaving given number
     * of characters between start and end tag. 
     * By default highlight is inserted.
     * 
     * \param aText Buffer where to insert tags
     * \param aNumOfCharacters Number of characters to highlight
     */
    virtual void insertTag( QString& aText, int aNumOfCharacters );
};
#endif /* CNTDISPLAYTEXTFORMATTER_H_ */
