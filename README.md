# RFID reader
[![Build](https://github.com/JSC-TechMinds/rfid-reader/actions/workflows/build.yml/badge.svg)](https://github.com/JSC-TechMinds/rfid-reader/actions/workflows/build.yml)
[![Documentation](https://github.com/JSC-TechMinds/rfid-reader/actions/workflows/doxygen.yml/badge.svg)](https://github.com/JSC-TechMinds/rfid-reader/actions/workflows/doxygen.yml)
[![License: Apache 2.0](https://img.shields.io/badge/license-Apache--2.0-green.svg)](https://github.com/JSC-TechMinds/rfid-reader/blob/main/LICENSE)
[![GitHub release](https://img.shields.io/github/release/JSC-TechMinds/rfid-reader.svg?maxAge=3600)](https://github.com/JSC-TechMinds/rfid-reader/releases)
[![JSC TechMinds](https://img.shields.io/badge/JSC-TechMinds-green.svg)](https://www.jsctm.cz/)
[![Donate](https://img.shields.io/badge/donate-PayPal-blueviolet.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=SESX9ABM7V8KA&source=url)

This is an Arduino library to support one specific RS485 RFID reader. There isn't much information about this model, so we include a complete datasheet and communication protocol specifications. You can find them [here](documentation).

![image](https://user-images.githubusercontent.com/4460766/219572463-b0cf4d4c-910d-45bd-9526-941c661914de.png)

## Documentation
- [Extended Doxygen Documentation](https://JSC-TechMinds.github.io/rfid-reader)
- [Examples](examples)

# Sync and async modes

This library supports synchronous (blocking) API and asynchronous (non-blocking) API. The main reason is that often synchronous API is easier to use and due to its simplicity, it's better while writing and debugging a test code. For production code we recommend using async API.

Switching to async mode is really easy! Just follow these steps:

- Install [TaskManagerIO](https://github.com/davetcc/TaskManagerIO) library. It's also listed as an (optional) dependency of this package.
- Modify your script accordingly:

```cpp
#include <TaskManagerIO.h>

// Enable async mode. If you're using PlatformIO, you can define this as a build flag.
#define USE_ASYNC_IO

void setup() {
    ...
}

void loop() {
    # Call taskManager regularly in a loop.
    taskManager.runLoop();
}
```

Besides these changes, the API is identical to the synchronous one. All the heavy work is handled by the libraries behind the scenes. You can also take a look at the [async example](/examples/AsyncDetectTag/AsyncDetectTag.ino).

## License

Copyright (c) JSC TechMinds. All rights reserved.

Licensed under the [Apache-2.0](LICENSE) license.

## Support

We hope our library helped to speed up your project development. You can support our effort to convert coffe and pizza into a code with a small donation. Any support is much appreciated.

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=SESX9ABM7V8KA&source=url)