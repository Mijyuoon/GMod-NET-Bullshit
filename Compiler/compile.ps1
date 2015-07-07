param([Parameter(Mandatory=$True)][string]$Makefile)

$Options = @{ LANG = ""; OUT = ""; SRC = ""; }
$ScriptPath = Get-Location;
$SysBinPath = $env:Path;
$SysLibPath = $env:Lib;
$SysIncPath = $env:Include;

# BEHOLD THE HARDCODED PATHS!
$IncPaths = (
    "D:\PROGRAMS\VS2012\VC\INCLUDE",
    "C:\Program Files (x86)\Windows Kits\8.0\include\um",
    "D:\DATA\SourceMod\public\src",
    (Join-Path $ScriptPath "int")
);
$LibPaths = (
    "D:\PROGRAMS\VS2012\VC\LIB",
    "D:\PROGRAMS\VS2012\VC\LIB",
    "D:\PROGRAMS\VS2012\VC\ATLMFC\LIB",
    "C:\Program Files (x86)\Windows Kits\8.0\lib\win8\um\x86",
    "D:\DATA\SourceMod\public\lib",
    "D:\DATA\SourceMod\glua13",
    (Join-Path $ScriptPath "int")
);
$BinPaths = (
    "D:\PROGRAMS\VS2012\Common7\IDE\CommonExtensions\Microsoft\TestWindow",
    "D:\PROGRAMS\VS2012\Common7\IDE",
    "D:\PROGRAMS\VS2012\VC\BIN",
    "D:\PROGRAMS\VS2012\Common7\Tools",
    "C:\Windows\Microsoft.NET\Framework\v4.0.30319",
    "C:\Program Files (x86)\Windows Kits\8.0\bin\x86",
    "C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Bin\"
);

function ExitScript {
    param([string]$Error="", $Line="N/A", $Text="");
    $env:Path = $SysBinPath;
    $env:Lib = $SysLibPath;
    $env:Include = $SysIncPath;
    Set-Location $ScriptPath;

    if($Error -ne "") {
        $message = "Error occurred(${Line}): $Error";
        if($Text -ne "") { $message += ": ``$Text``"; }
        throw $message;
    }
    
    Remove-Item (Join-Path $OutDir "*.exp");
    Remove-Item (Join-Path $OutDir "*.lib");
    Remove-Item "*.obj";
}

$lineNum = 1;
$lines = [IO.File]::ReadAllLines((Resolve-Path $Makefile));
$lines | % { ,@($lineNum++, $_.Trim()) } | % {
    $index, $text = $_;
    if($text -match "^([a-zA-Z0-9]+)(?:\s(.+))?$") {
        $optval = $Matches[2];
        switch($Matches[1]) {
            "LANG" {
                if($Options["LANG"] -ne "") {
                    ExitScript "Duplicate LANG statement" $index;
                } elseif($optval -notin "c#") {
                    ExitScript "Invalid parameter to LANG" $index $text;
                } else {
                    $Options["LANG"] = $optval;
                }
            }
            "OUT" {
                if($Options["OUT"] -ne "") {
                    ExitScript "Duplicate OUT statement" $index;
                } else {
                    $Options["OUT"] = $optval;
                }
            }
            "SRC" {
                if(!(Test-Path -PathType Container $optval)) {
                    ExitScript "Invalid path ``$optval``" $index;
                } else {
                    $Options["SRC"] = Resolve-Path $optval;
                }
            }
            "FILE" {
                Set-Location $Options["SRC"];
                if(!(Test-Path -PathType Leaf $optval)) {
                    ExitScript "Invalid path ``$optval``" $index;
                } else {
                    $path = Resolve-Path $optval;
                    if($path -in $Options["FILE"]) {
                        ExitScript "Duplicate file name ``$optval``" $index;
                    }else {
                        $Options["FILE"] += ,"`"$path`"";
                    }
                }
                Set-Location $ScriptPath;
            }
            "REF" { 
                if($optval -in $Options["REF"]) {
                    ExitScript "Duplicate reference ``$optval``" $index;
                }else {
                    $Options["REF"] += ,"`"$optval`"";
                }
            }
            "COPY" { 
                if($optval -in $Options["COPY"]) {
                    ExitScript "Duplicate copy target ``$optval``" $index;
                }else {
                    $Options["COPY"] += ,"$optval";
                }
            }
        }
    } elseif($text -notmatch "^(?://.*)?$") {
        ExitScript "Bad syntax" $index $text;
    }
}

if($Options["LANG"] -eq "") {
    ExitScript "No target language specified";
} elseif($Options["OUT"] -eq "") {
    ExitScript "No output file name specified";
}

[IO.Directory]::SetCurrentDirectory($ScriptPath);
$env:Path = "$([String]::Join(";", $BinPaths));$SysBinPath";
$env:Lib = "$([String]::Join(";", $LibPaths));$SysLibPath";
$env:Include = "$([String]::Join(";", $IncPaths));$SysIncPath";

$GmodLibrary = "$ScriptPath\int\GmodLibrary.cpp";
$OutPath = [IO.Path]::GetFullPath($Options["OUT"]);
$OutDir = ([IO.FileInfo]$OutPath).DirectoryName;
if(!(Test-Path -PathType Container $OutDir)) {
    New-Item -ItemType Directory -Path $OutDir | Out-Null
}

cl /nologo /clr /MD /c "$GmodLibrary"
if($LastExitCode -ne 0) {
    ExitScript "C++ compiler raised an error";
}

switch($Options["LANG"]) {
    "c#" {
        $paths = [String]::Join(" ", $Options["FILE"]);
        csc /nologo /t:module /addmodule:GmodLibrary.obj /out:CSharp.obj "$paths"
        if($LastExitCode -ne 0) {
            ExitScript "C# compiler raised an error";
        }

        $paths = "`"$([String]::Join(";", $LibPaths))`"";
        link /NOLOGO /LTCG /DLL /ASSEMBLYLINKRESOURCE:GmodLibrary.obj /ASSEMBLYMODULE:GmodLibrary.obj /OUT:"$OutPath" GmodLibrary.obj CSharp.obj
        if($LastExitCode -ne 0) {
            ExitScript "Linker raised an error";
        }
    }
}

$Options["COPY"] | % {
    if(!(Test-Path -PathType Container $_)) {
        New-Item -ItemType Directory -Path $_ | Out-Null
    }
    Copy-Item -Force $OutPath $_
}

ExitScript;
