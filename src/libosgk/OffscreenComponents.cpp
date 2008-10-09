/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Dan Rosen <dr@netscape.com>
 *   Frank Richter <res@crystalspace3d.org>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "defs_private.h"
#include "mozilla-config.h"

#include GECKO_INCLUDE(gfx,nsGfxCIID.h)
#include GECKO_INCLUDE(widget,nsWidgetsCID.h)

#include GECKO_INCLUDE(xpcom,nsIComponentRegistrar.h)
#include GECKO_INCLUDE(xpcom,nsISupports.h)

#include "OffscreenComponents.h"
#include "OffscreenDeviceContext.h"
#include "OffscreenWidget.h"

namespace OSGK
{
  namespace Impl
  {
    NS_GENERIC_FACTORY_CONSTRUCTOR(OffscreenWidget)
    NS_GENERIC_FACTORY_CONSTRUCTOR(OffscreenDeviceContext)

    NS_IMPL_ISUPPORTS1(FactoryImpl, nsIFactory)

    NS_IMETHODIMP FactoryImpl::CreateInstance(nsISupports *aOuter, const nsIID & iid, void * *result)
    {
      return ctorFunc (aOuter, iid, result);
    }

    NS_IMETHODIMP FactoryImpl::LockFactory(PRBool lock)
    {
      return NS_OK;
    }

    nsresult OffscreenComponents::Register (nsIComponentRegistrar* registrar)
    {
      static NS_DEFINE_CID(cidWindow, NS_WINDOW_CID);
      static NS_DEFINE_CID(cidChild,  NS_CHILD_CID);
      static NS_DEFINE_CID(cidDC,     NS_DEVICE_CONTEXT_CID);
    
      if (!offscreenWindowFactory)
	offscreenWindowFactory = new FactoryImpl (OffscreenWidgetConstructor);
      if (!offscreenDCFactory)
	offscreenDCFactory = new FactoryImpl (OffscreenDeviceContextConstructor);
    
      nsresult res;
    
      res = registrar->RegisterFactory (cidWindow, "OffscreenWindow",
	"@mozilla.org/widgets/window/offscreen;1", offscreenWindowFactory);
      if (NS_FAILED (res)) return res;
    
      res = registrar->RegisterFactory (cidChild, "Child OffscreenWindow",
	"@mozilla.org/widgets/child_window/offscreen;1", offscreenWindowFactory);
      if (NS_FAILED (res)) return res;
    
      res = registrar->RegisterFactory (cidDC, "Offscreen Device Context",
	"@mozilla.org/gfx/devicecontext/offscreen;1", offscreenDCFactory);
      if (NS_FAILED (res)) return res;
    
      return res;
    }
  } // namespace Impl
} // namespace OSGK
