#ifndef _AMP_DX11_WEBCAM_H_
#define _AMP_DX11_WEBCAM_H_

#include <api/Graphics/Dx.cpp>
#include <api/Screen/WinD11.cpp>
#include <SoftHMD/Camera.h>
#include <AMP/AMPTemplatePixels.h>
#include <AMP/AMPTemplateAtlas.h>
#include <AMP/AMPTemplateMatcher.h>

NAMESPACE_UPP

struct AmpDx11WebcamPipeline {
    // DX11 resources
    ID3D11Device* d3d_device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    ID3D11Texture2D* texture = nullptr;
    ID3D11ShaderResourceView* srv = nullptr;
    Size texture_size;

    // Camera resources
    Camera camera;
    Vector<CameraFrame> camera_frames;

    // AMP resources
    AmpTemplatePixelBuffer amp_frame;
    AmpTemplateAtlasManifest manifest;

    // Pipeline state
    bool is_running = false;
    bool use_amp_gpu = false;

    // Initialize pipeline
    bool Initialize() {
        // Initialize DX11
        // TODO: Setup DX11 device and texture

        // Initialize camera
        if (!camera.Open()) {
            LOG("AmpDx11WebcamPipeline: error: could not open camera");
            return false;
        }
        camera.SetVerbose(true);

        // Initialize AMP (load template atlas)
        if (!manifest.Load("share/imgs/amp_template_atlas.json", error)) {
            LOG("AmpDx11WebcamPipeline: error: could not load AMP template atlas");
            return false;
        }

        is_running = true;
        return true;
    }

    // Update pipeline (capture and process)
    void Update() {
        if (!is_running) return;

        // Capture frame from camera
        camera_frames.Clear();
        camera.PopFrames(camera_frames);

        if (camera_frames.IsEmpty()) return;

        // Get the most recent frame
        const CameraFrame& cam_frame = camera_frames[camera_frames.GetCount() - 1];

        // Convert to AMP pixel buffer
        if (!BuildAmpPixelBuffer(cam_frame.img, amp_frame, error)) {
            LOG("AmpDx11WebcamPipeline: error: could not build AMP pixel buffer");
            return;
        }

        // Process with AMP (use GPU if available)
        if (use_amp_gpu) {
            // Use AMP GPU matching
            if (!MatchAmpTemplatePixelsAmp(amp_frame, manifest, 50, "device_path", result, error)) {
                LOG("AmpDx11WebcamPipeline: error: AMP GPU matching failed");
                return;
            }
        } else {
            // Use AMP CPU matching
            if (!MatchAmpTemplatePixelsCpu(amp_frame, manifest, 50, result, error)) {
                LOG("AmpDx11WebcamPipeline: error: AMP CPU matching failed");
                return;
            }
        }

        // Upload processed frame to DX11 texture
        UploadFrameToDx11(cam_frame.img);
    }

    // Upload image to DX11 texture
    void UploadFrameToDx11(const Image& image) {
        // Create/Update DX11 texture with image data
        // TODO: Setup texture with image pixels

        // Record in results
        AmpTemplateMatchHit hit;
        hit.id = "current_frame";
        hit.x = image.GetWidth() / 2;
        hit.y = image.GetHeight() / 2;
        hit.score = 0.0;  // Placeholder
        hit.accepted = result.winner_index >= 0;

        amp_result.entries.Add(hit);
    }

    // Get results
    const AmpTemplateMatchResult& GetResults() const { return result; }

    // Cleanup
    void Shutdown() {
        is_running = false;

        // Cleanup DX11 resources
        // TODO: Release DX11 resources

        // Cleanup camera
        camera.Close();
    }

private:
    String error;
    AmpTemplateMatchResult result;
};

END_UPP_NAMESPACE

#endif