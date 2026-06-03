#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>

typedef GLXContext (*PFNGLXCREATECONTEXTATTRIBSARBPROC)(
    Display *dpy, GLXFBConfig config, GLXContext share_context,
    Bool direct, const int *attrib_list);

#define GLX_CONTEXT_MAJOR_VERSION_ARB    0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB    0x2092
#define GLX_CONTEXT_PROFILE_MASK_ARB     0x9126
#define GLX_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001

Display *globalDisplay;
Window globalWindow;
GLXContext globalGlxContext;
Atom wmDeleteWindow;

bool initWindowLinux(void) {
	globalDisplay = XOpenDisplay(NULL);
	
	int visualAttribs[] = {
		GLX_X_RENDERABLE,  True,
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_RENDER_TYPE,   GLX_RGBA_BIT,
		GLX_DOUBLEBUFFER,  True,
		GLX_DEPTH_SIZE,    24,
		GLX_STENCIL_SIZE,  8,
		None
	};

	int count;
	GLXFBConfig *fbcfg = glXChooseFBConfig(
		globalDisplay, 
		DefaultScreen(globalDisplay), 
		visualAttribs, 
		&count
	);
	XVisualInfo *visualInfo = glXGetVisualFromFBConfig(globalDisplay, fbcfg[0]);

	XSetWindowAttributes windowAttribs = {
		.colormap = XCreateColormap(
			globalDisplay, 
			RootWindow(globalDisplay, visualInfo->screen),
			visualInfo->visual, AllocNone
		),
		.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask
	};
	globalWindow = XCreateWindow(
		globalDisplay, RootWindow(globalDisplay, visualInfo->screen),
		0, 0,
		globalEngineState.windowWidth,
		globalEngineState.windowHeight,
		0, visualInfo->depth,
		InputOutput,
		visualInfo->visual,
		CWColormap | CWEventMask,
		&windowAttribs
	);

	XMapWindow(globalDisplay, globalWindow);
	XStoreName(globalDisplay, globalWindow, "Matalan tason pelimoottori");

	wmDeleteWindow = XInternAtom(globalDisplay, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(globalDisplay, globalWindow, &wmDeleteWindow, 1);

	PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB =
        (PFNGLXCREATECONTEXTATTRIBSARBPROC)
        glXGetProcAddressARB((const GLubyte *)"glXCreateContextAttribsARB");
    if (!glXCreateContextAttribsARB) return false;

    int contextAttribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB,    4,
        GLX_CONTEXT_MINOR_VERSION_ARB,    6,
        GLX_CONTEXT_PROFILE_MASK_ARB,     GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };
    globalGlxContext = glXCreateContextAttribsARB(
        globalDisplay,
		fbcfg[0], 
		NULL, True,
		contextAttribs
	);

	glXMakeCurrent(globalDisplay, globalWindow, globalGlxContext);
	XFree(fbcfg);
	XFree(visualInfo);
	return true;
}