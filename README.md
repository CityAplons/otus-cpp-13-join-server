# cmake-multiplatform-template
Tiny CMake template repository used for various C/CPP projects

> Based on the [course example](https://github.com/CityAplons/webinar-cicd/tree/main) and [some more advanced project](https://github.com/vdamewood/silikego)

### Motivation

The ability to create copy-pasted projects for the Advanced-CPP course and personal pet-repos.

### Building

```bash
mkdir -p build && cd build/
cmake -DCMAKE_BUILD_TYPE=Release ../
cmake --build .
```

You may add a `-DMAKE_PACKAGE=<OFF|ON>` to disable or enable packages of a project (it's enabled by default).
