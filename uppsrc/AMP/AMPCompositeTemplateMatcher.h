#ifndef _AMP_AMPCompositeTemplateMatcher_h_
#define _AMP_AMPCompositeTemplateMatcher_h_

struct AmpCompositeTemplateInput : Moveable<AmpCompositeTemplateInput> {
	String id;
	String kind = "template";
	String scale = "1";
	String preprocessing = "gray";
	float  threshold = 0;
	Image  image;
};

struct AmpCompositeTemplateMatcherOptions : Moveable<AmpCompositeTemplateMatcherOptions> {
	// Minimum accepted TM_CCOEFF_NORMED correlation. The default accepts all
	// valid scores in [-1, 1].
	double threshold = -1.0;
	String backend = "cpu"; // cpu or native-amp
	String device_path;
};

// Stateful, composite-atlas matcher. Prepare packs every supplied template
// into one immutable atlas once; Match then evaluates one frame against all
// atlas entries using TM_CCOEFF_NORMED. The composite matcher currently
// accepts gray preprocessing only: its live card/dealer callers are grayscale.
class AmpCompositeTemplateMatcher {
public:
	AmpCompositeTemplateMatcher() = default;
	~AmpCompositeTemplateMatcher();
	AmpCompositeTemplateMatcher(const AmpCompositeTemplateMatcher&) = delete;
	void operator=(const AmpCompositeTemplateMatcher&) = delete;

	bool Prepare(const Vector<AmpCompositeTemplateInput>& templates,
	             const AmpCompositeTemplateMatcherOptions& options, String& error);
	bool Match(const Image& frame, AmpTemplateMatchResult& result, String& error) const;
	bool MatchPixels(const AmpTemplatePixelBuffer& frame,
	                 AmpTemplateMatchResult& result, String& error) const;

	bool IsPrepared() const { return prepared_; }
	const AmpTemplateAtlasManifest& GetManifest() const { return manifest_; }
	const AmpTemplatePixelBuffer& GetAtlas() const { return atlas_; }

private:
	AmpTemplatePixelBuffer atlas_;
	AmpTemplateAtlasManifest manifest_;
	AmpCompositeTemplateMatcherOptions options_;
	Vector<int> template_sum_;
	Vector<int> template_sum_sq_;
	bool prepared_ = false;
#ifdef HAVE_SYSTEM_AMP
	struct NativeState;
	NativeState* native_ = nullptr;
#endif
};

#endif
