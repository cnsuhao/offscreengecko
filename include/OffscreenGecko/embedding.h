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

#ifndef __OFFSCREENGECKO_EMBEDDING_H__
#define __OFFSCREENGECKO_EMBEDDING_H__

/**\file
 * Embedding object
 */

#include "baseobj.h"
#include "componentmgr.h"
#include "geckomem.h"

/**
 * Embedding options object. Encapsulates parameters for configuring 
 * creation and initialization of the embedding environment.
 */
typedef OSGK_BaseObject OSGK_EmbeddingOptions;

/**
 * Create a new embedding options object. Encapsulates parameters for
 * configuring creation and initialization of the embedding environment.
 */
OSGK_EXTERN_C OSGK_API OSGK_EmbeddingOptions* osgk_embedding_options_create (void);

/**
 * Add a path to be searched for XULrunner upon initialization.
 * \param options The embedding options object.
 * \param path Search path to add.
 */
OSGK_EXTERN_C OSGK_API void osgk_embedding_options_add_search_path (
  OSGK_EmbeddingOptions* options, const char* path);

/**
 * Add a path with application-specific XPCOM components and type libraries.
 * \param options The embedding options object.
 * \param path Components path to add.
 */
OSGK_EXTERN_C OSGK_API void osgk_embedding_options_add_components_path (
  OSGK_EmbeddingOptions* options, const char* path);

#ifdef __cplusplus

namespace OSGK
{
  /**
   * Embedding options object. Encapsulates parameters for configuring 
   * creation and initialization of the embedding environment.
   */
  struct EmbeddingOptions : public CXXAPI::Wrap<OSGK_EmbeddingOptions>
  {
    /**
     * Create a new embedding options object. 
     */
    EmbeddingOptions() : WrapperType (osgk_embedding_options_create()) { }

    explicit EmbeddingOptions (WrappedType* obj) : WrapperType (obj) {}
    EmbeddingOptions& operator=(const EmbeddingOptions& other)
    {
      WrapperType::operator=(other);
      return *this;
    }

    /**
     * Add a path to be searched for XULrunner upon initialization.
     * \param path Search path to add.
     */
    void AddSearchPath (const char* path)
    {
      osgk_embedding_options_add_search_path (GetObject(), path);
    }

    /**
     * Add a path with application-specific XPCOM components and type libraries.
     * \param path Components path to add.
     */
    void AddComponentsPath (const char* path)
    {
      osgk_embedding_options_add_components_path (GetObject(), path);
    }
  };
  
} // namespace OSGK
#endif

/**
 * OffscreenGecko embedding object, the 'mother' of all OffscreenGecko
 * objects.
 */
typedef OSGK_BaseObject OSGK_Embedding;

/**
 * Create a new embedding object. An embedding is the 'mother' of all
 * OffscreenGecko objects.
 * \param geckoResult Returns the result code from Gecko. Can be 0.
 */
OSGK_EXTERN_C OSGK_API OSGK_Embedding* osgk_embedding_create (
  OSGK_GeckoResult* geckoResult OSGK_DEFAULT_ARG(0));

/**
 * Create a new embedding object. An embedding is the 'mother' of all
 * OffscreenGecko objects.
 * \param options A set of options for the embedding to be created.
 * \param geckoResult Returns the result code from Gecko. Can be 0.
 */
OSGK_EXTERN_C OSGK_API OSGK_Embedding* osgk_embedding_create_with_options (
  OSGK_EmbeddingOptions* options, OSGK_GeckoResult* geckoResult OSGK_DEFAULT_ARG(0));

/**
 * Get Gecko memory allocator object.
 * \param embedding Embedding object.
 * \return Pointer to Gecko memory allocator object.
 * \remarks The returned object has <b>no reference implicitly added</b>.
 */
OSGK_EXTERN_C OSGK_API OSGK_GeckoMem* osgk_embedding_get_gecko_mem (
  OSGK_Embedding* embedding);

/**
 * Get Gecko component manager wrapper.
 * \param embedding Embedding object.
 * \return Pointer to Gecko component manager wrapper.
 * \remarks The returned object has <b>no reference implicitly added</b>.
 */
OSGK_EXTERN_C OSGK_API OSGK_ComponentMgr* osgk_embedding_get_component_mgr (
  OSGK_Embedding* embedding);

OSGK_CLASSTYPE_DEF nsIComponentManager;
OSGK_CLASSTYPE_DEF nsIComponentRegistrar;
OSGK_CLASSTYPE_DEF nsIServiceManager;

/**
 * Get interface for actual Gecko component manager.
 * \param embedding Embedding object.
 * \return Interface of Gecko component manager.
 * \remarks The returned interface has <b>no reference implicitly added</b>.
 */
OSGK_EXTERN_C OSGK_API OSGK_CLASSTYPE_REF nsIComponentManager* 
osgk_embedding_get_gecko_component_manager (OSGK_Embedding* embedding);
/**
 * Get interface for actual Gecko component registrar.
 * \param embedding Embedding object.
 * \return Interface of Gecko component registrar.
 * \remarks The returned interface has <b>no reference implicitly added</b>.
 */
OSGK_EXTERN_C OSGK_API OSGK_CLASSTYPE_REF nsIComponentRegistrar* 
osgk_embedding_get_gecko_component_registrar (OSGK_Embedding* embedding);
/**
 * Get interface for actual Gecko service manager.
 * \param embedding Embedding object.
 * \return Interface of Gecko service manager.
 * \remarks The returned interface has <b>no reference implicitly added</b>.
 */
OSGK_EXTERN_C OSGK_API OSGK_CLASSTYPE_REF nsIServiceManager* 
osgk_embedding_get_gecko_service_manager (OSGK_Embedding* embedding);

#ifdef __cplusplus

namespace OSGK
{
  /**
   * OffscreenGecko embedding object, the 'mother' of all OffscreenGecko
   * objects.
   * \sa \ref BasicsCXX
   */
  struct Embedding : public CXXAPI::Wrap<OSGK_Embedding>
  {
    /**
     * Create a new embedding object.
     */
    Embedding() : WrapperType (osgk_embedding_create()) { }
    /**
     * Create a new embedding object.
     * \param result Returns the result code from Gecko. Can be 0.
     */
    Embedding (OSGK_GeckoResult& result) : 
      WrapperType (osgk_embedding_create (&result)) { }
    /**
     * Create a new embedding object.
     * \param opt A set of options for the embedding to be created.
     */
    Embedding(EmbeddingOptions& opt) : WrapperType (
      osgk_embedding_create_with_options (opt.GetObject())) { }
    /**
     * Create a new embedding object.
     * \param opt A set of options for the embedding to be created.
     * \param result Returns the result code from Gecko. Can be 0.
     */
    Embedding (EmbeddingOptions& opt, OSGK_GeckoResult& result) : 
      WrapperType (osgk_embedding_create_with_options (
        opt.GetObject(), &result)) { }

    explicit Embedding (WrappedType* obj) : WrapperType (obj) {}
    Embedding& operator=(const Embedding& other)
    {
      WrapperType::operator=(other);
      return *this;
    }

    /**
     * Get Gecko memory allocator object.
     */
    GeckoMem GetGeckoMem ()
    {
      OSGK_GeckoMem* geckoMem = 
        osgk_embedding_get_gecko_mem (GetObject ());
      /* A reference to geckoMem was *not* implicitly added. Add one so the 
         wrapper works right */
      osgk_addref (geckoMem);
      return GeckoMem (geckoMem);
    }
    
    /**
     * Get Gecko component manager wrapper.
     */
    ComponentMgr GetComponentMgr()
    {
      OSGK_ComponentMgr* compMgr = 
        osgk_embedding_get_component_mgr (GetObject ());
      /* A reference to compMgr was *not* implicitly added. Add one so the 
         wrapper works right */
      osgk_addref (compMgr);
      return ComponentMgr (compMgr);
    }
    
    /**
     * Get interface for actual Gecko component manager.
     * \return Interface of Gecko component manager.
     * \remarks The returned interface has <b>no reference implicitly added</b>.
     */
    nsIComponentManager* GetGeckoComponentManager ()
    {
      return osgk_embedding_get_gecko_component_manager (GetObject());
    }
    /**
     * Get interface for actual Gecko component registrar.
     * \return Interface of Gecko component registrar.
     * \remarks The returned interface has <b>no reference implicitly added</b>.
     */
    nsIComponentRegistrar*  GetGeckoComponentRegistrar ()
    {
      return osgk_embedding_get_gecko_component_registrar (GetObject ());
    }
    /**
     * Get interface for actual Gecko service manager.
     * \return Interface of Gecko service manager.
     * \remarks The returned interface has <b>no reference implicitly added</b>.
     */
    nsIServiceManager* GetGeckoServiceManager ()
    {
      return osgk_embedding_get_gecko_service_manager (GetObject ());
    }
  };
  
} // namespace OSGK
#endif

#endif // __OFFSCREENGECKO_EMBEDDING_H__
