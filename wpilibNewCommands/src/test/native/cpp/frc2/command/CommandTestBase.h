// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <utility>

#include <frc/simulation/DriverStationSim.h>

#include "frc2/command/CommandGroupBase.h"
#include "frc2/command/CommandScheduler.h"
#include "frc2/command/SetUtilities.h"
#include "frc2/command/SubsystemBase.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "make_vector.h"

namespace frc2 {

class CommandTestBase : public ::testing::Test {
 public:
  CommandTestBase();

  class TestSubsystem : public SubsystemBase {};

 protected:
  class MockCommand : public Command {
   public:
    MOCK_CONST_METHOD0(GetRequirements, wpi::SmallSet<Subsystem*, 4>());
    MOCK_METHOD0(IsFinished, bool());
    MOCK_CONST_METHOD0(RunsWhenDisabled, bool());
    MOCK_METHOD0(Initialize, void());
    MOCK_METHOD0(Execute, void());
    MOCK_METHOD1(End, void(bool interrupted));

    MockCommand() {
      m_requirements = {};
      EXPECT_CALL(*this, GetRequirements())
          .WillRepeatedly(::testing::Return(m_requirements));
      EXPECT_CALL(*this, IsFinished()).WillRepeatedly(::testing::Return(false));
      EXPECT_CALL(*this, RunsWhenDisabled())
          .WillRepeatedly(::testing::Return(true));
    }

    MockCommand(std::initializer_list<Subsystem*> requirements,
                bool finished = false, bool runWhenDisabled = true) {
      m_requirements.insert(requirements.begin(), requirements.end());
      EXPECT_CALL(*this, GetRequirements())
          .WillRepeatedly(::testing::Return(m_requirements));
      EXPECT_CALL(*this, IsFinished())
          .WillRepeatedly(::testing::Return(finished));
      EXPECT_CALL(*this, RunsWhenDisabled())
          .WillRepeatedly(::testing::Return(runWhenDisabled));
    }

    MockCommand(MockCommand&& other) {
      EXPECT_CALL(*this, IsFinished())
          .WillRepeatedly(::testing::Return(other.IsFinished()));
      EXPECT_CALL(*this, RunsWhenDisabled())
          .WillRepeatedly(::testing::Return(other.RunsWhenDisabled()));
      std::swap(m_requirements, other.m_requirements);
      EXPECT_CALL(*this, GetRequirements())
          .WillRepeatedly(::testing::Return(m_requirements));
    }

    MockCommand(const MockCommand& other) : Command{other} {}

    void SetFinished(bool finished) {
      EXPECT_CALL(*this, IsFinished())
          .WillRepeatedly(::testing::Return(finished));
    }

    ~MockCommand() {  // NOLINT
      auto& scheduler = CommandScheduler::GetInstance();
      scheduler.Cancel(this);
    }

   protected:
    std::unique_ptr<Command> TransferOwnership() && {  // NOLINT
      return std::make_unique<MockCommand>(std::move(*this));
    }

   private:
    wpi::SmallSet<Subsystem*, 4> m_requirements;
  };

  CommandScheduler GetScheduler();

  void SetUp() override;

  void TearDown() override;

  void SetDSEnabled(bool enabled);
};

template <typename T>
class CommandTestBaseWithParam : public ::testing::TestWithParam<T> {
 public:
  CommandTestBaseWithParam() {
    auto& scheduler = CommandScheduler::GetInstance();
    scheduler.CancelAll();
    scheduler.Enable();
    scheduler.GetActiveButtonLoop()->Clear();
  }

  class TestSubsystem : public SubsystemBase {};

 protected:
  class MockCommand : public Command {
   public:
    MOCK_CONST_METHOD0(GetRequirements, wpi::SmallSet<Subsystem*, 4>());
    MOCK_METHOD0(IsFinished, bool());
    MOCK_CONST_METHOD0(RunsWhenDisabled, bool());
    MOCK_METHOD0(Initialize, void());
    MOCK_METHOD0(Execute, void());
    MOCK_METHOD1(End, void(bool interrupted));

    MockCommand() {
      m_requirements = {};
      EXPECT_CALL(*this, GetRequirements())
          .WillRepeatedly(::testing::Return(m_requirements));
      EXPECT_CALL(*this, IsFinished()).WillRepeatedly(::testing::Return(false));
      EXPECT_CALL(*this, RunsWhenDisabled())
          .WillRepeatedly(::testing::Return(true));
    }

    MockCommand(std::initializer_list<Subsystem*> requirements,
                bool finished = false, bool runWhenDisabled = true) {
      m_requirements.insert(requirements.begin(), requirements.end());
      EXPECT_CALL(*this, GetRequirements())
          .WillRepeatedly(::testing::Return(m_requirements));
      EXPECT_CALL(*this, IsFinished())
          .WillRepeatedly(::testing::Return(finished));
      EXPECT_CALL(*this, RunsWhenDisabled())
          .WillRepeatedly(::testing::Return(runWhenDisabled));
    }

    MockCommand(MockCommand&& other) {
      EXPECT_CALL(*this, IsFinished())
          .WillRepeatedly(::testing::Return(other.IsFinished()));
      EXPECT_CALL(*this, RunsWhenDisabled())
          .WillRepeatedly(::testing::Return(other.RunsWhenDisabled()));
      std::swap(m_requirements, other.m_requirements);
      EXPECT_CALL(*this, GetRequirements())
          .WillRepeatedly(::testing::Return(m_requirements));
    }

    MockCommand(const MockCommand& other) : Command{other} {}

    void SetFinished(bool finished) {
      EXPECT_CALL(*this, IsFinished())
          .WillRepeatedly(::testing::Return(finished));
    }

    ~MockCommand() {  // NOLINT
      auto& scheduler = CommandScheduler::GetInstance();
      scheduler.Cancel(this);
    }

   protected:
    std::unique_ptr<Command> TransferOwnership() && {  // NOLINT
      return std::make_unique<MockCommand>(std::move(*this));
    }

   private:
    wpi::SmallSet<Subsystem*, 4> m_requirements;
  };

  CommandScheduler GetScheduler() { return CommandScheduler(); }

  void SetUp() override { frc::sim::DriverStationSim::SetEnabled(true); }

  void TearDown() override {
    CommandScheduler::GetInstance().GetActiveButtonLoop()->Clear();
  }

  void SetDSEnabled(bool enabled) {
    frc::sim::DriverStationSim::SetEnabled(enabled);
  }
};

}  // namespace frc2
