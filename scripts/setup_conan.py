#!/usr/bin/env python3
"""
One-time Conan setup for SQLCppBridge.

Detects the Android NDK and writes a resolved android-arm64 profile into
~/.conan2/profiles/ so it can be used by name without any hardcoded paths
in the project.

Usage:
    python scripts/setup_conan.py
"""

import os
import sys

PROFILES_DIR = os.path.expanduser("~/.conan2/profiles")
PROFILE_NAME = "android-arm64"

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.dirname(SCRIPT_DIR)
PROFILE_TEMPLATE = os.path.join(PROJECT_ROOT, "profiles", PROFILE_NAME)


def find_android_ndk():
    for var in ("ANDROID_NDK_ROOT", "ANDROID_NDK_HOME", "ANDROID_NDK"):
        path = os.environ.get(var)
        if path and os.path.isdir(path):
            return path

    sdk_root = (os.environ.get("ANDROID_HOME")
                or os.environ.get("ANDROID_SDK_ROOT")
                or os.path.expanduser("~/Library/Android/sdk"))
    ndk_base = os.path.join(sdk_root, "ndk")
    if os.path.isdir(ndk_base):
        versions = sorted(os.listdir(ndk_base), reverse=True)
        if versions:
            return os.path.join(ndk_base, versions[0])

    return None


def write_android_profile(ndk_path):
    with open(PROFILE_TEMPLATE) as f:
        base = f.read().rstrip()

    content = base + f"\n\n[conf]\ntools.android:ndk_path={ndk_path}\n"

    dest = os.path.join(PROFILES_DIR, PROFILE_NAME)
    if os.path.exists(dest):
        print(f"  Profile already exists: {dest}")
        print(f"  Skipping — edit it manually if the NDK path needs updating.")
        return dest, False

    os.makedirs(PROFILES_DIR, exist_ok=True)
    with open(dest, "w") as f:
        f.write(content)
    return dest, True


def main():
    print("Setting up Conan for SQLCppBridge...")
    print()

    ndk = find_android_ndk()
    if ndk:
        dest, written = write_android_profile(ndk)
        if written:
            print(f"  Android NDK:    {ndk}")
            print(f"  Profile written: {dest}")
        print()
    else:
        print("  WARNING: Android NDK not found.")
        print("  Install it via Android Studio or set ANDROID_NDK_ROOT, then re-run.")
        print("  Android profile was not generated.")
        print()

    print("Available build commands:")
    print("  macOS:         conan create . --build=missing")
    print("  iOS device:    conan create . -pr:h=profiles/ios -pr:b=default --build=missing")
    if ndk:
        print(f"  Android arm64: conan create . -pr:h={PROFILE_NAME} -pr:b=default --build=missing")
    print()


if __name__ == "__main__":
    main()
