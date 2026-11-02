#ifndef _AMP_AMPTemplateMatcher_h_
#define _AMP_AMPTemplateMatcher_h_

struct AmpTemplateMatchHit : Moveable<AmpTemplateMatchHit> {
	String id;
	int entry_index = -1;
	int x = -1;
	int y = -1;
	// Legacy AMP template APIs use SAD, where lower is better. The composite
	// CCOEFF matcher explicitly initializes its own higher-is-better sentinels.
	double score = DBL_MAX;
	bool accepted = false;
	// Task 0339: true when this prepared entry could not be evaluated at ANY
	// position because the entry's width/height exceeds the frame's -- a size
	// mismatch against one template scale, never a reason to fail the whole
	// Match() call. Still counted in `rejected` (unchanged semantics); this
	// flag lets a caller distinguish "ran but scored low" from "never fit".
	bool skipped_oversized = false;
};

struct AmpTemplateMatchResult : Moveable<AmpTemplateMatchResult> {
	Vector<AmpTemplateMatchHit> entries;
	int evidence_width = 0;
	int evidence_height = 0;
	Vector<int> evidence_rgb;
	int winner_index = -1;
	double winner_score = DBL_MAX;
	int accepted = 0;
	int rejected = 0;
	int candidate_count = 0;
	int checksum = 0;
	// Task 0339: count of entries with skipped_oversized == true (subset of
	// `rejected`). Both backends must agree: a size mismatch degrades this one
	// entry, never the whole call.
	int skipped = 0;
};

bool MatchAmpTemplatePixelsCpu(const AmpTemplatePixelBuffer& frame,
	                           const AmpTemplatePixelBuffer& atlas,
	                           const AmpTemplateAtlasManifest& manifest,
                           int threshold, AmpTemplateMatchResult& result,
                           String& error);

bool MatchAmpTemplatePixelsAmp(const AmpTemplatePixelBuffer& frame,
	                           const AmpTemplatePixelBuffer& atlas,
	                           const AmpTemplateAtlasManifest& manifest,
	                           int threshold, const String& device_path,
	                           AmpTemplateMatchResult& result, String& error);

bool MatchAmpTemplatesCpu(const Vector<int>& frame, int frame_width, int frame_height,
                      const Vector<int>& atlas, const AmpTemplateAtlasManifest& manifest,
                      int threshold, AmpTemplateMatchResult& result, String& error);

bool MatchAmpTemplatesAmp(const Vector<int>& frame, int frame_width, int frame_height,
                      const Vector<int>& atlas, const AmpTemplateAtlasManifest& manifest,
                      int threshold, const String& device_path,
                      AmpTemplateMatchResult& result, String& error);

#endif
