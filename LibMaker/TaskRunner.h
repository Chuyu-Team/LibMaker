#pragma once
#include <Windows.h>

#include <stdint.h>
#include <xutility>

#include "InterlockedQueue.h"


class Task
{
private:
    ULONG uRef = 1;

public:
    virtual ~Task() = default;

    void AddRef()
    {
        InterlockedIncrement(&uRef);
    }

    void Release()
    {
        if (InterlockedDecrement(&uRef) == 0)
        {
            delete this;
        }
    }

    virtual void ExecuteTaskRunner() = 0;
};

class ParallelTaskRunner
{
    static constexpr DWORD kDefaultParallelMaximum = 200;

    volatile ULONG uRef = 1;

    // 允许并行执行的最大个数，0代表默认值，Vista系统默认值为200。
    volatile DWORD uParallelMaximum = 0;

    InterlockedQueue<Task, 1024> oTaskQueue;

    // |uWeakCount| bPushLock | bStopWakeup | bPushLock |
    // | 31  ~  3 |    2      |     1       |    0      |
    union TaskRunnerFlagsType
    {
        volatile uint64_t fFlags64;
        uint32_t uWakeupCountAndPushLock;

        struct
        {
            volatile uint32_t bPushLock : 1;
            volatile uint32_t bStopWakeup : 1;
            volatile uint32_t bPopLock : 1;
            int32_t uWakeupCount : 29;
            // 当前已经启动的线程数
            uint32_t uParallelCurrent;
        };
    };
    TaskRunnerFlagsType TaskRunnerFlags = { 0 };
    enum : uint32_t
    {
        LockedQueuePushBitIndex = 0,
        StopWakeupBitIndex,
        LockedQueuePopBitIndex,
        WakeupCountStartBitIndex,
        WakeupOnceRaw = 1 << WakeupCountStartBitIndex,
        UnlockQueuePushLockBitAndWakeupOnceRaw = WakeupOnceRaw - (1u << LockedQueuePushBitIndex),
    };

    static_assert(sizeof(TaskRunnerFlags) == sizeof(uint64_t), "");

public:
    ParallelTaskRunner(bool _bStack = false)
        : uRef(_bStack ? UINT32_MAX : 1)
    {
    }

    ParallelTaskRunner(const ParallelTaskRunner&) = delete;

    void __fastcall AddRef() noexcept
    {
        if (uRef != UINT32_MAX)
        {
            InterlockedIncrement(&uRef);
        }
    }

    void __fastcall Release() noexcept
    {
        if (uRef != UINT32_MAX)
        {
            if (InterlockedDecrement(&uRef) == 0)
            {
                delete this;
            }
        }
    }

    template<typename Lambda>
    void __fastcall PostTask(Lambda&& _fnLambda) noexcept
    {
        class TaskLambda
            : public Task
            , public Lambda
        {

        public:
            TaskLambda(Lambda&& _fnLambda)
                : Lambda(std::move(_fnLambda))
            {
            }

            void ExecuteTaskRunner() override
            {
                Lambda::operator()();
            }
        };

        auto _pTask = new TaskLambda(std::move(_fnLambda));
        PostWork(_pTask);
        _pTask->Release();
    }

    void __fastcall SetParallelMaximum(DWORD _uParallelMaximum)
    {
        uParallelMaximum = _uParallelMaximum;
    }

private:
    bool __fastcall PostWork(_In_ Task* _pWork) noexcept
    {
        if (TaskRunnerFlags.bStopWakeup)
        {
            return false;
        }

        _pWork->AddRef();
        auto _uParallelMaximum = uParallelMaximum;
        if (_uParallelMaximum == 0)
            _uParallelMaximum = kDefaultParallelMaximum;

        // 将任务提交到任务队列
        AddRef();

        // 解除锁定，并且 WeakupCount + 1，也尝试提升 uParallelCurrent
        TaskRunnerFlagsType _uOldFlags = TaskRunnerFlags;
        TaskRunnerFlagsType _uNewFlags;
        for (;;)
        {
            if (_uOldFlags.uWakeupCountAndPushLock & (1 << LockedQueuePushBitIndex))
            {
                YieldProcessor();
                _uOldFlags.fFlags64 = TaskRunnerFlags.fFlags64;
                continue;
            }

            _uNewFlags = _uOldFlags;
            _uNewFlags.uWakeupCountAndPushLock += WakeupOnceRaw + (1 << LockedQueuePushBitIndex);

            if (_uNewFlags.uParallelCurrent < _uParallelMaximum && _uNewFlags.uWakeupCount >= (int32_t)_uNewFlags.uParallelCurrent)
            {
                _uNewFlags.uParallelCurrent += 1;
            }

            auto _uLast = InterlockedCompareExchange(&TaskRunnerFlags.fFlags64, _uNewFlags.fFlags64, _uOldFlags.fFlags64);
            if (_uLast == _uOldFlags.fFlags64)
            {
                oTaskQueue.Push(_pWork);
                _interlockedbittestandreset((volatile LONG*)&TaskRunnerFlags.uWakeupCountAndPushLock, LockedQueuePushBitIndex);
                break;
            }
            _uOldFlags.fFlags64 = _uLast;
        }

        // 并发送数量没有提升，所以无需从线程池拉发起新任务
        if (_uOldFlags.uParallelCurrent == _uNewFlags.uParallelCurrent)
            return true;

        // 如果是第一次那么再额外 AddRef，避免ExecuteTaskRunner调用时 TP_Pool 被释放了
        // ExecuteTaskRunner内部负责重新减少引用计数
        if (_uOldFlags.uParallelCurrent == 0u)
        {
            AddRef();
        }

        auto _bRet = QueueUserWorkItem([](LPVOID lpThreadParameter) -> DWORD
            {
                auto _pTaskRunner = (ParallelTaskRunner*)lpThreadParameter;

                _pTaskRunner->ExecuteTaskRunner();
                return 0;

            }, this, 0);

        if (!_bRet)
        {
            Release();
        }

        return true;
    }

    void __fastcall Close() noexcept
    {
        if (_interlockedbittestandset((volatile LONG*)&TaskRunnerFlags.uWakeupCountAndPushLock, StopWakeupBitIndex))
        {
            return;
        }

        for (;;)
        {
            auto _pTask = PopTask();
            if (!_pTask)
                break;

            _pTask->Release();
            Release();
        }

        Release();
    }

private:
    _Ret_maybenull_ Task* __fastcall PopTask() noexcept
    {
        for (;;)
        {
            if (!_interlockedbittestandset((volatile LONG*)&TaskRunnerFlags.uWakeupCountAndPushLock, LockedQueuePopBitIndex))
            {
                auto _pWork = oTaskQueue.Pop();

                if (_pWork)
                {
                    InterlockedAdd((volatile LONG*)&TaskRunnerFlags.uWakeupCountAndPushLock, -(WakeupOnceRaw + (1 << LockedQueuePopBitIndex)));
                }
                else
                {
                    _interlockedbittestandreset((volatile LONG*)&TaskRunnerFlags.uWakeupCountAndPushLock, LockedQueuePopBitIndex);
                }
                return _pWork;
            }
        }
    }

    void __fastcall ExecuteTaskRunner() noexcept
    {
    __START:
        for (;;)
        {
            auto _pWork = PopTask();
            if (!_pWork)
                break;

            _pWork->ExecuteTaskRunner();

            _pWork->Release();
            Release();
        }

        // 尝试释放 uParallelCurrent
        TaskRunnerFlagsType _uOldFlags = TaskRunnerFlags;
        TaskRunnerFlagsType _uNewFlags;
        for (;;)
        {
            // 任然有任务，重新执行 ExecuteTaskRunner
            if (_uOldFlags.uWakeupCount > 0)
            {
                goto __START;
            }

            _uNewFlags = _uOldFlags;
            _uNewFlags.uParallelCurrent -= 1;

            auto _uLast = InterlockedCompareExchange(&TaskRunnerFlags.fFlags64, _uNewFlags.fFlags64, _uOldFlags.fFlags64);
            if (_uLast == _uOldFlags.fFlags64)
                break;

            _uOldFlags.fFlags64 = _uLast;
        }

        if (_uNewFlags.uParallelCurrent == 0u)
        {
            // 对应上面 QueueUserWorkItem 的AddRef();
            Release();
        }
    }
};


class TaskWait
{
private:
    ULONG uRef = 0;
    HANDLE hEvent = CreateEventW(nullptr, FALSE, TRUE, nullptr);

public:
    ~TaskWait()
    {
        CloseHandle(hEvent);
    }

    void Lock()
    {
        if (InterlockedIncrement(&uRef) == 1)
        {
            ResetEvent(hEvent);
        }
    }

    void Unlock()
    {
        if (InterlockedDecrement(&uRef) == 0)
        {
            SetEvent(hEvent);
        }
    }

    void Wait()
    {
        WaitForSingleObject(hEvent, INFINITE);
    }
};