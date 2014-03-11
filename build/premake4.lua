solution "gengine"
   configurations { "Debug", "Release", "DebugEmscripten", "ReleaseEmscripten" }

   project "gengine"
      kind "ConsoleApp"
      language "C++"
      files { "../src/**.h", "../src/**.cpp" }

      includedirs { "../src" }

      links { "lua" }

      buildoptions { "-std=c++11" }
 
      configuration "Debug*"
         defines { "DEBUG" }
         flags { "Symbols" }
 
      configuration "Release*"
         defines { "NDEBUG" }
         flags { "Optimize" }

      configuration "*Emscripten"
         defines { "EMSCRIPTEN" }
         targetname "gengine.bc"
         postbuildcommands { "emcc gengine.bc -o gengine.html" }

      configuration "not *Emscripten"
         links { "SDL2" }
