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
#ifndef CNTABSTRACTVIEW_H_
#define CNTABSTRACTVIEW_H_

#include <cntabstractviewmanager.h>
#include <cntviewparams.h>
#include <cntabstractengine.h>

class HbView;

class CntAbstractView
{
      
public:
    virtual ~CntAbstractView(){}
    
public:
    /**
     * View is activated after its added to main window and is
     * ready to be drawn.
     */
    virtual void activate( const CntViewParameters aArgs ) = 0;
    
    /**
     * View is deactivated just before its removed from
     * main window.
     */
    virtual void deactivate() = 0;
    
    /**
     * Default view is not deleted when its removed from
     * main window. So, if view is NOT default, its deleted
     * after its removed from main window.
     */
    virtual bool isDefault() const = 0;
    
    /**
     * return the view implementation.
     */
    virtual HbView* view() const = 0;
    
    /*!
     * return the view id
     */
    virtual int viewId() const = 0;
	
	/*!
	 * set engine. This must be called right after C'tor.
	 */
	virtual void setEngine( CntAbstractEngine& aEngine ) = 0;
};
#endif /* CNTABSTRACTVIEW_H_ */
