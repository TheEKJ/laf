// LAF OS Library
// Copyright (C) 2018-2020  Igara Studio S.A.
// Copyright (C) 2012-2017  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_SYSTEM_H_INCLUDED
#define OS_SYSTEM_H_INCLUDED
#pragma once

#include "gfx/fwd.h"
#include "os/app_mode.h"
#include "os/capabilities.h"
#include "os/color_space.h"
#include "os/keys.h"
#include "os/scoped_handle.h"

#include <memory>
#include <stdexcept>

namespace os {

  class ColorSpaceConversion;
  class Display;
  class EventQueue;
  class Font;
  class FontManager;
  class Logger;
  class Menus;
  class NativeDialogs;
  class Surface;

  class DisplayCreationException : public std::runtime_error {
  public:
    DisplayCreationException(const char* msg) throw()
      : std::runtime_error(msg) { }
  };

  class System {
  protected:
    virtual ~System() { }
  public:
    virtual void dispose() = 0;

    // Windows-specific: The app name at the moment is used to receive
    // DDE messages (WM_DDE_INITIATE) and convert WM_DDE_EXECUTE
    // messages into Event::DropFiles. This allows to the user
    // double-click files in the File Explorer and open the file in a
    // running instance of your app.
    //
    // To receive DDE messages you have to configure the registry in
    // this way (HKCR=HKEY_CLASSES_ROOT):
    //
    //   HKCR\.appfile  (Default)="AppFile"
    //   HKCR\AppFile   (Default)="App File"
    //   HKCR\AppFile\shell\open\command             (Default)="C:\\...\\AppName.EXE"
    //   HKCR\AppFile\shell\open\ddeexec             (Default)="[open(\"%1\")]"
    //   HKCR\AppFile\shell\open\ddeexec\application (Default)="AppName"
    //   HKCR\AppFile\shell\open\ddeexec\topic       (Default)="system"
    //
    // The default value of "HKCR\AppFile\shell\open\ddeexec\application"
    // must match the "appName" given in this function.
    virtual void setAppName(const std::string& appName) = 0;

    // We can use this function to create an application that can run
    // in CLI and GUI mode depending on the given arguments, and in
    // this way avoid to showing the app in the macOS dock bar if we
    // are running in CLI only.
    virtual void setAppMode(AppMode appMode) = 0;

    // We might need to call this function when the app is launched
    // from Steam. It appears that there is a bug on OS X Steam client
    // where the app is launched, activated, and then the Steam client
    // is activated again.
    virtual void activateApp() = 0;

    virtual void finishLaunching() = 0;

    virtual Capabilities capabilities() const = 0;
    bool hasCapability(Capabilities c) const {
      return (int(capabilities()) & int(c)) == int(c);
    }

    // Disables loading wintab32.dll (sometimes a program can be
    // locked when we load the wintab32.dll, so we need a way to
    // opt-out loading this library.)
    virtual void useWintabAPI(bool enable) = 0;

    virtual Logger* logger() = 0;
    virtual Menus* menus() = 0;
    virtual NativeDialogs* nativeDialogs() = 0;
    virtual EventQueue* eventQueue() = 0;
    virtual bool gpuAcceleration() const = 0;
    virtual void setGpuAcceleration(bool state) = 0;
    virtual gfx::Size defaultNewDisplaySize() = 0;
    virtual Display* defaultDisplay() = 0;
    virtual Display* createDisplay(int width, int height, int scale) = 0;
    virtual Surface* createSurface(int width, int height, const os::ColorSpacePtr& colorSpace = nullptr) = 0;
    virtual Surface* createRgbaSurface(int width, int height, const os::ColorSpacePtr& colorSpace = nullptr) = 0;
    virtual Surface* loadSurface(const char* filename) = 0;
    virtual Surface* loadRgbaSurface(const char* filename) = 0;

    // New font manager
    virtual FontManager* fontManager() = 0;

    // Old font functions (to be removed)
    virtual Font* loadSpriteSheetFont(const char* filename, int scale = 1) = 0;
    virtual Font* loadTrueTypeFont(const char* filename, int height) = 0;

    // Returns true if the the given scancode key is pressed/actived.
    virtual bool isKeyPressed(KeyScancode scancode) = 0;

    // Returns the active pressed modifiers.
    virtual KeyModifiers keyModifiers() = 0;

    // Returns the latest unicode character that activated the given
    // scancode.
    virtual int getUnicodeFromScancode(KeyScancode scancode) = 0;

    // Indicates if you want to use dead keys or not. By default it's
    // false, which behaves as regular shortcuts. You should set this
    // to true when you're inside a text field in your app.
    virtual void setTranslateDeadKeys(bool state) = 0;

    // Color management
    virtual void listColorSpaces(
      std::vector<os::ColorSpacePtr>& list) = 0;
    virtual os::ColorSpacePtr createColorSpace(
      const gfx::ColorSpacePtr& colorSpace) = 0;
    virtual std::unique_ptr<ColorSpaceConversion> convertBetweenColorSpace(
      const os::ColorSpacePtr& src,
      const os::ColorSpacePtr& dst) = 0;

    // Set a default color profile for all displays (nullptr to use
    // the active monitor color profile and change it dynamically when
    // the window changes to another monitor).
    virtual void setDisplaysColorSpace(const os::ColorSpacePtr& cs) = 0;
    virtual os::ColorSpacePtr displaysColorSpace() = 0;
  };

  typedef ScopedHandle<System> SystemHandle;

  SystemHandle create_system();
  System* instance();
  void set_instance(System* system);

} // namespace os

#endif
