# Simplified Chinese language pack

The UI loads Qt translation catalogues from both the normal installation
directory and each installed Waywallen plugin's `translations/` directory.
This lets language packs be installed and updated independently.

Regenerate the catalogue after changing user-visible QML strings:

```sh
/usr/lib/qt6/bin/lupdate ui/qml ui/src -no-obsolete -locations relative \
  -ts ui/i18n/waywallen_zh_CN.ts
python3 ui/i18n/fill_zh_CN.py
/usr/lib/qt6/bin/lrelease ui/i18n/waywallen_zh_CN.ts \
  -qm ui/i18n/waywallen_zh_CN.qm
```

Build the installable plugin archive with an existing CMake build directory:

```sh
cmake --build build --target waywallen-zh-cn-plugin
```

Waywallen uses the system locale by default. Set `WAYWALLEN_LOCALE=zh_CN` to
force Simplified Chinese on a system whose locale is not Chinese.
