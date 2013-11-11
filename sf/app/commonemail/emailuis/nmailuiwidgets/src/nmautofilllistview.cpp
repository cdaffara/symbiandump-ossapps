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
* Description: List view for the autofill items. 
*
*/

#include "nmailuiwidgetsheaders.h"


static const int NmHorizontalPreferredWidth(500);
static const int NmVerticalPreferredWidth(300);

/*!
    \class NmAutoFillListView
    \brief List view for the autofill items.
    
    The main function is to handle the virtual keyboard openings and closings. 
*/

/*!
    Constructor of NmAutoFillListView.
    
    Registers widgetML and CSS files to HbStyle system for view items.
*/
NmAutoFillListView::NmAutoFillListView(QGraphicsItem *parent) : HbListView(parent)
{
}
/*!
    The popup size changes with the orientation. In horizontal orientation, the popup is wider.
    
    \param orientation 
 */
void NmAutoFillListView::orientationChanged(Qt::Orientation orientation)
{
    if (orientation == Qt::Horizontal) {
        setPreferredWidth(NmHorizontalPreferredWidth);
    }
    else {
        setPreferredWidth(NmVerticalPreferredWidth);
    }
}
