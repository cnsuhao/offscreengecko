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
#include "embedding.h"
#include "OffscreenWidget.h"
#include "pathutil.h"

#include GECKO_INCLUDE(dom,nsIScriptNameSpaceManager.h)
#include GECKO_INCLUDE(embed_base,nsEmbedAPI.h)
#include GECKO_INCLUDE(xpcom,nsCOMPtr.h)
#include GECKO_INCLUDE(xpcom,nsICategoryManager.h)
#include GECKO_INCLUDE(xpcom,nsIComponentManager.h)
#include GECKO_INCLUDE(xpcom,nsIComponentRegistrar.h)
#include GECKO_INCLUDE(xpcom,nsIServiceManager.h)
#include GECKO_INCLUDE(xpcom,nsServiceManagerUtils.h)
#include GECKO_INCLUDE(xpcom,nsXPCOMGlue.h)

namespace OSGK
{
  namespace Impl
  {
    void EmbeddingOptions::AddSearchPath (const char* path)
    {
      searchPaths.push_back (path);
    }

    void EmbeddingOptions::AddComponentsPath (const char* path)
    {
      GetDirectoryServiceOrCreate()->AddComponentsPath (path);
    }

    void EmbeddingOptions::SetProfileDir (const char* profileDir,
                                          const char* localProfileDir)
    {
      GetDirectoryServiceOrCreate()->SetProfileDirectory (profileDir);
      GetDirectoryServiceOrCreate()->SetLocalProfileDirectory (localProfileDir);
    }

    //-----------------------------------------------------------------------

// From nsXPCOMPrivate.h. Copied here for XPCOM_DLL
#if defined(XP_WIN) || defined(XP_OS2) || defined(WINCE)
#define XPCOM_DLL         "xpcom.dll"
#else // Unix
#define MOZ_DLL_SUFFIX    ".so"
#define XPCOM_DLL "libxpcom"MOZ_DLL_SUFFIX
#endif

    Embedding::Embedding (EmbeddingOptions* opt, 
      OSGK_GeckoResult& result) : xpcom_init_level (0), focusedBrowser (0),
        autoFocus (true)
    {
      new (&GetRefKeeper()) RefKeeper;

      nsresult xpcomState = NS_ERROR_NOT_AVAILABLE;
      std::string xpcomPath;
      if (opt != 0)
      {
        for (size_t i = 0; i < opt->searchPaths.size(); i++)
        {
          std::string path = opt->searchPaths[i];
          PathExpand (path);
          if ((path.length() > 0)
              && (!IsPathSeparator (path[path.length()-1])))
            path.push_back (FILE_PATH_SEPARATOR);
          path.append (XPCOM_DLL);
          xpcomState = XPCOMGlueStartup (path.c_str());
          if (NS_SUCCEEDED (xpcomState))
          {
            xpcomPath = path;
            break;
          }
        }
      }

      if (NS_FAILED (xpcomState))
      {
        static const GREVersionRange supportedGREVersions[] = {
          {"1.9", true, "2.0", false}
        };

        char grePath[PATH_MAX];
        nsresult res = GRE_GetGREPathWithProperties (supportedGREVersions,
          sizeof (supportedGREVersions) / sizeof (GREVersionRange),
          0, 0,
          grePath, sizeof (grePath));
        if (NS_SUCCEEDED (res))
        {
          xpcomState = XPCOMGlueStartup (grePath);
          if (NS_SUCCEEDED (xpcomState)) xpcomPath = grePath;
        }
      }
      if (NS_FAILED (xpcomState))
      {
        result = xpcomState;
        return;
      }

      /* If the glue started up successfully, the xpcom/xul library was
         loaded. Thus we should be able to use libxul functions happily
         past this point.
       */

      xpcom_init_level++;

      const nsDynamicFunctionLoad xulFuncs[] = {
        {"XRE_InitEmbedding", (NSFuncPtr*)&XRE_InitEmbedding},
        {"XRE_TermEmbedding", (NSFuncPtr*)&XRE_TermEmbedding},
        {"XRE_NotifyProfile", (NSFuncPtr*)&XRE_NotifyProfile},
        {0, 0}
      };

      nsresult res;
      res = XPCOMGlueLoadXULFunctions (xulFuncs);
      if (NS_FAILED (res))
      {
        result = res;
        return;
      }

      nsCOMPtr<nsILocalFile> binPath;
      {
        NS_ConvertASCIItoUTF16 greBinDir (xpcomPath.c_str());
        PRInt32 slashPos = greBinDir.RFindChar (FILE_PATH_SEPARATOR);
        if (slashPos != -1) greBinDir.SetLength (slashPos);

        res = NS_NewLocalFile (greBinDir, false, getter_AddRefs(binPath));
        if (NS_FAILED (res))
        {
          result = res;
          return;
        }
      }

      if (opt && opt->GetDirectoryService())
        opt->GetDirectoryService()->CreateProfileDirectories ();

      res = XRE_InitEmbedding (binPath, 0, 
        opt ? opt->GetDirectoryService() : 0, 0, 0);
      if (NS_FAILED (res))
      {
        result = res;
        return;
      }

      xpcom_init_level++;

      XRE_NotifyProfile ();

      res = NS_GetServiceManager (
        getter_AddRefs (GetRefKeeper().nsServMgr));
      if (NS_FAILED (res))
      {
        result = res;
        return;
      }

      res = NS_GetComponentManager (
        getter_AddRefs (GetRefKeeper().nsCompMgr));
      if (NS_FAILED (res))
      {
        result = res;
        return;
      }

      res = NS_GetComponentRegistrar (
        getter_AddRefs (GetRefKeeper().nsCompReg));
      if (NS_FAILED (res))
      {
        result = res;
        return;
      }

      res = components.Register (GetRefKeeper().nsCompReg);
      if (NS_FAILED (res))
      {
        result = res;
        return;
      }

      result = NS_OK;
    }

    Embedding::~Embedding()
    {
      GetRefKeeper().~RefKeeper();

      if (xpcom_init_level >= 2)
        XRE_TermEmbedding ();

      if (xpcom_init_level >= 1)
        XPCOMGlueShutdown ();
    }

    GeckoMem* Embedding::GetGeckoMem ()
    {
      if (!GetRefKeeper().geckoMem)
        GetRefKeeper().geckoMem.AttachNew (new GeckoMem);
      return GetRefKeeper().geckoMem;
    }

    ComponentMgr* Embedding::GetComponentMgr ()
    {
      if (!GetRefKeeper().compMgr)
        GetRefKeeper().compMgr.AttachNew (new ComponentMgr);
      return GetRefKeeper().compMgr;
    }

    nsIComponentManager* Embedding::GetGeckoComponentManager ()
    {
      return GetRefKeeper().nsCompMgr;
    }

    nsIComponentRegistrar* Embedding::GetGeckoComponentRegistrar ()
    {
      return GetRefKeeper().nsCompReg;
    }

    nsIServiceManager* Embedding::GetGeckoServiceManager ()
    {
      return GetRefKeeper().nsServMgr;
    }

    nsresult Embedding::RegisterJSGlobal (const char* name, 
      const char* contractID, unsigned int flags, BaseString*& previous)
    {
      nsCOMPtr<nsICategoryManager> catman =
        do_GetService (NS_CATEGORYMANAGER_CONTRACTID);
      if (!catman)
        return NS_ERROR_FAILURE;

      const char* category = (flags & jsgPrivileged) 
        ? JAVASCRIPT_GLOBAL_PRIVILEGED_PROPERTY_CATEGORY
        : JAVASCRIPT_GLOBAL_PROPERTY_CATEGORY;

      Str_WrapNSCStr* prev_ws = new Str_WrapNSCStr;
      nsresult rc = catman->AddCategoryEntry(category,
        name, contractID, PR_TRUE, PR_TRUE, 
        getter_Copies (prev_ws->GetNSCStr()));
      previous = prev_ws;

      return rc;
    }

    void Embedding::FocusBrowser (Browser* browser)
    {
      if (focusedBrowser == browser) return;

      if (focusedBrowser != 0)
        focusedBrowser->DoFocus (false, browser == 0);
      focusedBrowser = browser;
      if (focusedBrowser != 0)
        focusedBrowser->DoFocus (true, false);
    }

    void Embedding::DebugPrint (const wchar_t* format, ...)
    {
      va_list args;
      va_start (args, format);
      DebugPrintV (format, args);
      va_end (args);
    }

    void Embedding::DebugPrintV (const wchar_t* format, va_list args)
    {
      wprintf (L"OSGK: ");
      vwprintf (format, args);
      wprintf (L"\n");
    }

  } // namespace Impl
} // namespace OSGK

OSGK_EmbeddingOptions* osgk_embedding_options_create (void)
{
  return new OSGK::Impl::EmbeddingOptions ();
}

void osgk_embedding_options_add_search_path (OSGK_EmbeddingOptions* options, 
                                             const char* path)
{
  static_cast<OSGK::Impl::EmbeddingOptions*> (options)->AddSearchPath (path);
}

void osgk_embedding_options_add_components_path (OSGK_EmbeddingOptions* options, 
                                                 const char* path)
{
  static_cast<OSGK::Impl::EmbeddingOptions*> (options)->AddComponentsPath (path);
}

void osgk_embedding_options_set_profile_dir (OSGK_EmbeddingOptions* options, 
                                             const char* profileDir,
                                             const char* localProfileDir)
{
  static_cast<OSGK::Impl::EmbeddingOptions*> (options)->SetProfileDir (
    profileDir, localProfileDir);
}

OSGK_Embedding* osgk_embedding_create2 (unsigned int apiVer, 
                                        OSGK_EmbeddingOptions* options, 
                                        OSGK_GeckoResult* geckoResult)
{
  if (apiVer != OSGK_API_VERSION) return 0;

  OSGK_GeckoResult rc;
  OSGK::Impl::Embedding* e = new OSGK::Impl::Embedding (
    static_cast<OSGK::Impl::EmbeddingOptions*> (options), rc);
  if (geckoResult) *geckoResult = rc;
  if (NS_FAILED(rc))
  {
    delete e;
    return 0;
  }
  return e;
}

OSGK_GeckoMem* osgk_embedding_get_gecko_mem (OSGK_Embedding* embedding)
{
  return static_cast<OSGK::Impl::Embedding*> (embedding)->GetGeckoMem ();
}

OSGK_ComponentMgr* osgk_embedding_get_component_mgr (OSGK_Embedding* embedding)
{
  return static_cast<OSGK::Impl::Embedding*> (embedding)->GetComponentMgr ();
}

nsIComponentManager* osgk_embedding_get_gecko_component_manager (OSGK_Embedding* embedding)
{
  return static_cast<OSGK::Impl::Embedding*> (embedding)->GetGeckoComponentManager ();
}

nsIComponentRegistrar* osgk_embedding_get_gecko_component_registrar (OSGK_Embedding* embedding)
{
  return static_cast<OSGK::Impl::Embedding*> (embedding)->GetGeckoComponentRegistrar ();
}

nsIServiceManager* osgk_embedding_get_gecko_service_manager (OSGK_Embedding* embedding)
{
  return static_cast<OSGK::Impl::Embedding*> (embedding)->GetGeckoServiceManager ();
}

int osgk_embedding_register_js_global (OSGK_Embedding* embedding, 
  const char* name, const char* contractID, unsigned int flags, 
  OSGK_String** previousContract, OSGK_GeckoResult* geckoResult)
{
  OSGK::Impl::BaseString* prevStr = 0;
  nsresult rc = static_cast<OSGK::Impl::Embedding*> (embedding)->RegisterJSGlobal (
    name, contractID, flags, prevStr);
  if (previousContract != 0)
    *previousContract = prevStr;
  else
    delete prevStr;
  if (geckoResult != 0) *geckoResult = rc;
  return NS_SUCCEEDED(rc);
}

void osgk_embedding_clear_focus (OSGK_Embedding* embedding)
{
  static_cast<OSGK::Impl::Embedding*> (embedding)->FocusBrowser (0);
}

void osgk_embedding_set_auto_focus (OSGK_Embedding* embedding, int autoFocus)
{
  static_cast<OSGK::Impl::Embedding*> (embedding)->SetAutoFocus (autoFocus != 0);
}
  
int osgk_embedding_get_auto_focus (OSGK_Embedding* embedding)
{
  return static_cast<OSGK::Impl::Embedding*> (embedding)->GetAutoFocus ();
}
