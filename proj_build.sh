#!/bin/bash

cmake -B build_switch -DPLATFORM_SWITCH=ON
make -C build_switch nso-icon-tool.nro -j$(nproc)