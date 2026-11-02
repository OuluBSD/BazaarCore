#include "Ext.h"

namespace Upp {

void DiskBlobCache::Open(const String& root_dir)
{
	root = root_dir;
	RealizeDirectory(root);
}

String DiskBlobCache::GetPath(const String& key) const
{
	return AppendFileName(root, key);
}

bool DiskBlobCache::Load(const String& key, String& data_out) const
{
	String path = GetPath(key);
	if(!FileExists(path))
		return false;
	data_out = LoadFile(path);
	FileSetTime(path, GetSysTime()); // touch: this entry was just used
	return true;
}

void DiskBlobCache::Save(const String& key, const String& data)
{
	String path = GetPath(key);
	RealizeDirectory(GetFileDirectory(path));
	SaveFile(path, data);
}

bool DiskBlobCache::Delete(const String& key)
{
	String path = GetPath(key);
	if(!FileExists(path))
		return false;
	return FileDelete(path);
}

struct DiskBlobCacheEntry : Moveable<DiskBlobCacheEntry> {
	String path;
	int64  size;
	Time   mtime;
};

static void DiskBlobCacheWalk(const String& dir, Vector<DiskBlobCacheEntry>& out)
{
	for(FindFile ff(AppendFileName(dir, "*")); ff; ff.Next()) {
		if(ff.GetName() == "." || ff.GetName() == "..")
			continue;
		String path = AppendFileName(dir, ff.GetName());
		if(ff.IsFolder())
			DiskBlobCacheWalk(path, out);
		else if(ff.IsFile()) {
			DiskBlobCacheEntry& e = out.Add();
			e.path = path;
			e.size = ff.GetLength();
			e.mtime = FileGetTime(path);
		}
	}
}

int64 DiskBlobCache::EvictLru(int64 max_total_bytes)
{
	Vector<DiskBlobCacheEntry> entries;
	DiskBlobCacheWalk(root, entries);
	Sort(entries, [](const DiskBlobCacheEntry& a, const DiskBlobCacheEntry& b) { return a.mtime < b.mtime; });

	int64 total = 0;
	for(const DiskBlobCacheEntry& e : entries)
		total += e.size;

	int64 freed = 0;
	for(const DiskBlobCacheEntry& e : entries) {
		if(total <= max_total_bytes)
			break;
		if(FileDelete(e.path)) {
			total -= e.size;
			freed += e.size;
		}
	}
	return freed;
}

int64 DiskBlobCache::GetTotalSize() const
{
	Vector<DiskBlobCacheEntry> entries;
	DiskBlobCacheWalk(root, entries);
	int64 total = 0;
	for(const DiskBlobCacheEntry& e : entries)
		total += e.size;
	return total;
}

}
