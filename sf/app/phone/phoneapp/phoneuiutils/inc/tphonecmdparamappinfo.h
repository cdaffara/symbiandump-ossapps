/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
*     A paramater class for application information.
*
*/


#ifndef __TPHONECMDPARAMAPPINFO_H
#define __TPHONECMDPARAMAPPINFO_H

//  INCLUDES

#include "tphonecommandparam.h"

// DATA TYPES

// CLASS DECLARATION

/**
*  A paramater class for identifying view.
*/
class TPhoneCmdParamAppInfo : public TPhoneUICommandParam
    {   
    public:

        /**
        * Constructor.
        */
        IMPORT_C TPhoneCmdParamAppInfo();

        /**
        * Sets application uid.
        * @param    aAppUid, Id of app.
        * @return   None.
        */
        IMPORT_C void SetAppUid( const TUid& aAppUid );

        /**
        * Sets view uid.
        * @param    aViewUid, Id of view.
        * @return   None.
        */
        IMPORT_C void SetViewUid( const TUid& aViewUid );

        /**
        * Sets application parameter.
        * @param    aParam
        * @return   None.
        */
        IMPORT_C void SetParam( const TDesC8& aParam );

        /**
        * Sets Custom Message Id.
        * @param    aCustomMessageId
        * @return   None.
        */
        IMPORT_C void SetCustomMessageId( const TUid& aCustomMessageId );

        /**
        * Sets Custom Message.
        * @param    aCustomMessage
        * @return   None.
        */
        IMPORT_C void SetCustomMessage( const TDesC8& aCustomMessage );

        /**
        * Sets multiple instances status for the application
        * @param aMultipleInstances is ETrue if there are multiple instances
        *  of the application.
        */
        IMPORT_C void SetMultipleInstances( TBool aMultipleInstances );

        /**
        * Returns App uid.
        * @param    None.
        * @return   iAppUid.
        */
        IMPORT_C TUid AppUid() const;

        /**
        * Returns View uid.
        * @param    None.
        * @return   iViewUid.
        */
        IMPORT_C TUid ViewUid() const;

        /**
        * Returns App parameter.
        * @param    None.
        * @return   iParam.
        */
        IMPORT_C const TDesC8& Param() const;

        /**
        * Returns Custom Message Id.
        * @param    None.
        * @return   iCustomMessageId.
        */
        IMPORT_C TUid CustomMessageId() const;

        /**
        * Returns CustomMessage.
        * @param    None.
        * @return   iMessage.
        */
        IMPORT_C const TDesC8& CustomMessage() const;

          /**
        * Returns multiple instance status of the application.
        * @return Returns ETrue if there are multiple instances of the
        *  application.
        */
        IMPORT_C TBool MultipleInstances() const;

  private:    

        /**
        * App UID.
        */
        TUid iAppUid;

        /**
        * App view UID.
        */
        TUid iViewUid;

        /**
        * App parameter
        */
        TPtrC8 iParam;

        /**
        * Custom message ID.
        */
        TUid iCustomMessageId;

        /**
        * Custom message
        */
        TPtrC8 iCustomMessage;

        /**
        * Indicates that they are multiple instance for this application
        */
        TBool iMultipleInstances;

    };

#endif // __TPHONECMDPARAMAPPINFO_H   
            
// End of File
