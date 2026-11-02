# AMP Agent
AMP (Advanced Multi-Processor) compatibility layer and headers.

## Native C++ AMP execution rule

Native C++ AMP code is not supported in debug mode in this workspace. Follow
this split strictly:

- Build and run native AMP/GPGPU code only in **release** mode, without an
  attached debugger.
- When diagnosing a defect, use a **debug** build with the CPU backend. Keep
  the same prepared inputs and compare its deterministic results with the
  release native-AMP evidence after the CPU issue is fixed.
- Do not treat an AMP debug crash, or an AMP compatibility-mode CPU fallback,
  as evidence about native GPU correctness or performance.
