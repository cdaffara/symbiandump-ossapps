// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef PBAPERRORREPORTER_H
#define PBAPERRORREPORTER_H

/**
Interface to report errors to Obex during asynchronous get requests
*/
class MPbapErrorReporter
	{
public:
	virtual void SendServiceUnavailableError() =0;
	virtual void SendPreconditionFailedError() =0;
	virtual void SendNotFoundError() =0;
	};
	
#endif //PBAPERRORREPORTER_H
