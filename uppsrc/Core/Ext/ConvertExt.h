template<class T>
String BinStr(const T& o, const char* chrs = ".|")
{
	String s;
	const byte *b = (const byte *)&o + sizeof(T) - 1;
	for(int i = (int)sizeof(T) - 1; i >= 0; i--, b--)
		for(int j = 7; j >= 0; j--)
			s.Cat(chrs[(*b & (1 << j)) ? 1 : 0]);
	return s;
}
