#ifndef CIRCE_UTILS_WIN32_UTILS_H
#define CIRCE_UTILS_WIN32_UTILS_H

#if !defined(__GNUC__) && !defined(__MINGW32__)

#define WIN32CONSOLE()                                                         \
  /* Open a new console window */                                              \
  AllocConsole();                                                              \
  AttachConsole(GetCurrentProcessId());                                        \
  /*-- Associate std input/output with newly opened console window: */         \
  freopen("CONIN$", "r", stdin);                                               \
  freopen("CONOUT$", "w", stdout);                                             \
  freopen("CONOUT$", "w", stderr);

#else
#define WIN32CONSOLE()                                                         \
  {}
#endif

#endif // CIRCE_UTILS_WIN32_UTILS_H
