#pragma once
#include "atomic-stack.hpp"
#include "memory.h"
#include "atomic.h"

#include <memory>
#include <cassert>

namespace bf {
struct MallocInfo : atomic::Node {
	MallocInfo(int size) : alloc_size(size), extra_info(-1) {
	}
	long alloc_size;
	long extra_info;
};

inline void* AttachMallocInfo(MallocInfo* header, int size) {
	new (header)MallocInfo(size);
	return reinterpret_cast<void*>(++header);
}

inline MallocInfo* DetachMallocInfo(void* ptr) {
	MallocInfo* header = reinterpret_cast<MallocInfo*>(ptr);
	--header;
	return header;
}

class MemoryBucket {
public:
	MemoryBucket(unsigned long size);

	MallocInfo* Pop();
	void Push(MallocInfo* ptr);

private:
	atomic::Stack free_list;

	const unsigned long alloc_size;
	long alloc_count;
};

/// global�θ� ����ϱ� ������ ���ϰ� �Ҹ��� ���� ����
class MemoryPool {
public:
	MemoryPool();

	void* Allocate(int size);
	void Deallocate(void* ptr, long extra_info);

private:
	/// todo : ������ �� �����ؾ���
	enum Config {
		/// �Ժη� �ٲٸ� �ȵ�. ö���� ����� �ٲ� ��
		MAX_BUCKET_COUNT = 1024 / 32 + 1024 / 128 + 2048 / 256, ///< ~1024���� 32����, ~2048���� 128����, ~4096���� 256����
		MAX_ALLOC_SIZE = 4096
	};

	/// ���ϴ� ũ���� �޸𸮸� ������ �ִ� Ǯ�� O(1) access�� ���� ���̺�
	MemoryBucket* bucket_table[MAX_ALLOC_SIZE + 1];
};

extern MemoryPool* Pool;
/// ����� ��� �޾ƾ߸� xnew/xdelete ����� �� �ְ�...
//struct PooledAllocatable {};

template <class T, class... Args>
T* xnew(Args&&... args) {
	//static_assert(true == std::is_convertible<T, PooledAllocatable>::value, "[fs::memory-pool.h] only allowed when PooledAllocatable");

	void* alloc = Pool->Allocate(sizeof(T));
	new (alloc)T(std::forward<Args>(args)...);
	return reinterpret_cast<T*>(alloc);
}

template <class T>
void xdelete(T* object) {
	//static_assert(true == std::is_convertible<T, PooledAllocatable>::value, "[fs::memory-pool.h] only allowed when PooledAllocatable");

	object->~T();
	Pool->Deallocate(object, sizeof(T));
}
}