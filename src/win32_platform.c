#include <windows.h>
#include <GL/gl.h>

typedef HGLRC (WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC)(
	HDC hDC, HGLRC hShareContext,
	const int *attribList);
	
#define WGL_CONTEXT_MAJOR_VERSION_ARB    0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB    0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB     0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001

LRESULT CALLBACK windowProcWin(
	HWND hWnd, UINT uMsg,
	WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_CLOSE:
		case WM_DESTROY: {
			globalEngineState.isRunning = false;
			PostQuitMessage(0);
			return 0;
		}
	}
	return DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

bool initWindowWin(void) {
	HINSTANCE hInst = GetModuleHandleA(NULL);
	WNDCLASSEXA windowClass = {
		.cbSize        = sizeof(WNDCLASSEXA),
		.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
		.lpfnWndProc   = windowProcWin,
		.hInstance     = hInst,
		.lpszClassName = "Pelimoottoriluokka"
	};
	if (!RegisterClassExA(&windowClass)) return false;

	HWND hWnd = CreateWindowExA(
		0, windowClass.lpszClassName, "Matalan tason pelimoottori",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		globalEngineState.windowWidth, globalEngineState.windowHeight,
		NULL, NULL, hInst, NULL
	);

	HDC hDC = GetDC(hWnd);
	PIXELFORMATDESCRIPTOR pfd = {
		.nSize        = sizeof(PIXELFORMATDESCRIPTOR),
		.nVersion     = 1,
		.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		.iPixelType   = PFD_TYPE_RGBA,
		.cColorBits   = 32,
        .cAlphaBits   = 8,
		.cDepthBits   = 24,
		.cStencilBits = 8
	};
	int pixelFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, pixelFormat, &pfd);

	HGLRC tempContext = wglCreateContext(hDC);
	wglMakeCurrent(hDC, tempContext);
	
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB =
		(PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(tempContext);
	
	int contextAttribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 6,
		WGL_CONTEXT_PROFILE_MASK_ARB,
		WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};
	HGLRC hRC = wglCreateContextAttribsARB(hDC, NULL, contextAttribs);
	wglMakeCurrent(hDC, hRC);
	return true;
}