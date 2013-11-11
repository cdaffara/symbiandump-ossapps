/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Colors used in screensaver.
*
*/

#ifndef SNSRCOLORS_H
#define SNSRCOLORS_H

#include <QColor>
#include <QString>

namespace SnsrColors
{
// in active mode icons and text should take the color from the theme using these roles
static const QLatin1String WidgetColorRole("qtc_screensaver");

// in power save mode icons and texts should be always white
static const QColor PowerSaveModeWidgetColor(Qt::white);

// background should always be black
static const QColor BackgroundColor(Qt::black);
}

#endif /* SNSRCOLORS_H */
