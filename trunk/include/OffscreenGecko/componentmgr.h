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

#ifndef __OFFSCREENGECKO_COMPONENTMGR_H__
#define __OFFSCREENGECKO_COMPONENTMGR_H__

/**\file
 * Gecko component manager wrapper
 */

#include "baseobj.h"

/**
 * Gecko component manager wrapper.
 * Provides access to commonly used methods from the Gecko component manager
 * and registrar in a somewhat convenient manner.
 */
OSGK_DERIVEDTYPE(OSGK_ComponentMgr);

struct nsID;
typedef struct nsID nsCID;
OSGK_CLASSTYPE_DEF nsIFactory;

/**
 * Register an XPCOM component factory with Gecko's component manager.
 * \param compMgr Component manager wrapper object.
 * \param aClass Class ID of the class produced by the factory.
 * \param className Name of the class produced by the factory.
 * \param contractID Contract ID for the class produced by the factory.
 * \param factory The factory.
 * \param geckoResult Optionally returns the Gecko result code for the
 *   registration.
 * \return Whether the registration was successful (non-null) or not (null).
 */
OSGK_EXTERN_C OSGK_API int osgk_componentmgr_register_xpcom_factory (
  OSGK_ComponentMgr* compMgr, const nsCID* aClass, const char* className,
  const char* contractID, OSGK_CLASSTYPE_REF nsIFactory* factory,
  OSGK_GeckoResult* geckoResult OSGK_DEFAULT_ARG(0));

#ifdef __cplusplus

namespace OSGK
{
  /**
   * Gecko component manager wrapper.
   * Provides access to commonly used methods from the Gecko component manager
   * and registrar in a somewhat convenient manner.
   */
  struct ComponentMgr : public CXXAPI::Wrap<OSGK_ComponentMgr>
  {
    explicit ComponentMgr (WrappedType* obj) : WrapperType (obj) {}
    ComponentMgr& operator=(const ComponentMgr& other)
    {
      WrapperType::operator=(other);
      return *this;
    }

    /**
     * Register an XPCOM component factory with Gecko's component manager.
     * \param aClass Class ID of the class produced by the factory.
     * \param className Name of the class produced by the factory.
     * \param contractID Contract ID for the class produced by the factory.
     * \param factory The factory.
     * \param geckoResult Returns the Gecko result code for the registration.
     * \return Whether the registration was successful (true) or not (false).
     */
    bool RegisterXPCOMFactory (const nsCID& aClass, 
                               const char* className,
                               const char* contractID, 
                               nsIFactory* factory,
                               OSGK_GeckoResult& geckoResult)
    {
      return osgk_componentmgr_register_xpcom_factory (GetObject(),
        &aClass, className, contractID, factory, &geckoResult) != 0;
    }
    /**
     * Register an XPCOM component factory with Gecko's component manager.
     * \param aClass Class ID of the class produced by the factory.
     * \param className Name of the class produced by the factory.
     * \param contractID Contract ID for the class produced by the factory.
     * \param factory The factory.
     * \return Whether the registration was successful (true) or not (false).
     */
    bool RegisterXPCOMFactory (const nsCID& aClass, 
                               const char* className,
                               const char* contractID, 
                               nsIFactory* factory)
    {
      return osgk_componentmgr_register_xpcom_factory (GetObject(),
        &aClass, className, contractID, factory) != 0;
    }
  };
  
} // namespace OSGK
#endif

#endif // __OFFSCREENGECKO_COMPONENTMGR_H__
