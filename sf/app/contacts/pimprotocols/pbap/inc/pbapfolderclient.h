// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef PBAPFOLDERCLIENT_H
#define PBAPFOLDERCLIENT_H

#include "pbaprequest.h"

class CContactDatabase;
class MPbapExporter;
class MPbapErrorReporter;
class CPbapContactDbViews;
class CPbapLogWrapper;
class CPbapVCardExporterUtil;

class MVirtualFolderClient
	{
public:
	virtual CContactDatabase& ContactDB() const =0;
	virtual MPbapExporter& Exporter() =0;
	virtual MPbapErrorReporter& ErrorReporter() =0;
	virtual CPbapContactDbViews& ContactDbViews() =0;
	virtual CPbapLogWrapper& LogClient() const =0;
	virtual CPbapVCardExporterUtil& ExporterUtil() = 0;
	};

#endif //PBAPFOLDERCLIENT_H
