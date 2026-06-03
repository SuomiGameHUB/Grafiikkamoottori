#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
	bool isRunning;
	int32_t windowWidth;
	int32_t windowHeight;
} EngineState;

static EngineState globalEngineState = {
	.isRunning = false,
	.windowWidth = 1280,
	.windowHeight = 720
};

#if defined(_WIN32)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include "win32_platform.c"
#elif defined(__linux__)
    #include "linux_platform.c"
#endif

int main(int argc, char **argv) {
	(void)argc; (void)argv;
	bool success = false;
#if defined(_WIN32)
	success = initWindowWin();
#elif defined(__linux__)
	success = initWindowLinux();
#endif
	if (!success) return 1;
	globalEngineState.isRunning = true;

	while (globalEngineState.isRunning) {
#if defined(_WIN32)
		MSG msg;
		while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				globalEngineState.isRunning = false;
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
#elif defined(__linux__)
		while (XPending(globalDisplay)) {
			XEvent event;
			XNextEvent(globalDisplay, &event);
			if (event.type == ConfigureNotify) {
				globalEngineState.windowWidth  = event.xconfigure.width;
				globalEngineState.windowHeight = event.xconfigure.height;
			}
			if (event.type == ClientMessage) {
				if (event.xclient.data.l[0] == (long)wmDeleteWindow)
					globalEngineState.isRunning = false;
			}
		}
#endif
		glViewport(0, 0, globalEngineState.windowWidth, globalEngineState.windowHeight);
		glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

#if defined(_WIN32)
		SwapBuffers(wglGetCurrentDC());
#elif defined(__linux__)
		glXSwapBuffers(globalDisplay, globalWindow);
#endif
	}

	return 0;
}