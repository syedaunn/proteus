/*
    Proteus -- High-performance query processing on heterogeneous hardware.

                            Copyright (c) 2017
        Data Intensive Applications and Systems Laboratory (DIAS)
                École Polytechnique Fédérale de Lausanne

                            All Rights Reserved.

    Permission to use, copy, modify and distribute this software and
    its documentation is hereby granted, provided that both the
    copyright notice and this permission notice appear in all copies of
    the software, derivative works or modified versions, and any
    portions thereof, and that both notices appear in supporting
    documentation.

    This code is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. THE AUTHORS
    DISCLAIM ANY LIABILITY OF ANY KIND FOR ANY DAMAGES WHATSOEVER
    RESULTING FROM THE USE OF THIS SOFTWARE.
*/
#include "device-manager.hpp"

DeviceManager &DeviceManager::getInstance() {
  static DeviceManager instance;
  return instance;
}

const topology::cpunumanode &DeviceManager::getAvailableCPUNumaNode(
    const void *, size_t cpu_req) {
  return topology::getInstance()
      .getCpuNumaNodes()[cpu_req %
                         topology::getInstance().getCpuNumaNodeCount()];
}

const topology::core &DeviceManager::getAvailableCPUCore(const void *,
                                                         size_t cpu_req) {
  size_t core_index = cpu_req % topology::getInstance().getCoreCount();
  // NOTE: Assuming all CPUs have the same number of cores!
  size_t cpunumacnt = topology::getInstance().getCpuNumaNodeCount();
  size_t numanode = core_index % cpunumacnt;
  const auto &cpunumanode = topology::getInstance().getCpuNumaNodes()[numanode];
  return cpunumanode.getCore(core_index / cpunumacnt);
}

class gpu_index {
 private:
  std::vector<size_t> d;

 private:
  gpu_index() {
    const auto &topo = topology::getInstance();
    d.reserve(topo.getGpuCount());
    size_t cpus = topo.getCpuNumaNodeCount();
    for (size_t j = 0; d.size() < topo.getGpuCount(); ++j) {
      size_t cpu = j % cpus;
      size_t gpur = j / cpus;
      const auto &numanode = topo.getCpuNumaNodes()[cpu];
      const auto &gpus = numanode.local_gpus;
      if (gpur >= gpus.size()) continue;
      d.emplace_back(gpus[gpur]);
    }
  }

  friend const topology::gpunode &DeviceManager::getAvailableGPU(
      const void *, size_t gpu_req);
};

const topology::gpunode &DeviceManager::getAvailableGPU(const void *,
                                                        size_t gpu_req) {
  static const gpu_index index;
  size_t gpu_i = gpu_req % topology::getInstance().getGpuCount();
  return topology::getInstance().getGpus()[index.d[gpu_i]];
}