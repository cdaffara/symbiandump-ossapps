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

#include <QGraphicsSceneMouseEvent>
#include "cxuizoomslider.h"

/*!
    \class CxuiZoomSlider
    \brief CxuiZoomSlider is a camera specific slider that is derived from HbSlider

    CxuiZoomslider has been created so that we are able to control and filter user
    mouse press events. All mouse press events that happen in slider rect are
    accepted in this class so that they won't be given any other components.
    This has to be done to prevent the slider to dismiss when accidentally 
    missing e.g.slider thumb touch area (camera view underneath will get the 
    event and hide the slider). 
    
*/

CxuiZoomSlider::CxuiZoomSlider(QGraphicsItem *parentItem)
: HbSlider(parentItem)
{

}


CxuiZoomSlider::~CxuiZoomSlider()
{

}


/*
* CxuiZoomSlider::mousePressEvent()
*/
void CxuiZoomSlider::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    HbSlider::mousePressEvent(event);
    
    // accept event so that it will not be given to the components
    // underneath this slider
    event->accept();
}

/*!
* Adding zoom buttons to the slider
*/
void CxuiZoomSlider::addZoomButtons()
{
    // get current slider elements
    QList<QVariant> elements = sliderElements();

    // add increase and decrease elements to the slider
    elements << HbSlider::IncreaseElement << HbSlider::DecreaseElement;
    setSliderElements(elements);

    // set icons for the increase and decrease element
    setElementIcon(HbSlider::DecreaseElement , HbIcon("qtg_mono_minus"));
    setElementIcon(HbSlider::IncreaseElement , HbIcon("qtg_mono_plus"));
}

//End of file
