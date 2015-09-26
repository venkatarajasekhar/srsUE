/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2015 The srsUE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/******************************************************************************
 *  File:         msg_queue.h
 *  Description:  Thread-safe queue of srsue_byte_buffer structs.
 *  Reference:
 *****************************************************************************/

#ifndef MSG_QUEUE_H
#define MSG_QUEUE_H

#include "common/common.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

namespace srsue {

class msg_queue
{
public:
  msg_queue(uint32_t capacity_ = 10)
    :head(0)
    ,tail(0)
    ,unread(0)
    ,unread_bytes(0)
    ,capacity(capacity_)
  {
    buf = new srsue_byte_buffer_t[capacity];
  }

  ~msg_queue()
  {
    delete [] buf;
  }

  void write(srsue_byte_buffer_t *msg)
  {
    boost::mutex::scoped_lock lock(mutex);
    while(is_full()) not_full.wait(lock);
    buf[head] = *msg;
    head = (head+1)%capacity;
    unread++;
    unread_bytes += msg->N_bytes;
    lock.unlock();
    not_empty.notify_one();
  }

  void write(uint8_t *payload, uint32_t nof_bytes)
  {
    boost::mutex::scoped_lock lock(mutex);
    while(is_full()) not_full.wait(lock);
    memcpy(buf[head].msg, payload, nof_bytes);
    buf[head].N_bytes = nof_bytes;
    head = (head+1)%capacity;
    unread++;
    unread_bytes += nof_bytes;
    lock.unlock();
    not_empty.notify_one();
  }

  void read(srsue_byte_buffer_t *msg)
  {
    boost::mutex::scoped_lock lock(mutex);
    while(is_empty()) not_empty.wait(lock);
    *msg = buf[tail];
    tail = (tail+1)%capacity;
    unread--;
    unread_bytes -= msg->N_bytes;
    lock.unlock();
    not_full.notify_one();
  }

  uint32_t read(uint8_t *payload)
  {
    boost::mutex::scoped_lock lock(mutex);
    while(is_empty()) not_empty.wait(lock);
    memcpy(payload, buf[tail].msg, buf[tail].N_bytes);
    uint32_t r = buf[tail].N_bytes;
    tail = (tail+1)%capacity;
    unread--;
    unread_bytes -= r;
    lock.unlock();
    not_full.notify_one();
    return r;
  }

  bool try_read(srsue_byte_buffer_t *msg)
  {
    boost::mutex::scoped_lock lock(mutex);
    if(is_empty())
    {
      return false;
    }else{
      *msg = buf[tail];
      tail = (tail+1)%capacity;
      unread--;
      unread_bytes -= msg->N_bytes;
      lock.unlock();
      not_full.notify_one();
      return true;
    }
  }

  uint32_t size()
  {
    boost::mutex::scoped_lock lock(mutex);
    return unread;
  }

  uint32_t size_bytes()
  {
    boost::mutex::scoped_lock lock(mutex);
    return unread_bytes;
  }

  uint32_t size_tail_bytes()
  {
    boost::mutex::scoped_lock lock(mutex);
    return buf[tail].N_bytes;
  }

private:
  bool     is_empty() const { return unread == 0; }
  bool     is_full() const { return unread == capacity; }

  boost::condition      not_empty;
  boost::condition      not_full;
  boost::mutex          mutex;
  srsue_byte_buffer_t  *buf;
  uint32_t              capacity;
  uint32_t              unread;
  u_int32_t             unread_bytes;
  uint32_t              head;
  uint32_t              tail;
};

} // namespace srsue


#endif // MSG_QUEUE_H