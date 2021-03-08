# Dear ImGui - Config window from protobuf

Create a config window from a protobuf. The protobuf is modified with interaction with the GUI.

## Building

```
git clone git@github.com:buq2/imgui-config-window.git
cd imgui-config-window
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=11 -DBUILD_TEST_PACKAGE=On
cmake --build build --parallel 12
```

## Docker

Build:
```
docker build -t imgui-config-window .
```

Run:
```
# Get x11docker from: https://github.com/mviereck/x11docker
x11docker --gpu imgui-config-window test_package/bin/test_package 1
```
