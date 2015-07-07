using System;
using System.Drawing;
using System.Runtime.InteropServices;

class Stuff {
    static Color MsgCol = Color.FromArgb(40, 255, 60);

    [GModOpen]
    public static void ModOpen(IntPtr L) {
        Lua.Push(L => {
            string msg = Lua.ToString(L, 1);
            GMod.ConColorMsg(MsgCol, msg);
        });
        Lua.SetGlobal("Bullshit");
    }
}