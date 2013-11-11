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

#include "ut_cntdisplaytextformatter.h"
#include "cntdisplaytextformatter.h"
#include <hbcolorscheme.h>

void TestCntDisplayTextFormatter::testFormatter()
{
    QColor color = HbColorScheme::color("qtc_lineedit_selected");
    QColor bg = HbColorScheme::color("qtc_lineedit_marker_normal");
    QString foo = QString("<span style=\"background-color:%1;color:%2\">f</span>oo").arg(bg.name().toUpper()).arg(color.name().toUpper());
    
    QContactDetailFilter filter;
    filter.setDetailDefinitionName( QContactDisplayLabel::DefinitionName );
    filter.setMatchFlags( QContactFilter::MatchStartsWith );
    filter.setValue( "f" );
    
    CntDisplayTextFormatter* format = new CntHTMLDisplayTextFormatter;
    QString result = format->formattedText("foo", filter );
    QVERIFY( foo == result );
    QVERIFY( "" == format->formattedText("", filter) );
    // invalid filter
    QContactRelationshipFilter invalidFilter;
    QVERIFY( "foo" == format->formattedText("foo", invalidFilter) );
    QVERIFY( "" == format->formattedText("", invalidFilter) );
    
    // dummy returns always the given text, dispite of the filter
    CntDisplayTextFormatter* dummy = new CntDummyDisplayTextFormatter;
    QVERIFY( "foo" == dummy->formattedText("foo", filter) );
    QVERIFY( "foo" == dummy->formattedText("foo", invalidFilter) );
    QVERIFY( "" == dummy->formattedText("", filter) );
}
