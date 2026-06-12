from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
from conan.tools.files import copy
import os


class SQLCppBridgeConan(ConanFile):
    name = "sqlcppbridge"
    version = "1.1.25"
    description = "SQL to C++ bridge written in modern C++"
    license = "BSD-3-Clause"
    url = "https://github.com/GVNG/SQLCPPBridgeFramework"
    homepage = "https://github.com/GVNG/SQLCPPBridgeFramework"
    topics = ("sqlite", "sql", "c++", "orm")
    package_type = "library"

    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
    }

    exports_sources = "CMakeLists.txt", "SQLBridge/*", "cmake/*", "LICENSE.md"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def requirements(self):
        # Android uses sqlite3multipleciphers via FetchContent (not on ConanCenter).
        # All other platforms get plain sqlite3 from ConanCenter.
        if self.settings.os != "Android":
            self.requires("sqlite3/3.50.4")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["BUILD_SHARED"] = self.options.shared
        # Let Conan's sqlite3 package satisfy the dependency on non-Android;
        # on Android keep FetchContent so we get sqlite3multipleciphers.
        tc.variables["FETCH_SQLITE"] = self.settings.os == "Android"
        tc.variables["SQLCPPBRIDGE_INSTALL"] = True
        tc.generate()
        # Emit find-package glue for Conan-provided sqlite3
        if self.settings.os != "Android":
            from conan.tools.cmake import CMakeDeps
            deps = CMakeDeps(self)
            deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        copy(self, "LICENSE.md",
             src=self.source_folder,
             dst=os.path.join(self.package_folder, "licenses"))
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "SQLCppBridge")
        self.cpp_info.set_property("cmake_target_name", "SQLCppBridge::SQLCppBridge")
        self.cpp_info.libs = ["SQLCppBridge"]
        if self.settings.os == "Android":
            self.cpp_info.libs.append("sqlite3mc_static")
        if self.settings.os in ("Linux", "FreeBSD"):
            self.cpp_info.system_libs = ["pthread", "dl", "m"]
        elif self.settings.os == "Macos":
            self.cpp_info.frameworks = ["Security"]
