// GmodLibrary.cpp - GLua to .NET bindings
#include <LuaFuncs.h>
#pragma comment(lib, "lua_shared.lib")
#pragma comment(lib, "tier0.lib")

#using "System.Drawing.dll"
using namespace System;
using System::Drawing::Color;
using namespace System::Reflection;
using namespace System::Runtime::InteropServices;

[AttributeUsage(AttributeTargets::Method)]
ref class GModOpenAttribute : Attribute {};

[AttributeUsage(AttributeTargets::Method)]
ref class GModCloseAttribute : Attribute {};

generic<typename T>
static void InvokeIfHasAttr(... array<Object^>^ args) {
	auto typeList = Assembly::GetCallingAssembly()->GetTypes();
	for each(Type^ type in typeList) {
		auto methods = type->GetMethods();
		for each(MethodInfo^ method in methods) {
			if(Attribute::IsDefined(method, T::typeid)) {
				method->Invoke(nullptr, args);
			}
		}
	}
}
static String^ CharPtrToGCString(CStr str, int len) {
	char* chrval = const_cast<char*>(str);
	IntPtr ptrval = static_cast<IntPtr>(chrval);
	return Marshal::PtrToStringAnsi(ptrval, len);
}

static String^ CharPtrToGCString(CStr str) {
	char* chrval = const_cast<char*>(str);
	IntPtr ptrval = static_cast<IntPtr>(chrval);
	return Marshal::PtrToStringAnsi(ptrval);
}

[UnmanagedFunctionPointer(CallingConvention::Cdecl)]
public delegate int LuaFunc(IntPtr state);

public enum class LuaType {
	None = -1,
	Nil,
	Bool,
	LightUserData,
	Number, 
	String, 
	Table,
	Function,
	UserData,
	Thread,
	
	Entity,
	Vector,
	Angle,
	PhysObj,
	Save,
	Restore,
	DmgInfo,
	EffectData,
	MoveData,
	RecipientFilter,
	UserCmd,
	SVehicle,
	Material,
	Panel,
	Particle,
	ParticleEmitter,
	Texture,
	UserMsg,
	ConVar,
	IMesh,
	Matrix,
	Sound,
	PixVisHandle,
	DLight,
	Video,
	File,
	Locomotion,
	PathFollower,
	NavArea,
	AudioChannel
};
public enum class LuaStatus {
	Normal = 0,
	RunError = 2,
	MemError = 4,
	ErrError = 5,
};

#define LUA_S ((lua_State)state)
public ref class Lua {
private:
	static const array<String^>^ _TypeName = gcnew array<String^> {
		"no value",
		"nil",
		"boolean",
		"UserData",
		"number",
		"string",
		"table",
		"function",
		"UserData",
		"thread",
		"Entity",
		"Vector",
		"Angle",
		"PhysObj",
		"ISave",
		"IRestore",
		"CTakeDamageInfo",
		"CEffectData",
		"CMoveData",
		"CRecipientFilter",
		"CUserCmd",
		"<ScVehicle>",
		"IMaterial",
		"Panel",
		"CLuaParticle",
		"CLuaEmitter",
		"ITexture",
		"bf_read",
		"ConVar",
		"IMesh",
		"VMatrix",
		"CSoundPatch",
		"pixelvis_handle_t",
		"dlight_t",
		"IVideoWriter",
		"File",
		"CLuaLocomotion",
		"PathFollower",
		"CNavArea",
		"IGModAudioChannel"
	};
public:
	static const int Env = -10001;
	static const int Reg = -10000;
	static const int Glob = -10002;

	static int GetTop(IntPtr state) {
		return lua_gettop(LUA_S);
	}
	static void SetTop(IntPtr state, int index) {
		lua_settop(LUA_S, index);
	}
	static void PushValue(IntPtr state, int index) {
		lua_pushvalue(LUA_S, index);
	}
	static void Remove(IntPtr state, int index) {
		lua_remove(LUA_S, index);
	}
	static void Insert(IntPtr state, int index) {
		lua_insert(LUA_S, index);
	}
	static void Replace(IntPtr state, int index) {
		lua_replace(LUA_S, index);
	}
	static bool CheckStack(IntPtr state, int extra) {
		return (lua_checkstack(LUA_S, extra) != 0);
	}
	static bool IsNumber(IntPtr state, int index) {
		return (lua_isnumber(LUA_S, index) != 0);
	}
	static bool IsString(IntPtr state, int index) {
		return (lua_isstring(LUA_S, index) != 0);
	}
	static bool IsUserData(IntPtr state, int index) {
		return (lua_isuserdata(LUA_S, index) != 0);
	}
	static bool IsType(IntPtr state, int index, LuaType type) {
		return (Type(state, index) == type);
	}
	static LuaType Type(IntPtr state, int index) {
		int vtype = lua_type(LUA_S, index);
		if(vtype == static_cast<int>(LuaType::UserData) 
		&& luaL_getmetafield(LUA_S, index, "MetaID")) {
			vtype = lua_tointeger(LUA_S, -1);
			lua_settop(LUA_S, -2);
		}
		return static_cast<LuaType>(vtype);
	}
	static String^ TypeName(IntPtr state, LuaType type) {
		int vtype = static_cast<int>(type) + 1;
		if(vtype < 0 || vtype > _TypeName->Length)
			return "<unknown>";
		return _TypeName[vtype];
	}
	static String^ TypeName(IntPtr state, int index) {
		int vtype = lua_type(LUA_S, index);
		if(vtype == static_cast<int>(LuaType::UserData)
		&& luaL_getmetafield(LUA_S, index, "MetaName")) {
			String^ tname = CharPtrToGCString(lua_tolstring(LUA_S, -1, 0));
			lua_settop(LUA_S, -2);
			return tname;
		}
		return CharPtrToGCString(lua_typename(LUA_S, vtype));
	}
	static bool Equal(IntPtr state, int idx1, int idx2) {
		return (lua_equal(LUA_S, idx1, idx2) != 0);
	}
	static bool RawEqual(IntPtr state, int idx1, int idx2) {
		return (lua_rawequal(LUA_S, idx1, idx2) != 0);
	}
	static bool LessThan(IntPtr state, int idx1, int idx2) {
		return (lua_lessthan(LUA_S, idx1, idx2) != 0);
	}
	static double ToNumber(IntPtr state, int index) {
		return lua_tonumber(LUA_S, index);
	}
	static int ToInteger(IntPtr state, int index) {
		return lua_tointeger(LUA_S, index);
	}
	static bool ToBoolean(IntPtr state, int index) {
		return (lua_toboolean(LUA_S, index) != 0);
	}
	static String^ ToString(IntPtr state, int index) {
		size_t outlen = 0;
		CStr lstr = lua_tolstring(LUA_S, index, &outlen);
		return CharPtrToGCString(lstr, outlen);
	}
	static LuaFunc^ ToCFunction(IntPtr state, int index) {
		lua_CFunc funcptr = lua_tocfunction(LUA_S, index);
		Delegate^ func = Marshal::GetDelegateForFunctionPointer(
			static_cast<IntPtr>(funcptr), LuaFunc::typeid
		);
		return static_cast<LuaFunc^>(func);
	}
	static IntPtr ToPointer(IntPtr state, int index) {
		return static_cast<IntPtr>(const_cast<void*>(lua_topointer(LUA_S, index)));
	}
	static void Push(IntPtr state) {
		lua_pushnil(LUA_S);
	}
	static void Push(IntPtr state, double value) {
		lua_pushnumber(LUA_S, value);
	}
	static void Push(IntPtr state, int value) {
		lua_pushinteger(LUA_S, value);
	}
	static void Push(IntPtr state, String^ value) {
		IntPtr lstr = Marshal::StringToHGlobalAnsi(value);
		lua_pushlstring(LUA_S, static_cast<CStr>(lstr.ToPointer()), value->Length);
		Marshal::FreeHGlobal(lstr);
	}
	static void Push(IntPtr state, LuaFunc^ value) {
		IntPtr funcptr = Marshal::GetFunctionPointerForDelegate(value);
		lua_pushcclosure(LUA_S, static_cast<lua_CFunc>(funcptr.ToPointer()), 0);
	}
	static void Push(IntPtr state, bool value) {
		lua_pushboolean(LUA_S, value ? 1 : 0);
	}
	static void Pop(IntPtr state, int count) {
		lua_settop(LUA_S, -count-1);
	}
	static void Concat(IntPtr state, int count) {
		lua_concat(LUA_S, count);
	}
	static void GetTable(IntPtr state, int index) {
		lua_gettable(LUA_S, index);
	}
	static void GetField(IntPtr state, int index, String^ field) {
		IntPtr lstr = Marshal::StringToHGlobalAnsi(field);
		lua_getfield(LUA_S, index, static_cast<CStr>(lstr.ToPointer()));
		Marshal::FreeHGlobal(lstr);
	}
	static void RawGet(IntPtr state, int index) {
		lua_rawget(LUA_S, index);
	}
	static void RawGetI(IntPtr state, int index, int num) {
		lua_rawgeti(LUA_S, index, num);
	}
	static void CreateTable(IntPtr state, int karr, int ktbl) {
		lua_createtable(LUA_S, karr, ktbl);
	}
	static bool GetMetaTable(IntPtr state, int index) {
		return (lua_getmetatable(LUA_S, index) != 0);
	}
	static void GetFEnv(IntPtr state, int index) {
		lua_getfenv(LUA_S, index);
	}
	static void SetTable(IntPtr state, int index) {
		lua_settable(LUA_S, index);
	}
	static void SetField(IntPtr state, int index, String^ field) {
		IntPtr lstr = Marshal::StringToHGlobalAnsi(field);
		lua_setfield(LUA_S, index, static_cast<CStr>(lstr.ToPointer()));
		Marshal::FreeHGlobal(lstr);
	}
	static void RawSet(IntPtr state, int index) {
		lua_rawset(LUA_S, index);
	}
	static void RawSetI(IntPtr state, int index, int num) {
		lua_rawseti(LUA_S, index, num);
	}
	static bool SetMetaTable(IntPtr state, int index) {
		return (lua_setmetatable(LUA_S, index) != 0);
	}
	static int SetFEnv(IntPtr state, int index) {
		return (lua_setfenv(LUA_S, index) != 0);
	}
	static void Call(IntPtr state, int argnum, int retnum) {
		lua_call(LUA_S, argnum, retnum);
	}
	static LuaStatus PCall(IntPtr state, int argnum, int retnum, int errfn) {
		return static_cast<LuaStatus>(lua_pcall(LUA_S, argnum, retnum, errfn));
	}
	static void Error(IntPtr state) {
		lua_error(LUA_S);
	}
	static void Error(IntPtr state, String^ fmt, ... array<Object^>^ args) {
		IntPtr lstr = Marshal::StringToHGlobalAnsi(String::Format(fmt, args));
		CStr fmsg = lua_pushfstring(LUA_S, "%s", static_cast<CStr>(lstr.ToPointer()));
		lua_settop(LUA_S, -2);
		Marshal::FreeHGlobal(lstr);
		luaL_error(LUA_S, "%s", fmsg);
	}
	static void ArgError(IntPtr state, int index, String^ msg) {
		IntPtr lstr = Marshal::StringToHGlobalAnsi(msg);
		CStr fmsg = lua_pushfstring(LUA_S, "%s", static_cast<CStr>(lstr.ToPointer()));
		lua_settop(LUA_S, -2);
		Marshal::FreeHGlobal(lstr);
		luaL_argerror(LUA_S, index, fmsg);
	}
	static void TypeError(IntPtr state, int index, String^ tname) {
		IntPtr lstr = Marshal::StringToHGlobalAnsi(tname);
		CStr fmsg = lua_pushfstring(LUA_S, "%s", static_cast<CStr>(lstr.ToPointer()));
		lua_settop(LUA_S, -2);
		Marshal::FreeHGlobal(lstr);
		luaL_typerror(LUA_S, index, fmsg);
	}
	static bool Next(IntPtr state, int index) {
		return (lua_next(LUA_S, index) != 0);
	}
	static bool GetMetaField(IntPtr state, int index, String^ field) {
		IntPtr lstr = Marshal::StringToHGlobalAnsi(field);
		int flag = luaL_getmetafield(LUA_S, index, static_cast<CStr>(lstr.ToPointer()));
		Marshal::FreeHGlobal(lstr);
		return (flag != 0);
	}
	static bool CallMetaField(IntPtr state, int index, String^ field) {
		IntPtr lstr = Marshal::StringToHGlobalAnsi(field);
		int flag = luaL_callmeta(LUA_S, index, static_cast<CStr>(lstr.ToPointer()));
		Marshal::FreeHGlobal(lstr);
		return (flag != 0);
	}
	static int CheckInteger(IntPtr state, int index) {
		return luaL_checkinteger(LUA_S, index);
	}
	static double CheckNumber(IntPtr state, int index) {
		return luaL_checknumber(LUA_S, index);
	}
	static String^ CheckString(IntPtr state, int index) {
		size_t outlen = 0;
		CStr lstr = luaL_checklstring(LUA_S, index, &outlen);
		return CharPtrToGCString(lstr, outlen);
	}
	static int CheckOption(IntPtr state, int index, array<String^>^ list) {
		String^ optval = CheckString(state, index);
		for(int i = 0; i < list->Length; i++)
			if(optval == list[i]) return i;
		ArgError(state, index, "invalid option '" + optval + "'");
		return 0;
	}
	static int CheckOption(IntPtr state, int index, array<String^>^ list, String^ defval) {
		String^ optval = OptString(state, index, defval);
		for(int i = 0; i < list->Length; i++)
			if(optval == list[i]) return i;
		ArgError(state, index, "invalid option '" + optval + "'");
		return 0;
	}
	static void CheckType(IntPtr state, int index, LuaType type) {
		if(Type(state, index) != type)
			TypeError(state, index, TypeName(state, type));
	}
	static void CheckAny(IntPtr state, int index) {
		luaL_checkany(LUA_S, index);
	}
	static int OptInteger(IntPtr state, int index, int defval) {
		return luaL_optinteger(LUA_S, index, defval);
	}
	static double OptNumber(IntPtr state, int index, double defval) {
		return luaL_optnumber(LUA_S, index, defval);
	}
	static String^ OptString(IntPtr state, int index, String^ defval) {
		if(lua_type(LUA_S, index) <= 0)
			return defval;
		return CheckString(state, index);
	}
	static bool NewMetaTable(IntPtr state, String^ tname) {
		IntPtr lstr = Marshal::StringToHGlobalAnsi(tname);
		int flag = luaL_newmetatable(LUA_S, static_cast<CStr>(lstr.ToPointer()));
		Marshal::FreeHGlobal(lstr);
		return (flag != 0);
	}
	static bool NewMetaTable(IntPtr state, String^ tname, int tnum) {
		IntPtr lstr = Marshal::StringToHGlobalAnsi(tname);
		int flag = luaL_newmetatable_type(LUA_S, static_cast<CStr>(lstr.ToPointer()), tnum);
		Marshal::FreeHGlobal(lstr);
		return (flag != 0);
	}
};
#undef LUA_S

struct {
	unsigned char r, g, b, a;
} __color_tmp;

public ref class GMod {
private:
	[DllImport("tier0", EntryPoint = "?ConColorMsg@@YAXABVColor@@PBDZZ")]
	static void _ConColorMsg(void* _1, void* _2, void* _3);
	[DllImport("tier0", EntryPoint = "?ConMsg@@YAXPBDZZ")]
	static void _ConMsg(void* _1, void* _2);
public:
	static void ConColorMsg(Color color, String^ msg) {
		IntPtr lstr = Marshal::StringToHGlobalAnsi(msg);
		__color_tmp.r = color.R;
		__color_tmp.g = color.G;
		__color_tmp.b = color.B;
		__color_tmp.a = 255;
		_ConColorMsg(&__color_tmp, "%s", lstr.ToPointer());
		Marshal::FreeHGlobal(lstr);
	}
	static void ConMsg(String^ msg) {
		IntPtr lstr = Marshal::StringToHGlobalAnsi(msg);
		_ConMsg("%s", lstr.ToPointer());
		Marshal::FreeHGlobal(lstr);
	}
};

extern "C" __declspec(dllexport)
int gmod13_open(lua_State L) {
	InvokeIfHasAttr<GModOpenAttribute^>(gcnew IntPtr(L));
	return 0;
}

extern "C" __declspec(dllexport)
int gmod13_close(lua_State L) {
	InvokeIfHasAttr<GModCloseAttribute^>(gcnew IntPtr(L));
	return 0;
}
