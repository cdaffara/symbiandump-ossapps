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
* Description:  tracing stub used to get stubs included first 
*
*/

#ifndef VIDEOCOLLETIONTRACE_H
#define VIDEOCOLLETIONTRACE_H

#include "apaidstub.h"
#include "apgclistub.h"
#include "coemainstub.h"
#include "apgtaskstub.h"

#define ERROR( aErr, aMsg )
#define ERROR_1( aErr, aMsg, aP1 )
#define ERROR_2( aErr, aMsg, aP1, aP2 )
#define ERROR_3( aErr, aMsg, aP1, aP2, aP3 )
#define ERROR_GEN( aMsg )
#define ERROR_GEN_1( aMsg, aP1 )
#define ERROR_GEN_2( aMsg, aP1, aP2 )
#define ERROR_GEN_3( aMsg, aP1, aP2, aP3 )

#define ERROR_PARAM(_p)

#define TRAPD_ERR( aErr, aStmt ) TRAP_IGNORE( aStmt )
#define TRAP_ERR( aErr, aStmt )  TRAP_IGNORE( aStmt )

#define TRAP_AND_LEAVE(_s,_t) { _s; }


#define INFO( aMsg )
#define INFO_1( aMsg, aP1 )
#define INFO_2( aMsg, aP1, aP2 )
#define INFO_3( aMsg, aP1, aP2, aP3 )
#define INFO_4( aMsg, aP1, aP2, aP3, aP4 )
#define INFO_5( aMsg, aP1, aP2, aP3, aP4, aP5 )
#define INFO_PARAM( aParam )
#define INFOQSTR_1( aMsg, aP1 );
#define INFOQSTR_2( aMsg, aP1, aP2 );


#define CURRENT_CLIENT( aMsg )
#define CLIENT( aRMessage2 )
#define CLIENT_1( aRMessage2, aMsg )


#define HEAP( aMsg )
#define HEAP_1( aMsg, aP1 )
#define HEAP_2( aMsg, aP1, aP2 )
#define HEAP_3( aMsg, aP1, aP2, aP3 )
#define HEAP_4( aMsg, aP1, aP2, aP3, aP4 )

#define FUNC_LOG
#define FUNC_LOG_ADDR(P1)
#define FUNC_LOG_LC( ptr )
#define FUNC_LOG_WITH_CLIENT
#define FUNC_LOG_WITH_CLIENT_LC( ptr )

#define TIMESTAMP( aCaption )
#define TIMESTAMP_1( aCaption, aP1 )
#define TIMESTAMP_2( aCaption, aP1, aP2 )
#define TIMESTAMP_3( aCaption, aP1, aP2, aP3 )


#define ASSERT_ALWAYS_TRACE
#define ASSERT_TRACE( _s )

#endif // VIDEOCOLLETIONTRACE_H
