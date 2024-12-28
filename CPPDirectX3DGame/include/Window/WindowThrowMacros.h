#pragma once

#define O_EXCEPT( hr ) Window::HrException( __LINE__,__FILE__,(hr) )
#define O_LAST_EXCEPT() Window::HrException( __LINE__,__FILE__,GetLastError() )
#define O_NOGFX_EXCEPT() Window::NoGfxException( __LINE__,__FILE__ )