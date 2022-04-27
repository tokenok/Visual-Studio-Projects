#pragma once

#include <vector>
#include <iostream>
#include <string>

#include "sorting.h"

class Visualizer;

template <class T, class A = std::allocator<T>>
class TraceVector {
public:
	typedef T value_type;
	typedef T& reference;

	class iterator {
	public:
		typedef T value_type;
		typedef std::ptrdiff_t difference_type;	
		typedef T* pointer;
		typedef T& reference;		
		typedef std::random_access_iterator_tag iterator_category;

		iterator() : vec_{ NULL }, pointer_{ 0 }, vis_{ NULL } {}
		iterator(std::vector<value_type>* vec, Visualizer* vis) : vec_{ vec }, pointer_{ 0 }, vis_{ vis } {}
		iterator(std::vector<value_type>* vec, int size, Visualizer* vis) : vec_{ vec }, pointer_{ size }, vis_{ vis } {}

		iterator& operator=(const iterator& other) { 
			this->vec_ = other.vec_; 
			this->pointer_ = other.pointer_;
			return *this;
		}
		iterator& operator++() { ++pointer_; return *this; }
		iterator operator++(int) { iterator old(*this); ++*this; return old; }
		iterator& operator--() { --pointer_; return *this; }
		iterator operator--(int) { iterator old(*this); --*this; return old; }
		iterator& operator+=(const difference_type& diff) { pointer_ += diff; return *this; }
		iterator& operator-=(const difference_type& diff) { pointer_ -= diff; return *this; }
		iterator operator+(const difference_type& rhs) const { return iterator(vec_, pointer_ + rhs, vis_); }
		iterator operator-(const difference_type& rhs) const { return iterator(vec_, pointer_ - rhs, vis_); }
		difference_type operator-(iterator rhs) const { return pointer_ - rhs.pointer_; }
		difference_type operator+(iterator rhs) const { return pointer_ + rhs.pointer_; }
		reference operator[](size_t _Off) { iterator _It(vec_, pointer_, vis_); _It.pointer_ += _Off; return (*_It); }

		bool operator<(const iterator& rhs) const { return pointer_ < rhs.pointer_; }
		bool operator<=(const iterator& rhs) const { return pointer_ <= rhs.pointer_; }
		bool operator>(const iterator& rhs) const { return pointer_ > rhs.pointer_; }
		bool operator>=(const iterator& rhs) const { return pointer_ >= rhs.pointer_; }
		bool operator==(const iterator& rhs) const { return pointer_ == rhs.pointer_; }
		bool operator!=(const iterator& rhs) const { return !(*this == rhs); }

		//ACCESS
		reference operator*() const {
			if (vis_) {
				if (vis_->options.draw_options & DrawOption::Accesses)
					vis_->Draw(pointer_, Visualizer::DrawEventType::Access);
				++vis_->tv.access_count;
			}
			return vec_->at(pointer_);
		}

		void setVisualizer(Visualizer* vis) { 
			this->vis_ = vis;
		}

		int pointer_;
		std::vector<value_type>* vec_;
		Visualizer* vis_ = NULL;
	};

	TraceVector() {}
	TraceVector(size_t size) {
		vec_.resize(size);
	}

	//ACCESS
	reference operator[](size_t index) {
		if (vis_ && vis_->options.draw_options & DrawOption::Accesses)
			vis_->Draw(index, Visualizer::DrawEventType::Access);
		++access_count;
		return vec_[index];
	}

	TraceVector& operator=(const TraceVector<value_type>& tv) {
		vec_ = tv.vec_;
		return *this;
	}

	TraceVector& operator=(const std::vector<value_type>& vec) { 
		clear();
		for (unsigned i = 0; i < vec.size(); i++)
			push_back(vec[i]);
		return *this; 
	}

	void push_back(value_type value) {
		vec_.push_back(value);
		vec_.back().index_ = vec_.size() - 1;
		vec_.back().sorted_index_ = vec_.size() - 1;
		vec_.back().TV_ = this;
	}
	void resize(size_t new_size) { vec_.resize(new_size); }
	void resize(size_t new_size, value_type val) { vec_.resize(new_size, val); }
	void reserve(size_t new_capacity) { vec_.reserve(new_capacity); }
	void clear() { vec_.clear(); }
	size_t size() { return vec_.size(); }
	iterator begin() { return iterator(&vec_, vis_); }
	iterator end() { return iterator(&vec_, vec_.size(), vis_); }
	reference at(size_t pos) { return vec_.at(pos); }
	reference front() { return vec_.front(); }
	reference back() { return vec_.back(); }
//	const_reference at(size_type pos) const { return vec_.at(pos); }

	void setVisualizer(Visualizer* vis) {
		this->vis_ = vis;
		/*for (auto & it : vec_)
			it.setVisualizer(vis);*/
	//	vec_.front().setVisualizer(vis);
	}

	void reset_counts() {
		access_count = 0;
		swap_count = 0;
		compare_count = 0;
		assignment_count = 0;
	}

	std::vector<value_type> vec_;
	Visualizer* vis_ = NULL;

	unsigned long long access_count = 0;
	unsigned long long swap_count = 0;
	unsigned long long compare_count = 0;
	unsigned long long assignment_count = 0;
};

class TraceInt {
public:
	unsigned value = 0;

	unsigned index_ = 0;
	unsigned sorted_index_ = 0;
	TraceVector<TraceInt>* TV_ = NULL;

	constexpr TraceInt() noexcept : value() {}
	constexpr TraceInt(const unsigned & v) noexcept : value(v) {}
	TraceInt(const TraceInt& other) {
		this->value = other.value;
		this->index_ = other.index_;
		this->sorted_index_ = other.sorted_index_;
		this->TV_ = other.TV_;
	}

	constexpr const unsigned& get() const noexcept { return value; }

	TraceInt& operator=(const TraceInt& rhs);
	bool operator<(const TraceInt& rhs) const;
	bool operator>(const TraceInt& rhs) const;

	TraceInt& operator++() { ++value; return *this; }
	TraceInt operator++(int) { TraceInt tmp(*this); operator++(); return tmp; }
	TraceInt& operator--() { --value; return *this; }
	TraceInt operator--(int) { TraceInt tmp(*this); operator--(); return tmp; }
	TraceInt& operator+=(const TraceInt& rhs) { value += rhs.value; return *this; }
	TraceInt& operator-=(const TraceInt& rhs) { value -= rhs.value; return *this; }
	TraceInt& operator/=(const TraceInt& rhs) { value /= rhs.value; return *this; }
	operator unsigned() const { return value; }
	operator unsigned() { return value; }

	friend constexpr TraceInt operator+(const TraceInt& lhs, const unsigned& rhs) noexcept { return TraceInt(lhs.get() + rhs); }
	friend constexpr TraceInt operator+(const unsigned& lhs, const TraceInt& rhs) noexcept { return TraceInt(lhs + rhs.get()); }
	friend constexpr TraceInt operator+(const TraceInt& lhs, const TraceInt& rhs) noexcept { return TraceInt(lhs.get() + rhs.get()); }
	friend constexpr TraceInt operator-(const TraceInt& lhs, const unsigned& rhs) noexcept { return TraceInt(lhs.get() - rhs); }
	friend constexpr TraceInt operator-(const unsigned& lhs, const TraceInt& rhs) noexcept { return TraceInt(lhs - rhs.get()); }
	friend constexpr TraceInt operator-(const TraceInt& lhs, const TraceInt& rhs) noexcept { return TraceInt(lhs.get() - rhs.get()); }

	friend constexpr TraceInt operator+(const TraceInt& lhs, const int& rhs) noexcept { return TraceInt(lhs.get() + rhs); }
	friend constexpr TraceInt operator+(const int& lhs, const TraceInt& rhs) noexcept { return TraceInt(lhs + rhs.get()); }	
	friend constexpr TraceInt operator-(const TraceInt& lhs, const int& rhs) noexcept { return TraceInt(lhs.get() - rhs); }
	friend constexpr TraceInt operator-(const int& lhs, const TraceInt& rhs) noexcept { return TraceInt(lhs - rhs.get()); }

	friend std::ostream& operator<<(std::ostream& os, const TraceInt& ti) { os << ti.value;	return os; }
};

void swap(TraceInt& lhs, TraceInt& rhs);

void iter_swap(const TraceVector<TraceInt>::iterator& _Left, const TraceVector<TraceInt>::iterator& _Right);

//This can be removed, it doesn't do anything anymore
//template<class _Ty = void>
//struct TraceLess {
//	constexpr bool operator()(const _Ty& _Left, const _Ty& _Right) {
//		//	printf("%d < %d\n", _Left, _Right);
//		/*if (_Left.TV_->vis_ && _Left.TV_->vis_->draw_options & DrawOption::Comparisons)
//			_Left.TV_->vis_->Draw(_Left.pointer_, _Right.pointer_, Visualizer::DrawEventType::Comparison);*/
//		//++_Left.TV_->compare_count;
//		//		++g_compare_count;
//		return (_Left < _Right);
//	}
//};

//template<>
//struct TraceLess<void> {	// transparent functor for operator<
//	typedef int is_transparent;
//
//	template<class _Ty1,
//		class _Ty2>
//		constexpr auto operator()(_Ty1&& _Left, _Ty2&& _Right) const
//		-> decltype(static_cast<_Ty1&&>(_Left)
//					< static_cast<_Ty2&&>(_Right)) {	// transparently apply operator< to operands
//		return (static_cast<_Ty1&&>(_Left)
//				< static_cast<_Ty2&&>(_Right));
//	}
//};


























