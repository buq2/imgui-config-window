from conans import ConanFile, CMake, tools

class IMGUIConfigWindowConan(ConanFile):
    name = "imgui-config-window"
    version = "0.01"
    license = "MIT"
    url = "https://github.com/buq2/imgui-config-window"
    description = "https://github.com/buq2/imgui-config-window"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    generators = "cmake"
    topics = ("conan", "imgui", "gui", "graphical", "sdl2")
    requires = ["imgui/[>1.70]", "sdl2/[>2.0.0]@bincrafters/stable",
        "glew/[>2.0.0]@bincrafters/stable", "protobuf/[>3.0.0]@bincrafters/stable",
        "protoc_installer/[>3.0.0]@bincrafters/stable"]
        
    _cmake = None

    def configure(self):
        self.options["sdl2"].iconv = False
    
    def _configure_cmake(self):
        if self._cmake:
            return self._cmake
        self._cmake = CMake(self)
        self._cmake.configure(source_folder="imgui-config-window")
        return self._cmake

    def source(self):
        self.run("git clone git@github.com:buq2/imgui-config-window.git")

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        self.copy(pattern="LICENSE.txt", dst="licenses")
        self.copy("*.h", dst="include")
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
