/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MCALENCOMMANDHANDLER_H
#define MCALENCOMMANDHANDLER_H



#include "calencontext.h"
#include "caleninstanceid.h"
#include <caltime.h>
#include <vwsdef.h>

// Local class used wrap the current command and information
// to restore the current context
class TCalenCommand
    {
    public:
        TCalenCommand();
        void SetCommandAndContextL( TInt aCommand,
                                     MCalenContext& aContext);

        TInt Command() const;
        void GetContextL( MCalenContext& aContext ) const ;

    private:
        TInt iCommand;
        TCalTime iFocusDateAndTime;
        TCalenInstanceId iInstanceId;
        TVwsViewId iViewId;
    };

inline TCalenCommand::TCalenCommand()
    {
    }

inline void TCalenCommand::SetCommandAndContextL( TInt aCommand,
                                                             MCalenContext& aContext)
    {
    iCommand = aCommand;
    iFocusDateAndTime = aContext.FocusDateAndTimeL();
    iInstanceId = aContext.InstanceId();
    iViewId = aContext.ViewId();
    }

inline TInt TCalenCommand::Command() const
    {
    return iCommand;
    }

inline void TCalenCommand::GetContextL( MCalenContext& aContext ) const
    {
    aContext.SetFocusDateAndTimeAndInstanceL( iFocusDateAndTime,
                                              iInstanceId,
                                              iViewId );
    }



/**
 * MCalenCommandHandler is an interface for a customisation
 * to populate a menupane and toolbar and handle any commands
 */
class MCalenCommandHandler
    {
public:
    virtual TBool HandleCommandL( const TCalenCommand& aCommand ) = 0;
    };

#endif // MCALENCOMMANDHANDLER_H

// End of file
