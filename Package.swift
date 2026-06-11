// swift-tools-version:5.9

import PackageDescription

let package = Package(
    name: "SQLCppBridge",
    platforms: [
        .iOS(.v17),
        .macOS(.v11),
        .watchOS(.v6)
    ],
    products: [
        .library(
            name: "SQLCppBridge",
            targets: ["SQLCppBridge"]
        )
    ],
    targets: [
        .target(
            name: "SQLCppBridge",
            path: "SQLBridge",
            sources: [
                "src"
            ],
            publicHeadersPath: "include",
            cxxSettings: [
                .headerSearchPath("include")
            ],
            linkerSettings: [
                .linkedLibrary("sqlite3")
            ]
        )
    ],
    cxxLanguageStandard: .cxx14
)
