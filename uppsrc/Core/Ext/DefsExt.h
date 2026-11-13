inline bool IsClose(double a, double b)
{
	if(IsFin(a) && IsFin(b))
		return fabs(a - b) < 0.001;
	return *(const uint64 *)&a == *(const uint64 *)&b;
}
