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

namespace andino {

/// @brief Hardware configuration.
struct Hw {
  /// @brief Left encoder channel U pin. Connected to GPIO 34 (Input only).
  static constexpr int kLeftEncoderChannelUGpioPin{34};
  /// @brief Left encoder channel V pin. Connected to GPIO 35 (Input only).
  static constexpr int kLeftEncoderChannelVGpioPin{35};
  /// @brief Left encoder channel W pin. Connected to GPIO 32.
  static constexpr int kLeftEncoderChannelWGpioPin{32};

  /// @brief Right encoder channel U pin. Connected to GPIO 36 (Sensor VP, Input only).
  static constexpr int kRightEncoderChannelUGpioPin{36};
  /// @brief Right encoder channel V pin. Connected to GPIO 39 (Sensor VN, Input only).
  static constexpr int kRightEncoderChannelVGpioPin{39};
  /// @brief Right encoder channel W pin. Connected to GPIO 33.
  static constexpr int kRightEncoderChannelWGpioPin{33};

  /// @brief Left motor driver PWM (speed) pin. Connected to GPIO 25.
  static constexpr int kLeftMotorPwmGpioPin{25};
  /// @brief Left motor driver direction pin. Connected to GPIO 26.
  static constexpr int kLeftMotorDirGpioPin{26};
  /// @brief Left motor driver enable pin. Connected to GPIO 27.
  static constexpr int kLeftMotorEnableGpioPin{27};

  /// @brief Right motor driver PWM (speed) pin. Connected to GPIO 14.
  static constexpr int kRightMotorPwmGpioPin{14};
  /// @brief Right motor driver direction pin. Connected to GPIO 13.
  static constexpr int kRightMotorDirGpioPin{13};
  /// @brief Right motor driver enable pin. Connected to GPIO 23.
  static constexpr int kRightMotorEnableGpioPin{23};

  /// @brief IMU sensor I2C SCL pin. Connected to GPIO 22.
  static constexpr int kImuI2cSclPin{22};
  /// @brief IMU sensor I2C SDA pin. Connected to GPIO 21.
  static constexpr int kImuI2cSdaPin{21};
};

}  // namespace andino
