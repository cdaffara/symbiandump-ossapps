/*
 * calenservices.h
 *
 *  Created on: Jun 21, 2010
 *      Author: zabinpio
 */

#ifndef CALENSERVICES_H_
#define CALENSERVICES_H_

#include "calencontext.h"
#include "calennotificationhandler.h"
#include "hb_calencommands.hrh"

/*!
 Mocked class MCalenServices
 */
class AgendaUtil;

class MCalenServices
{
public:
    MCalenServices() {
        
    }
    
    MCalenContext& Context(){
			return mContext;
    	}
    
    int getFirstView(){ return 0;}
    bool IssueCommandL( quint32 aCommand  ){ mLastCommand  = aCommand; return true;}
    AgendaUtil* agendaInterface() {return 0;}
    
    void RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                    TCalenNotification aNotification ) {}
    
    virtual void RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                            RArray<TCalenNotification>& aNotifications )
    {}
    
    void IssueNotificationL( quint32 aNotification ) { mLastCommand  = aNotification; }
    
    QString* InfobarTextL() {return 0;}
    
    quint32 lastCommand() {return mLastCommand;}
    
    quint32 mLastCommand;
    MCalenContext mContext;
    
    ~MCalenServices() {
        
    }
};
#endif /* CALENSERVICES_H_ */
