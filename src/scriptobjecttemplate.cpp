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

#include "OffscreenGecko/scriptvariant.h"

#include "componentmgr.h"
#include "embedding.h"
#include "scriptobjecttemplate.h"
#include "scriptvariant.h"

#include GECKO_INCLUDE(dom,nsIScriptNameSpaceManager.h)
#include GECKO_INCLUDE(js,jsapi.h)
#include GECKO_INCLUDE(js,jsobj.h)
#include GECKO_INCLUDE(xpcom,nsComponentManagerUtils.h)
#include GECKO_INCLUDE(xpcom,nsICategoryManager.h)
#include GECKO_INCLUDE(xpcom,nsIUUIDGenerator.h)
#include GECKO_INCLUDE(xpcom,nsIServiceManager.h)
#include GECKO_INCLUDE(xpcom,nsServiceManagerUtils.h)

OSGK_ScriptObjectTemplate* osgk_sot_create (OSGK_Embedding* embedding,
  OSGK_CreateObjFunc createFunc, OSGK_DestroyObjFunc destroyFunc,
  void* createParam)
{
  return new OSGK::Impl::ScriptObjectTemplate (
    *(static_cast<OSGK::Impl::Embedding*> (embedding)),
    createFunc, destroyFunc, createParam);
}

int osgk_sot_add_property (OSGK_ScriptObjectTemplate* templ, const char* propName, 
  void* propTag, OSGK_GetPropertyFunc getter, OSGK_SetPropertyFunc setter)
{
  return static_cast<OSGK::Impl::ScriptObjectTemplate*> (templ)->AddProperty (
    propName, propTag, getter, setter);
}

int osgk_sot_add_function (OSGK_ScriptObjectTemplate* templ, 
  const char* methodName, void* methTag, OSGK_FunctionCallFunc callFunc)
{
  return static_cast<OSGK::Impl::ScriptObjectTemplate*> (templ)->AddFunction (
    methodName, methTag, callFunc);
}

//OSGK_EXTERN_C OSGK_API OSGK_ScriptVariant* osgk_sot_instantiate (
  //OSGK_ScriptObjectTemplate* templ, void** objTag);

int osgk_sot_register (OSGK_ScriptObjectTemplate* templ, 
  OSGK_Embedding* embedding, const char* name, unsigned int flags)
{
  return static_cast<OSGK::Impl::ScriptObjectTemplate*> (templ)->Register (
    static_cast<OSGK::Impl::Embedding*> (embedding), name, flags);
}

namespace OSGK
{
  namespace Impl
  {
    nsresult ScriptObjectTemplate::MemberProp::GetValue (ScriptObjectTemplate* templ,
      void* objTag, nsIVariant** _retval)
    {
      if (_retval == 0) return NS_ERROR_INVALID_POINTER;
      if (getter == 0) return NS_ERROR_NOT_IMPLEMENTED;
      OSGK::ScriptVariant var (getter (objTag, memberTag));
      if (var.GetObject() == 0) return NS_ERROR_FAILURE;
      return static_cast<ScriptVariant*> (var.GetObject())->ConvertToIVariant (_retval);
    }

    nsresult ScriptObjectTemplate::MemberProp::SetValue (ScriptObjectTemplate* templ,
      void* objTag, nsIVariant* val)
    {
      if (setter == 0) return NS_ERROR_NOT_IMPLEMENTED;
      Ref<ScriptVariant> var;
      var.AttachNew (new ScriptVariant (val));
      return setter (objTag, memberTag, (ScriptVariant*)var);
    }

    //-----------------------------------------------------------------------

    struct MyScriptVariant : public OSGK::ScriptVariant
    {
      MyScriptVariant() : OSGK::ScriptVariant ((OSGK_ScriptVariant*)0) {}

      MyScriptVariant& operator=(const OSGK::ScriptVariant& other)
      {
        OSGK::ScriptVariant::operator=(other);
        return *this;
      }
    };

    class MethodScriptObject : public nsIXPCScriptable
    {
      OSGK_GeckoMem* mem;
      OSGK_FunctionCallFunc callFunc;
      void* objTag;
      void* methTag;
    public:
      MethodScriptObject (OSGK_GeckoMem* mem,
        OSGK_FunctionCallFunc callFunc, void* objTag,
        void* methTag) : mem (mem), callFunc (callFunc), objTag (objTag), 
        methTag (methTag) {}

      NS_DECL_ISUPPORTS
      //NS_DECL_NSISECURITYCHECKEDCOMPONENT
      NS_DECL_NSIXPCSCRIPTABLE

      struct nsMemory
      {
        static void* _Clone (OSGK_GeckoMem* mem, const void* p, PRSize s)
        {
          return osgk_geckomem_clone (mem, p, s);
        }
      };
    };

    // The nsIXPCScriptable map declaration that will generate stubs for us...
    #define XPC_MAP_CLASSNAME           MethodScriptObject
    #define XPC_MAP_QUOTED_CLASSNAME   "OSGK::MethodScriptObject"
    #define                             XPC_MAP_WANT_CALL
    #define XPC_MAP_FLAGS               0
    // Hack around nsMemory use in xpc_map_end.h
    #define Clone(p, s)                 _Clone(mem, p, s) 
    #include GECKO_INCLUDE(xpconnect,xpc_map_end.h)        /* This will #undef the above */
    #undef Clone

    NS_IMPL_ISUPPORTS1(MethodScriptObject, nsIXPCScriptable)

    NS_IMETHODIMP MethodScriptObject::Call(nsIXPConnectWrappedNative *wrapper, 
                                           JSContext * cx, JSObject * obj, 
                                           PRUint32 argc, jsval * argv, jsval * vp, 
                                           PRBool *_retval)
    {
      MyScriptVariant* params = 0;
      if (argc > 0)
      {
        nsCOMPtr<nsIXPConnect> xpconnect (do_GetService("@mozilla.org/js/xpc/XPConnect;1"));
        params = new MyScriptVariant[argc];
        for (size_t i = 0; i < argc; i++)
        {
          nsCOMPtr<nsIVariant> value;
          nsresult rv = xpconnect->JSToVariant (cx, argv[i], getter_AddRefs(value));
          if (NS_FAILED(rv)) continue;
          params[i] = OSGK::ScriptVariant (new ScriptVariant (value));
        }
      }
      OSGK_ScriptVariant* callResult = 0;
      nsresult rc = callFunc (objTag, methTag, argc,
        reinterpret_cast<OSGK_ScriptVariant**> (params), &callResult);
      delete[] params;
      if (NS_SUCCEEDED(rc) && (callResult != 0))
      {
        nsCOMPtr<nsIVariant> value;
        nsresult rc2 = static_cast<ScriptVariant*> (callResult)->ConvertToIVariant (
          getter_AddRefs(value));
        if (NS_FAILED(rc2))
          rc = rc2;
        else
        {
          rc = NS_SUCCESS_I_DID_SOMETHING;
          void *mark;
          jsval *argv = JS_PushArguments(cx, &mark, "%iv", value.get());
          *vp = *argv;
          JS_PopArguments(cx, mark);
        }
      }
      if (callResult != 0) osgk_release (callResult);
      return rc;
    }

    nsresult ScriptObjectTemplate::MemberFunc::GetValue (ScriptObjectTemplate* templ,
      void* objTag, nsIVariant** _retval)
    {
      if (callFunc == 0) return NS_ERROR_NOT_IMPLEMENTED;

      nsresult rv = NS_OK;
      nsCOMPtr<nsIWritableVariant> v = do_CreateInstance("@mozilla.org/variant;1", &rv);
      NS_ENSURE_SUCCESS(rv, rv);
      nsCOMPtr<nsISupports> meth = new MethodScriptObject (templ->embedding.GetGeckoMem(),
        callFunc, objTag, memberTag);
      rv = v->SetAsISupports (meth);
      NS_ENSURE_SUCCESS(rv, rv);

      *_retval = v;
      NS_ADDREF(*_retval);
      return NS_OK;
    }

    //-----------------------------------------------------------------------

    // The nsIXPCScriptable map declaration that will generate stubs for us...
    #define XPC_MAP_CLASSNAME           ScriptObjectTemplate::ScriptObject
    #define XPC_MAP_QUOTED_CLASSNAME   "OSGK::ScriptObjectTemplate::ScriptObject"
    #define                             XPC_MAP_WANT_NEWRESOLVE
    #define                             XPC_MAP_WANT_GETPROPERTY
    #define                             XPC_MAP_WANT_SETPROPERTY
    #define XPC_MAP_FLAGS               nsIXPCScriptable::ALLOW_PROP_MODS_DURING_RESOLVE
    // Hack around nsMemory use in xpc_map_end.h
    #define Clone(p, s)                 _Clone(templ->embedding.GetGeckoMem(), p, s) 
    #include GECKO_INCLUDE(xpconnect,xpc_map_end.h)        /* This will #undef the above */
    #undef Clone

    NS_IMPL_ISUPPORTS1(ScriptObjectTemplate::ScriptObject, 
                       //nsISecurityCheckedComponent,
                       nsIXPCScriptable)

    ScriptObjectTemplate::ScriptObject::ScriptObject (
      ScriptObjectTemplate* templ) : templ (templ)
    {
      objTag = this;

      OSGK_ScriptObjectCreateParams createParm;
      createParm.createParam = templ->createParam;
      if (templ->createFunc != 0) templ->createFunc (&createParm, &objTag);
    }

    ScriptObjectTemplate::ScriptObject::~ScriptObject ()
    {
      if (templ->destroyFunc != 0) templ->destroyFunc (objTag);
    }

    NS_IMETHODIMP
    ScriptObjectTemplate::ScriptObject::NewResolve(nsIXPConnectWrappedNative *wrapper,
                                                   JSContext * cx, JSObject * obj,
                                                   jsval id, PRUint32 flags,
                                                   JSObject * *objp, PRBool *_retval)
    {
      nsresult rv = NS_OK;
      if (JSVAL_IS_STRING(id)) {
        /* If the member to resolve is one of the user-added properties
           define it with JS so it can be set as well */
        JSString *str = JSVAL_TO_STRING(id);
        const PRUnichar *name = reinterpret_cast<const PRUnichar *>
                                                (JS_GetStringChars(str));
        NS_ConvertUTF16toUTF8 name_utf8 (name);
        if (templ->members.count (name_utf8.get()) == 0) return rv;
        BaseMember* member = templ->members[name_utf8.get()];
        if (member == 0) return rv;

        uintN attrs = 0;
        if (member->ReadOnly())
          attrs = JSPROP_READONLY;

        *objp = obj;
        *_retval = OBJ_DEFINE_PROPERTY(cx, obj, id, JSVAL_VOID,
                                       nsnull, nsnull,
                                       JSPROP_ENUMERATE | JSPROP_PERMANENT | attrs,
                                       nsnull);
      }
      return rv;
    }

    NS_IMETHODIMP
    ScriptObjectTemplate::ScriptObject::GetProperty(nsIXPConnectWrappedNative * wrapper,
                                                    JSContext * cx, JSObject * obj,
                                                    jsval id, jsval * vp, PRBool * _retval)
    {
      nsresult rv = NS_OK;
      if (JSVAL_IS_STRING(id)) {
        JSString *str = JSVAL_TO_STRING(id);
        const PRUnichar *name = reinterpret_cast<const PRUnichar *>
                                                (JS_GetStringChars(str));
        NS_ConvertUTF16toUTF8 name_utf8 (name);
        if (templ->members.count (name_utf8.get()) == 0) return rv;
        BaseMember* member = templ->members[name_utf8.get()];
        if (member == 0) return rv;

        nsCOMPtr<nsIVariant> value;
        rv = member->GetValue (templ, objTag, getter_AddRefs(value));
        if (!value) return rv;
        if (NS_FAILED(rv)) return rv;

        rv = NS_SUCCESS_I_DID_SOMETHING;
        void *mark;
        jsval *argv = JS_PushArguments(cx, &mark, "%iv", value.get());
        *vp = *argv;
        JS_PopArguments(cx, mark);
      }
      return rv;
    }

    NS_IMETHODIMP
    ScriptObjectTemplate::ScriptObject::SetProperty(nsIXPConnectWrappedNative * wrapper,
                                                    JSContext * cx, JSObject * obj,
                                                    jsval id, jsval * vp, PRBool * _retval)
    {
      nsresult rv = NS_OK;
      if (JSVAL_IS_STRING(id)) {
        JSString *str = JSVAL_TO_STRING(id);
        const PRUnichar *name = reinterpret_cast<const PRUnichar *>
                                                (JS_GetStringChars(str));
        NS_ConvertUTF16toUTF8 name_utf8 (name);
        if (templ->members.count (name_utf8.get()) == 0) return rv;
        BaseMember* member = templ->members[name_utf8.get()];
        if (member == 0) return rv;

        nsCOMPtr<nsIVariant> value;
        if (vp == 0) return NS_ERROR_INVALID_POINTER;

        nsCOMPtr<nsIXPConnect> xpconnect (do_GetService("@mozilla.org/js/xpc/XPConnect;1"));
        rv = xpconnect->JSToVariant (cx, *vp, getter_AddRefs(value));
        if (!value) return rv;
        if (NS_FAILED(rv)) return rv;

        rv = member->SetValue (templ, objTag, value);
        if (NS_FAILED(rv)) return rv;
        rv = NS_SUCCESS_I_DID_SOMETHING;
      }
      return rv;
    }

    //-----------------------------------------------------------------------

    NS_IMPL_ISUPPORTS1(ScriptObjectTemplate::ScriptObjectFactory, nsIFactory)

    NS_IMETHODIMP ScriptObjectTemplate::ScriptObjectFactory::CreateInstance(
      nsISupports *aOuter, const nsIID & iid, void * *result)
    {
      if (aOuter != 0)
        return NS_ERROR_NO_AGGREGATION;

      ScriptObject* inst = new ScriptObject (this->templ);
      if (inst == 0) return NS_ERROR_OUT_OF_MEMORY;
      NS_ADDREF(inst);

      nsresult rv = inst->QueryInterface(iid, result);
      NS_RELEASE(inst);
      return rv;
    }

    NS_IMETHODIMP ScriptObjectTemplate::ScriptObjectFactory::LockFactory(PRBool lock)
    {
      return NS_OK;
    }

    //-----------------------------------------------------------------------

    ScriptObjectTemplate::ScriptObjectTemplate (Embedding& embedding,
      OSGK_CreateObjFunc createFunc, 
      OSGK_DestroyObjFunc destroyFunc, void* createParam) : 
      embedding (embedding), createFunc (createFunc), destroyFunc (destroyFunc),
      createParam (createParam) 
    {
      char buf[128];
      _snprintf (buf, sizeof (buf), "@offscreengecko/template-%x;1", this);
      contractID = buf;

      nsresult rv;
      nsCOMPtr<nsIUUIDGenerator> genUUID = do_CreateInstance("@mozilla.org/uuid-generator;1", &rv);
      genUUID->GenerateUUIDInPlace (&classID);

      ScriptObjectFactory* objFact = new ScriptObjectFactory (this);
      factory = objFact;
      embedding.GetComponentMgr ()->RegisterXPCOMFactory (classID, 
        "OSGK_ScriptObjectTemplate", contractID.c_str(), factory);
    }

    ScriptObjectTemplate::~ScriptObjectTemplate()
    {
      ComponentMgr* compmgr = embedding.GetComponentMgr ();
      if (compmgr != 0) // Can get 0 during shutdown
        compmgr->UnregisterXPCOMFactory (classID, factory);
    }

    bool ScriptObjectTemplate::AddProperty (const char* propName, void* propTag,
      OSGK_GetPropertyFunc getter, OSGK_SetPropertyFunc setter)
    {
      if (members.count (propName) > 0) return false;

      Ref<BaseMember> newMember;
      newMember.AttachNew (new MemberProp (propTag, getter, setter));
      members[propName] = newMember;
      return true;
    }

    bool ScriptObjectTemplate::AddFunction (const char* methodName, void* methTag,
      OSGK_FunctionCallFunc callFunc)
    {
      if (members.count (methodName) > 0) return false;

      Ref<BaseMember> newMember;
      newMember.AttachNew (new MemberFunc (methTag, callFunc));
      members[methodName] = newMember;
      return true;
    }

    bool ScriptObjectTemplate::Register (Embedding* embed, const char* name, unsigned int flags)
    {
      nsCOMPtr<nsICategoryManager> catman =
        do_GetService (NS_CATEGORYMANAGER_CONTRACTID);
      if (!catman)
        return false;

      const char* category = (flags & jsgPrivileged) 
        ? JAVASCRIPT_GLOBAL_PRIVILEGED_PROPERTY_CATEGORY
        : JAVASCRIPT_GLOBAL_PROPERTY_CATEGORY;

      nsresult rc = catman->AddCategoryEntry(category,
        name, contractID.c_str(), PR_TRUE, PR_TRUE, 0);

      /* Add backreference so the template stays alive as long as the factory is
         registered. Conversely, any unregistering of the global should clear
         templRef. */ 
      factory->templRef = this;

      return NS_SUCCEEDED(rc);
    }
  } // namespace Impl
} // namespace OSGK
