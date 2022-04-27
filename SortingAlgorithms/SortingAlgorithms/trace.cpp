#include "trace.h"
#include "Visualizer.h"


//ASSIGNMENT
TraceInt& TraceInt::operator=(const TraceInt& rhs) {
	this->value = rhs.value;
	this->sorted_index_ = rhs.sorted_index_;
	this->TV_ = rhs.TV_;
	if (TV_ && TV_->vis_ && TV_->vis_->options.draw_options & DrawOption::Assignments)
		TV_->vis_->Draw(this->index_, Visualizer::DrawEventType::Assignment);
	
	if (TV_) ++TV_->assignment_count;
	return *this;
}

//COMPARISON
bool TraceInt::operator<(const TraceInt& rhs) const {
	if (TV_ && TV_->vis_ && TV_->vis_->options.draw_options & DrawOption::Comparisons)
		TV_->vis_->Draw(this->index_, rhs.index_, Visualizer::DrawEventType::Comparison);
	if (TV_ ) ++TV_->compare_count;
	return this->value < rhs.value;
}

//COMPARISON
bool TraceInt::operator>(const TraceInt& rhs) const {
	if (TV_ && TV_->vis_ && TV_->vis_->options.draw_options & DrawOption::Comparisons)
		TV_->vis_->Draw(this->index_, rhs.index_, Visualizer::DrawEventType::Comparison);
	if (TV_) ++TV_->compare_count;
	return this->value < rhs.value;
}

//SWAP
void swap(TraceInt& lhs, TraceInt& rhs) {
	if (lhs.TV_ && lhs.TV_->vis_ && lhs.TV_->vis_->options.draw_options & DrawOption::PreSwaps)
		lhs.TV_->vis_->Draw(lhs.index_, rhs.index_, Visualizer::DrawEventType::Swap);
	if (lhs.TV_) ++lhs.TV_->swap_count;
	std::swap(lhs, rhs);
	if (lhs.TV_ && lhs.TV_->vis_ && lhs.TV_->vis_->options.draw_options & DrawOption::PostSwaps)
		lhs.TV_->vis_->Draw(lhs.index_, rhs.index_, Visualizer::DrawEventType::Swap);
}

//SWAP
void iter_swap(const TraceVector<TraceInt>::iterator& _Left, const TraceVector<TraceInt>::iterator& _Right) {
	//printf("swap(%d(%d), %d(%d))\n", left.pointer_, left.vec_[left.pointer_], right.pointer_, right.vec_[right.pointer_]);
	if (_Left.vis_ && _Left.vis_->options.draw_options & DrawOption::PreSwaps)
		_Left.vis_->Draw(_Left.pointer_, _Right.pointer_, Visualizer::DrawEventType::Swap);
	if (_Left.vis_) ++_Left.vis_->tv.swap_count;
	std::swap(*_Left, *_Right);
	if (_Left.vis_ && _Left.vis_->options.draw_options & DrawOption::PostSwaps)
		_Left.vis_->Draw(_Left.pointer_, _Right.pointer_, Visualizer::DrawEventType::Swap);
}






































