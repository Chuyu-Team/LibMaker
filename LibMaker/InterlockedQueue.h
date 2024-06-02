#pragma once

template<class Entry, size_t kMaxBlockSize>
class InterlockedQueue
{
private:
    struct Block
    {
        size_t uLastReadIndex = 0;
        size_t uLastWriteIndex = 0;
        Block* pNextBlock = nullptr;
        Entry* arrLoopBuffer[kMaxBlockSize];

        bool IsEmpty()
        {
            return uLastReadIndex == uLastWriteIndex;
        }

        bool IsFull()
        {
            return uLastReadIndex + kMaxBlockSize == uLastWriteIndex;
        }
    };

    Block* pFirstReadBlock = nullptr;
    Block* pLastWriteBlock = nullptr;

public:
    Entry* Pop() noexcept
    {
        if (!pFirstReadBlock)
            return nullptr;

        for (;;)
        {
            // ��ǰ����Ȼ��Ԫ�أ�
            if (!pFirstReadBlock->IsEmpty())
            {
                auto _pTmp = pFirstReadBlock->arrLoopBuffer[pFirstReadBlock->uLastReadIndex % kMaxBlockSize];
                pFirstReadBlock->uLastReadIndex += 1;
                return _pTmp;
            }

            // ������ת����һ��
            if (!pFirstReadBlock->pNextBlock)
                return nullptr;

            auto _pPendingDelete = pFirstReadBlock;
            pFirstReadBlock = pFirstReadBlock->pNextBlock;
            delete _pPendingDelete;
        }

        return nullptr;
    }

    void Push(_In_ Entry* _pEntry)
    {
        if (!pLastWriteBlock)
        {
            pFirstReadBlock = pLastWriteBlock = new Block();
        }

        // ������˾ͳ������ӵ���һ��
        if (pLastWriteBlock->IsFull())
        {
            auto _pNextBlock = new Block();
            pLastWriteBlock->pNextBlock = _pNextBlock;
            pLastWriteBlock = _pNextBlock;
        }

        pLastWriteBlock->arrLoopBuffer[pLastWriteBlock->uLastWriteIndex % kMaxBlockSize] = _pEntry;
        pLastWriteBlock->uLastWriteIndex += 1;
    }
};