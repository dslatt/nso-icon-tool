#!/bin/bash
git config --global --add safe.directory /__w/nso-icon-tool/nso-icon/tool
cmake -B build_switch -DPLATFORM_SWITCH=ON
make -C build_switch nso-icon-tool.nro -j$(nproc)
