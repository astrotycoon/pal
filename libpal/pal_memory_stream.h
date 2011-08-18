/*
	Copyright (c) 2011 John McCutchan <john@johnmccutchan.com>

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software. If you use this software
	in a product, an acknowledgment in the product documentation would be
	appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.

	3. This notice may not be removed or altered from any source
	distribution.
*/

#pragma once

#include "libpal/pal_mem_blob.h"
#include "libpal/pal_stream_interface.h"

class palMemoryStream : public palStreamInterface {
  palMemBlob _fixed_blob;
  palGrowingMemoryBlob _growing_blob;
  uint64_t _position;
  bool _own_growing_blob;
  bool _growable;
  bool _can_seek;
  bool _can_read;
  bool _can_write;
public:
  palMemoryStream();
  virtual ~palMemoryStream();

  // fixed size
  int Create(const palMemBlob& blob, bool can_write);
  // growable with initial capacity
  int Create(palAllocatorInterface* allocator, int capacity);

  int Reset();

  void GetBlob(palMemBlob* blob);
  void StealBlob(palMemBlob* blob);

  virtual bool CanRead() const;
  virtual bool CanWrite() const;
  virtual bool CanSeek() const;

  virtual uint64_t GetPosition() const;

  virtual uint64_t GetLength() const;

  virtual void Flush();

  virtual int SetLength(uint64_t length);

  virtual int Seek(int64_t offset, palStreamSeekOrigin origin);
  virtual int Read(void* buffer, uint64_t buffer_offset, uint64_t count_bytes);
  virtual int Write(const void* buffer, uint64_t buffer_offset, uint64_t count_bytes);
  // Stream must be seekable
  virtual int OffsetRead(void* buffer, uint64_t buffer_offset, uint64_t count_bytes, uint64_t stream_offset);
  virtual int OffsetWrite(const void* buffer, uint64_t buffer_offset, uint64_t count_bytes, uint64_t stream_offset);
};