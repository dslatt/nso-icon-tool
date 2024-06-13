/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 */

module.exports = async () => {
  const {loadYoga, default: Yoga} = require('yoga-layout');
  globalThis.Yoga = Yoga ?? (await loadYoga());
};

Object.defineProperty(globalThis, 'YGBENCHMARK', {
  get: () => globalThis.test,
});
