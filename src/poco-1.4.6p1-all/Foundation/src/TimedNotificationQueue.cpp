//
// TimedNotificationQueue.cpp
//
// $Id: //poco/1.4/Foundation/src/TimedNotificationQueue.cpp#1 $
//
// Library: Foundation
// Package: Notifications
// Module:  TimedNotificationQueue
//
// Copyright (c) 2009, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "Poco/TimedNotificationQueue.h"
#include "Poco/Notification.h"
#include <limits>


namespace Poco {


TimedNotificationQueue::TimedNotificationQueue()
{
}


TimedNotificationQueue::~TimedNotificationQueue()
{
	clear();
}


void TimedNotificationQueue::enqueueNotification(Notification::Ptr pNotification, Timestamp timestamp)
{
	poco_check_ptr (pNotification);

	FastMutex::ScopedLock lock(_mutex);
	_nfQueue.insert(NfQueue::value_type(timestamp, pNotification));
	_nfAvailable.set();
}


Notification* TimedNotificationQueue::dequeueNotification()
{
	FastMutex::ScopedLock lock(_mutex);

	NfQueue::iterator it = _nfQueue.begin();
	if (it != _nfQueue.end())
	{
		Timestamp::TimeDiff sleep = -it->first.elapsed();
		if (sleep <= 0)
		{
			Notification::Ptr pNf = it->second;
			_nfQueue.erase(it);
			return pNf.duplicate();
		}
	}
	return 0;
}


Notification* TimedNotificationQueue::waitDequeueNotification()
{
	for (;;)
	{
		_mutex.lock();
		NfQueue::iterator it = _nfQueue.begin();
		if (it != _nfQueue.end())
		{
			_mutex.unlock();
			Timestamp::TimeDiff sleep = -it->first.elapsed();
			if (sleep <= 0)
			{
				return dequeueOne(it).duplicate();
			}
			else if (!wait(sleep))
			{
				return dequeueOne(it).duplicate();
			}
			else continue;
		}
		else
		{
			_mutex.unlock();
		}
		_nfAvailable.wait();
	}
}


Notification* TimedNotificationQueue::waitDequeueNotification(long milliseconds)
{
	while (milliseconds >= 0)
	{
		_mutex.lock();
		NfQueue::iterator it = _nfQueue.begin();
		if (it != _nfQueue.end())
		{
			_mutex.unlock();
			Poco::Timestamp now;
			Timestamp::TimeDiff sleep = it->first - now;
			if (sleep <= 0)
			{
				return dequeueOne(it).duplicate();
			}
			else if (sleep <= 1000*Timestamp::TimeDiff(milliseconds))
			{
				if (!wait(sleep))
				{
					return dequeueOne(it).duplicate();
				}
				else 
				{
					milliseconds -= static_cast<long>((now.elapsed() + 999)/1000);
					continue;
				}
			}
		}
		else
		{
			_mutex.unlock();
		}
		if (milliseconds > 0)
		{
			Poco::Timestamp now;
			_nfAvailable.tryWait(milliseconds);
			milliseconds -= static_cast<long>((now.elapsed() + 999)/1000);
		}
		else return 0;
	}
	return 0;
}


bool TimedNotificationQueue::wait(Timestamp::TimeDiff interval)
{
	const Timestamp::TimeDiff MAX_SLEEP = 8*60*60*Timestamp::TimeDiff(1000000); // sleep at most 8 hours at a time
	while (interval > 0)
	{
		Timestamp now;
		Timestamp::TimeDiff sleep = interval <= MAX_SLEEP ? interval : MAX_SLEEP;
		if (_nfAvailable.tryWait(static_cast<long>((sleep + 999)/1000)))
			return true;
		interval -= now.elapsed();
	}
	return false;
}


bool TimedNotificationQueue::empty() const
{
	FastMutex::ScopedLock lock(_mutex);
	return _nfQueue.empty();
}

	
int TimedNotificationQueue::size() const
{
	FastMutex::ScopedLock lock(_mutex);
	return static_cast<int>(_nfQueue.size());
}


void TimedNotificationQueue::clear()
{
	FastMutex::ScopedLock lock(_mutex);
	_nfQueue.clear();	
}


Notification::Ptr TimedNotificationQueue::dequeueOne(NfQueue::iterator& it)
{
	FastMutex::ScopedLock lock(_mutex);
	Notification::Ptr pNf = it->second;
	_nfQueue.erase(it);
	return pNf;
}


} // namespace Poco
