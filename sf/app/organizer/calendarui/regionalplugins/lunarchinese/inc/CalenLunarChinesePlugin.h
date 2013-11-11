/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Calendar Lunar Plugin (Test code)
 *
*/


 
#ifndef __CALENLUNARCHINESEPLUGIN_H__
#define __CALENLUNARCHINESEPLUGIN_H__

//SYSTEM INCLUDES
#include <ecom.h>

//CALENDAR INCLUDES
#include <calencommandhandler.h>
#include <calennotificationhandler.h>
#include <calenservices.h>
#include <calencustomisation.h>

#include "CalendarVariant.hrh"


//FORWARD DECLARE
class QString;
class HbTranslator;
class HbWidget;
class HbMenu;

class CCalenLunarInfoProvider;
class CCalenLunarLocalizer;
class CCalenLunarLocalizedInfo;
class CalenRegionalInfo;
class HbMessageBox;




//CLASS DECLARATION
class CCalenLunarChinesePlugin :public CCalenCustomisation,
								public MCalenCommandHandler,
								public MCalenNotificationHandler
												
									
	{
	
	public:
	    
	    static CCalenLunarChinesePlugin* NewL( MCalenServices* aServices );
	    virtual ~CCalenLunarChinesePlugin();
    
	private:
		CCalenLunarChinesePlugin( MCalenServices* aServices);
		void ConstructL();
		
	public: //From CCalenCustomisation		
		HbWidget* InfobarL( );
        QString* InfobarTextL();        
        MCalenCommandHandler* CommandHandlerL( TInt aCommand );
        void CustomiseMenu(HbMenu* aHbMenu);
        
    public:// From MCalenCommandHandler
        TBool HandleCommandL( const TCalenCommand& aCommand );
        TAny* CalenCommandHandlerExtensionL( TUid aExtensionUid );
    
    public://From MCalenNotificationHandler 
        void HandleNotification( const TCalenNotification aNotification );
        
    private:
        void FormatExtraRowStringL();
        void UpdateLocalizerInfoL();
       void ExecuteMessageDialogL( QString aMsgText );
       
    public:
        void ShowDetailsL();
        
    private:
        
		/**
		* Language independent provider of lunar calendar information
		*/
		CCalenLunarInfoProvider* iInfoProvider;

		/**
		* Language specific localizer of lunar calendar information
		*/
		CCalenLunarLocalizer* iLocalizer;

        /**
		* Localized lunar info for currently focused day  
		*/
		CCalenLunarLocalizedInfo* iLocInfo;
		
		/**
		* Currently displayed text for extra row
		*/ 
		TPtrC iExtraRowText;
		MCalenServices* iServices;
		/**
		* This text object is used for hitchcock
		* infobar in Month/Day/Week.
		*/
		HBufC* iInfoBarText;
	    TInt iStart;
	    TInt iEnd;
	    HbTranslator* iTranslator;
	    
	    CalenRegionalInfo* iRegionalInfo ;
	    
	    HbMessageBox* mPopup;
	};
	
#endif //__CALENLUNARCHINESEPLUGIN_H__
