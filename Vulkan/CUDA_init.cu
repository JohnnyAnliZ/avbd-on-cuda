#include "AVBD.hpp"
#include <stdexcept>


inline void checkCudaErrors(cudaError_t result) {
    if (result != cudaSuccess) {
        throw std::runtime_error(cudaGetErrorString(result));
    }
}


int AVBD::initCuda(uint8_t *vkDeviceUUID, size_t UUID_SIZE) {
  int current_device = 0;
  int device_count = 0;
  int devices_prohibited = 0;

  cudaDeviceProp deviceProp;
  checkCudaErrors(cudaGetDeviceCount(&device_count));

  if (device_count == 0) {
    fprintf(stderr, "CUDA error: no devices supporting CUDA.\n");
    exit(EXIT_FAILURE);
  }

  // Find the GPU which is selected by Vulkan
  while (current_device < device_count) {
    checkCudaErrors(cudaGetDeviceProperties(&deviceProp, current_device));

    int computeMode = 0;
    checkCudaErrors(cudaDeviceGetAttribute(
        &computeMode,
        cudaDevAttrComputeMode,
        current_device
    ));

    if (computeMode != cudaComputeModeProhibited) {
      // Compare the cuda device UUID with vulkan UUID
      int ret = memcmp((void *)&deviceProp.uuid, vkDeviceUUID, UUID_SIZE);
      if (ret == 0) {
        checkCudaErrors(cudaSetDevice(current_device));
        checkCudaErrors(cudaGetDeviceProperties(&deviceProp, current_device));
        printf("GPU Device %d: \"%s\" with compute capability %d.%d\n\n",
               current_device, deviceProp.name, deviceProp.major,
               deviceProp.minor);

        return current_device;
      }

    } else {
      devices_prohibited++;
    }

    current_device++;
  }

  if (devices_prohibited == device_count) {
    fprintf(stderr,
            "CUDA error:"
            " No Vulkan-CUDA Interop capable GPU found.\n");
    exit(EXIT_FAILURE);
  }

  return -1;
}

