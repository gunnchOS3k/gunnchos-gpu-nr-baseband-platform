# Repository visibility packet

**Repo:** `gunnchOS3k/gunnchos-gpu-nr-baseband-platform`  
**Status:** `EXTERNAL_PENDING`  
**Agent constraint:** never change GitHub visibility.

## Why blocked

This repository is **private**. A prospective supervisor who is not a collaborator will see HTTP 404, not the UML, vectors, or CPU evidence in this branch.

Inspect only (owner):

```bash
gh repo view gunnchOS3k/gunnchos-gpu-nr-baseband-platform --json isPrivate,visibility,nameWithOwner
```

## Owner action (do not automate)

Choose one, after a secret scan:

1. Add the supervisor as a collaborator, or  
2. Publish a sanitized public snapshot / GitHub release, or  
3. Keep private and share a tarball out of band.

This agent must not run `gh repo edit --visibility`.

## Expected evidence to drop the blocker

Public URL **or** confirmed collaborator access recorded by the owner in the research-portal dashboard.

## Related

CPU work on `cursor/supervisor-ready-portfolio-release-001` can proceed while private. Draft PRs on a private repo are allowed.
