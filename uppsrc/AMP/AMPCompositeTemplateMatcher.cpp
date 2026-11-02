#include "AMP.h"

NAMESPACE_UPP

#ifdef HAVE_SYSTEM_AMP
static concurrency::accelerator GetNativeAmpDeviceZero(const String& device_path)
{
	// Do not enumerate devices here. On this setup the native C++ AMP runtime
	// must use its default accelerator (device index 0) directly.
	return device_path.IsEmpty() ? concurrency::accelerator()
	                            : concurrency::accelerator(device_path.ToWString().ToStd());
}

struct AmpCompositeTemplateMatcher::NativeState {
	concurrency::accelerator accelerator;
	concurrency::accelerator_view view;
	concurrency::array<int, 1> atlas_rgb;
	concurrency::array<int, 1> atlas_gray;
	concurrency::array<int, 1> atlas_contrast;
	concurrency::array<int, 1> entry_x;
	concurrency::array<int, 1> entry_y;
	concurrency::array<int, 1> entry_width;
	concurrency::array<int, 1> entry_height;
	concurrency::array<int, 1> entry_mode;
	concurrency::array<int, 1> entry_otsu_threshold;
	concurrency::array<int, 1> entry_sum;
	concurrency::array<int, 1> entry_sum_sq;

	NativeState(const AmpTemplatePixelBuffer& atlas, const AmpTemplateAtlasManifest& manifest,
	            const String& device_path)
		: accelerator(GetNativeAmpDeviceZero(device_path))
		, view(accelerator.default_view)
		, atlas_rgb(concurrency::extent<1>(atlas.rgb.GetCount()), view)
		, atlas_gray(concurrency::extent<1>(atlas.gray.GetCount()), view)
		, atlas_contrast(concurrency::extent<1>(atlas.local_contrast.GetCount()), view)
		, entry_x(concurrency::extent<1>(manifest.entries.GetCount()), view)
		, entry_y(concurrency::extent<1>(manifest.entries.GetCount()), view)
		, entry_width(concurrency::extent<1>(manifest.entries.GetCount()), view)
		, entry_height(concurrency::extent<1>(manifest.entries.GetCount()), view)
		, entry_mode(concurrency::extent<1>(manifest.entries.GetCount()), view)
		, entry_otsu_threshold(concurrency::extent<1>(manifest.entries.GetCount()), view)
		, entry_sum(concurrency::extent<1>(manifest.entries.GetCount()), view)
		, entry_sum_sq(concurrency::extent<1>(manifest.entries.GetCount()), view)
	{
		Vector<int> xs, ys, widths, heights, modes, otsu_thresholds, sums, sum_sqs;
		for(const AmpTemplateAtlasEntry& entry : manifest.entries) {
			AmpTemplatePreprocessing mode = AMP_TEMPLATE_GRAY;
			String error;
			ASSERT(ParseAmpTemplatePreprocessing(entry.preprocessing, mode, error));
			xs.Add(entry.x);
			ys.Add(entry.y);
			widths.Add(entry.width);
			heights.Add(entry.height);
			modes.Add((int)mode);
			otsu_thresholds.Add(mode == AMP_TEMPLATE_OTSU
			                    ? AmpOtsuThresholdCrop(atlas.gray, atlas.width, entry.x, entry.y,
			                                           entry.width, entry.height)
			                    : 0);
			int sum = 0, sum_sq = 0;
			for(int ty = 0; ty < entry.height; ty++)
				for(int tx = 0; tx < entry.width; tx++) {
					int value = atlas.gray[(entry.y + ty) * atlas.width + entry.x + tx];
					sum += value;
					sum_sq += value * value;
				}
			sums.Add(sum);
			sum_sqs.Add(sum_sq);
		}
		concurrency::copy(atlas.rgb.Begin(), atlas.rgb.End(), atlas_rgb);
		concurrency::copy(atlas.gray.Begin(), atlas.gray.End(), atlas_gray);
		concurrency::copy(atlas.local_contrast.Begin(), atlas.local_contrast.End(), atlas_contrast);
		concurrency::copy(xs.Begin(), xs.End(), entry_x);
		concurrency::copy(ys.Begin(), ys.End(), entry_y);
		concurrency::copy(widths.Begin(), widths.End(), entry_width);
		concurrency::copy(heights.Begin(), heights.End(), entry_height);
		concurrency::copy(modes.Begin(), modes.End(), entry_mode);
		concurrency::copy(otsu_thresholds.Begin(), otsu_thresholds.End(), entry_otsu_threshold);
		concurrency::copy(sums.Begin(), sums.End(), entry_sum);
		concurrency::copy(sum_sqs.Begin(), sum_sqs.End(), entry_sum_sq);
	}
};
#endif

AmpCompositeTemplateMatcher::~AmpCompositeTemplateMatcher()
{
#ifdef HAVE_SYSTEM_AMP
	delete native_;
#endif
}

bool AmpCompositeTemplateMatcher::Prepare(const Vector<AmpCompositeTemplateInput>& templates,
	                                          const AmpCompositeTemplateMatcherOptions& options,
	                                          String& error)
{
	prepared_ = false;
#ifdef HAVE_SYSTEM_AMP
	delete native_;
	native_ = nullptr;
#endif
	atlas_ = AmpTemplatePixelBuffer();
	manifest_ = AmpTemplateAtlasManifest();
	options_ = options;
	if(templates.IsEmpty()) {
		error = "composite matcher needs at least one template";
		return false;
	}
	if(options_.threshold < -1.0 || options_.threshold > 1.0) {
		error = "CCOEFF threshold must be in [-1, 1]";
		return false;
	}
	int width = 0, height = 0;
	Index<String> ids;
	for(const AmpCompositeTemplateInput& input : templates) {
		if(input.id.IsEmpty() || input.image.IsEmpty()) {
			error = "template id or image is empty";
			return false;
		}
		if(!input.preprocessing.IsEmpty() && input.preprocessing != "gray") {
			error = "composite CCOEFF matcher currently supports gray preprocessing only";
			return false;
		}
		if(ids.Find(input.id) >= 0) {
			error = "duplicate template id: " + input.id;
			return false;
		}
		ids.Add(input.id);
		width = max(width, input.image.GetWidth());
		height += input.image.GetHeight();
	}
	ImageBuffer atlas_image(width, height);
	for(int y = 0; y < height; y++)
		for(int x = 0; x < width; x++)
			atlas_image[y][x] = RGBA{ 0, 0, 0, 255 };
	manifest_.atlas_name = "prepared-composite-template-atlas";
	manifest_.atlas_width = width;
	manifest_.atlas_height = height;
	int y = 0;
	for(const AmpCompositeTemplateInput& input : templates) {
		AmpTemplateAtlasEntry& entry = manifest_.entries.Add();
		entry.id = input.id;
		entry.kind = input.kind.IsEmpty() ? "template" : input.kind;
		entry.scale = input.scale.IsEmpty() ? "1" : input.scale;
		entry.x = 0;
		entry.y = y;
		entry.width = input.image.GetWidth();
		entry.height = input.image.GetHeight();
		entry.preprocessing = input.preprocessing.IsEmpty() ? "gray" : input.preprocessing;
		entry.threshold = input.threshold;
		for(int row = 0; row < entry.height; row++) {
			const RGBA* src = input.image[row];
			RGBA* dst = atlas_image[y + row];
			for(int x = 0; x < entry.width; x++)
				dst[x] = src[x];
		}
		y += entry.height;
	}
	if(!manifest_.Validate(error) || !BuildAmpPixelBuffer((Image)atlas_image, atlas_, error))
		return false;
	template_sum_.SetCount(manifest_.entries.GetCount(), 0);
	template_sum_sq_.SetCount(manifest_.entries.GetCount(), 0);
	for(int entry_index = 0; entry_index < manifest_.entries.GetCount(); entry_index++) {
		const AmpTemplateAtlasEntry& entry = manifest_.entries[entry_index];
		for(int ty = 0; ty < entry.height; ty++)
			for(int tx = 0; tx < entry.width; tx++) {
				int value = atlas_.gray[(entry.y + ty) * atlas_.width + entry.x + tx];
				template_sum_[entry_index] += value;
				template_sum_sq_[entry_index] += value * value;
			}
	}
	if(options_.backend == "native-amp") {
#ifndef HAVE_SYSTEM_AMP
		error = "native-amp requested, but this build has no native C++ AMP support";
		return false;
#else
		try {
			native_ = new NativeState(atlas_, manifest_, options_.device_path);
		}
		catch(...) {
			error = "could not create native AMP accelerator state";
			return false;
		}
#endif
	}
	prepared_ = true;
	return true;
}

bool AmpCompositeTemplateMatcher::Match(const Image& frame, AmpTemplateMatchResult& result,
	                                       String& error) const
{
	AmpTemplatePixelBuffer pixels;
	if(!BuildAmpPixelBuffer(frame, pixels, error))
		return false;
	return MatchPixels(pixels, result, error);
}

bool AmpCompositeTemplateMatcher::MatchPixels(const AmpTemplatePixelBuffer& frame,
	                                             AmpTemplateMatchResult& result, String& error) const
{
	if(!prepared_) {
		error = "composite matcher is not prepared";
		return false;
	}
	if(!frame.IsValid()) {
		error = "composite matcher received invalid frame";
		return false;
	}
	auto finish_cpu = [&]() {
		result = AmpTemplateMatchResult();
		result.winner_score = -DBL_MAX;
		for(int entry_index = 0; entry_index < manifest_.entries.GetCount(); entry_index++) {
			const AmpTemplateAtlasEntry& entry = manifest_.entries[entry_index];
			AmpTemplateMatchHit& hit = result.entries.Add();
			hit.id = entry.id;
			hit.entry_index = entry_index;
			hit.score = -DBL_MAX;
			if(entry.width > frame.width || entry.height > frame.height) {
				// This one prepared scale doesn't fit this frame: the sliding-window
				// loops below simply won't execute (0 iterations), so the hit stays
				// unaccepted. Recording it distinctly lets a caller diagnose "never
				// fit" instead of "scored low" -- it must never abort the whole call.
				hit.skipped_oversized = true;
				result.skipped++;
			}
			const int n = entry.width * entry.height;
			const double inv_n = 1.0 / n;
			const double var_t = max((double)template_sum_sq_[entry_index] -
			                         template_sum_[entry_index] * (double)template_sum_[entry_index] * inv_n, 0.0);
			for(int y = 0; y + entry.height <= frame.height; y++)
				for(int x = 0; x + entry.width <= frame.width; x++) {
					int sum_i = 0, sum_ii = 0, sum_it = 0;
					for(int ty = 0; ty < entry.height; ty++)
						for(int tx = 0; tx < entry.width; tx++) {
							int a = frame.gray[(y + ty) * frame.width + x + tx];
							int b = atlas_.gray[(entry.y + ty) * atlas_.width + entry.x + tx];
							sum_i += a; sum_ii += a * a; sum_it += a * b;
						}
					const double var_i = max((double)sum_ii - sum_i * (double)sum_i * inv_n, 0.0);
					const double denom = sqrt(var_i * var_t);
					double score = denom > 1e-12
					             ? ((double)sum_it - sum_i * (double)template_sum_[entry_index] * inv_n) / denom
					             : 0.0;
					score = clamp(score, -1.0, 1.0);
					result.candidate_count++;
					if(score > hit.score) { hit.score = score; hit.x = x; hit.y = y; }
				}
			hit.accepted = hit.score >= options_.threshold;
			if(hit.accepted) {
				result.accepted++;
				if(hit.score > result.winner_score) { result.winner_score = hit.score; result.winner_index = entry_index; }
			}
			else result.rejected++;
		}
		for(int value : frame.rgb) result.checksum = (result.checksum * 31 + value) & 0x7fffffff;
		return true;
	};
	if(options_.backend == "cpu")
		return finish_cpu();
	if(options_.backend != "native-amp") {
		error = "unknown composite matcher backend: " + options_.backend;
		return false;
	}
#ifndef HAVE_SYSTEM_AMP
	error = "native-amp requested, but this build has no native C++ AMP support";
	return false;
#else
	if(!native_ || !frame.IsValid()) {
		error = "native AMP matcher has invalid prepared state or frame";
		return false;
	}
	Vector<int> candidate_entry, candidate_x, candidate_y;
	// Task 0339: an entry that doesn't fit this frame contributes zero candidate
	// positions and is recorded as skipped_oversized below -- it must never abort
	// the whole call (that was the confirmed bug: native-amp aborted the ENTIRE
	// Match() when candidate_entry ended up empty, i.e. every entry oversized,
	// while the cpu backend already just quietly left such entries unmatched).
	Vector<bool> entry_fits;
	entry_fits.SetCount(manifest_.entries.GetCount(), false);
	for(int entry_index = 0; entry_index < manifest_.entries.GetCount(); entry_index++) {
		const AmpTemplateAtlasEntry& entry = manifest_.entries[entry_index];
		if(entry.width > frame.width || entry.height > frame.height)
			continue; // this scale cannot fit this frame at any position
		entry_fits[entry_index] = true;
		for(int y = 0; y + entry.height <= frame.height; y++)
			for(int x = 0; x + entry.width <= frame.width; x++) {
				candidate_entry.Add(entry_index);
				candidate_x.Add(x);
				candidate_y.Add(y);
			}
	}
	Vector<double> score_values;
	if(!candidate_entry.IsEmpty()) {
		try {
			concurrency::array<int, 1> frame_rgb(concurrency::extent<1>(frame.rgb.GetCount()), native_->view);
			concurrency::array<int, 1> frame_gray(concurrency::extent<1>(frame.gray.GetCount()), native_->view);
			concurrency::array<int, 1> frame_contrast(concurrency::extent<1>(frame.local_contrast.GetCount()), native_->view);
			concurrency::array<int, 1> candidates_entry(concurrency::extent<1>(candidate_entry.GetCount()), native_->view);
			concurrency::array<int, 1> candidates_x(concurrency::extent<1>(candidate_x.GetCount()), native_->view);
			concurrency::array<int, 1> candidates_y(concurrency::extent<1>(candidate_y.GetCount()), native_->view);
			concurrency::copy(frame.rgb.Begin(), frame.rgb.End(), frame_rgb);
			concurrency::copy(frame.gray.Begin(), frame.gray.End(), frame_gray);
			concurrency::copy(frame.local_contrast.Begin(), frame.local_contrast.End(), frame_contrast);
			concurrency::copy(candidate_entry.Begin(), candidate_entry.End(), candidates_entry);
			concurrency::copy(candidate_x.Begin(), candidate_x.End(), candidates_x);
			concurrency::copy(candidate_y.Begin(), candidate_y.End(), candidates_y);
			concurrency::array_view<int, 1> frame_rgb_view(frame_rgb);
			concurrency::array_view<int, 1> frame_gray_view(frame_gray);
			concurrency::array_view<int, 1> frame_contrast_view(frame_contrast);
			concurrency::array_view<int, 1> candidates_entry_view(candidates_entry);
			concurrency::array_view<int, 1> candidates_x_view(candidates_x);
			concurrency::array_view<int, 1> candidates_y_view(candidates_y);
			concurrency::array_view<int, 1> atlas_rgb_view(native_->atlas_rgb);
			concurrency::array_view<int, 1> atlas_gray_view(native_->atlas_gray);
			concurrency::array_view<int, 1> atlas_contrast_view(native_->atlas_contrast);
			concurrency::array_view<int, 1> entry_x_view(native_->entry_x);
			concurrency::array_view<int, 1> entry_y_view(native_->entry_y);
			concurrency::array_view<int, 1> entry_width_view(native_->entry_width);
			concurrency::array_view<int, 1> entry_height_view(native_->entry_height);
			concurrency::array_view<int, 1> entry_mode_view(native_->entry_mode);
			concurrency::array_view<int, 1> entry_otsu_view(native_->entry_otsu_threshold);
			concurrency::array_view<int, 1> entry_sum_view(native_->entry_sum);
			concurrency::array_view<int, 1> entry_sum_sq_view(native_->entry_sum_sq);
			concurrency::array<double, 1> scores(concurrency::extent<1>(candidate_entry.GetCount()), native_->view);
			concurrency::array_view<double, 1> scores_view(scores);
			int atlas_width = atlas_.width;
			int frame_width = frame.width;
			parallel_for_each(native_->view, scores_view.extent, [=](concurrency::index<1> index) PARALLEL_AMP {
				int candidate = index[0];
				int entry = candidates_entry_view[candidate];
				int sum_i = 0, sum_ii = 0, sum_it = 0;
				for(int ty = 0; ty < entry_height_view[entry]; ty++)
					for(int tx = 0; tx < entry_width_view[entry]; tx++) {
						int fi = (candidates_y_view[candidate] + ty) * frame_width + candidates_x_view[candidate] + tx;
						int ai = (entry_y_view[entry] + ty) * atlas_width + entry_x_view[entry] + tx;
						int a = frame_gray_view[fi], b = atlas_gray_view[ai];
						sum_i += a; sum_ii += a * a; sum_it += a * b;
					}
				int n = entry_width_view[entry] * entry_height_view[entry];
				double inv_n = 1.0 / n;
				double var_i = (double)sum_ii - sum_i * (double)sum_i * inv_n;
				double var_t = (double)entry_sum_sq_view[entry] - entry_sum_view[entry] * (double)entry_sum_view[entry] * inv_n;
				if(var_i < 0) var_i = 0;
				if(var_t < 0) var_t = 0;
				double denom = concurrency::fast_math::sqrt((float)(var_i * var_t));
				double score = denom > 1e-12 ? ((double)sum_it - sum_i * (double)entry_sum_view[entry] * inv_n) / denom : 0.0;
				scores_view[candidate] = score < -1.0 ? -1.0 : score > 1.0 ? 1.0 : score;
			});
			score_values.SetCount(candidate_entry.GetCount());
			concurrency::copy(scores, score_values.Begin());
		}
		catch(...) {
			error = "native AMP match submission failed";
			return false;
		}
	}
	result = AmpTemplateMatchResult();
	result.winner_score = -DBL_MAX;
	result.candidate_count = candidate_entry.GetCount();
	for(int entry_index = 0; entry_index < manifest_.entries.GetCount(); entry_index++) {
		AmpTemplateMatchHit& hit = result.entries.Add();
		hit.id = manifest_.entries[entry_index].id;
		hit.entry_index = entry_index;
		hit.score = -DBL_MAX;
		if(!entry_fits[entry_index]) {
			// Never abort the whole call over one oversized scale: this entry
			// simply has no candidates, exactly like the cpu backend's sliding
			// window running zero iterations for the same condition.
			hit.skipped_oversized = true;
			result.skipped++;
			result.rejected++;
			continue;
		}
		for(int candidate = 0; candidate < candidate_entry.GetCount(); candidate++)
			if(candidate_entry[candidate] == entry_index && score_values[candidate] > hit.score) {
				hit.score = score_values[candidate]; hit.x = candidate_x[candidate]; hit.y = candidate_y[candidate];
			}
		hit.accepted = hit.score >= options_.threshold;
		if(hit.accepted) { result.accepted++; if(hit.score > result.winner_score) { result.winner_score = hit.score; result.winner_index = entry_index; } }
		else result.rejected++;
	}
	for(int value : frame.rgb) result.checksum = (result.checksum * 31 + value) & 0x7fffffff;
	return true;
#endif
}

END_UPP_NAMESPACE
