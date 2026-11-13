template<class T> inline bool IsPositive(const T& o);
template<>        inline bool IsPositive(const Size& o) { return o.cx > 0 && o.cy > 0; }

inline bool IsFilePosLess(const Point& a, const Point& b)
{
	return a.y != b.y ? a.y < b.y : a.x < b.x;
}

inline bool IsFilePosLessOrEqual(const Point& a, const Point& b)
{
	return a.y != b.y ? a.y <= b.y : a.x <= b.x;
}

inline bool IsFilePosGreater(const Point& a, const Point& b)
{
	return a.y != b.y ? a.y > b.y : a.x > b.x;
}

inline bool IsRangesOverlapping(const Point& begin0, const Point& end0,
	                              const Point& begin1, const Point& end1)
{
	bool begin_less = IsFilePosLessOrEqual(begin0, begin1);
	bool end_greater = IsFilePosLessOrEqual(end1, end0);
	bool begin1_less = IsFilePosLess(begin1, end0);
	bool end1_greater = IsFilePosLess(begin0, end1);
	bool begin1_in_between = begin_less && begin1_less;
	bool end1_in_between = end_greater && end1_greater;
	bool is_1_subset = begin_less && end_greater;
	bool is_0_subset = !begin_less && !end_greater;
	return begin1_in_between || end1_in_between || is_1_subset || is_0_subset;
}

inline bool IsSubset(const Point& begin0, const Point& end0,
	                   const Point& begin1, const Point& end1)
{
	return IsFilePosLessOrEqual(begin0, begin1) && IsFilePosLessOrEqual(end1, end0);
}
