// Code in this file is inspired by:
// https://github.com/hbrobotics/ros_arduino_bridge/blob/indigo-devel/ros_arduino_firmware/src/libraries/ROSArduinoBridge/motor_driver.h
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
#pragma once

#include "digital_out.h"
#include "pwm_out.h"

namespace andino {

/// @brief This class allows to control a motor by enabling it, selecting direction and setting its
/// speed. The driver is expected to have separate PWM (speed) and DIR (direction) inputs.
class Motor {
 public:
  /// @brief Constructs a new Motor object.
  ///
  /// @param enable_digital_out Digital output connected to motor enable pin.
  /// @param direction_digital_out Digital output connected to motor direction pin.
  /// @param speed_pwm_out PWM output connected to motor speed PWM pin.
  Motor(const DigitalOut* enable_digital_out, const DigitalOut* direction_digital_out,
        const PwmOut* speed_pwm_out)
      : enable_digital_out_(enable_digital_out),
        direction_digital_out_(direction_digital_out),
        speed_pwm_out_(speed_pwm_out) {}

  /// @brief Initializes the motor.
  void begin();

  /// @brief Enables the motor.
  ///
  /// @param enabled True to enable the motor, false otherwise.
  void enable(bool enabled);

  /// @brief Sets the motor speed.
  ///
  /// @param speed Motor speed value (signed: positive for forward, negative for backward).
  void set_speed(int speed);

 private:
  /// Minimum speed magnitude (negative speeds are considered as positive backward speeds).
  static constexpr int kMinSpeed{0};

  /// Maximum speed magnitude.
  static constexpr int kMaxSpeed{255};

  /// Digital output connected to motor enable pin.
  const DigitalOut* enable_digital_out_;

  /// Digital output connected to motor direction pin.
  const DigitalOut* direction_digital_out_;

  /// PWM output connected to motor speed PWM pin.
  const PwmOut* speed_pwm_out_;
};

}  // namespace andino
