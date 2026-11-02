#ifndef _Core_Ext_OtherExt_h_
#define _Core_Ext_OtherExt_h_

#include <Core/Core.h>
#include <optional>
#include <typeindex>

NAMESPACE_UPP

template <class T>
struct ManagedStatic {
	T o;
	bool destructed = false;
	const char* file;
	int line;
	
	typedef ManagedStatic CLASSNAME;
	ManagedStatic(const char* f, int l);
	template <class Arg> ManagedStatic(const char* f, int l, const Arg& value);
	~ManagedStatic() {
		if (!destructed)
			Destruct();
	}
	void Destruct() {if (!destructed) {Clear(); destructed = true;}}
	void Clear() {o.Clear();}
};

template <class T> ManagedStatic<T>::ManagedStatic(const char* f, int l) : file(f), line(l) {CallInExitBlock([this]{this->Destruct();});}
template <class T> template <class Arg>
ManagedStatic<T>::ManagedStatic(const char* f, int l, const Arg& value) : file(f), line(l), o(value) {CallInExitBlock([this]{this->Destruct();});}

template <class T>
struct ManagedStaticThreadLocal {
	T o;
	bool destructed = false;
	const char* file;
	int line;
	
	typedef ManagedStaticThreadLocal CLASSNAME;
	ManagedStaticThreadLocal(const char* f, int l);
	template <class Arg> ManagedStaticThreadLocal(const char* f, int l, const Arg& value);
	~ManagedStaticThreadLocal() {if (!destructed) Destruct();}
	void Destruct() {if (!destructed) {Clear(); destructed = true;}}
	void Clear() {o.Clear();}
};

template <class T> ManagedStaticThreadLocal<T>::ManagedStaticThreadLocal(const char* f, int l) : file(f), line(l) {}
template <class T> template <class Arg>
ManagedStaticThreadLocal<T>::ManagedStaticThreadLocal(const char* f, int l, const Arg& value) : file(f), line(l), o(value) {}

#define MAKE_STATIC(t, x) static ::UPP::ManagedStatic<t> __##x(__FILE__,__LINE__); t& x = __##x.o;
#define MAKE_STATIC_(t, x, param) static ::UPP::ManagedStatic<t> __##x(__FILE__,__LINE__,param); t& x = __##x.o;
#define MAKE_STATIC_LOCAL(t, x) thread_local static ::UPP::ManagedStaticThreadLocal<t> __##x(__FILE__,__LINE__); t& x = __##x.o;

template <typename T>
class TrackChanges {
public:
	TrackChanges() {
		change_count = 0;
	}
	TrackChanges(TrackChanges<T> && o) : value(std::move(o.value)) {
		change_count = 0;
	}
	TrackChanges(const TrackChanges<T>& o) : value(o.Get()) {
		change_count = 0;
	}
	
	TrackChanges<T>& operator=(TrackChanges<T> && o) {
		++change_count;
		value = std::move(o.value);
		return *this;
	}
	
	TrackChanges<T>& operator=(const TrackChanges<T>& o) {
		++change_count;
		value = o.value;
		return *this;
	}
	
	template <typename Func>
	void Set(Func func) {
		++change_count;
		func(value);
	}
	
	bool UpdateChangeCountBookmark(uint32* change_count_bookmark) const {
		uint32 new_value = *change_count_bookmark;
		uint32 prev = change_count.exchange(new_value);
		return prev != new_value;
	}
	
	const T& Get() const {
		return value;
	}
	
	T* operator->() {return &value;}
	const T* operator->() const {return &value;}
	
	T& operator*() {return value;}
	void operator++() {++change_count;}
private:
	T value;
	mutable Atomic change_count;
};


using NullOpt = std::nullopt_t;

#define null_opt std::nullopt

template <class T> using Optional = std::optional<T>;

//template <class T, class ...Args> std::optional<T> MakeOptional(Args... args) {return std::make_optional(args...);}
template <class T> std::optional<T> MakeOptional(const T& o) {return std::make_optional(o);}



template <class T> void RemoveLast(T& o) {
	int c = o.GetCount();
	if (c > 0)
		o.Remove(c-1);
}

template <class T, int I>
struct FixedArray : Moveable<FixedArray<T,I>> {
	
	static const int size = I;
	
	struct Iterator {
		T* ptr = 0;
		
		Iterator() {}
		Iterator(T* p) : ptr(p) {}
		T* operator->() const {return ptr;}
		T& operator*() const {return *ptr;}
		bool operator!=(const Iterator& i) const {return ptr != i.ptr;}
		bool operator==(const Iterator& i) const {return ptr == i.ptr;}
		void operator++() {++ptr;}
		void operator--() {--ptr;}
	};
	
	T vector[I];
	
	FixedArray() {}
	FixedArray(const Nuller& n) {for(int i = 0; i < I; i++) vector[i] = n;}
	int GetCount() const {return size;}
	T&       operator[](int i)       {ASSERT(i >= 0 && i < size); return vector[i];}
	const T& operator[](int i) const {ASSERT(i >= 0 && i < size); return vector[i];}
	void Clear() {for(int i = 0; i < I; i++) vector[i] = Null;}
	void operator=(const Nuller& n) {for(int i = 0; i < I; i++) vector[i] = n;}
	void operator=(const T& value) {for(int i = 0; i < I; i++) this->vector[i] = value;}
	T& Top() {return vector[I-1];}
	
	Iterator begin() {return Iterator(&vector[0]);}
	Iterator end() {return Iterator(&vector[0] + I);}
	
	bool IsEmpty() const {return false;}
	void SetAll(const T& o) {for(int i = 0; i < I; i++) vector[i] = o;}
	String ToString() const {return "FixedArray";}
	int ToInt() const {return I;}
	double ToDouble() const {return I;}
	operator double() const {return I;}
	
	hash_t GetHashValue() const {CombineHash c; for(int i = 0; i < I; i++) c.Put(UPP::GetHashValue<T>(vector[i])); return c;}
	T* Get() {return vector;}
	
};


struct TypeCls : Moveable<TypeCls>, std::type_index {
	TypeCls() : std::type_index(typeid(void)) {}
	TypeCls(const TypeCls& t) : std::type_index(t) {}
	TypeCls(const std::type_info& t) : std::type_index(t) {}
	operator bool() const {return *this != std::type_index(typeid(void));}
	String GetName() const {return CppDemangle(this->name());}
	hash_t GetHashValue() const {return this->hash_code();}
	String ToString() const {return this->name();}
};

template <class T> TypeCls AsTypeCls(T* o=0) {return typeid(T);}
template <class T> String AsTypeName(T* o=0) {return CppDemangle(typeid(T).name());}
// Note: AsTypeHash is considered to be persistent and cross-platform (& cross-compiler),
//       which currently requires the user to set the type string.
//       If std had demangler or mangled name was standard, this would be unnecessary.
//       Usage: call TypedStringHasher<T>("name of T") early in constructor or main.
//       Use AsTypeCls().GetHashValue() if you need easy & non-persistent hash.
template <class T> hash_t TypedStringHasher(const char* s);
template <class T> hash_t AsTypeHash(T* o=0) {return TypedStringHasher<T>(0);}
inline TypeCls AsVoidTypeCls() {return TypeCls();} // for explicit naming

template <class T> using TypeMap = ArrayMap<TypeCls, T>;



struct TypeStringHasherIndex {
	static VectorMap<hash_t, String>& Map();
	static String ToString(hash_t h);
};

template <class T>
hash_t TypedStringHasher(const char* s) {
	static hash_t h = 0;
	if (!h && s) {
		h = memhash(s, (int)strlen(s));
		TypeStringHasherIndex::Map().GetAdd(h) = s;
	}
	return h;
}

END_UPP_NAMESPACE

#endif
