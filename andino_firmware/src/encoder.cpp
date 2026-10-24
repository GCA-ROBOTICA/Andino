// Code in this file is inspired by:
// https://github.com/hbrobotics/ros_arduino_bridge/blob/indigo-devel/ros_arduino_firmware/src/libraries/ROSArduinoBridge/encoder_driver.ino
//
// ----------------------------------------------------------------------------
// ros_arduino_bridge's license follows:
//
// Software License Agreement (BSD License)
//
// Copyright (c) 2012, Patrick Goebel.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//   * Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above
//     copyright notice, this list of conditions and the following
//     disclaimer in the documentation and/or other materials provided
//     with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

// BSD 3-Clause License
//
// Copyright (c) 2023, Ekumen Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#include "encoder.h"

#include <stdint.h>

#include "interrupt_in.h"

namespace andino {

constexpr int8_t Encoder::kTicksDelta[];

const InterruptIn::InterruptCallback Encoder::kCallbacks[kInstancesMax] = {callback_0, callback_1};

void Encoder::callback_0() {
  if (Encoder::instances_[0] != nullptr) {
    Encoder::instances_[0]->callback();
  }
}

void Encoder::callback_1() {
  if (Encoder::instances_[1] != nullptr) {
    Encoder::instances_[1]->callback();
  }
}

Encoder* Encoder::instances_[kInstancesMax] = {nullptr, nullptr};
int Encoder::instance_count_ = 0;

void Encoder::begin() {
  // The current implementation only supports two instances of this class to be constructed. This
  // prevents reaching a buffer overflow.
  if (instance_count_ == kInstancesMax) {
    return;
  }

  channel_u_interrupt_in_->begin();
  channel_u_interrupt_in_->attach(kCallbacks[instance_count_]);
  channel_v_interrupt_in_->begin();
  channel_v_interrupt_in_->attach(kCallbacks[instance_count_]);
  channel_w_interrupt_in_->begin();
  channel_w_interrupt_in_->attach(kCallbacks[instance_count_]);

  // Read initial 3-phase Hall state.
  state_ = static_cast<uint8_t>((channel_u_interrupt_in_->read() << 2) |
                                (channel_v_interrupt_in_->read() << 1) |
                                channel_w_interrupt_in_->read());

  instances_[instance_count_] = this;
  instance_count_++;
}

long Encoder::read() { return count_; }

void Encoder::reset() { count_ = 0L; }

void Encoder::callback() {
  // Read current Hall state: bit 2 = U, bit 1 = V, bit 0 = W.
  const uint8_t new_state = static_cast<uint8_t>((channel_u_interrupt_in_->read() << 2) |
                                                 (channel_v_interrupt_in_->read() << 1) |
                                                 channel_w_interrupt_in_->read());

  // Form 6-bit index from previous 3 bits and current 3 bits.
  const uint8_t index = static_cast<uint8_t>(((state_ & 0x07) << 3) | (new_state & 0x07));

  // Update ticks count based on transition lookup table.
  count_ += kTicksDelta[index];
  state_ = new_state;
}

}  // namespace andino
