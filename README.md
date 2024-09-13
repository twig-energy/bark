# twig-cpp-datadog-client

This library contains our European power product list.

## Building and installing

Create a copy of `CMakeUserPresets.template.json` called `CMakeUserPreset.json`. Fix the `VCPKG_ROOT` environment variable in the new preset file, to point to somewhere on your system where you have vcpkg installed. Now you can run with presets `dev` etc.

You might want too remove the `clang-tidy` preset inherit from `dev-common` while developing to **greatly** speed up compilation time.
