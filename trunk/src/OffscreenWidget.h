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

#ifndef __OFFSCREENWIDGET_H__
#define __OFFSCREENWIDGET_H__

#include "browser.h"
#include "embedding.h"
#include "EventHelpers.h"
#include "nsBaseWidget.h"

// FF3b1 Gecko has hat, XULRunner1.9b2pre doesn't
class nsIMenuListener;

namespace OSGK
{
  namespace Impl
  {
    class OffscreenWidget : public nsBaseWidget, public NativeWidget
    {
    protected:
      bool visible : 1;
      bool enabled : 1;
    
      OffscreenWidget* parent;
      Browser* browser;
      nsRefPtr<gfxImageSurface> surface;
    
      virtual nsresult CommonCreate (nsNativeWidget aNativeParent,
	nsIWidget *aParent, const nsRect     &aRect, 
	EVENT_CALLBACK aHandleEventFunction, nsIDeviceContext *aContext,
	nsIAppShell *aAppShell = nsnull, nsIToolkit *aToolkit = nsnull,
	nsWidgetInitData *aInitData = nsnull);

      Embedding* GetEmbedding()
      {
        if (browser != 0)
          return browser->GetEmbedding();
        else
          return parent->GetEmbedding();
      }
      void DebugPrint (const wchar_t* format, ...)
      {
      #ifdef _DEBUG
        Embedding* e = GetEmbedding();
        if (!e) return;

        va_list args;
        va_start (args, format);
        e->DebugPrintV (format, args);
        va_end (args);
      #endif
      }

      static void SetModifierFlags (const EventHelpers::KeyState& kstate, 
        nsInputEvent& event);
      bool EventKey (const EventHelpers::KeyState& kstate, 
        unsigned int keyCode, PRUint32 msg, bool isChar, PRUint32 flags = 0);
    
      bool DispatchMouseEvent (nsMouseEvent& event);
      nsresult SetFocusedChild (OffscreenWidget* child)
      {
        if (browser != 0)
        {
          browser->SetFocusedWidget (child);
          NS_OK;
        }
        else if (parent != 0)
          return parent->SetFocusedChild (child);
        return NS_ERROR_FAILURE;
      }
      bool DispatchToChild (nsGUIEvent& event);
    public:
      OffscreenWidget() : visible (false), enabled (true), parent (0),
        browser (0) {}
    
      NS_IMETHOD Create(nsIWidget        *aParent,
			const nsRect     &aRect,
			EVENT_CALLBACK   aHandleEventFunction,
			nsIDeviceContext *aContext,
			nsIAppShell      *aAppShell = nsnull,
			nsIToolkit       *aToolkit = nsnull,
			nsWidgetInitData *aInitData = nsnull);
      NS_IMETHOD Create(nsNativeWidget aParent,
			const nsRect     &aRect,
			EVENT_CALLBACK   aHandleEventFunction,
			nsIDeviceContext *aContext,
			nsIAppShell      *aAppShell = nsnull,
			nsIToolkit       *aToolkit = nsnull,
			nsWidgetInitData *aInitData = nsnull);
    
      NS_IMETHOD              SetParent(nsIWidget* aNewParent)
      { 
	if (parent != 0) parent->RemoveChild (this);
        parent = static_cast<OffscreenWidget*> (aNewParent); 
	if (parent != 0) parent->AddChild (this);
	return NS_OK;
      }
      virtual nsIWidget*      GetParent(void)
      { return parent; }
    
      NS_IMETHOD Show(PRBool aState) { visible = aState != 0; return NS_OK; }
    
      NS_IMETHOD IsVisible(PRBool & aState) { aState = visible; return NS_OK; }
    
      NS_IMETHOD ConstrainPosition(PRBool aAllowSlop,
				   PRInt32 *aX,
				   PRInt32 *aY)
      {
	return NS_OK;
      }
    
      NS_IMETHOD Move(PRInt32 aX, PRInt32 aY)
      { return Resize (aX, aY, mBounds.width, mBounds.height, false); }
    
      NS_IMETHOD Resize(PRInt32 aWidth,
			PRInt32 aHeight,
			PRBool   aRepaint)
      {
	return Resize (mBounds.x, mBounds.y, aWidth, aHeight, aRepaint);
      }
    
      NS_IMETHOD Resize(PRInt32 aX,
			PRInt32 aY,
			PRInt32 aWidth,
			PRInt32 aHeight,
			PRBool   aRepaint)
      {
        if (browser == 0)
        {
          if ((aWidth != mBounds.width) || (aHeight != mBounds.height))
            surface = 0;
	  mBounds.SetRect (aX, aY, aWidth, aHeight);
	  if (aRepaint) Update ();
        }
	return NS_OK;
      }
    
      NS_IMETHOD Enable(PRBool aState) { enabled = aState != 0; return NS_OK; }
    
      NS_IMETHOD IsEnabled(PRBool *aState) { *aState = enabled; return NS_OK; }
    
      NS_IMETHOD SetFocus(PRBool aRaise = PR_FALSE)
      { 
        return SetFocusedChild (this);
      }
    
      NS_IMETHOD Invalidate(PRBool aIsSynchronous)
      { return Invalidate (mBounds, aIsSynchronous); }

      NS_IMETHOD Invalidate(const nsRect & aRect, PRBool aIsSynchronous)
      { 
        if (parent != 0)
        {
          parent->Invalidate (aRect, aIsSynchronous);
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
    
      NS_IMETHOD Update();

      NS_IMETHOD AddMenuListener(nsIMenuListener * aListener)
      { return NS_ERROR_NOT_IMPLEMENTED; }
    
      NS_IMETHOD SetColorMap(nsColorMap *aColorMap) { return NS_ERROR_NOT_IMPLEMENTED; }
    
      NS_IMETHOD Scroll(PRInt32 aDx, PRInt32 aDy, nsRect *aClipRect)
      { return NS_ERROR_NOT_IMPLEMENTED; }
    
      NS_IMETHOD ScrollWidgets(PRInt32 aDx, PRInt32 aDy) { return NS_ERROR_NOT_IMPLEMENTED; }
    
      NS_IMETHOD SetTitle(const nsAString& aTitle) { return NS_ERROR_NOT_IMPLEMENTED; }
    
      NS_IMETHOD SetMenuBar(nsIMenuBar * aMenuBar) { return NS_ERROR_NOT_IMPLEMENTED; }
    
      NS_IMETHOD ShowMenuBar(PRBool aShow) { return NS_ERROR_NOT_IMPLEMENTED; }
    
      NS_IMETHOD WidgetToScreen(const nsRect& aOldRect, nsRect& aNewRect)
      {
	aNewRect = aOldRect;
	return NS_ERROR_NOT_IMPLEMENTED;
      }
    
      NS_IMETHOD ScreenToWidget(const nsRect& aOldRect, nsRect& aNewRect)
      {
	aNewRect = aOldRect;
	return NS_ERROR_NOT_IMPLEMENTED;
      }
    
      NS_IMETHOD BeginResizingChildren(void) { return NS_ERROR_NOT_IMPLEMENTED; }
      
      NS_IMETHOD EndResizingChildren(void) { return NS_ERROR_NOT_IMPLEMENTED; }
    
      NS_IMETHOD GetPreferredSize(PRInt32& aWidth, PRInt32& aHeight)
      { aWidth = 100; aHeight = 100; return NS_ERROR_NOT_IMPLEMENTED; }
    
      NS_IMETHOD SetPreferredSize(PRInt32 aWidth, PRInt32 aHeight)
      { return NS_ERROR_NOT_IMPLEMENTED; }
    
      NS_IMETHOD DispatchEvent(nsGUIEvent* event, nsEventStatus & aStatus);
    
      NS_IMETHOD CaptureRollupEvents(nsIRollupListener * aListener, PRBool aDoCapture, PRBool aConsumeRollupEvent)
      { return NS_ERROR_NOT_IMPLEMENTED; }

      virtual void* GetNativeData(PRUint32 aDataType);
    
      gfxASurface *GetThebesSurface();

      virtual nsIWidget* GetWidget() { return this; }

      void EventMouseMove (const EventHelpers::KeyState& kstate, int x, int y);
      void EventMouseButton (const EventHelpers::KeyState& kstate, 
        int mouseX, int mouseY,
        OSGK_MouseButton button, OSGK_MouseButtonEventType eventType);

      bool EventKey (const EventHelpers::KeyState& kstate, 
        unsigned int keyCode, bool down, bool isChar);
      bool EventKeyPress (const EventHelpers::KeyState& kstate, 
        unsigned int key, bool isChar);
    };
    
  } //  namespace Impl
} // namespace OSGK


#endif // __OFFSCREENWIDGET_H__
