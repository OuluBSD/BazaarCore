inline String         HexStr(int i)       { return Format("0x%X", i); }
inline String         HexStr64(int64 i)   { return Format("0x%X", i); }
#if CPU_64
inline String         HexStrPtr(void *i)  { return Format("0x%X", (int64)i); }
#else
inline String         HexStrPtr(void *i)  { return Format("0x%X", (int)i); }
#endif
template <class T>
String HexStrPtr(const T *o) { return HexStrPtr((void *)o); }
