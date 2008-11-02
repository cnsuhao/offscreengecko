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
 ****
 *
 * Portions of this file are from the Mozilla WinEmbed sample; license follows.
 *
 * Version: Mozilla-sample-code 1.0
 *
 * Copyright (c) 2002 Netscape Communications Corporation and
 * other contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this Mozilla sample software and associated documentation files
 * (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do so, subject to the
 * following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Copyright (c) 2002 Netscape Communications Corporation and
 * other contributors
 *
 * Contributor(s):
 *   Doug Turner <dougt@netscape.com>
 *   Adam Lock <adamlock@netscape.com>
 *
 * ***** END LICENSE BLOCK ***** */

#include "defs_private.h"
#include "mozilla-config.h"

#include "wrap_gfxasurface.h"

#include "browser.h"
#include "embedding.h"
#include "OffscreenWidget.h"

// HACKs to allow inclusion of nsPresContext.h
#define nsAString_h___
class nsAFlatCString;
// HACKs to allow inclusion of nsIFrame.h
#define nsString_h___

#include GECKO_INCLUDE(content,nsIDocument.h)
#include GECKO_INCLUDE(docshell,nsIDocShellTreeItem.h)
#include GECKO_INCLUDE(dom,nsIDOMDocument.h)
#include GECKO_INCLUDE(dom,nsIDOMWindow.h)
#include GECKO_INCLUDE(embed_base,nsEmbedCID.h)
#include GECKO_INCLUDE(generic,nsHTMLReflowState.h)
#include GECKO_INCLUDE(layout,nsIDocumentViewer.h)
#include GECKO_INCLUDE(layout,nsIFrame.h)
#include GECKO_INCLUDE(layout,nsPresContext.h)
#include GECKO_INCLUDE(webbrwsr,nsIWebBrowserSetup.h)
#include GECKO_INCLUDE(widget,nsIWidget.h)
#include GECKO_INCLUDE(xpcom,nsComponentManagerUtils.h)
#include GECKO_INCLUDE(xpcom,nsIWeakReference.h)
#include GECKO_INCLUDE(xpcom,nsIWeakReferenceUtils.h)
#include GECKO_INCLUDE(xpcom,nsStringAPI.h)
#include GECKO_INCLUDE(xpcom,nsWeakReference.h)

#ifdef MOZ_WIDGET_GTK2
#include "gtk/gtk.h"
#endif

/* Depends on whether a modification I made to gecko makes it into the source -
   see https://bugzilla.mozilla.org/show_bug.cgi?id=407531 */
//#define HAVE_GECKO_AA_GRAY

OSGK_Browser* osgk_browser_create (OSGK_Embedding* embedding, 
                                   int width, int height)
{
  OSGK_GeckoResult rc;
  OSGK::Impl::Browser* b = new OSGK::Impl::Browser (rc,
    static_cast<OSGK::Impl::Embedding*> (embedding), width, height);
  if (NS_FAILED(rc))
  {
    delete b;
    return 0;
  }
  static_cast<OSGK::Impl::Embedding*> (embedding)->FocusBrowserDefault (b);
  return b;
}
  
void osgk_browser_navigate (OSGK_Browser* browser, const char* uri)
{
  static_cast<OSGK::Impl::Browser*> (browser)->Navigate (uri);
}

int osgk_browser_query_dirty (OSGK_Browser* browser)
{
  return static_cast<OSGK::Impl::Browser*> (browser)->QueryDirty ();
}

const unsigned char* osgk_browser_lock_data (OSGK_Browser* browser_, 
  int* isDirty)
{
  OSGK::Impl::Browser* browser = static_cast<OSGK::Impl::Browser*> (browser_);
  if (isDirty) *isDirty = browser->QueryDirty();
  return browser->LockData ();
}

void osgk_browser_unlock_data (OSGK_Browser* browser,
  const unsigned char* data)
{
  static_cast<OSGK::Impl::Browser*> (browser)->UnlockData (data);
}

void osgk_browser_event_mouse_move (OSGK_Browser* browser, int x, int y)
{
  static_cast<OSGK::Impl::Browser*> (browser)->EventMouseMove (x, y);
}

void osgk_browser_event_mouse_button (OSGK_Browser* browser, 
  OSGK_MouseButton button, OSGK_MouseButtonEventType eventType)
{
  static_cast<OSGK::Impl::Browser*> (browser)->EventMouseButton (button, eventType);
}

void osgk_browser_event_mouse_wheel (OSGK_Browser* browser, OSGK_WheelAxis axis, 
  OSGK_WheelDirection direction)
{
  static_cast<OSGK::Impl::Browser*> (browser)->EventMouseWheel (axis, direction);
}

int osgk_browser_event_key (OSGK_Browser* browser, unsigned int key,
  OSGK_KeyboardEventType eventType)
{
  return static_cast<OSGK::Impl::Browser*> (browser)->EventKey (key, eventType);
}

void osgk_browser_set_antialias (OSGK_Browser* browser, OSGK_AntiAliasType aaType)
{
  static_cast<OSGK::Impl::Browser*> (browser)->SetAntialias (aaType);
}

OSGK_AntiAliasType osgk_browser_get_antialias (OSGK_Browser* browser)
{
  return static_cast<OSGK::Impl::Browser*> (browser)->GetAntialias ();
}

void osgk_browser_focus (OSGK_Browser* browser)
{
  OSGK::Impl::Browser* b = static_cast<OSGK::Impl::Browser*> (browser);
  b->GetEmbedding()->FocusBrowser (b);
}

void osgk_browser_resize (OSGK_Browser* browser, int width, int height)
{
  static_cast<OSGK::Impl::Browser*> (browser)->Resize (width, height);
}

int osgk_browser_set_user_data (OSGK_Browser* browser, unsigned int key, void* data, 
                                int overrideData)
{
  return static_cast<OSGK::Impl::Browser*> (browser)->SetUserData (key, data,
    overrideData != 0);
}

int osgk_browser_get_user_data (OSGK_Browser* browser, unsigned int key, void** data)
{
  if (data == 0) return 0;
  return static_cast<OSGK::Impl::Browser*> (browser)->GetUserData (key, *data);
}

OSGK_LoadState osgk_browser_query_load_state (OSGK_Browser* browser)
{
  return static_cast<OSGK::Impl::Browser*> (browser)->QueryLoadState ();
}

float osgk_browser_query_load_progress (OSGK_Browser* browser)
{
  return static_cast<OSGK::Impl::Browser*> (browser)->QueryLoadProgress ();
}

int osgk_browser_get_preferred_dimensions (OSGK_Browser* browser,
  int* preferredWidth, int* preferredHeight,
  int maxWidth)
{
  return static_cast<OSGK::Impl::Browser*> (browser)->GetPreferredDimensions (
    *preferredWidth, *preferredHeight, maxWidth);
}


namespace OSGK
{
  namespace Impl
  {
    class Container : public nsIWebBrowserChrome,
                      public nsIEmbeddingSiteWindow,
                      public nsIWebProgressListener,
                      public nsSupportsWeakReference,
                      public nsIInterfaceRequestor
    {
      nsIWebBrowser* webBrowser;
      PRUint32     chromeFlags;
      
      OSGK_LoadState loadState;
      float loadProgress;
    public:
      Container ();
      virtual ~Container ();

      NS_DECL_ISUPPORTS
      NS_DECL_NSIWEBBROWSERCHROME
      NS_DECL_NSIWEBPROGRESSLISTENER
      NS_DECL_NSIEMBEDDINGSITEWINDOW
      NS_DECL_NSIINTERFACEREQUESTOR

      OSGK_LoadState QueryLoadState () { return loadState; }
      float QueryLoadProgress () { return loadProgress; }
    };

    Browser::Browser (OSGK_GeckoResult& result, Embedding* embedding, 
                      int width, int height) :
      embedding (embedding), updateState (updClean), mouseX (-1),
      mouseY (-1), focusedWidget (0), userData (0)
    {
      nsresult rv;
      webBrowser = do_CreateInstance (NS_WEBBROWSER_CONTRACTID, &rv);
      if (NS_FAILED(rv))
      {
        result = rv;
        return; 
      }

      webNav = do_QueryInterface (webBrowser, &rv);
      if (NS_FAILED(rv))
      {
        result = rv;
        return; 
      }

      container = new Container;
      rv = webBrowser->SetContainerWindow (
        static_cast<nsIWebBrowserChrome*> (container));
      if (NS_FAILED(rv))
      {
        result = rv;
        return; 
      }

      baseWindow = do_QueryInterface (webBrowser, &rv);
      if (NS_FAILED(rv))
      {
        result = rv;
        return; 
      }

      rv = baseWindow->InitWindow (static_cast<NativeWidget*> (this), 
        nsnull, 0, 0, width, height);
      if (NS_FAILED(rv))
      {
        result = rv;
        return; 
      }
      rv = baseWindow->Create(); 
      if (NS_FAILED(rv))
      {
        result = rv;
        return; 
      }

      nsCOMPtr<nsIWebProgressListener> listener(
        static_cast<nsIWebProgressListener*> (container));
      nsCOMPtr<nsIWeakReference> thisListener (do_GetWeakReference(listener));
      webBrowser->AddWebBrowserListener(thisListener, 
        NS_GET_IID(nsIWebProgressListener));

      baseWindow->SetPositionAndSize(0, 0, width, height, PR_TRUE);
      baseWindow->SetVisibility(PR_TRUE);

      gfxIntSize size (width, height);
      surface = new gfxImageSurface (size,
        gfxASurface::ImageFormatARGB32);

      Navigate ("about:blank");

      SetAntialias (aaGray);

      result = NS_OK;
    }

    Browser::~Browser ()
    {
      GetEmbedding()->Unfocus (this);
      delete userData;
    }

    void Browser::ProcessToolkitEvents()
    {
    #ifdef MOZ_WIDGET_GTK2
      gtk_main_iteration_do (FALSE);
    #endif
    }
    
    void Browser::Navigate (const char* uri)
    {
      webNav->LoadURI (NS_ConvertUTF8toUTF16 (uri).get(), 
        nsIWebNavigation::LOAD_FLAGS_NONE, 0, 0, 0);
    }

    const unsigned char* Browser::LockData ()
    {
      if (lastLock.surface == surface)
      {
        lastLock.lockCount++;
        return lastLock.p;
      }
      else if (lastLock.surface != 0)
      {
        locks.push_back (lastLock);
      }
      ProcessToolkitEvents ();

      if (updateState == updNeedsUpdate) UpdateBrowser();
      updateState = updClean;
      lastLock.surface = surface;
      lastLock.lockCount = 1;
      lastLock.p = surface->Data();
      return lastLock.p;
    }
      
    void Browser::UnlockData (const unsigned char* p)
    {
      if (lastLock.p == p)
      {
        lastLock.lockCount--;
        if (lastLock.lockCount <= 0)
        {
          lastLock.surface = 0;
          return;
        }
      }
      else
      {
        for (LockedBufferVec::iterator lock = locks.begin(); lock < locks.end(); lock++)
        {
          if (lock->p == p)
          {
            lock->lockCount--;
            if (lock->lockCount <= 0)
            {
              locks.erase (lock);
            }
            break;
          }
        }
      }
    }

    void Browser::UpdateBrowser()
    {
      if (!widget) return;
      widget->Update();
    }

    void Browser::EventMouseMove (int x, int y)
    {
      if (!widget) return;
      embedding->UpdateAutoFocus (this);
      mouseX = x; mouseY = y;
      if (MouseInside())
      {
        widget->EventMouseMove (kstate, mouseX, mouseY);
      }
    }

    void Browser::EventMouseButton (OSGK_MouseButton button, 
      OSGK_MouseButtonEventType eventType)
    {
      if (!widget) return;
      embedding->UpdateAutoFocus (this);
      if (MouseInside())
      {
        widget->EventMouseButton (kstate, mouseX, mouseY,
          button, eventType);
      }
    }

    void Browser::EventMouseWheel (OSGK_WheelAxis axis, 
                                   OSGK_WheelDirection direction)
    {
      if (!widget) return;
      embedding->UpdateAutoFocus (this);
      int flags = 0, delta = 0;
      switch (axis)
      {
        case waHorizontal: flags = nsMouseScrollEvent::kIsHorizontal; break;
        case waVertical:   flags = nsMouseScrollEvent::kIsVertical; break;
        default: return;
      }
      switch (direction)
      {
        case wdNegativePage:
          flags |= nsMouseScrollEvent::kIsFullPage;
        case wdNegative:
          delta = -1;
          break;
        case wdPositivePage:
          flags |= nsMouseScrollEvent::kIsFullPage;
        case wdPositive:
          delta = 1;
          break;
        default:
          return;
      }
      widget->EventMouseWheel (kstate, mouseX, mouseY, flags, delta);
    }

    bool Browser::EventKey (unsigned int key, OSGK_KeyboardEventType eventType)
    {
      if (!widget) return false;
      embedding->UpdateAutoFocus (this);
      if (focusedWidget == 0) return false;

      bool isChar;
      unsigned int geckoKey = EventHelpers::KeyOSGKToGecko (key, isChar);
      if (geckoKey == 0) return false;
      bool down;
      switch (eventType)
      {
        case keDown: down = true;  break;
        case keUp:   down = false; break;
        case kePress:
          return focusedWidget->EventKeyPress (kstate, geckoKey, isChar);
        default: return false;
      }

      EventHelpers::KeyState newState (kstate);
      newState.ApplyKey (key, down);

      bool taken = focusedWidget->EventKey (newState, geckoKey, down, isChar);
      kstate = newState;
      return taken;
    }

    void Browser::SetAntialias (OSGK_AntiAliasType aaType)
    {
      switch (aaType)
      {
      case aaNone:
        aaMode = gfxContext::MODE_ALIASED;
        break;
      case aaGray:
#ifdef HAVE_GECKO_AA_GRAY
        aaMode = gfxContext::MODE_COVERAGE_GRAY;
        break;
#endif
      case aaSubpixel:
        aaMode = gfxContext::MODE_COVERAGE;
        break;
      }
    }
      
    OSGK_AntiAliasType Browser::GetAntialias ()
    {
      switch (aaMode)
      {
      case gfxContext::MODE_ALIASED:
        return aaNone;
#ifdef HAVE_GECKO_AA_GRAY
      case gfxContext::MODE_COVERAGE_GRAY:
        return aaGray;
#endif
      default:
      case gfxContext::MODE_COVERAGE:
        return aaSubpixel;
      }
    }

    void Browser::DoFocus (bool haveFocus, bool focusExternal)
    {
      if (!widget) return;
      if (focusedWidget != 0)
        focusedWidget->ChangeFocus (haveFocus, focusExternal);
      else
      {
        static_cast<OffscreenWidget*> (widget->GetFirstChild())->ChangeFocus (
          haveFocus, focusExternal);
      }
    }

    void Browser::Resize (int width, int height)
    {
      if (!widget) return;
      gfxIntSize size (width, height);
      surface = new gfxImageSurface (size,
        gfxASurface::ImageFormatARGB32);

      widget->ResizeFromTheSourceOfPower (width, height);
      baseWindow->SetSize (width, height, true);
    }

    bool Browser::SetUserData (unsigned int key, void* data, bool overrideData)
    {
      UserDataHash& userData = this->GetUserDataHash();

      if (!overrideData && (userData.count (key) > 0)) return false;
      userData[key] = data;
      return true;
    }

    bool Browser::GetUserData (unsigned int key, void*& data)
    {
      if (!userData) return false;

      UserDataHash& userData = this->GetUserDataHash();
      if (userData.count (key) == 0) return false;
      data = userData[key];
      return true;
    }

    OSGK_LoadState Browser::QueryLoadState ()
    {
      if (!container) return loadFinished;
      ProcessToolkitEvents ();
      return container->QueryLoadState();
    }
    
    float Browser::QueryLoadProgress ()
    {
      if (!container) return 1.0f;
      ProcessToolkitEvents ();
      return container->QueryLoadProgress();
    }
    
    bool Browser::GetPreferredDimensions (int& preferredWidth, 
					  int& preferredHeight,
					  int maxWidth)
    {
      nsCOMPtr<nsIDOMWindow> domWindow;
      webBrowser->GetContentDOMWindow (getter_AddRefs(domWindow));
      if (!domWindow) return false;
      
      // Mostly lifted from Gecko DocumentViewerImpl::SizeToContent()
      nsCOMPtr<nsIDOMDocument> domDoc;
      domWindow->GetDocument (getter_AddRefs(domDoc));
      NS_ENSURE_TRUE(domDoc, false);
      
      nsCOMPtr<nsIDocument> doc (do_QueryInterface(domDoc));
      NS_ENSURE_TRUE(doc, false);
      
      nsIPresShell* presShell = doc->GetPrimaryShell();
      NS_ENSURE_TRUE(presShell, false);
      
      nsPresContext* presContext = presShell->GetPresContext ();
      NS_ENSURE_TRUE(presContext, false);
      
      nsCOMPtr<nsISupports> container (presContext->GetContainer());

      nsCOMPtr<nsIDocShellTreeItem> docShellAsItem(do_QueryInterface(container));
      NS_ENSURE_TRUE(docShellAsItem, false);

      nsCOMPtr<nsIDocShellTreeItem> docShellParent;
      docShellAsItem->GetSameTypeParent(getter_AddRefs(docShellParent));

      // It's only valid to access this from a top frame.  Doesn't work from
      // sub-frames.
      NS_ENSURE_TRUE(!docShellParent, false);

      // Flush out all content and style updates. We can't use a resize reflow
      // because it won't change some sizes that a style change reflow will.
      doc->FlushPendingNotifications(Flush_Layout);
  
      nsIFrame *root = presShell->GetRootFrame();
      NS_ENSURE_TRUE(root, false);
  
      maxWidth = presContext->DevPixelsToAppUnits (maxWidth-1);
      nscoord prefWidth;
      {
	nsCOMPtr<nsIRenderingContext> rcx;
	presShell->CreateRenderingContext(root, getter_AddRefs(rcx));
	NS_ENSURE_TRUE(rcx, NS_ERROR_FAILURE);
	nscoord minWidth = root->GetMinWidth(rcx);
	prefWidth = root->GetPrefWidth(rcx);
	if ((maxWidth >= 0) && (prefWidth > maxWidth))
	  prefWidth = maxWidth;
	if (prefWidth < minWidth) prefWidth = minWidth;
      }

      nsresult rv = presShell->ResizeReflow(prefWidth, NS_UNCONSTRAINEDSIZE);
      NS_ENSURE_SUCCESS(rv, false);
    
      PRInt32 width, height;
  
      // so how big is it?
      nsRect shellArea = presContext->GetVisibleArea();
      // Protect against bogus returns here
      NS_ENSURE_TRUE(shellArea.width != NS_UNCONSTRAINEDSIZE &&
		     shellArea.height != NS_UNCONSTRAINEDSIZE,
		     false);
      width = presContext->AppUnitsToDevPixels(shellArea.width);
      height = presContext->AppUnitsToDevPixels(shellArea.height);
  
      /* presContext's size was calculated in app units and has already been
	 rounded to the equivalent pixels (so the width/height calculation
	 we just performed was probably exact, though it was based on
	 values already rounded during ResizeReflow). In a surprising
	 number of instances, this rounding makes a window which for want
	 of one extra pixel's width ends up wrapping the longest line of
         text during actual window layout. This makes the window too short,
	 generally clipping the OK/Cancel buttons. Here we add one pixel
	 to the calculated width, to circumvent this problem. */
      preferredWidth = width + 1;
      preferredHeight = height;
      
      return true;
    }
    
    //-----------------------------------------------------------------------

    NS_IMPL_ADDREF(Container)
    NS_IMPL_RELEASE(Container)

    NS_INTERFACE_MAP_BEGIN(Container)
       NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, nsIWebBrowserChrome)
       NS_INTERFACE_MAP_ENTRY(nsIWebBrowserChrome)
       NS_INTERFACE_MAP_ENTRY(nsIEmbeddingSiteWindow)
       NS_INTERFACE_MAP_ENTRY(nsIWebProgressListener)
       NS_INTERFACE_MAP_ENTRY(nsISupportsWeakReference)
       NS_INTERFACE_MAP_ENTRY(nsIInterfaceRequestor)
    NS_INTERFACE_MAP_END

    Container::Container () : loadState (loadFinished), loadProgress (1.0f)
    {
    }

    Container::~Container ()
    {
    }

    NS_IMETHODIMP Container::GetInterface(const nsIID &aIID, void** aInstancePtr)
    {
        NS_ENSURE_ARG_POINTER(aInstancePtr);

        *aInstancePtr = 0;
        if (aIID.Equals(NS_GET_IID(nsIDOMWindow)))
        {
            if (webBrowser)
            {
                return webBrowser->GetContentDOMWindow((nsIDOMWindow **) aInstancePtr);
            }
            return NS_ERROR_NOT_INITIALIZED;
        }
        return QueryInterface(aIID, aInstancePtr);
    }

    NS_IMETHODIMP Container::SetStatus (PRUint32 aType, const PRUnichar* aStatus)
    {
      return NS_OK;
    }

    NS_IMETHODIMP Container::GetWebBrowser (nsIWebBrowser** aWebBrowser)
    {
      NS_ENSURE_ARG_POINTER(aWebBrowser);
      *aWebBrowser = webBrowser;
      NS_IF_ADDREF(*aWebBrowser);
      return NS_OK;
    }

    NS_IMETHODIMP Container::SetWebBrowser (nsIWebBrowser* aWebBrowser)
    {
      webBrowser = aWebBrowser;
      return NS_OK;
    }

    NS_IMETHODIMP Container::GetChromeFlags(PRUint32* aChromeMask)
    {
      *aChromeMask = chromeFlags;
      return NS_OK;
    }

    NS_IMETHODIMP Container::SetChromeFlags(PRUint32 aChromeMask)
    {
      chromeFlags = aChromeMask;
      return NS_OK;
    }

    NS_IMETHODIMP Container::DestroyBrowserWindow(void)
    {
      return NS_OK;
    }


    // IN: The desired browser client area dimensions.
    NS_IMETHODIMP Container::SizeBrowserTo (PRInt32 aWidth, PRInt32 aHeight)
    {
      /* This isn't exactly correct: we're setting the whole window to
         the size requested for the browser. At time of writing, though,
         it's fine and useful for winEmbed's purposes. */
      //ContainerUI::SizeTo(this, aWidth, aHeight);
      //mSizeSet = PR_TRUE;
      //return NS_OK;
      return NS_ERROR_NOT_IMPLEMENTED;
    }


    NS_IMETHODIMP Container::ShowAsModal (void)
    {
      /*if (mDependentParent)
        AppCallbacks::EnableChromeWindow(mDependentParent, PR_FALSE);

      mContinueModalLoop = PR_TRUE;
      AppCallbacks::RunEventLoop(mContinueModalLoop);

      if (mDependentParent)
        AppCallbacks::EnableChromeWindow(mDependentParent, PR_TRUE);*/

      return NS_OK;
    }

    NS_IMETHODIMP Container::IsWindowModal (PRBool *_retval)
    {
      *_retval = PR_FALSE;
      return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP Container::ExitModalEventLoop (nsresult aStatus)
    {
      //mContinueModalLoop = PR_FALSE;
      return NS_OK;
    }

    NS_IMETHODIMP Container::SetDimensions(PRUint32 aFlags, PRInt32 x, PRInt32 y, PRInt32 cx, PRInt32 cy)
    {
      return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP Container::GetDimensions(PRUint32 aFlags, PRInt32 *x, PRInt32 *y, PRInt32 *cx, PRInt32 *cy)
    {
      if (aFlags & nsIEmbeddingSiteWindow::DIM_FLAGS_POSITION)
      {
          *x = 0;
          *y = 0;
      }
      if (aFlags & nsIEmbeddingSiteWindow::DIM_FLAGS_SIZE_INNER ||
          aFlags & nsIEmbeddingSiteWindow::DIM_FLAGS_SIZE_OUTER)
      {
          *cx = 0;
          *cy = 0;
      }
      return NS_ERROR_NOT_IMPLEMENTED;
    }

    /* void setFocus (); */
    NS_IMETHODIMP Container::SetFocus()
    {
      //WebBrowserChromeUI::SetFocus(this);
      return NS_OK;
    }

    /* attribute wstring title; */
    NS_IMETHODIMP Container::GetTitle(PRUnichar * *aTitle)
    {
     NS_ENSURE_ARG_POINTER(aTitle);

     *aTitle = nsnull;
     
     return NS_ERROR_NOT_IMPLEMENTED;
    }
    NS_IMETHODIMP Container::SetTitle(const PRUnichar * aTitle)
    {
      return NS_ERROR_NOT_IMPLEMENTED;
    }

    /* attribute boolean visibility; */
    NS_IMETHODIMP Container::GetVisibility(PRBool * aVisibility)
    {
      NS_ENSURE_ARG_POINTER(aVisibility);
      *aVisibility = PR_TRUE;
      return NS_OK;
    }
    NS_IMETHODIMP Container::SetVisibility(PRBool aVisibility)
    {
      return NS_OK;
    }

    /* attribute nativeSiteWindow siteWindow */
    NS_IMETHODIMP Container::GetSiteWindow(void * *aSiteWindow)
    {
      NS_ENSURE_ARG_POINTER(aSiteWindow);

      *aSiteWindow = 0;//mNativeWindow;
      return NS_OK;
    }

    NS_IMETHODIMP Container::OnProgressChange(nsIWebProgress *progress, nsIRequest *request,
                                                      PRInt32 curSelfProgress, PRInt32 maxSelfProgress,
                                                      PRInt32 curTotalProgress, PRInt32 maxTotalProgress)
    {
      //WebBrowserChromeUI::UpdateProgress(this, curTotalProgress, maxTotalProgress);
      loadProgress = (float)curTotalProgress/(float)maxTotalProgress;
      return NS_OK;
    }

    NS_IMETHODIMP Container::OnStateChange(nsIWebProgress *progress, nsIRequest *request,
                                                   PRUint32 progressStateFlags, nsresult status)
    {
      if ((progressStateFlags & STATE_START) && (progressStateFlags & STATE_IS_DOCUMENT))
      {
        //WebBrowserChromeUI::UpdateBusyState(this, PR_TRUE);
	if (loadState != loadLoading) loadProgress = 0.0f;
	loadState = loadLoading;
      }

      if ((progressStateFlags & STATE_STOP) && (progressStateFlags & STATE_IS_DOCUMENT))
      {
        /*WebBrowserChromeUI::UpdateBusyState(this, PR_FALSE);
        WebBrowserChromeUI::UpdateProgress(this, 0, 100);
        WebBrowserChromeUI::UpdateStatusBarText(this, nsnull);
        ContentFinishedLoading();*/
	loadState = loadFinished;
	loadProgress = 1.0f;
      }

      return NS_OK;
    }


    NS_IMETHODIMP Container::OnLocationChange(nsIWebProgress* aWebProgress,
                                                     nsIRequest* aRequest,
                                                     nsIURI *location)
    {
      /*PRBool isSubFrameLoad = PR_FALSE; // Is this a subframe load
      if (aWebProgress) {
        nsCOMPtr<nsIDOMWindow>  domWindow;
        nsCOMPtr<nsIDOMWindow>  topDomWindow;
        aWebProgress->GetDOMWindow(getter_AddRefs(domWindow));
        if (domWindow) { // Get root domWindow
          domWindow->GetTop(getter_AddRefs(topDomWindow));
        }
        if (domWindow != topDomWindow)
          isSubFrameLoad = PR_TRUE;
      }
      if (!isSubFrameLoad)
        WebBrowserChromeUI::UpdateCurrentURI(this);*/
      return NS_OK;
    }

    NS_IMETHODIMP 
    Container::OnStatusChange(nsIWebProgress* aWebProgress,
                                     nsIRequest* aRequest,
                                     nsresult aStatus,
                                     const PRUnichar* aMessage)
    {
      //WebBrowserChromeUI::UpdateStatusBarText(this, aMessage);
      return NS_OK;
    }



    NS_IMETHODIMP 
    Container::OnSecurityChange(nsIWebProgress *aWebProgress, 
                                        nsIRequest *aRequest, 
                                        PRUint32 state)
    {
      return NS_OK;
    }

  } // namespace Impl
} // namespace OSGK
