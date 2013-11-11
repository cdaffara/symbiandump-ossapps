/*******************************************************************************
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     Nokia Corporation - initial implementation 
 *******************************************************************************/
package org.eclipse.swt.internal.qt.graphics;

import org.eclipse.swt.events.DisposeListener; 
import org.eclipse.swt.events.DisposeEvent;

/**
 * Listener for widget disposal for automatic surface cache cleanup
 */ 

final class SurfaceCacheItemDisposeListener implements DisposeListener {
    
    public void widgetDisposed(DisposeEvent e) {
        SurfaceCache cache = SurfaceCache.getInstance();
        if (cache != null) {
            try {
                cache.deleteSurface(e.widget);
            } catch (IllegalStateException is) {
                // ignore 
            }
        }    
    }
}