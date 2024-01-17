-- premake5.lua
workspace "VulkanRayTracer"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "VulkanRayTracer"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "VulkanRayTracer"