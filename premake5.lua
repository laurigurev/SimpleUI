-- SimpleUI build file
workspace "SimpleUI"
	architecture "x64"
	language "C++"
	toolset "msc"
	configurations { "Debug", "Release" }
	flags { "MultiProcessorCompile" }

project "SimpleUI"
	kind "ConsoleApp"
	targetdir "bin/%{cfg.buildcfg}"
	links { "d3d11", "dxgi", "dxguid", "d3dcompiler" }

	files { "src/**.h", "src/**.hpp", "src/**.cpp" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

