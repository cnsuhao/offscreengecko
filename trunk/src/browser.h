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
 * The Original Code is OffscreenGecko code.
 *
 * The Initial Developer of the Original Code is
 * Frank Richter <frank.richter@gmail.com>.
 * Portions created by the Initial Developer are Copyright (C) 2007
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
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

#ifndef __BROWSER_H__
#define __BROWSER_H__

#include "OffscreenGecko/browser.h"

#include "baseobj_private.h"
#include "defs_private.h"
#include "eventhelpers.h"
#include "NativeWidget.h"
#include "ref.h"

#include GECKO_INCLUDE(docshell,nsIWebNavigation.h)
#include GECKO_INCLUDE(thebes,gfxContext.h)
#include GECKO_INCLUDE(thebes,gfxImageSurface.h)
#include GECKO_INCLUDE(uriloader,nsIWebProgressListener.h)
#include GECKO_INCLUDE(webbrwsr,nsIEmbeddingSiteWindow.h)
#include GECKO_INCLUDE(webbrwsr,nsIWebBrowser.h)
#include GECKO_INCLUDE(webbrwsr,nsIWebBrowserChrome.h)
#include GECKO_INCLUDE(widget,nsIBaseWindow.h)
#include GECKO_INCLUDE(xpcom,nsAutoPtr.h)
#include GECKO_INCLUDE(xpcom,nsCOMPtr.h)
#include GECKO_INCLUDE(xpcom,nsIInterfaceRequestor.h)

#include <vector>

namespace OSGK
{
  namespace Impl
  {
    class Container;
    class Embedding;
    class OffscreenWidget;
    
    class Browser : public BaseObject<OSGK_Browser>, public NativeWidget
    {
      Ref<Embedding> embedding;
      
      nsCOMPtr<nsIWebBrowser> webBrowser;
      nsCOMPtr<nsIWebNavigation> webNav;
      nsCOMPtr<nsIBaseWindow> baseWindow;
      nsRefPtr<Container> container;

      OffscreenWidget* widget;
      nsRefPtr<gfxImageSurface> surface;
      OffscreenWidget* focusedWidget;

      struct LockedBuffer
      {
        const unsigned char* p;
        int lockCount;
        nsRefPtr<gfxImageSurface> surface;

        LockedBuffer() : p (0), lockCount (0) {}
      };
      LockedBuffer lastLock;
      typedef std::vector<LockedBuffer> LockedBufferVec;
      LockedBufferVec locks;

      int updateState;
      EventHelpers::KeyState kstate;

      gfxContext::AntialiasMode aaMode;

      int mouseX, mouseY;
      bool MouseInside () const { return (mouseX >= 0) && (mouseY >= 0); }
    public:
      Browser (OSGK_GeckoResult& result, Embedding* embedding,
	int width, int height);
      ~Browser ();
      void SetWidget (OffscreenWidget* widget)
      { 
        this->widget = widget; 
        focusedWidget = widget;
      }
      OffscreenWidget* GetWidget() const { return widget; }
      void SetFocusedWidget (OffscreenWidget* widget)
      { focusedWidget = widget; }
      Embedding* GetEmbedding() { return embedding; }

      Browser* GetBrowser() { return this; }

      void Navigate (const char* uri);
      bool QueryDirty() { return updateState >= updDirty; }
      const unsigned char* LockData ();
      void UnlockData (const unsigned char* p);

      void EventMouseMove (int x, int y);
      void EventMouseButton (OSGK_MouseButton button, 
        OSGK_MouseButtonEventType eventType);

      bool EventKey (unsigned int key, OSGK_KeyboardEventType eventType);

      void SetAntialias (OSGK_AntiAliasType aaType);
      OSGK_AntiAliasType GetAntialias ();
      gfxContext::AntialiasMode GetGeckoAA() const { return aaMode; }

      void DoFocus (bool haveFocus, bool focusExternal);

      void Resize (int width, int height);

      gfxASurface* GetSurface() { return surface; }
      enum
      { 
        updClean, updDirty, updNeedsUpdate
      };
      void SetUpdateState (int newState)
      {
        if (newState > updateState)
          updateState = newState;
      }
      void UpdateBrowser();
    };
  } // namespace Impl
} // namespace OSGK

#endif // __BROWSER_H__
