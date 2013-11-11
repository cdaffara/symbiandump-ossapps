/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#ifndef BEDROCK_PROVISIONING_GLOBAL_H
#define BEDROCK_PROVISIONING_GLOBAL_H

#include <QtCore/qglobal.h>

// Export Settings.
#ifndef BEDROCKPROVISIONING_EXPORT
  #if defined (BUILDING_BEDROCK_PROVISIONING)
    #define BEDROCKPROVISIONING_EXPORT Q_DECL_EXPORT
  #else
    #define BEDROCKPROVISIONING_EXPORT Q_DECL_IMPORT
  #endif
#endif

#endif // BEDROCKPROVISIONINGGLOBAL_H
