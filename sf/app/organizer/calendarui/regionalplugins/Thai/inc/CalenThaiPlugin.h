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
* Description:  
 *
*/


#ifndef __CALENTHAIPLUGIN_H__
#define __CALENTHAIPLUGIN_H__

#include <e32base.h>
#include <coneresloader.h>

#include <hblabel.h>

#include <calencommandhandler.h>
#include <calennotificationhandler.h>
#include <calenservices.h>


#include "calencustomisation.h"

class HbWidget;
class QString;
class HbMenu;

class CalenPluginLabel : public HbLabel
    {
public:
    CalenPluginLabel(QGraphicsItem *parent=0);
    ~CalenPluginLabel();     

private:
    void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );    
    };

NONSHARABLE_CLASS(CCalenThaiPlugin) : public CCalenCustomisation,
                                      public MCalenCommandHandler,
                                      public MCalenNotificationHandler   
    {
    public: // public API 
            static CCalenThaiPlugin* NewL( MCalenServices* aServices );
            virtual ~CCalenThaiPlugin();
                
    protected://From CCalenCustomisation        
        	
            HbWidget* InfobarL( );               
			MCalenCommandHandler* CommandHandlerL( TInt aCommand );
			QString* InfobarTextL();
			void CustomiseMenu(HbMenu* aHbMenu);
            
    protected:// From MCalenCommandHandler
            TBool HandleCommandL( const TCalenCommand& aCommand );
        
    protected://From MCalenNotificationHandler 
            void HandleNotification(const TCalenNotification aNotification );
        
    private:
           
            void UpdateLocalizerInfoL();
        
    private: // own methods
            CCalenThaiPlugin(MCalenServices* aServices);
            void ConstructL();
            
    private: // data
            TBuf<200> iThaiYearText;            
            MCalenServices* iServices;           
    };


#endif // __CALENTHAIPLUGIN_H__
