-- SimpleUI build file
workspace "SimpleUI"
	configurations { "Debug", "Release" }

project "SimpleUI"
	kind "ConsoleApp"
	language "C"
	targetdir "bin/%{cfg.buildcfg}"
	links { "d3d11", "dxgi", "dxguid", "d3dcompiler" }

	files { "src/**.h", "src/**.c" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

