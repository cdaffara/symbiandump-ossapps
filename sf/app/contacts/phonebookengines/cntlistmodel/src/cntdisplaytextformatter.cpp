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

#include "cntdisplaytextformatter.h"
#include <qcontactdetailfilter.h>
#include <qcontactdisplaylabel.h>
#include <hbcolorscheme.h>
#include <cntdebug.h>
#include <QStringList>

CntHTMLDisplayTextFormatter::CntHTMLDisplayTextFormatter()
{
}

CntHTMLDisplayTextFormatter::~CntHTMLDisplayTextFormatter()
{
}
   
QString CntHTMLDisplayTextFormatter::formattedText( const QString aText, const QContactFilter& aCriteria )
{
    CNT_LOG_ARGS( "filter:" << aText )
    if ( aCriteria.type() == QContactFilter::ContactDetailFilter ) 
    {
        const QContactDetailFilter& filter = static_cast<const QContactDetailFilter&>( aCriteria );
        if ( filter.detailDefinitionName() == QContactDisplayLabel::DefinitionName && 
             filter.matchFlags() & QContactFilter::MatchStartsWith )
        {
            QString formattedText;
            // go through the words (e.g. Lastname, Firstname) and apply list of pattern to them.
            foreach ( QString text, aText.split(QRegExp("\\s+"), QString::SkipEmptyParts) )
            {
                bool match( false );
                // go through every search criteria word
                foreach (QString pattern, filter.value().toStringList() )
                {
                    if ( text.startsWith(pattern, Qt::CaseInsensitive) )
                    {
                        insertTag( text, pattern.length() );
                        formattedText.append( text );
                        match = true;
                        break; // break this inner foreach
                    }
                }
                
                // if no match found, original text is returned
                if ( !match )
                    formattedText.append( text );
                
                // put spaces back between words (split() looses them)
                formattedText.append( " " );
            }
            return formattedText.trimmed();
        }
    }
    return aText;
}

void CntHTMLDisplayTextFormatter::insertTag( QString& aText, int aChars )
{
    QColor highlight = HbColorScheme::color("qtc_lineedit_marker_normal");
    QColor color = HbColorScheme::color("qtc_lineedit_selected");
    
    QString start = QString(TAG_START).arg( highlight.name().toUpper() ).arg(color.name().toUpper());
    aText.prepend( start );
    aText.insert( start.length() + aChars, TAG_END );
}
// End of File
