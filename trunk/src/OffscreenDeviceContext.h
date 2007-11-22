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

#ifndef __OFFSCREENDEVICECONTEXT_H__
#define __OFFSCREENDEVICECONTEXT_H__

#include "defs_private.h"

#include GECKO_INCLUDE(gfx,nsIDeviceContext.h)
#include GECKO_INCLUDE(xpcom,nsComponentManagerUtils.h)

class OffscreenDeviceContext : public nsIDeviceContext
{
  nsCOMPtr<nsIDeviceContext> thebesDC;

  void UpdateUnits()
  {
    mAppUnitsPerDevPixel = thebesDC->AppUnitsPerDevPixel();
    mAppUnitsPerInch = thebesDC->AppUnitsPerInch();
    mAppUnitsPerDevNotScaledPixel = thebesDC->UnscaledAppUnitsPerDevPixel();
    mPixelScale = thebesDC->GetPixelScale();
  }
public:
  OffscreenDeviceContext()
  {
    thebesDC = do_CreateInstance ("@mozilla.org/gfx/devicecontext;1");
    UpdateUnits ();
  }

  NS_DECL_ISUPPORTS

  NS_IMETHOD  Init(nsNativeWidget aWidget)
  { 
    nsresult rv = thebesDC->Init (0); 
    UpdateUnits ();
    return rv;
  }
  NS_IMETHOD  InitForPrinting(nsIDeviceContextSpec* aDevSpec)
  { return thebesDC->InitForPrinting (aDevSpec); }
  NS_IMETHOD  CreateRenderingContext(nsIView *aView, nsIRenderingContext *&aContext)
  { return thebesDC->CreateRenderingContext (aView, aContext); }
  NS_IMETHOD  CreateRenderingContext(nsIWidget *aWidget, nsIRenderingContext *&aContext)
  { return thebesDC->CreateRenderingContext (aWidget, aContext); }
  NS_IMETHOD  CreateRenderingContext(nsIRenderingContext *&aContext)
  { return thebesDC->CreateRenderingContext (aContext); }
  NS_IMETHOD  CreateRenderingContextInstance(nsIRenderingContext *&aContext)
  { return thebesDC->CreateRenderingContextInstance (aContext); }
  NS_IMETHOD  SupportsNativeWidgets(PRBool &aSupportsWidgets)
  { 
    //return thebesDC->SupportsNativeWidgets (aSupportsWidgets); 
    aSupportsWidgets = PR_TRUE;
    return NS_OK;
  }
  NS_IMETHOD PrepareNativeWidget(nsIWidget* aWidget, void** aOut)
  { return NS_OK; }
  NS_IMETHOD  GetSystemFont(nsSystemFontID aID, nsFont *aFont) const
  { return thebesDC->GetSystemFont (aID, aFont); }
  NS_IMETHOD  GetMetricsFor(const nsFont& aFont, nsIAtom* aLangGroup,
                            nsIFontMetrics*& aMetrics)
  { return thebesDC->GetMetricsFor (aFont, aLangGroup, aMetrics); }
  NS_IMETHOD  GetMetricsFor(const nsFont& aFont, nsIFontMetrics*& aMetrics)
  { return thebesDC->GetMetricsFor (aFont, aMetrics); }
  NS_IMETHOD CheckFontExistence(const nsString& aFaceName)
  { return thebesDC->CheckFontExistence (aFaceName); }
  NS_IMETHOD FirstExistingFont(const nsFont& aFont, nsString& aFaceName)
  { return thebesDC->FirstExistingFont (aFont, aFaceName); }
  NS_IMETHOD GetLocalFontName(const nsString& aFaceName, nsString& aLocalName,
                              PRBool& aAliased)
  { return thebesDC->GetLocalFontName (aFaceName, aLocalName, aAliased); }
  NS_IMETHOD FontMetricsDeleted(const nsIFontMetrics* aFontMetrics)
  { return thebesDC->FontMetricsDeleted (aFontMetrics); }
  NS_IMETHOD FlushFontCache(void)
  { return thebesDC->FlushFontCache (); }
  NS_IMETHOD GetDepth(PRUint32& aDepth)
  { return thebesDC->GetDepth (aDepth); }
  NS_IMETHOD GetPaletteInfo(nsPaletteInfo& aPaletteInfo)
  { return thebesDC->GetPaletteInfo (aPaletteInfo); }
  NS_IMETHOD GetDeviceSurfaceDimensions(nscoord &aWidth, nscoord &aHeight)
  { return thebesDC->GetDeviceSurfaceDimensions (aWidth, aHeight); }
  NS_IMETHOD GetRect ( nsRect &aRect )
  { return thebesDC->GetRect (aRect); }
  NS_IMETHOD GetClientRect(nsRect &aRect)
  { return thebesDC->GetClientRect (aRect); }
  NS_IMETHOD PrepareDocument(PRUnichar * aTitle, 
                             PRUnichar*  aPrintToFileName)
  { return thebesDC->PrepareDocument (aTitle, aPrintToFileName); }
  NS_IMETHOD BeginDocument(PRUnichar*  aTitle, 
                           PRUnichar*  aPrintToFileName,
                           PRInt32     aStartPage, 
                           PRInt32     aEndPage)
  { 
    return thebesDC->BeginDocument (aTitle, aPrintToFileName,
      aStartPage, aEndPage); 
  }
  NS_IMETHOD EndDocument(void)
  { return thebesDC->EndDocument (); }
  NS_IMETHOD AbortDocument(void)
  { return thebesDC->AbortDocument (); }
  NS_IMETHOD BeginPage(void)
  { return thebesDC->BeginPage (); }
  NS_IMETHOD EndPage(void)
  { return thebesDC->EndPage (); }
  NS_IMETHOD ClearCachedSystemFonts()
  { return thebesDC->ClearCachedSystemFonts (); }
  virtual PRBool CheckDPIChange()
  { return thebesDC->CheckDPIChange (); }
  virtual PRBool SetPixelScale(float aScale)
  { 
    PRBool rv = thebesDC->SetPixelScale (aScale);
    UpdateUnits ();
    return rv;
  }
};

#endif // __OFFSCREENDEVICECONTEXT_H__
