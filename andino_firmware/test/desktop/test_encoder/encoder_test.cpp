// BSD 3-Clause License
//
// Copyright (c) 2024, Ekumen Inc.
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "interrupt_in.h"

namespace andino {
namespace test {
namespace {

using ::testing::_;
using ::testing::Return;

class MockInterruptIn : public andino::InterruptIn {
 public:
  MockInterruptIn(const int gpio_pin) : andino::InterruptIn(gpio_pin) {}
  MOCK_METHOD(void, begin, (), (const, override));
  MOCK_METHOD(int, read, (), (const, override));
  MOCK_METHOD(void, attach, (andino::InterruptIn::InterruptCallback callback), (const, override));
};

class EncoderTest : public testing::Test {
 protected:
  MockInterruptIn channel_u_interrupt_in_{0};
  MockInterruptIn channel_v_interrupt_in_{0};
  MockInterruptIn channel_w_interrupt_in_{0};
  andino::Encoder encoder_{&channel_u_interrupt_in_, &channel_v_interrupt_in_,
                           &channel_w_interrupt_in_};
};

static andino::InterruptIn::InterruptCallback channel_u_callback_{nullptr};
static andino::InterruptIn::InterruptCallback channel_v_callback_{nullptr};
static andino::InterruptIn::InterruptCallback channel_w_callback_{nullptr};

TEST_F(EncoderTest, Initialize) {
  EXPECT_CALL(channel_u_interrupt_in_, begin()).Times(1);
  EXPECT_CALL(channel_v_interrupt_in_, begin()).Times(1);
  EXPECT_CALL(channel_w_interrupt_in_, begin()).Times(1);
  EXPECT_CALL(channel_u_interrupt_in_, attach(_))
      .Times(1)
      .WillOnce(::testing::SaveArg<0>(&channel_u_callback_));
  EXPECT_CALL(channel_v_interrupt_in_, attach(_))
      .Times(1)
      .WillOnce(::testing::SaveArg<0>(&channel_v_callback_));
  EXPECT_CALL(channel_w_interrupt_in_, attach(_))
      .Times(1)
      .WillOnce(::testing::SaveArg<0>(&channel_w_callback_));

  // Initial read when begin() is called (U=1, V=0, W=0 -> state 4).
  EXPECT_CALL(channel_u_interrupt_in_, read()).Times(1).WillOnce(Return(1));
  EXPECT_CALL(channel_v_interrupt_in_, read()).Times(1).WillOnce(Return(0));
  EXPECT_CALL(channel_w_interrupt_in_, read()).Times(1).WillOnce(Return(0));

  encoder_.begin();

  EXPECT_NE(channel_u_callback_, nullptr);
  EXPECT_NE(channel_v_callback_, nullptr);
  EXPECT_NE(channel_w_callback_, nullptr);
  EXPECT_EQ(channel_u_callback_, channel_v_callback_);
  EXPECT_EQ(channel_v_callback_, channel_w_callback_);
}

TEST_F(EncoderTest, ReadIncreasingTicksCount) {
  // begin() initial read: State 4 (100)
  EXPECT_CALL(channel_u_interrupt_in_, read())
      .Times(7)
      .WillOnce(Return(1))  // begin(): state 4 (100)
      .WillOnce(Return(1))  // step 1: state 6 (110)
      .WillOnce(Return(0))  // step 2: state 2 (010)
      .WillOnce(Return(0))  // step 3: state 3 (011)
      .WillOnce(Return(0))  // step 4: state 1 (001)
      .WillOnce(Return(1))  // step 5: state 5 (101)
      .WillOnce(Return(1)); // step 6: state 4 (100)

  EXPECT_CALL(channel_v_interrupt_in_, read())
      .Times(7)
      .WillOnce(Return(0))  // begin()
      .WillOnce(Return(1))  // step 1
      .WillOnce(Return(1))  // step 2
      .WillOnce(Return(1))  // step 3
      .WillOnce(Return(0))  // step 4
      .WillOnce(Return(0))  // step 5
      .WillOnce(Return(0)); // step 6

  EXPECT_CALL(channel_w_interrupt_in_, read())
      .Times(7)
      .WillOnce(Return(0))  // begin()
      .WillOnce(Return(0))  // step 1
      .WillOnce(Return(0))  // step 2
      .WillOnce(Return(1))  // step 3
      .WillOnce(Return(1))  // step 4
      .WillOnce(Return(1))  // step 5
      .WillOnce(Return(0)); // step 6

  EXPECT_CALL(channel_u_interrupt_in_, begin()).Times(1);
  EXPECT_CALL(channel_v_interrupt_in_, begin()).Times(1);
  EXPECT_CALL(channel_w_interrupt_in_, begin()).Times(1);
  EXPECT_CALL(channel_u_interrupt_in_, attach(_))
      .Times(1)
      .WillOnce(::testing::SaveArg<0>(&channel_u_callback_));
  EXPECT_CALL(channel_v_interrupt_in_, attach(_)).Times(1);
  EXPECT_CALL(channel_w_interrupt_in_, attach(_)).Times(1);

  encoder_.begin();
  EXPECT_EQ(encoder_.read(), 0);

  // 4 -> 6 (+1)
  channel_u_callback_();
  EXPECT_EQ(encoder_.read(), 1);

  // 6 -> 2 (+1)
  channel_u_callback_();
  EXPECT_EQ(encoder_.read(), 2);

  // 2 -> 3 (+1)
  channel_u_callback_();
  EXPECT_EQ(encoder_.read(), 3);

  // 3 -> 1 (+1)
  channel_u_callback_();
  EXPECT_EQ(encoder_.read(), 4);

  // 1 -> 5 (+1)
  channel_u_callback_();
  EXPECT_EQ(encoder_.read(), 5);

  // 5 -> 4 (+1)
  channel_u_callback_();
  EXPECT_EQ(encoder_.read(), 6);
}

TEST_F(EncoderTest, ReadDecreasingTicksCount) {
  // begin() initial read: State 4 (100)
  // Backward sequence: 4 -> 5 -> 1 -> 3 -> 2 -> 6 -> 4
  EXPECT_CALL(channel_u_interrupt_in_, read())
      .Times(7)
      .WillOnce(Return(1))  // begin(): state 4 (100)
      .WillOnce(Return(1))  // step 1: state 5 (101)
      .WillOnce(Return(0))  // step 2: state 1 (001)
      .WillOnce(Return(0))  // step 3: state 3 (011)
      .WillOnce(Return(0))  // step 4: state 2 (010)
      .WillOnce(Return(1))  // step 5: state 6 (110)
      .WillOnce(Return(1)); // step 6: state 4 (100)

  EXPECT_CALL(channel_v_interrupt_in_, read())
      .Times(7)
      .WillOnce(Return(0))  // begin()
      .WillOnce(Return(0))  // step 1
      .WillOnce(Return(0))  // step 2
      .WillOnce(Return(1))  // step 3
      .WillOnce(Return(1))  // step 4
      .WillOnce(Return(1))  // step 5
      .WillOnce(Return(0)); // step 6

  EXPECT_CALL(channel_w_interrupt_in_, read())
      .Times(7)
      .WillOnce(Return(0))  // begin()
      .WillOnce(Return(1))  // step 1
      .WillOnce(Return(1))  // step 2
      .WillOnce(Return(1))  // step 3
      .WillOnce(Return(0))  // step 4
      .WillOnce(Return(0))  // step 5
      .WillOnce(Return(0)); // step 6

  EXPECT_CALL(channel_u_interrupt_in_, begin()).Times(1);
  EXPECT_CALL(channel_v_interrupt_in_, begin()).Times(1);
  EXPECT_CALL(channel_w_interrupt_in_, begin()).Times(1);
  EXPECT_CALL(channel_u_interrupt_in_, attach(_))
      .Times(1)
      .WillOnce(::testing::SaveArg<0>(&channel_u_callback_));
  EXPECT_CALL(channel_v_interrupt_in_, attach(_)).Times(1);
  EXPECT_CALL(channel_w_interrupt_in_, attach(_)).Times(1);

  encoder_.begin();
  EXPECT_EQ(encoder_.read(), 0);

  // 4 -> 5 (-1)
  channel_u_callback_();
  EXPECT_EQ(encoder_.read(), -1);

  // 5 -> 1 (-1)
  channel_u_callback_();
  EXPECT_EQ(encoder_.read(), -2);

  // 1 -> 3 (-1)
  channel_u_callback_();
  EXPECT_EQ(encoder_.read(), -3);

  // 3 -> 2 (-1)
  channel_u_callback_();
  EXPECT_EQ(encoder_.read(), -4);

  // 2 -> 6 (-1)
  channel_u_callback_();
  EXPECT_EQ(encoder_.read(), -5);

  // 6 -> 4 (-1)
  channel_u_callback_();
  EXPECT_EQ(encoder_.read(), -6);
}

TEST_F(EncoderTest, ResetTicksCount) {
  EXPECT_CALL(channel_u_interrupt_in_, read())
      .Times(3)
      .WillOnce(Return(1))  // begin(): state 4
      .WillOnce(Return(1))  // step 1: state 6
      .WillOnce(Return(0)); // step 2: state 2

  EXPECT_CALL(channel_v_interrupt_in_, read())
      .Times(3)
      .WillOnce(Return(0))
      .WillOnce(Return(1))
      .WillOnce(Return(1));

  EXPECT_CALL(channel_w_interrupt_in_, read())
      .Times(3)
      .WillOnce(Return(0))
      .WillOnce(Return(0))
      .WillOnce(Return(0));

  EXPECT_CALL(channel_u_interrupt_in_, begin()).Times(1);
  EXPECT_CALL(channel_v_interrupt_in_, begin()).Times(1);
  EXPECT_CALL(channel_w_interrupt_in_, begin()).Times(1);
  EXPECT_CALL(channel_u_interrupt_in_, attach(_))
      .Times(1)
      .WillOnce(::testing::SaveArg<0>(&channel_u_callback_));
  EXPECT_CALL(channel_v_interrupt_in_, attach(_)).Times(1);
  EXPECT_CALL(channel_w_interrupt_in_, attach(_)).Times(1);

  encoder_.begin();
  EXPECT_EQ(encoder_.read(), 0);

  channel_u_callback_();
  EXPECT_EQ(encoder_.read(), 1);

  channel_u_callback_();
  EXPECT_EQ(encoder_.read(), 2);

  encoder_.reset();
  EXPECT_EQ(encoder_.read(), 0);
}

}  // namespace
}  // namespace test
}  // namespace andino

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  if (RUN_ALL_TESTS()) {
  }

  // Always return zero-code and allow PlatformIO to parse results.
  return 0;
}
