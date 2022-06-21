workspace "Aves"
	architecture "x64"
    configurations { "Debug", "Release" }


project "Aves"
    location "Aves"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

    files 
    { 
        "%{prj.name}/src/**.hpp", 
        "%{prj.name}/src/**.cpp" 
    }

    includedirs
    {   
        "Aves/vendor/spdlog/include",
        "Aves/vendor/glm-master",
        "Aves/vendor/glad/include",
        "Aves/vendor/glfw_precompiled/include",
        "Aves/vendor/Klein/public",
        "Aves/vendor/json/include"
    }

    libdirs
    {
        "Aves/vendor/glfw_precompiled/lib-vc2022"
    }

    links 
    { 
        "glfw3.lib",
        "opengl32.lib"
    }

    defines
    {

    }


    filter { "configurations:Debug" }
        defines { "DEBUG" }
        symbols "On"

    filter { "configurations:Release" }
        defines { "NDEBUG" }
        optimize "On"