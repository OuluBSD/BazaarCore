#ifndef _Core_DiskBlobCache_h_
#define _Core_DiskBlobCache_h_

// Generic disk-backed key -> blob cache with size-bounded LRU eviction.
// Deliberately content-agnostic: the caller owns key naming (a relative
// path under the cache root - '/' in a key creates subdirectories) and
// blob (de)serialization. Pair with AppRegistry::GetCacheDir() for a
// ready-made, per-app/per-profile cache root (see ai/skills/app-registry.md).
//
// LRU is tracked via each file's mtime, refreshed on both Load and Save,
// rather than a separate index file - simplest thing that works, and the
// filesystem is already doing the bookkeeping. Revisit only if profiling
// shows the directory walk in EvictLru is too slow at real cache sizes.
class DiskBlobCache {
	String root;

public:
	void   Open(const String& root_dir);
	bool   IsOpen() const                        { return !root.IsEmpty(); }

	String GetRoot() const                       { return root; }
	String GetPath(const String& key) const;

	bool   Load(const String& key, String& data_out) const;
	void   Save(const String& key, const String& data);
	// Removes one specific entry, e.g. because the caller knows its
	// source data changed and a stale cached blob must not be served
	// again. Returns false if the key had no cached entry to begin with.
	bool   Delete(const String& key);

	// Deletes oldest-mtime files first until the remaining total size is
	// <= max_total_bytes (or nothing is left). Returns bytes freed.
	int64  EvictLru(int64 max_total_bytes);

	// Sum of all cached file sizes under the root. O(file count) - a
	// directory walk, not tracked incrementally.
	int64  GetTotalSize() const;
};

#endif
