# GitHub Workflows

This directory contains GitHub Actions workflows for building and releasing Arduino projects.

## Workflows

### Build Workflows

- `build_App_BlinkSample.yaml` - Builds the BlinkSample application
- `build_App_DccSniffer.yaml` - Builds the DccSniffer application
- `build_App_Signal.yaml` - Builds the Signal application with release support
- `build_UnitTest.yaml` - Runs unit tests

### Release Workflow (App/Signal)

The `build_App_Signal.yaml` workflow includes special release support:

#### How to Create a Release with Assets

1. **Create a new tag** for your release:
   ```bash
   git tag -a App_Signal_vX.Y.Z -m "Release version X.Y.Z"
   git push origin App_Signal_vX.Y.Z
   ```

2. **Publish a GitHub Release**:
   - Go to the [Releases page](https://github.com/RalfSondershaus/Arduino/releases)
   - Click "Draft a new release"
   - Select your tag
   - Fill in the release notes
   - Click "Publish release"

3. **Automatic Asset Upload**:
   - When you publish the release, the `build_release` job automatically runs
   - It builds the Signal application
   - Uploads the following assets to the release:
     - `Signal.hex` - The compiled hex file for flashing to Arduino
     - `Signal.elf` - The ELF file with debug information

#### Release Assets

The workflow uses `softprops/action-gh-release@v2` to upload build artifacts directly to the GitHub release. This modern action replaces the deprecated `actions/upload-release-asset@v1`.

#### Workflow Details

- **Trigger**: Release must be published on the `master` branch
- **Build artifacts**: Saved to `Build/Bin/` directory
- **Artifacts uploaded**: Both `.hex` and `.elf` files
- **Permissions**: Requires `contents: write` permission to upload assets

## Build Artifacts

All build workflows also upload artifacts that can be downloaded from the Actions tab:
- Regular builds: `build-output`
- Release builds: `signal-release-<tag-name>`

These artifacts contain all files from the `Build/Bin/` directory.
