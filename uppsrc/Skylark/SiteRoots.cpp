#include "Skylark.h"

#ifdef PLATFORM_POSIX
#include <stdlib.h>
#include <limits.h>
#endif

namespace Upp {

struct SkylarkSiteRootsState {
	bool   overridden = false;
	String template_root;
	String static_root;
};

static thread__ SkylarkSiteRootsState *t_site_roots = NULL;

void SkylarkSiteRoots::Set(const String& template_root, const String& static_root)
{
	Clear();
	t_site_roots = new SkylarkSiteRootsState;
	t_site_roots->overridden = true;
	t_site_roots->template_root = template_root;
	t_site_roots->static_root = static_root;
}

void SkylarkSiteRoots::Clear()
{
	delete t_site_roots;
	t_site_roots = NULL;
}

bool SkylarkSiteRoots::IsOverridden()
{
	return t_site_roots && t_site_roots->overridden;
}

String SkylarkSiteRoots::TemplateRoot()
{
	if(IsOverridden() && t_site_roots->template_root.GetCount())
		return t_site_roots->template_root;
	return SkylarkApp::Config().path;
}

String SkylarkSiteRoots::StaticRoot()
{
	if(IsOverridden() && t_site_roots->static_root.GetCount())
		return t_site_roots->static_root;
	return SkylarkApp::Config().path;
}

// A request path is accepted only as plain relative segments: no leading '/',
// no backslash, no NUL, no empty, "." or ".." segment.
static bool SafeRelPath(const String& rel)
{
	if(rel.IsEmpty() || rel[0] == '/' || rel.Find('\\') >= 0 || rel.Find('\0') >= 0)
		return false;
	for(const String& seg : Split(rel, '/', false))
		if(seg.IsEmpty() || seg == "." || seg == "..")
			return false;
	return true;
}

// Only these extensions are served from a site root. Templates (.witz), HTML
// and anything executable are never served as static content.
static String StaticMimeOf(const String& ext)
{
	static const char *table[][2] = {
		{ ".css", "text/css" }, { ".js", "text/javascript" },
		{ ".png", "image/png" }, { ".jpg", "image/jpeg" }, { ".jpeg", "image/jpeg" },
		{ ".gif", "image/gif" }, { ".webp", "image/webp" }, { ".svg", "image/svg+xml" },
		{ ".ico", "image/x-icon" }, { ".woff", "font/woff" }, { ".woff2", "font/woff2" },
		{ ".txt", "text/plain" }, { ".json", "application/json" },
	};
	for(const auto& row : table)
		if(ext == row[0])
			return row[1];
	return String();
}

// Resolves symlinks where the platform allows it; elsewhere the lexical path is used.
static String RealPathOf(const String& path)
{
#ifdef PLATFORM_POSIX
	char buf[PATH_MAX];
	if(!realpath(path, buf))
		return String();
	return buf;
#else
	return NormalizePath(path);
#endif
}

bool SkylarkSiteRoots::ResolveStatic(const String& rel, String& full, String& mime)
{
	if(!SafeRelPath(rel))
		return false;
	mime = StaticMimeOf(ToLower(GetFileExt(rel)));
	if(mime.IsEmpty())
		return false;
	String root = StaticRoot();
	String candidate = AppendFileName(root, rel);
	if(!FileExists(candidate))
		return false;
	String real_root = RealPathOf(root);
	String real_file = RealPathOf(candidate);
	if(real_root.IsEmpty() || real_file.IsEmpty() || !real_file.StartsWith(real_root + "/"))
		return false;
	full = real_file;
	return true;
}

};
