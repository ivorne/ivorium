#include "ivorium.hpp"

#include <ivorium_graphics/ivorium_graphics.hpp>
#include <ivorium_systems/ivorium_systems.hpp>
#include <ivorium_core/ivorium_core.hpp>

#include <ivorium_config.hpp>

#include <FreeImage.h>

#if IV_GLPLATFORM_GLFW
#include <GLFW/glfw3.h>
#endif

namespace iv
{

static bool l_initialized;

#if IV_GLPLATFORM_GLFW
static void glfw_error_callback( int error, const char * description )
{
	startup_warning( SRC_INFO, description );
}
#endif

void ivorium_init()
{
	//  initializa only once
	if( l_initialized )
		return;
	l_initialized = true;
	
	// glfw initialization
#if IV_GLPLATFORM_GLFW
	if( !glfwInit() )
	{
		startup_warning( SRC_INFO, "Call to glfwInit() failed." );
		return;
	}
	
	glfwSetErrorCallback( &glfw_error_callback );
#endif
	
	// freeimage initialization
	FreeImage_Initialise();
	
	// StreamResourceProvider
	StreamResourceProvider::RegisterType< DataStream_Subprovider, DataStream >();
	StreamResourceProvider::RegisterType< StreamFont_Subprovider, Font, StreamFont >();
	StreamResourceProvider::RegisterType< XcfInfo_Subprovider, XcfInfo >();
	StreamResourceProvider::RegisterType< StreamTexture_Subprovider, Texture, StreamTexture >();
	
	// VirtualResourceProvider
	VirtualResourceProvider::RegisterType< FlatShader_Subprovider, FlatShader >();
	VirtualResourceProvider::RegisterType< SquareMesh_Subprovider, Mesh, SquareMesh >();
}

void ivorium_destroy()
{
    FreeImage_DeInitialise();
    glfwTerminate();
    
    StreamResourceProvider::UnregisterAllTypes();
    VirtualResourceProvider::UnregisterAllTypes();
}

}
