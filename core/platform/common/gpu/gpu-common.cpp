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

#include "common/gpu/gpu-common.hpp"

#include <cassert>
#include <cstdlib>

#include "common/common.hpp"
#include "memory/memory-manager.hpp"
#include "topology/topology.hpp"
#include "util/timing.hpp"

void launch_kernel(CUfunction function, void **args, dim3 gridDim,
                   dim3 blockDim, cudaStream_t strm) {
  gpu_run(cuLaunchKernel(function, gridDim.x, gridDim.y, gridDim.z, blockDim.x,
                         blockDim.y, blockDim.z, 0, (CUstream)strm, args,
                         nullptr));
}

void launch_kernel(CUfunction function, void **args, dim3 gridDim,
                   cudaStream_t strm) {
  launch_kernel(function, args, gridDim, defaultBlockDim, strm);
}

void launch_kernel(CUfunction function, void **args, cudaStream_t strm) {
  launch_kernel(function, args, defaultGridDim, defaultBlockDim, strm);
}

extern "C" {
void launch_kernel(CUfunction function, void **args) {
  launch_kernel(function, args, defaultGridDim, defaultBlockDim, nullptr);
}

void launch_kernel_strm(CUfunction function, void **args, cudaStream_t strm) {
  launch_kernel(function, args, strm);
  gpu_run(cudaStreamSynchronize(strm));
}

void launch_kernel_strm_sized(CUfunction function, void **args,
                              cudaStream_t strm, unsigned int blockX,
                              unsigned int gridX) {
  launch_kernel(function, args, gridX, blockX, strm);
}

void launch_kernel_strm_single(CUfunction function, void **args,
                               cudaStream_t strm) {
  launch_kernel_strm_sized(function, args, strm, 1, 1);
}
}

// Disable by default, as, active, it does not guarantee NUMA locality!
bool allow_readwrite = false;

mmap_file::mmap_file(std::string name, data_loc loc)
    : mmap_file(name, loc, ::getFileSize(name.c_str()), 0) {}

mmap_file::mmap_file(std::string name, data_loc loc, size_t bytes,
                     size_t offset = 0)
    : loc(loc), filesize(bytes) {
  time_block t("Topen (" + name + ", " + std::to_string(offset) + ":" +
               std::to_string(offset + filesize) + "): ");
  readonly = false;

  size_t real_filesize = ::getFileSize(name.c_str());
  assert(offset + filesize <= real_filesize);

  fd = -1;
  if (allow_readwrite) fd = open(name.c_str(), O_RDWR, 0);

  if (fd == -1) {
    fd = open(name.c_str(), O_RDONLY, 0);
    readonly = true;
    if (fd != -1) LOG(INFO) << "Opening file " << name << " as read-only";
  }

  if (fd == -1) {
    string msg("[Storage: ] Failed to open input file " + name);
    LOG(ERROR) << msg;
    throw runtime_error(msg);
  }

  // Execute mmap
  {
    time_block t("Tmmap: ");
    data =
        mmap(nullptr, filesize, PROT_READ | (readonly ? 0 : PROT_WRITE),
             (readonly ? MAP_PRIVATE : MAP_SHARED) | MAP_POPULATE, fd, offset);
    assert(data != MAP_FAILED);
  }

  // gpu_run(cudaHostRegister(data, filesize, 0));
  if (loc == PINNED) {
    if (readonly) {
      void *data2;
      {
        time_block t("Talloc-readonly: ");

        data2 = MemoryManager::mallocPinned(filesize);
      }
      memcpy(data2, data, filesize);
      munmap(data, filesize);
      close(fd);
      data = data2;
      gpu_data = data;
    } else {
      time_block t("Talloc: ");
      gpu_data = NUMAPinnedMemAllocator::reg(data, filesize);
    }
  } else {
    gpu_data = data;
  }

  if (loc == GPU_RESIDENT) {
    std::cout << "Dataset on device: "
              << topology::getInstance().getActiveGpu().id << std::endl;
    gpu_data = MemoryManager::mallocGpu(filesize);
    gpu_run(cudaMemcpy(gpu_data, data, filesize, cudaMemcpyDefault));
    munmap(data, filesize);
    close(fd);
  }
}

mmap_file::~mmap_file() {
  if (loc == GPU_RESIDENT) gpu_run(cudaFree(gpu_data));

  // gpu_run(cudaHostUnregister(data));
  // if (loc == PINNED)       gpu_run(cudaFreeHost(data));
  if (loc == PINNED) {
    if (readonly) {
      MemoryManager::freePinned(data);
    } else {
      NUMAPinnedMemAllocator::unreg(gpu_data);
      munmap(data, filesize);
      close(fd);
    }
  }

  if (loc == PAGEABLE) {
    munmap(data, filesize);
    close(fd);
  }
}

const void *mmap_file::getData() const { return gpu_data; }

size_t mmap_file::getFileSize() const { return filesize; }

extern "C" {
int get_ptr_device(const void *p) {
  const auto *g = topology::getInstance().getGpuAddressed(p);
  return g ? -1 : g->id;
}

// FIXME: rename function...........
int get_ptr_device_or_rand_for_host(const void *p) {
  const auto *g = topology::getInstance().getGpuAddressed(p);
  if (g) return g->id;
  const auto *c = topology::getInstance().getCpuNumaNodeAddressed(p);
  size_t local_gpus = c->local_gpus.size();
  if (local_gpus == 1)
    return c->local_gpus[0];
  else if (local_gpus > 0)
    return c->local_gpus[rand() % local_gpus];
  else
    return rand();
}

void memcpy_gpu(void *dst, const void *src, size_t size, bool is_volatile) {
  assert(!is_volatile);
#ifndef NCUDA
  cudaStream_t strm;
  gpu_run(cudaStreamCreate(&strm));
  gpu_run(cudaMemcpyAsync(dst, src, size, cudaMemcpyDefault, strm));
  gpu_run(cudaStreamSynchronize(strm));
  gpu_run(cudaStreamDestroy(strm));
#else
  memcpy(dst, src, size);
#endif
}
}

cudaStream_t createNonBlockingStream() {
  cudaStream_t strm = nullptr;
  gpu_run(cudaStreamCreateWithFlags(&strm, cudaStreamNonBlocking));
  return strm;
}

void syncAndDestroyStream(cudaStream_t strm) {
  gpu_run(cudaStreamSynchronize(strm));
  gpu_run(cudaStreamDestroy(strm));
}