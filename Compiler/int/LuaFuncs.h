// LuaFuncs.h - Lua function prototypes

struct lua_Debug {
  int __crap_1;
  const char *name;	/* (n) */
  const char *defn;	/* (n) 'g', 'l', 'f', 'm' */
  const char *type;	/* (S) 'L', 'C', 'm', 't' */
  const char *source;	/* (S) */
  int currentline;	/* (l) */
  int upvals;		/* (u) */
  int linedefined;	/* (S) */
  int lastlinedefined;	/* (S) */
  char short_src[60]; /* (S) */
  /* private part */
  int __crap_2;  /* active function */
};

typedef void* lua_State;
typedef const char* CStr;
typedef int (*lua_CFunc)(lua_State);

// Basic stack manipulation
extern "C" int lua_gettop(lua_State);
extern "C" void lua_settop(lua_State, int);
extern "C" void lua_pushvalue(lua_State, int);
extern "C" void lua_remove(lua_State, int);
extern "C" void lua_insert(lua_State, int);
extern "C" void lua_replace(lua_State, int);
extern "C" int lua_checkstack(lua_State, int);

// Access functions (stack -> C)
extern "C" int lua_isnumber(lua_State, int);
extern "C" int lua_isstring(lua_State, int);
extern "C" int lua_iscfunction(lua_State, int);
extern "C" int lua_isuserdata(lua_State, int);
extern "C" int lua_type(lua_State, int);
extern "C" CStr lua_typename(lua_State, int);

extern "C" int lua_equal(lua_State, int, int);
extern "C" int lua_rawequal(lua_State, int, int);
extern "C" int lua_lessthan(lua_State, int, int);

extern "C" double lua_tonumber(lua_State, int);
extern "C" int lua_tointeger(lua_State, int);
extern "C" int lua_toboolean(lua_State, int);
extern "C" CStr lua_tolstring(lua_State, int, size_t*);
extern "C" size_t lua_objlen(lua_State, int);
extern "C" lua_CFunc lua_tocfunction(lua_State, int);
extern "C" void* lua_touserdata(lua_State, int); //unused
extern "C" lua_State lua_tothread(lua_State, int); //unused
extern "C" const void* lua_topointer(lua_State, int);

// Push functions (C -> stack)
extern "C" void lua_pushnil(lua_State);
extern "C" void lua_pushnumber(lua_State, double);
extern "C" void lua_pushinteger(lua_State, int);
extern "C" void lua_pushstring(lua_State, CStr);
extern "C" void lua_pushlstring(lua_State, CStr, size_t);
extern "C" CStr lua_pushfstring(lua_State, CStr, ...);
extern "C" void lua_pushcclosure(lua_State, lua_CFunc, int);
extern "C" void lua_pushboolean(lua_State, int);
extern "C" void lua_pushlightuserdata(lua_State, void*); //unused
extern "C" int lua_pushthread(lua_State); //unused

// Getter functions (Lua -> stack)
extern "C" void lua_gettable(lua_State, int);
extern "C" void lua_getfield(lua_State, int, CStr);
extern "C" void lua_rawget(lua_State, int);
extern "C" void lua_rawgeti(lua_State, int, int);
extern "C" void lua_createtable(lua_State, int, int);
extern "C" void* lua_newuserdata(lua_State, size_t); //unused
extern "C" int lua_getmetatable(lua_State, int);
extern "C" void lua_getfenv(lua_State, int);

// Setter functions (stack -> Lua)
extern "C" void lua_settable(lua_State, int);
extern "C" void lua_setfield(lua_State, int, CStr);
extern "C" void lua_rawset(lua_State, int);
extern "C" void lua_rawseti(lua_State, int, int);
extern "C" int lua_setmetatable(lua_State, int);
extern "C" int lua_setfenv(lua_State, int);

// Load and call functions
extern "C" void lua_call(lua_State, int, int);
extern "C" int lua_pcall(lua_State, int, int, int);
extern "C" int lua_cpcall(lua_State, lua_CFunc, void*); //unused

// Miscellaneous functions
extern "C" int __cdecl lua_error(lua_State);
extern "C" int lua_next(lua_State, int);
extern "C" void lua_concat(lua_State, int);
extern "C" int lua_getstack(lua_State, int, lua_Debug*);
extern "C" int lua_getinfo(lua_State, CStr, lua_Debug*);

// Aux library functions
extern "C" int luaL_error(lua_State, CStr, ...);
extern "C" int luaL_argerror(lua_State, int, CStr);
extern "C" int luaL_typerror(lua_State, int, CStr);
extern "C" int luaL_checkinteger(lua_State, int);
extern "C" double luaL_checknumber(lua_State, int);
extern "C" CStr luaL_checklstring(lua_State, int, size_t*);
extern "C" void luaL_checkany(lua_State, int);
extern "C" void luaL_checktype(lua_State, int, int);
extern "C" int luaL_optinteger(lua_State, int, int);
extern "C" double luaL_optnumber(lua_State, int, double);

extern "C" int luaL_getmetafield(lua_State, int, CStr);
extern "C" int luaL_callmeta(lua_State, int, CStr);
extern "C" int luaL_newmetatable(lua_State, CStr);
extern "C" int luaL_newmetatable_type(lua_State, CStr, int);
