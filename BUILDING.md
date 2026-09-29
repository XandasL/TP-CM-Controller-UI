# Building and publishing

This repository follows the official `TwilitRealm/mod-template` native mod workflow.

## Local test build

```sh
cmake -B build
cmake --build build
```

A local build is platform-specific and is intended only for testing.

## Multi-platform distribution build

Push the repository to GitHub. The workflow in `.github/workflows/build.yml` builds:

- Windows AMD64
- Windows ARM64
- Linux x86_64
- Linux ARM64
- macOS Apple Silicon
- macOS Intel
- iOS ARM64
- Android ARM64

When every platform job succeeds, the `Combine bundles` job creates a single distributable `.dusk` as the `mod-combined` artifact.

For a release, create and push a tag such as `v1.0.0`. The workflow attaches the combined `.dusk` to the GitHub release.

```sh
git tag v1.0.0
git push origin v1.0.0
```

Do not upload one of the local/per-platform `.dusk` files to the Dusklight mod site. Use the combined bundle.
