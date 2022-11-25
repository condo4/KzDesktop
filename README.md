# KaZoe Desktop
This project provide a desktop environement base on "Qt Wayland Compositor".

It combine features:

- Wayland compositor
- Window manager
- customizable Task bar
- customizable systray
- homepage provided by an external application
- VNC server managed by a plugin


# Build and run
```sh
git clone https://kazoe.org/cgit/kazoe/kazoe-desktop.git
cd kazoe-desktop
mkdir build
cmake ..
make
./kzdesktop
```

DesktopManager use [standard XDG paths](https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html).
This paths can be override by setting [XDG_DATA_HOME and XDG_DATA_DIRS](https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html).


## Design





## Protocol
https://specifications.freedesktop.org/menu-spec/menu-spec-1.0.html#desktop-entry-extensions-examples
