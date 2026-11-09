#include "Skylark.h"

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

};
