# Building the Project

There really isn't much to build but this is to complete the third-party libraries setup

## Setup

Clone the repository:

```
git clone https://github.com/khlorz/AoLibrary
cd AoLibrary
```

## Install Third-Party Libraries

Inside the `include/third_party/` folder, clone or download the following:

### Google Benchmark

```
cd [Your chosen folder]
git clone https://github.com/google/benchmark [Your chosen folder]
```

Copy and paste the `include/benchmark` contents inside `include/third-party/google_benchmark/include/`

The built static-libraries should be put in `include/third-party/google_benchmark/lib/`

Google benchmark is only needed for the developer that tests and benchmarks the library, otherwise, this library can be skipped

### Abseil

```
git clone https://github.com/abseil/abseil-cpp [Your chosen folder]
```

Copy and paste the contents of `absl` to `include/third-party/absl/`

### Boost

Download Boost from:
https://www.boost.org/

Copy and paste the contents of `boost` to `include/third-party/boost/`

**NOTE#1:** This library can be skipped by default unless you edit `aol/internal/config.h` to use boost library

**NOTE#2:** If configured to use boost, there is no need to build boost. Currently, the library uses header-only files

### mimalloc

```
git clone https://github.com/microsoft/mimalloc [Your chosen folder]
```

Copy and paste the contents of `include` (from mimalloc) to `include/third-party/mimalloc/include/`

The built static-libraries should be put in `include/third-party/mimalloc/lib/` to its respective build type of either `debug` or `release`


### tsl (Tessil libraries)

```
git clone https://github.com/Tessil/ordered-map [Your chosen folder]
```

Copy and paste the contents of `include/tsl` to `include/third-party/tsl/`


### type_safe

```
git clone https://github.com/foonathan/type_safe [Your chosen folder]
git clone https://github.com/foonathan/debug_assert [Your chosen folder]
```

Copy and paste the contents of `include/type_safe` to `include/third-party/type_safe/`

Copy and paste `debug_assert.hpp` to `include/third-party/type_safe/detail`

**NOTE:** To minimize additional library includes, you might have to modify `detail/assert.hpp` and `detail/force_inline.hpp` #include's of `debug_assert.hpp`


### unordered_dense

```
git clone https://github.com/martinus/unordered_dense [Your chosen folder]
```

Copy and paste the contents of `include/ankerl` to `include/third-party/unordered_dense/`
