cmake_minimum_required( VERSION 3.10 )

#-------- project ---------------------------------------------------
project( ivorium )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED True )

#-------------- configure ---------------------------
if( CMAKE_SYSTEM_NAME MATCHES "Linux" )
	set( IV_ENABLE_VALGRIND True )
	set( IV_TEXTOUT_COUT True )
	set( IV_GLPLATFORM_GLFW True )
	set( IV_ENABLE_TRANSFORM_FEEDBACK True )
	set( IV_FSPLATFORM_XDG True )
	set( IV_CONFIG_FS_ENABLED True )
	
	set( SOLOUD_BACKEND_SDL2 OFF CACHE BOOL "" FORCE )
	set( SOLOUD_BACKEND_ALSA ON CACHE BOOL "" FORCE )
endif()

if( CMAKE_SYSTEM_NAME MATCHES "Windows" )
	set( IV_ENABLE_VALGRIND False )
	set( IV_TEXTOUT_COUT True )
	set( IV_GLPLATFORM_GLFW True )
	set( IV_FSPLATFORM_WIN True )
	set( IV_CONFIG_FS_ENABLED True )
endif()

if( CMAKE_SYSTEM_NAME MATCHES "Darwin" )
	set( IV_ENABLE_VALGRIND True )
	set( IV_TEXTOUT_COUT True )
	set( IV_GLPLATFORM_GLFW True )
	set( IV_FSPLATFORM_XDG True )
	set( IV_CONFIG_FS_ENABLED True )
endif()

if( CMAKE_SYSTEM_NAME MATCHES "Android" )
	set( IV_ENABLE_VALGRIND False )
	set( IV_TEXTOUT_COUT False )
	set( IV_ENABLE_ANDROID_JNI True )
	set( IV_GLPLATFORM_GLFM True )
	set( IV_FSPLATFORM_ANDROID True )
	set( IV_CONFIG_FS_ENABLED True )
	
	set( SOLOUD_BACKEND_SDL2 OFF CACHE BOOL "" FORCE )
	set( SOLOUD_BACKEND_OPENSLES ON CACHE BOOL "" FORCE )
endif()

if( CMAKE_SYSTEM_NAME MATCHES "Emscripten" )
	set( IV_ENABLE_VALGRIND False )
	set( IV_TEXTOUT_COUT True )
	set( IV_GLPLATFORM_GLFM True )
	set( IV_WEBGL_USED True )
	set( IV_FSPLATFORM_EMSCRIPTEN True )
	set( IV_CONFIG_FS_ENABLED False )
	
	set( SOLOUD_BACKEND_SDL2 ON CACHE BOOL "" FORCE )
endif()

#-------- info ---------------------
if( IV_ENABLE_ANDROID_JNI )
	message( STATUS "IV_ENABLE_ANDROID_JNI" )
endif()
if( IV_TEXTOUT_COUT )
	message( STATUS "IV_TEXTOUT_COUT" )
endif()
if( IV_ENABLE_VALGRIND )
	message( STATUS "IV_ENABLE_VALGRIND" )
endif()
if( IV_GLPLATFORM_GLFW )
	message( STATUS "IV_GLPLATFORM_GLFW" )
endif()
if( IV_GLPLATFORM_GLFM )
	message( STATUS "IV_GLPLATFORM_GLFM" )
endif()
if( IV_WEBGL_USED )
	message( STATUS "IV_WEBGL_USED" )
endif()
if( IV_ENABLE_TRANSFORM_FEEDBACK )
	message( STATUS "IV_ENABLE_TRANSFORM_FEEDBACK" )
endif()
if( IV_FSPLATFORM_XDG )
	message( STATUS "IV_FSPLATFORM_XDG" )
endif()
if( IV_FSPLATFORM_WIN )
	message( STATUS "IV_FSPLATFORM_WIN" )
endif()
if( IV_FSPLATFORM_ANDROID )
	message( STATUS "IV_FSPLATFORM_ANDROID" )
endif()
if( IV_FSPLATFORM_EMSCRIPTEN )
	message( STATUS "IV_FSPLATFORM_EMSCRIPTEN" )
endif()
if( IV_CONFIG_FS_ENABLED )
	message( STATUS "IV_CONFIG_FS_ENABLED" )
endif()

#-------- binary ---------------------------------------------------
file(
	GLOB_RECURSE cpp_SOURCES
	LIST_DIRECTORIES false
	CONFIGURE_DEPENDS
	"src/*.cpp"
)

add_library( ivorium "${cpp_SOURCES}" )
set_target_properties( ivorium PROPERTIES OUTPUT_NAME ivorium )
target_include_directories( ivorium PUBLIC "src" )
target_compile_options( ivorium PUBLIC "-Werror=undef" )

set_property( TARGET ivorium PROPERTY IVORIUM_RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources )

#-------- configure -------------------------------------------------
configure_file( "src/ivorium_config.hpp.in" "include/ivorium_config.hpp" )
target_include_directories( ivorium PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/include" )

#-------- libraries -------------------------------------------------
# glm
add_subdirectory( libs/glm EXCLUDE_FROM_ALL )
target_link_libraries( ivorium PUBLIC glm )

# utfcpp
add_subdirectory( libs/utfcpp EXCLUDE_FROM_ALL )
target_link_libraries( ivorium PUBLIC utf8cpp )

# filesystem
add_subdirectory( libs/filesystem EXCLUDE_FROM_ALL )
target_link_libraries( ivorium PUBLIC ghc_filesystem )

# FreeImage
add_subdirectory( libs/freeimage-cmake EXCLUDE_FROM_ALL )
target_link_libraries( ivorium PRIVATE FreeImage )
if( IV_GLPLATFORM_GLFM )
	target_compile_options( ivorium PUBLIC "-DFREEIMAGE_COLORORDER=FREEIMAGE_COLORORDER_RGB" )
endif()

# SoLoud
add_subdirectory( libs/soloud/contrib EXCLUDE_FROM_ALL )
target_link_libraries( ivorium PUBLIC soloud )
target_include_directories( ivorium PUBLIC "libs/soloud/include" )
if( CMAKE_SYSTEM_NAME MATCHES "Emscripten" )
	target_link_options( ivorium PUBLIC "SHELL:-s USE_SDL_MIXER=1" )
	target_link_options( ivorium PUBLIC "SHELL:-s USE_SDL=2" )
endif()

# windowing and OpenGL
if( IV_GLPLATFORM_GLFW )
	
	# OpenGL 3+
	if( POLICY CMP0072 )
		cmake_policy( SET CMP0072 NEW )
	endif()
	find_package( OpenGL REQUIRED )
	target_include_directories( ivorium PRIVATE "${OPENGL_INCLUDE_DIR}" )
	target_link_libraries( ivorium PUBLIC "${OPENGL_gl_LIBRARY}" )

	# glew - submodule
	add_subdirectory( "libs/glew-cmake" EXCLUDE_FROM_ALL )
	target_link_libraries( ivorium PUBLIC libglew_static )
	
	# glfw - submodule
	set( GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE )
	set( GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE )
	set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE )
	add_subdirectory( "libs/glfw" EXCLUDE_FROM_ALL )
	target_link_libraries( ivorium PUBLIC glfw )
	
endif()

if( IV_GLPLATFORM_GLFM )
	
	# GLES 3
	if( CMAKE_SYSTEM_NAME MATCHES "Android" )
		find_library( lib-GLESv3 GLESv3 )
		target_link_libraries( ivorium PRIVATE ${lib-GLESv3} )
	endif()
	
	# WebGL 2
	if( CMAKE_SYSTEM_NAME MATCHES "Emscripten" )
		target_link_options( ivorium PUBLIC "SHELL:-s USE_WEBGL2=1" )
	endif()
	
	# glfm
	add_subdirectory( "libs/glfm" )
	target_link_libraries( ivorium PUBLIC glfm )
	
endif()

#------------- documentation --------------------------------
add_subdirectory( "libs/doxyboot-cmake" )

set( DOXYBOOT_HTML_HEADER "doxyboot/header.html" )
set( DOXYBOOT_HTML_FOOTER "doxyboot/footer.html" )
doxyboot_add_docs( ivorium_doc ivorium "src" )
