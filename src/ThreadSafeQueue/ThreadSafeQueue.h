#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <string>

template <typename T> 
class ThreadSafeQueue 
{
public:
    ThreadSafeQueue() 
    {
        m_bTermination = false;
    }
    ThreadSafeQueue(ThreadSafeQueue const& other)
    {
        std::lock_guard<std::mutex> lk(other.m_Mutex);
        m_Queue = other.m_Queue;
    }

    ~ThreadSafeQueue(void) {
    }

    /*
    * @brief �ȴ����в�Ϊ��ѹ��Ԫ��,���ߵȴ�m_bTerminationΪ��ʱѹ��Ԫ��
    * @return ����Ϊ��ʱ����false����Ϊ��ʱ����true
    */
    bool WaitAndPop (T& value)
    {
        std::unique_lock<std::mutex> lock(m_Mutex);
        m_ConditionVariable.wait(lock, [this] { return ((!m_Queue.empty()) || m_bTermination); });
        //��Ϊ�������
        if (!m_Queue.empty()) 
        {
            value = m_Queue.front();
            //value = std::move(m_Queue.front());
            m_Queue.front().reset();
            m_Queue.pop();
            std::cout << "ѹ��" << std::endl;
            return true;
        }
        //����Ϊ���򷵻�ʧ��
        return false;
    }

    std::shared_ptr<T> WaitAndPop() 
    {
        std::unique_lock<std::mutex> lock(m_Mutex);
        m_ConditionVariable.wait(lock, [this] { return ((!m_Queue.empty()) || m_bTermination); });
        if (!m_Queue.empty())
        {
            std::shared_ptr<T> res(std::make_shared<T>(m_Queue.front()));
            m_Queue.pop();
            std::cout << "ѹ��" << std::endl;
            return res;
        }
        return nullptr;
    }

    /*
    * @brief ֱ��ѹ��Ԫ��
    */
    bool TryPop(T& value)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if (m_Queue.empty())
        {
            return false;
        }
        value = m_Queue.front();
        m_Queue.pop();
        return true;
    }

    //����Ϊ�շ���null
    std::shared_ptr<T> TryPop() 
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if (m_Queue.empty()) 
        {
            return nullptr;
        }
        std::shared_ptr<T> res(std::make_shared<T>(m_Queue.front()));
        m_Queue.pop();
        return res;
    }


    void Push(T new_value) 
    {
        if (m_bTermination) 
        {
            return;
        }
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Queue.push(new_value);
        std::cout << "ѹ��" << std::endl;
        m_ConditionVariable.notify_one();
    }

    bool Empty() 
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_Queue.empty();
    }
    int Size()
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_Queue.size();
    }
    //���ö���Ϊ�˳�״̬�����˳�״̬�£�������ӣ�����ִ�г��ӣ���������Ϊ��ʱ��wait_and_pop����������
    void Termination()
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_bTermination = true;
        m_ConditionVariable.notify_all();
    }

    // �����˳�״̬
    bool IsTermination()
    {
        return m_bTermination;
    }

private:
    std::mutex m_Mutex;
    std::queue<T> m_Queue;
    std::condition_variable m_ConditionVariable;
    std::atomic<bool> m_bTermination;
};

#endif // !THREAD_SAFE_QUEUE_H
