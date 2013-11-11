/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This class interfaces with Context Framework Client API.
*
*/



#ifndef CALARMCONTEXTFWSUPPORT_H
#define CALARMCONTEXTFWSUPPORT_H


#include <e32base.h>
#include <cflistener.h>

#include "alarmsourcecontextdef.h"
#include "AlmAlertVariant.hrh"


class CCFClient;
class CAlmAlarmControl;
class CCFActionSubscription;
class CCFContextSubscription;

/**
* Context Framework support for Alarm UI.
*
* This class interfaces with Context Framework Client API to
* define, subscribe and register contexts, actions and scripts 
* for Alarm UI.
*
*  @lib AknAlarmService
*  @since S60 5.0
**/
NONSHARABLE_CLASS( CAlarmContextFwSupport ) : public CBase, 
                                              public MCFListener
{
public: // constructors and destructor

    /** 
    * Symbian constructor 
    */
    static CAlarmContextFwSupport* NewL(CAlmAlarmControl* aAlarmControl);

    /** 
    * c++ destructor 
    */
    virtual ~CAlarmContextFwSupport();

private: // from base class MCFListener

    /**
    * Indicates a change in context for subscribed clients.
    * If client has subscribed to partial context class path,
    * only the changed context will be indicated.
    * @since S60 5.0
    * @param aChangedContext: Context indication.
    */
    void ContextIndicationL(const CCFContextIndication& aChangedContext);

    /**
    * Indicates that specified action is needed to be executed.
    * When action indication is received certain rule has been triggered
    * and action is needed to be performed.
    * @since S60 5.0
    * @param aActionToExecute Action indication.
    */
    void ActionIndicationL(const CCFActionIndication& aActionToExecute);

    /**
    * Indicates that specified action is needed to be executed.
    * When action indication is received certain rule has been triggered
    * and action is needed to be performed.
    * @since S60 5.0
    * @param aError  Error code.
    * @param aSource Error source.
    * @param aType   Error type.
    */
    void HandleContextFrameworkError( TCFError aError, 
                                      const TDesC& aSource, 
                                      const TDesC& aType );

	// Changes made for Context Framework API changes (SCB Id: MKAN-765D93)
		
	/**
	* Returns an extension interface.
	* The extension interface is mapped with the extension UID.
	*
	* The default implemementation returns NULL.
	*
	* @since S60 5.0
	* @param aExtensionUid: The identifier of the extension.
	* @return Pointer to the extension.
	*/
	TAny* Extension( const TUid& aExtensionUid ) const;
                                          
private:
    /**
    * @brief Defines a new context
    * @since S60 5.0
    * @param aContextSource  Source of the context.
    * @param aContextType    Type of the context.
    * @param aSecurityPolicy Needed capabilities for the context.
    * @return None  
    */
    void DefineContextL( const TDesC& aContextSource,
                         const TDesC& aContextType,
                         const TSecurityPolicy& aSecurityPolicy );
    /**
    * @brief Defines an action
    * @since S60 5.0
    * @param aAction Action identifier
    * @return None
    */
    void DefineActionL(const TAlarmUIActionValues aAction);

    /**
    * @brief Subscribe an action
    * @since S60 5.0
    * @param aAction Action identifier
    * @return None
    */
    void SubscribeActionL(const TAlarmUIActionValues aAction);

    /**
    * @brief Unsusbscribe all actions.
    * @since S60 5.0
    */
    void UnsubscribeActions();

	
    /**
    * @brief Publish a context.
    * @since S60 5.0
    * @param aSource	Source of context
    * @param aType		Type of context
    * @param aValue		Value of context
    * @return None
    */
    void PublishContextL( const TDesC& aSource, 
                          const TDesC& aType,
                          const TDesC& aValue );
    

    /**
    * @brief Subscribe a context.
    * @since S60 5.0
    * @param aSource	Source of context
    * @param aType		Type of context
    * @return
    */
    void SubscribeContextL( const TDesC& aSource, 
                          const TDesC& aType );
    
                          
    /**
    * @brief Unsubscribe all contexts
    * @since S60 5.0
    */
    void UnsubscribeContexts();


    /**
    * @brief Create a connection to Context Framework Server 
    * @since S60 5.0
    */
    void OpenCFClientL();

    /**
    * @brief Unsusbscribe all actions and close the CCFClient session.
    * @since S60 5.0
    */
    void CloseCFClient();

public:
    /**
    * @brief Subscribe to actions and publish the new alarm status.
    * @since S60 5.0
    */
    void StartL(const TAlarmSourceStateValues aEvent);

    /**
    * @brief Publish 'alaram inactive' state and close the session
    * @since S60 5.0
    */
    void StopL();

    /**
    * 
    * @since S60 5.0
    */
    void PublishAlarmResultL(const TAlarmSourceResultValues aResult);

private: // constructors

    /** 
    * hidden default contructor 
    */
    CAlarmContextFwSupport(CAlmAlarmControl* aAlarmControl);

    /** 
    * Symbian 2nd phase constructor 
    */
    void ConstructL();

private: // data

    /**
    * Pointer to the Context Framework's client side API
    * Own.
    **/
    CCFClient* iCFClient;

    /** 
    * Array of subscribed actions.
    * Own.
    */
    RPointerArray<CCFActionSubscription> iSubscribedActions;
    
    /** 
    * Array of subscribed contexts.
    * Own.
    */
    RPointerArray<CCFContextSubscription> iSubscribedContexts;

    /**
    * Pointer to the alarm control.
    * Not own.
    */
    CAlmAlarmControl* iAlarmControl;

};

#endif // CALARMCONTEXTFWSUPPORT_H


// End of File
