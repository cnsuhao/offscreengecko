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

#ifndef __SCRIPTOBJECTTEMPLATE_H__
#define __SCRIPTOBJECTTEMPLATE_H__

#include <string>

#include "OffscreenGecko/scriptobjecttemplate.h"

#include "baseobj_private.h"
#include "ref.h"
#include "refcounted.h"
#include "stl_hash_map"

#include GECKO_INCLUDE(caps,nsISecurityCheckedComponent.h)
#include GECKO_INCLUDE(xpcom,nsIGenericFactory.h)
#include GECKO_INCLUDE(xpcom,nsIVariant.h)
#include GECKO_INCLUDE(xpcom,nsXPCOM.h)
#include GECKO_INCLUDE(xpconnect,nsIXPCScriptable.h)

namespace OSGK
{
  namespace Impl
  {
    class Embedding;

    class ScriptObjectTemplate : public BaseObject<OSGK_ScriptObjectTemplate>
    {
    private:
      class BaseMember : public RefCounted
      {
      protected:
        void* memberTag;
      public:
        BaseMember (void* tag) : memberTag (tag) {}

        virtual bool ReadOnly() const = 0;
        virtual nsresult GetValue (ScriptObjectTemplate* templ, 
          void* objTag, nsIVariant** _retval) = 0;
        virtual nsresult SetValue (ScriptObjectTemplate* templ, 
          void* objTag, nsIVariant* val) = 0;
      };

      class MemberProp : public BaseMember
      {
        OSGK_GetPropertyFunc getter;
        OSGK_SetPropertyFunc setter;
      public:
        MemberProp (void* tag, OSGK_GetPropertyFunc getter,
          OSGK_SetPropertyFunc setter) : BaseMember (tag), getter (getter),
            setter (setter) {}

        bool ReadOnly() const { return setter == 0; }
        nsresult GetValue (ScriptObjectTemplate* templ,
          void* objTag, nsIVariant** _retval);
        nsresult SetValue (ScriptObjectTemplate* templ,
          void* objTag, nsIVariant* val);
      };

      class MemberFunc : public BaseMember
      {
        OSGK_FunctionCallFunc callFunc;
      public:
        MemberFunc (void* tag, OSGK_FunctionCallFunc callFunc) : 
          BaseMember (tag), callFunc (callFunc) {}

        bool ReadOnly() const { return true; }
        nsresult GetValue (ScriptObjectTemplate* templ, 
          void* objTag, nsIVariant** _retval);
        nsresult SetValue (ScriptObjectTemplate* templ, void* objTag, 
          nsIVariant* val)
        { return NS_ERROR_NOT_IMPLEMENTED; }
      };

      stdext::hash_map<std::string, Ref<BaseMember> > members;

      nsCID classID;
      std::string contractID;

      Embedding& embedding;
      OSGK_CreateObjFunc createFunc;
      OSGK_DestroyObjFunc destroyFunc;
      void* createParam;

      class ScriptObject : public nsIXPCScriptable
      {
        Ref<ScriptObjectTemplate> templ;
        void* objTag;
      public:
        ScriptObject (ScriptObjectTemplate* templ);
        ~ScriptObject ();

        NS_DECL_ISUPPORTS
        NS_DECL_NSIXPCSCRIPTABLE

        struct nsMemory
        {
          static void* _Clone (OSGK_GeckoMem* mem, const void* p, PRSize s)
          {
            return osgk_geckomem_clone (mem, p, s);
          }
        };
      };

      class ScriptObjectFactory : public nsIFactory
      {
        friend class ScriptObjectTemplate;
        ScriptObjectTemplate* templ;
        Ref<ScriptObjectTemplate> templRef;
      public:
        ScriptObjectFactory (ScriptObjectTemplate* templ) : templ (templ) {}

        NS_DECL_ISUPPORTS
        NS_DECL_NSIFACTORY
      };
      ScriptObjectFactory* factory;
    public:
      ScriptObjectTemplate (Embedding& embedding,
        OSGK_CreateObjFunc createFunc, 
        OSGK_DestroyObjFunc destroyFunc, void* createParam);
      ~ScriptObjectTemplate();

      bool AddProperty (const char* propName, void* propTag,
        OSGK_GetPropertyFunc getter, OSGK_SetPropertyFunc setter);
      bool AddFunction (const char* methodName, void* methTag,
        OSGK_FunctionCallFunc callFunc);

      bool Register (Embedding* embed, const char* name, unsigned int flags);
    };
  } // namespace Impl
} // namespace OSGK

#endif // __SCRIPTOBJECTTEMPLATE_H__
