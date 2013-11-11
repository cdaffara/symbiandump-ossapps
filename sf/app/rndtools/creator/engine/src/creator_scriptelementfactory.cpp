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
* Description: 
*
*/



#include "creator_traces.h"
#include "creator_scriptelementfactory.h"
#include "creator_contactelement.h"
#include "creator_calendarelement.h"
#include "creator_messageelement.h"
#include "creator_fileelement.h"
//#include "creator_browserelement.h"
#include "creator_logelement.h"
//#include "creator_noteelement.h"
//#include "creator_impselement.h"
#include "creator_landmarkelement.h"
#include "creator_mailboxelement.h"
//#include "creator_connectionmethodelement.h"

CCreatorScriptElement* TCreatorScriptElementFactory::CreateElementL(CCreatorEngine* aEngine, const TDesC& aElementName, const TDesC& aContext )
    {   
    LOGSTRING2("TCreatorScriptElementFactory::CreateElementL: %S", &aElementName);
    
    if( aContext.Length() >= KFields().Length() && 
    	aContext.Right(KFields().Length()) == KFields)
    	{
    	// This is a field element under 'fields'.
    	return CCreatorScriptElement::NewL(aEngine, aElementName, aContext);
    	}    
    else if( aElementName == creatorcontact::KContact )
        {
        return CCreatorContactElement::NewL(aEngine, aElementName, aContext);  
        }
    else if( aElementName == creatorcontact::KContactSet)
        {
        return CCreatorContactSetElement::NewL(aEngine, aElementName, aContext);
        }
    else if( aElementName == creatorcontact::KContactGroup )
        {
        return CCreatorContactGroupElement::NewL(aEngine, aElementName, aContext);
        }
    else if( aElementName == KFields)
        {
        return CCreatorScriptElement::NewL(aEngine, aElementName, aContext);
        }
    else if( aElementName == KScript )
        {
        return CCreatorScriptRoot::NewL(aEngine, aElementName, aContext);
        }
    else if( aElementName == creatorcalendar::KCalendar )
        {
        return CCreatorCalendarElement::NewL(aEngine, aElementName, aContext);
        }
    else if( aElementName == creatormsg::KMessage )
        {
        return CCreatorMessageElement::NewL(aEngine, aElementName, aContext);
        }
    else if( aElementName == creatorfile::KFile )
        {
        return CCreatorFileElement::NewL(aEngine, aElementName, aContext);
        }
    /*
    else if( aElementName == creatorbrowser::KBookmark ||
            aElementName == creatorbrowser::KBookmarkFolder ||
            aElementName == creatorbrowser::KSavedPage ||
            aElementName == creatorbrowser::KSavedPageFolder )
        {
        return CCreatorBrowserElement::NewL(aEngine, aElementName, aContext);
        }
    */
    else if( aElementName == creatorlog::KLog )
        {
        return CCreatorLogElement::NewL(aEngine, aElementName, aContext);
        }
/*
    else if( aElementName == creatornote::KNote )
        {
        return CCreatorNoteElement::NewL(aEngine, aElementName, aContext);
        }
*/
/*
	#ifdef __PRESENCE
    else if( aElementName == creatorimps::KImpsServer )
        {
        return CCreatorImpsServerElement::NewL(aEngine, aElementName, aContext);
        }
	#endif // __PRESENCE
*/
    else if( aElementName == creatorlandmark::KLandmark )
        {
        return CCreatorLandmarkElement::NewL(aEngine, aElementName, aContext);
        }
    else if( aElementName == creatormailbox::Kmailbox )
        {
        return CCreatorMailboxElement::NewL(aEngine, aElementName, aContext);
        }
/*    
    else if( aElementName == creatorconnectionmethod::KCm )
    	{
    	return CCreatorConnectionMethodElement::NewL(aEngine, aElementName, aContext);
    	}
*/    	
    else
        {
        return CCreatorScriptElement::NewL(aEngine, aElementName, aContext);
        }       
    }
