---
generated_by: "OpenAI Codex (GPT-6; exact variant not exposed)"
timestamp: "2026-11-06T10:37:46-06:00"
---
# Publish PBL-2500
- Fresh fixture screenshots must show P-SHOCK before building the gallery.
- `python3 execution/build_store_media.py` verifies six exact200×228frames; default first,3s then2.5s holds;15.5s loop.
- `python3 execution/publish.py` dry-run validates media. `--publish` performs user-authorized SDK upload with GIF first andfourstills.
- Production clean-build strips FACET_* fixtureflags; archive exact SDK-published bundle afterward.
- Appname PBL-2500; dialwordmark P-SHOCK; internalrepository/fixtureprefixFacet retained intentionally.
- `python3 execution/verify_public.py` verifies publicversion/galleryorder/exactGIF/PBW/fourPNGpixels, following the observed storeR2redirect withoutlogging signedqueries.
- Publicationreceipt prevents blindduplicateupload. Fresh independentreview accepts localmedia andpublicproof.

## 2026-11-06 — updating an existing release
Before replacing the active receipt, verify the existing public release and archive its receipt, build metadata, gallery, and proof under release/<version>/. Remove only the old active receipt after confirming the archive is identical. Keep the same appUUID and increment package version. Refresh native captures before rebuilding storemedia. Publicfrontend may temporarily serve the previous version/gallery even after the developer dashboard says Published; do not re-upload blindly. In this update, a later public fetch converged and exact verification passed.
