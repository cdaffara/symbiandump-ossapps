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
* Description:  Message store server process implementation.
*
*/

#include <e32std.h>

#include "MessageStoreClientServer.h"

// ==========================================================================
// Server process entry-point
// ==========================================================================
TInt E32Main()
  {
  // Set the priority of this process to slightly higher than background, in order to give the
  // UI the highest priority.
  RProcess myProcess;
  myProcess.SetPriority( EPriorityBackground );
  
  RThread myThread;
  myThread.SetPriority( EPriorityMore );

  TInt returnValue = MessageStoreServerThreadFunction( (TAny*)ETrue );  

  return returnValue;
  
  } // end E32Main

