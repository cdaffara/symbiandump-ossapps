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
* Description: Equalizer widget stub for Music Player playback view.
*
*/

// System includes
#include <QStringList>
#include <EqualizerConstants.h>

// User includes
#include "..\inc\mpequalizerwidget.h"
#include "..\inc\mpenginefactory.h"
#include "..\inc\mpengine.h"
#include "mptrace.h"


/*!
 Constructs the Equalizer widget.
 */
MpEqualizerWidget::MpEqualizerWidget() :
    showEqualizerWidgetCount( 0 ),
    closeEqualizerWidgetCount( 0 ),
    mMpEngine(0)
{
    TX_ENTRY    
    initialize();    
    TX_EXIT
}

/*!
 Destructs the widget.
 */
MpEqualizerWidget::~MpEqualizerWidget()
{
	TX_LOG
}

/*!
 Stub
 */
void MpEqualizerWidget::initialize()
{    
    mMpEngine = MpEngineFactory::sharedEngine();  
}

/*!
 Stub
 */
void MpEqualizerWidget::prepareDialog()
{
}

/*!
 Stub
 */
void MpEqualizerWidget::presetSelected(int index)
{
    Q_UNUSED(index)
}

/*!
Stub
 */
void MpEqualizerWidget::cancelSelected(bool checked)
{
    TX_ENTRY
    Q_UNUSED(checked);
}

/*!
Stub
 */
void MpEqualizerWidget::okSelected(bool checked)
{
    Q_UNUSED(checked);
}

/*!
Stub
 */
void MpEqualizerWidget::show()
{
 showEqualizerWidgetCount++;
}

/*!
Stub
 */
void MpEqualizerWidget::close()
{
 closeEqualizerWidgetCount++;
}

/*!
  Return the localized string base on preset name.
 */
QString MpEqualizerWidget::getLocalizedString( QString name )
{
    Q_UNUSED(name)
    return QString();
}

//End of File
