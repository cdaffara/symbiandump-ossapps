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


 
#ifndef __CALENLUNARVIETNAMESEPLUGIN_H__
#define __CALENLUNARVIETNAMESEPLUGIN_H__

//SYSTEM INCLUDES
#include <e32base.h>
#include <ecom.h>
#include <ConeResLoader.h> 

#include <QtGui>

#include <hblabel.h>

//CALENDAR INCLUDES
#include <calencommandhandler.h>
#include <calennotificationhandler.h>
#include <calenservices.h>
#include <eiklabel.h>
#include <COECNTRL.H>
#include <calencustomisation.h>

#include "CalendarVariant.hrh"

#include "calendarui_debug.h"

//FORWARD DECLARE
class QString;

class HbWidget;
class HbMenu;

class CEikonEnv;
class CCalenLunarInfoProvider;
class CCalenLunarLocalizer;
class CCalenLunarLocalizedInfo;

//CLASS DECLARATION
class CCalenLunarVietnamesePlugin :public CCalenCustomisation,
								public MCalenCommandHandler,
								public MCalenNotificationHandler
												
									
	{
	
	
	public:
	    
	    
		static CCalenLunarVietnamesePlugin* NewL( MCalenServices* aServices );
		virtual ~CCalenLunarVietnamesePlugin();
    
	private:
		CCalenLunarVietnamesePlugin( MCalenServices* aServices);
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
        void HandleNotification(const TCalenNotification aNotification );
        
    private:
        void SetLabelContentL( HbLabel& aLabel );        
        void FormatExtraRowStringL( HbLabel& aLabel,TBool aTwoLines);
        void SetLunarLocalizerL();
        void UpdateLocalizerInfoL();
        void ExecuteMessageDialogL( TDesC& aMsgText );
       
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
		
		/**
		* This control is used in view.
		*/
	    HbLabel* iLabelControl;
	    TInt iStart;
	    TInt iEnd;
	    TRect iRect;
	    TInt iResourceFileOffset;
		
	};
	
class CalenPluginLabel : public HbLabel
    {
    Q_OBJECT
    
	public:
		CalenPluginLabel( CCalenLunarVietnamesePlugin& aPlugin,QGraphicsItem *parent=0);
		~CalenPluginLabel();     
		
	public slots:	
	void showLunarData();
	
	private:
		void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 ); 
		void mousePressEvent(QGraphicsSceneMouseEvent *event);
	private:
		CCalenLunarVietnamesePlugin& iPlugin;	
    };	

#endif //__CALENLUNARVIETNAMESEPLUGIN_H__



 									
