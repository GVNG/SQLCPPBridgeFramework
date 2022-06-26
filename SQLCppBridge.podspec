Pod::Spec.new do |spec|

  # ―――  Spec Metadata  ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――― #
  #
  #  These will help people to find your library, and whilst it
  #  can feel like a chore to fill in it's definitely to your advantage. The
  #  summary should be tweet-length, and the description more in depth.
  #

  spec.name         = "SQLCppBridge"
  spec.version      = "1.0.18"
  spec.summary      = "Simplification of using SQL from C++ code"

  # This description is used to generate tags and improve search results.
  spec.description  = <<-DESC
  The purpose of this framework is to provide the most simplest way of mapping an
  arbitrary structure of C ++ / STL derived classes on a flat SQL-table. The
  framework also provides methods for reading and writing (synchronous / asynchronous),
delete and edit data both as whole containers and as individual elements of them.
Currently SQLite implementation for MacOSX and iOS is used for data storing but you
can work adapters further to be compatible with any SQL-like library. In addition,
all the work on forming SQL statements is made inside the library and does not require
intervention and any improvements on the outside.
                   DESC

  spec.homepage     = "https://github.com/GVNG/SQLCPPBridgeFramework"
  # spec.screenshots  = "www.example.com/screenshots_1.gif", "www.example.com/screenshots_2.gif"
  spec.license      = "BSD 3-Clause"
  spec.author             = { "Roman Makhnenko" => "gvng@dataart.com" }

  # ――― Platform Specifics ――――――――――――――――――――――――――――――――――――――――――――――――――――――― #
  #
  #  If this Pod runs only on iOS or OS X, then specify the platform and
  #  the deployment target. You can optionally include the target after the platform.
  #

  #  When using multiple platforms
  spec.ios.deployment_target = "12.0"
  spec.osx.deployment_target = "10.10"
  spec.watchos.deployment_target = "5.0"
  # spec.tvos.deployment_target = "9.0"

  # ――― Source Location ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――― #
  #
  #  Specify the location from where the source should be retrieved.
  #  Supports git, hg, bzr, svn and HTTP.
  #

  spec.source       = { :git => "https://github.com/GVNG/SQLCPPBridgeFramework.git", :tag => "v1.0.18" }

  # ――― Source Code ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― #
  #
  #  CocoaPods is smart about how it includes source code. For source files
  #  giving a folder will include any swift, h, m, mm, c & cpp files.
  #  For header files it will include any header in the folder.
  #  Not including the public_header_files will make all headers public.
  #

  spec.source_files  = "SQLBridge/**/*.{h,cpp}"
  #spec.exclude_files = "Classes/Exclude"

  spec.public_header_files = "SQLBridge/include/*.h"
  spec.xcconfig = {"CLANG_CXX_LANGUAGE_STANDARD" => "c++14", "CLANG_CXX_LIBRARY" => "libc++"}

  # ――― Project Linking ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――― #
  #
  #  Link your library with frameworks, or libraries. Libraries do not include
  #  the lib prefix of their name.
  #

  spec.library = "sqlite3"

end
