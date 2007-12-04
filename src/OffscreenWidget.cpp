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

#include "defs_private.h"

#include "wrap_gfxASurface.h"

#include GECKO_INCLUDE(gfx,nsGfxCIID.h)
#include GECKO_INCLUDE(gfx,nsIDeviceContext.h)
#include GECKO_INCLUDE(gfx,nsIRenderingContext.h)

#include GECKO_INCLUDE(thebes,gfxContext.h)

#include GECKO_INCLUDE(xpcom,nsComponentManagerUtils.h)

#include "identstrings.h"
#include "OffscreenWidget.h"

namespace OSGK
{
  namespace Impl
  {
    DECLARE_IDENTSTRINGS_BEGIN(IdEventMsg, PRUint32, "%u")
      IDENTSTRINGS_ID(NS_KEY_DOWN)
      IDENTSTRINGS_ID(NS_KEY_UP)
      IDENTSTRINGS_ID(NS_KEY_PRESS)
    DECLARE_IDENSTRINGS_END;
    static IdEventMsg idEventMsg;

    void OffscreenWidget::CreateRegion (nsCOMPtr<nsIRegion>& rgn)
    {
      static NS_DEFINE_CID(kRegionCID, NS_REGION_CID);

      rgn = do_CreateInstance (kRegionCID);
    }

    nsresult OffscreenWidget::CommonCreate (nsNativeWidget aNativeParent,
                                            nsIWidget *aParent, const nsRect &aRect, 
                                            EVENT_CALLBACK aHandleEventFunction, 
                                            nsIDeviceContext *aContext,
                                            nsIAppShell *aAppShell, 
                                            nsIToolkit *aToolkit,
                                            nsWidgetInitData *aInitData)
    {
      nsIWidget *baseParent = aInitData &&
                             (aInitData->mWindowType == eWindowType_dialog ||
                              aInitData->mWindowType == eWindowType_toplevel ||
                              aInitData->mWindowType == eWindowType_invisible) ?
                             nsnull : aParent;

      if (aNativeParent != 0)
      {
        NativeWidget* nv = reinterpret_cast<NativeWidget*> (aNativeParent);
        browser = nv->GetBrowser ();
        if (browser) browser->SetWidget (this);
        baseParent = nv->GetWidget();
      }

      //mIsTopWidgetWindow = (nsnull == baseParent);
      mBounds.width = aRect.width;
      mBounds.height = aRect.height;

      BaseCreate(baseParent, aRect, aHandleEventFunction, aContext,
                 aAppShell, aToolkit, aInitData);
      parent = static_cast<OffscreenWidget*> (baseParent);

      return NS_OK;
    }

    OffscreenWidget::OffscreenWidget() : visible (false), enabled (true), 
      parent (0), browser (0)
    {
      CreateRegion (dirtyRegion);
      dirtyRegion->Init ();
    }

    NS_IMETHODIMP OffscreenWidget::Create(nsIWidget        *aParent,
                                          const nsRect     &aRect,
                                          EVENT_CALLBACK   aHandleEventFunction,
                                          nsIDeviceContext *aContext,
                                          nsIAppShell      *aAppShell,
                                          nsIToolkit       *aToolkit,
                                          nsWidgetInitData *aInitData)
    {
      return CommonCreate (0, aParent, aRect, aHandleEventFunction,
        aContext, aAppShell, aToolkit, aInitData);
    }

    NS_IMETHODIMP OffscreenWidget::Create(nsNativeWidget   aParent,
                                          const nsRect     &aRect,
                                          EVENT_CALLBACK   aHandleEventFunction,
                                          nsIDeviceContext *aContext,
                                          nsIAppShell      *aAppShell,
                                          nsIToolkit       *aToolkit,
                                          nsWidgetInitData *aInitData)
    {
      return CommonCreate (aParent, 0, aRect, aHandleEventFunction,
        aContext, aAppShell, aToolkit, aInitData);
    }

    NS_IMETHODIMP OffscreenWidget::Validate()
    {
      return dirtyRegion->Init ();
    }

    NS_IMETHODIMP OffscreenWidget::InvalidateRegion(const nsIRegion *aRegion, PRBool aIsSynchronous)
    {
      nsCOMPtr<nsIRegion> newRegion;
      CreateRegion (newRegion);
      newRegion->SetTo (*aRegion);
      newRegion->Intersect (0, 0, mBounds.width, mBounds.height);
      if (newRegion->IsEmpty()) return NS_OK;
      dirtyRegion->Union (*newRegion);
      if (parent != 0)
      {
        newRegion->Offset (mBounds.x, mBounds.y);
        parent->InvalidateRegion (newRegion, aIsSynchronous);
      }
      else
      {
        if (aIsSynchronous)
	{
	  Update();
	}
	else
	{
          if (browser) browser->SetUpdateState (Browser::updNeedsUpdate);
	}
      }
      return NS_OK;
    }

    struct IntersectingRect : public nsRect
    {
      IntersectingRect (const nsRect& r) : nsRect (r) {}

      PRBool IntersectRect(const nsRect &aRect1, const nsRect &aRect2)
      {
        nscoord  xmost1 = aRect1.XMost();
        nscoord  ymost1 = aRect1.YMost();
        nscoord  xmost2 = aRect2.XMost();
        nscoord  ymost2 = aRect2.YMost();
        nscoord  temp;

        x = std::max(aRect1.x, aRect2.x);
        y = std::max(aRect1.y, aRect2.y);

        // Compute the destination width
        temp = std::min(xmost1, xmost2);
        if (temp <= x) {
          Empty();
          return PR_FALSE;
        }
        width = temp - x;

        // Compute the destination height
        temp = std::min(ymost1, ymost2);
        if (temp <= y) {
          Empty();
          return PR_FALSE;
        }
        height = temp - y;

        return PR_TRUE;
      }
    };

    NS_IMETHODIMP OffscreenWidget::Invalidate(const nsRect & aRect, PRBool aIsSynchronous)
    { 
      IntersectingRect newRect (aRect);
      newRect.MoveBy (mBounds.x, mBounds.y);
      if (!newRect.IntersectRect (newRect, mBounds)) return NS_OK;

      dirtyRegion->Union (newRect.x-mBounds.x, newRect.y-mBounds.y, 
        newRect.width, newRect.height);
      if (parent != 0)
      {
        parent->Invalidate (newRect, aIsSynchronous);
      }
      else
      {
        if (aIsSynchronous)
        {
	  Update();
	}
	else
	{
          if (browser) browser->SetUpdateState (Browser::updNeedsUpdate);
	}
      }
      return NS_OK;
    }

    NS_IMETHODIMP OffscreenWidget::Update()
    {
      PRBool result = true;
      nsEventStatus eventStatus = nsEventStatus_eIgnore;

      nsCOMPtr<nsIRenderingContext> rc;
      nsresult rv = mContext->CreateRenderingContextInstance (*getter_AddRefs(rc));
      if (NS_FAILED(rv)) {
        NS_WARNING("CreateRenderingContextInstance failed");
        return NS_ERROR_FAILURE;
      }

      nsRefPtr<gfxASurface> targetSurface (GetThebesSurface ());
      rv = rc->Init (mContext, targetSurface);
      if (NS_FAILED(rv)) {
        NS_WARNING("RC::Init failed");
        return NS_ERROR_FAILURE;
      }

      rc->SetClipRegion (*dirtyRegion, nsClipCombine_kReplace);

      gfxContext* thebesContext = reinterpret_cast<gfxContext*> (
        rc->GetNativeGraphicData (nsIRenderingContext::NATIVE_THEBES_CONTEXT));

      if (mEventCallback)
      {
        nsPaintEvent event(PR_TRUE, NS_PAINT, this);

        NS_ADDREF(event.widget);

        //event.region = nsnull;
        // @@@ Should use actual dirty area here some time.
        //event.rect = &mBounds;
        event.region = dirtyRegion;
        event.rect = 0;

        // We're rendering with translucency, we're going to be
        // rendering the whole window; make sure we clear it first
        thebesContext->SetOperator(gfxContext::OPERATOR_CLEAR);
        thebesContext->Paint();
        thebesContext->SetOperator(gfxContext::OPERATOR_OVER);

        event.renderingContext = rc;
        DispatchEvent (&event, eventStatus);
        result = eventStatus == nsEventStatus_eConsumeNoDefault;
        event.renderingContext = nsnull;

        //if (mIsTranslucent) {
          // Data from offscreen drawing surface was copied to memory bitmap of transparent
          // bitmap. Now it can be read from memory bitmap to apply alpha channel and after
          // that displayed on the screen.
          //UpdateTranslucentWindow();
        /*} else if (result) {
          // Only update if DispatchWindowEvent returned TRUE; otherwise, nothing handled
          // this, and we'll just end up painting with black.
          thebesContext->PopGroupToSource();
          thebesContext->SetOperator(gfxContext::OPERATOR_SOURCE);
          thebesContext->Paint();
        }*/
        
        NS_RELEASE(event.widget);
      }

      nsIWidget* child = mFirstChild;
      while (child != 0)
      {
        PRBool vis;
        if (NS_SUCCEEDED (child->IsVisible (vis)) && vis)
        {
          nsRect childBounds;
          child->GetBounds (childBounds);
          if (dirtyRegion->ContainsRect (childBounds.x, childBounds.y,
            childBounds.width, childBounds.height))
	  {
	    child->Update ();

            thebesContext->Translate (gfxPoint (childBounds.x, childBounds.y));
    	    thebesContext->DrawSurface (child->GetThebesSurface(), 
	      gfxSize (childBounds.width, childBounds.height));
	    thebesContext->Translate (gfxPoint (-childBounds.x, -childBounds.y));
	  }
        }

        child = child->GetNextSibling();
      }

      if (browser) browser->SetUpdateState (Browser::updDirty);
      dirtyRegion->Init ();

      return result ? NS_OK : NS_ERROR_FAILURE;
    }

    NS_IMETHODIMP OffscreenWidget::DispatchEvent(nsGUIEvent* event, nsEventStatus & aStatus)
    {
      aStatus = nsEventStatus_eIgnore;

      // skip processing of suppressed blur events
      /*if ((event->message == NS_DEACTIVATE || event->message == NS_LOSTFOCUS) &&
          BlurEventsSuppressed())
        return NS_OK;*/

      if (nsnull != mEventCallback) {
        aStatus = (*mEventCallback)(event);
      }

      // Dispatch to event listener if event was not consumed
      if ((aStatus != nsEventStatus_eIgnore) && (nsnull != mEventListener)) {
        aStatus = mEventListener->ProcessEvent(*event);
      }

      // the window can be destroyed during processing of seemingly innocuous events like, say,
      // mousedowns due to the magic of scripting. mousedowns will return nsEventStatus_eIgnore,
      // which causes problems with the deleted window. therefore:
      if (mOnDestroyCalled)
        aStatus = nsEventStatus_eConsumeNoDefault;
      return NS_OK;
    }

    void* OffscreenWidget::GetNativeData(PRUint32 aDataType)
    {
      switch (aDataType) {
        case NS_NATIVE_WIDGET:
        case NS_NATIVE_WINDOW:
          return static_cast<NativeWidget*> (this);
        case NS_NATIVE_GRAPHIC:
          return static_cast<NativeWidget*> (this);
        case NS_NATIVE_PLUGIN_PORT:
        case NS_NATIVE_COLORMAP:
        default:
          break;
      }

      return NULL;
    }

    gfxASurface *OffscreenWidget::GetThebesSurface()
    {
      if (browser) return browser->GetSurface ();
      if (surface == 0)
      {
        gfxIntSize size (mBounds.width, mBounds.height);
        surface = new gfxImageSurface (size,
          gfxASurface::ImageFormatARGB32);
      }
      return surface;
    }

    void OffscreenWidget::EventMouseMove (const EventHelpers::KeyState& kstate, 
                                          int x, int y)
    {
      nsMouseEvent event (true, NS_MOUSE_MOVE, this,
        nsMouseEvent::eReal);
      SetModifierFlags (kstate, event);
      event.refPoint.MoveTo (x, y);
      DispatchMouseEvent (event);
    }

    void OffscreenWidget::EventMouseButton (const EventHelpers::KeyState& kstate, 
                                            int mouseX, int mouseY,
                                            OSGK_MouseButton button, 
                                            OSGK_MouseButtonEventType eventType)
    {
      unsigned int clickCount = 0;
      PRUint32 msg;
      switch (eventType)
      {
        case meDown:        
          msg = NS_MOUSE_BUTTON_DOWN; 
          clickCount = 1;
          break;
        case meUp:          
          msg = NS_MOUSE_BUTTON_UP;   
          clickCount = 1;
          break;
        case meDoubleClick: 
          msg = NS_MOUSE_BUTTON_UP; 
          clickCount = 2;
          break;
        default: return;
      }
      PRInt16 btn;
      switch (button)
      {
        case mbLeft:   btn = nsMouseEvent::eLeftButton;   break;
        case mbRight:  btn = nsMouseEvent::eRightButton;  break;
        case mbMiddle: btn = nsMouseEvent::eMiddleButton; break;
        default: return;
      }
      nsMouseEvent event (true, msg, this, nsMouseEvent::eReal);
      SetModifierFlags (kstate, event);
      event.refPoint.MoveTo (mouseX, mouseY);
      event.button = btn;
      event.clickCount = clickCount;
      DispatchMouseEvent (event);
    }

    void OffscreenWidget::SetModifierFlags (const EventHelpers::KeyState& kstate,
                                            nsInputEvent& event)
    {
      event.isAlt     = kstate.Get (EventHelpers::KeyState::ksAlt);
      event.isControl = kstate.Get (EventHelpers::KeyState::ksCtrl);
      event.isMeta    = kstate.Get (EventHelpers::KeyState::ksMeta);
      event.isShift   = kstate.Get (EventHelpers::KeyState::ksShift);
    }

    bool OffscreenWidget::EventKey (const EventHelpers::KeyState& kstate, 
                                    unsigned int key, PRUint32 msg, bool isChar, 
                                    PRUint32 flags)
    {
      nsKeyEvent event (true, msg, this);
      event.flags |= flags;
      SetModifierFlags (kstate, event);
      if (isChar)
      {
        event.charCode = key;
        event.isChar = true;
      }
      else
      {
        event.keyCode = key;
      }
      bool b = DispatchToChild (event); // @@@ Hm, is that right?
      DebugPrint (L"Event %ws %ws %u flags %u => %d", 
        idEventMsg.IdentStr (msg), isChar ? L"char" : L"key", key, flags,
        int (b));
      return b;
    }

    bool OffscreenWidget::EventKey (const EventHelpers::KeyState& kstate, 
                                    unsigned int key, bool down, bool isChar)
    {
      bool ret = EventKey (kstate, key, 
        PRUint32 (down ? NS_KEY_DOWN : NS_KEY_UP), isChar);
      // Modelled after the behaviour of the Win32 widget code
      PRUint32 flags = ret ? NS_EVENT_FLAG_NO_DEFAULT : 0;
      if (EventKey (kstate, key, PRUint32 (NS_KEY_PRESS), isChar, flags))
        ret = true; // @@@ Correct? Win32 widget seems to ignore that.
      return ret;
    }

    bool OffscreenWidget::EventKeyPress (const EventHelpers::KeyState& kstate, 
                                         unsigned int key, bool isChar)
    {
      return EventKey (kstate, key, PRUint32 (NS_KEY_PRESS), isChar);
    }
    
    bool OffscreenWidget::DispatchMouseEvent (nsMouseEvent& event)
    {
      if ((event.refPoint.x < mBounds.width)
        && (event.refPoint.y < mBounds.height))
      {
        nsIWidget* child = mFirstChild;
        while (child != 0)
        {
          OffscreenWidget* osw = static_cast<OffscreenWidget*> (child);
          if ((event.refPoint.x >= osw->mBounds.x)
            && (event.refPoint.y >= osw->mBounds.y))
          {
            nsMouseEvent newEvent (event);
            newEvent.widget = child;
            newEvent.refPoint.x -= osw->mBounds.x;
            newEvent.refPoint.y -= osw->mBounds.y;
            if (osw->DispatchMouseEvent (newEvent))
              return true;
          }
          child = child->GetNextSibling();
        }
        nsEventStatus status;
        DispatchEvent (&event, status);
        return true;
      }
      return false;
    }

    bool OffscreenWidget::DispatchToChild (nsGUIEvent& event)
    {
      nsEventStatus status;
      DispatchEvent (&event, status);
      if (status == nsEventStatus_eIgnore)
      {
        nsIWidget* child = mFirstChild;
        while (child != 0)
        {
          OffscreenWidget* osw = static_cast<OffscreenWidget*> (child);
          event.widget = child;
          if (osw->DispatchToChild (event)) return true;
          child = child->GetNextSibling();
        }
        return false;
      }
      return true;
    }

  } //  namespace Impl
} // namespace OSGK

