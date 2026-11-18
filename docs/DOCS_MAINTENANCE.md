# Maintaining Focus Documentation

This site uses the same general navigation and presentation language as the Mippi portfolio documentation and is hosted from the Focus repository's `/docs` directory.

## Source of truth

Before documenting changed behavior, inspect the current Focus C++ source and relevant tests or engine constraints. The public README is an introduction, not proof of complete feature behavior. If a detail is unverified in a live editor, distinguish its source implementation from testing status.

## Separate Design and Technical responsibilities

Each of the 23 feature topics has two complementary, independently useful pages:

- **DESIGN.md:** The user manual. Explain what a setting does, why to use it, where to find it, expected results, concrete workflows, examples, exceptions and common questions. Avoid internal function names unless the user actually needs them.
- **TECHNICAL.md:** The implementation reference. Document native property types, applicability checks, Actor and Component targets, state ownership, package serialization, undo, editor delegates, native API constraints and supported versus unsupported execution paths. Link to the relevant C++ source.
  
Never fill both pages with the same copied README material or generic text under different headings. Keep native implementation explanations primarily in Technical and practical instructions primarily in Design. Both should link to each other.

## Adding or changing features

1. Inspect the changed C++ implementation and current behavior, including relevant engine constraints.
2. Update the existing topic's Design and Technical pages, or create a new paired topic within the most suitable numbered category.
3. Add or update that category's `README.md` table row, preserve existing stable paths, and keep the root `README.md` category count accurate.
4. Include distinct examples, troubleshooting and limitations. Cite implementation source filenames/functions precisely.
5. Check local Markdown links, search, breadcrumb navigation, Design/Technical switching and desktop/mobile layout.
6. Describe automated and human QA separately; never present unexecuted test cases as successful tests.

## Publishing

Use GitHub repository **Settings → Pages → Deploy from a branch → main → /docs**. The expected site address is https://mippi-the-dork.github.io/Focus/ . Verify the actual deployment and its browser experience after merge.

## Maintaining version accuracy

Update `site-manifest.json`, the home page and relevant feature references when a new Focus version ships. If behavior is specific to an older release, label it historical rather than mixing it into the current user manual. Source links to `main` always reflect the newest code; use a release tag or commit when citing a prior implementation.
