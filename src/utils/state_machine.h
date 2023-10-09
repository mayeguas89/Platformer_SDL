#pragma once

#include <functional>
#include <optional>
#include <queue>
#include <vector>

class State
{
public:
  State() = default;
  ~State() = default;

  void OnExit()
  {
    for (auto& f: onExitCallbacks_)
    {
      f();
    }
  };

  void OnRun(const float deltaTime)
  {
    for (auto& f: onStateCallbacks_)
    {
      f(deltaTime);
    }
  }

  void OnEnter()
  {
    for (auto& f: onEnterCallbacks_)
    {
      f();
    }
  };

  void RegisterEnterCallback(std::function<void()> f)
  {
    onEnterCallbacks_.push_back(f);
  }

  void RegisterExitCallback(std::function<void()> f)
  {
    onExitCallbacks_.push_back(f);
  }

  void RegisterOnStateCallback(std::function<void(const float)> f)
  {
    onStateCallbacks_.push_back(f);
  }

protected:
  std::vector<std::function<void()>> onEnterCallbacks_;
  std::vector<std::function<void(const float)>> onStateCallbacks_;
  std::vector<std::function<void()>> onExitCallbacks_;
};

class StateMachine
{
public:
  StateMachine(const std::optional<State>& state = std::nullopt): currentState_{state}
  {
    if (currentState_)
      currentState_->OnEnter();
  }

  void Update(const float deltaTime)
  {
    while (!newStates_.empty())
    {
      if (currentState_)
        currentState_->OnExit();
      currentState_ = newStates_.front();
      currentState_->OnEnter();
      newStates_.pop();
    }

    if (currentState_)
      currentState_->OnRun(deltaTime);
  }

  void ChangeState(const State& newState)
  {
    newStates_.push(newState);
  }

private:
  std::optional<State> currentState_;
  std::queue<State> newStates_;
};