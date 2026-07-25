# Repository Guidelines

## Project Structure & Module Organization
Open Stop Motion Studio is a single .NET 10 desktop app (`OpenStopMotionStudio.csproj`) built with Avalonia. Runtime boot flows from `Program.cs` to `App.axaml.cs`, then through `GUI/SplashWindow.axaml.cs`, where `InitializationService` executes startup tasks before opening `ProjectWindow`.

Core behavior is split between `Core/` services and `GUI/` partial window logic. Camera orchestration lives in `Core/CameraManager.cs` and `Core/CameraAdapterFactory.cs`: generic device enumeration is merged with Windows SDK discovery for Canon/Nikon/Sony, then adapter selection is resolved by connection kind. UI behavior for timeline, histogram, overlays, and RAW import is segmented into `GUI/MainWindow.*.cs` partials to keep interaction domains isolated. Update checks are handled by `Core/UpdateService.cs` using `versions.json` (local + optional remote manifest).

## Build, Test, and Development Commands
- `dotnet restore Open-Stop-Motion-Studio.sln` (CI restore)
- `dotnet build Open-Stop-Motion-Studio.sln --configuration Release --no-restore` (CI build)
- `dotnet run --project OpenStopMotionStudio.csproj --configuration Debug --no-build` (launcher run path)
- `start.bat` (Windows local bootstrap: restore/build/run)
- `./build.sh <version>` (Linux/macOS release archives)
- `build.bat <version>` (Windows release zip)
- Single test project run (CI pattern): `dotnet test <path-to-test-csproj> --configuration Release --no-build --verbosity normal`

## Coding Style & Naming Conventions
Enforced project settings in `OpenStopMotionStudio.csproj` are `Nullable=enable` and `ImplicitUsings=enable`; target is `net10.0`. Build outputs are redirected by `Directory.Build.props` to `.artifacts/`.

No repository-level `.editorconfig`, StyleCop config, linter config, formatter config, or pre-commit hooks were found. Follow existing C# conventions in the codebase: PascalCase for types/methods, `_camelCase` for private fields, and feature-focused partial classes for large UI code-behind.

## Testing Guidelines
`ci.yml` runs tests only if `*Tests*.csproj` files exist, then executes `dotnet test` per discovered test project. Add new test projects with `Tests` in the `.csproj` name to be auto-detected by CI.

## Commit & Pull Request Guidelines
Recent history mixes free-form messages and Conventional Commit-style prefixes; release automation in `.github/workflows/auto-tag-release.yml` derives SemVer from commit content. Prefer:
- `feat:` for minor bumps
- `fix:`, `perf:`, `refactor:` for patch bumps
- `type!:` or `BREAKING CHANGE:` for major bumps

Legacy keywords `Release`, `Update`, and `Hotfix` are also parsed, but Conventional Commit prefixes are the safest path. No PR template file was found.