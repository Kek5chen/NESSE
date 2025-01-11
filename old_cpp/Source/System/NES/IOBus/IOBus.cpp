#include "IOBus.hpp"

IOBus::IOBus() : mCPU(this), mMemory(this), mPPU(this), mAPU(this) {
}
