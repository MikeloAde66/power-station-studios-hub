# AI Auto-Routing — Design Doc (stub)

Status: not started. Per planning discussion, this needs its own spec
before any implementation, separate from the rest of the Power Station
hub build.

Open questions to resolve before building:
- What signal(s) drive an auto-routing decision (track/plugin name,
  detected instrument/source type, user history, explicit rules)?
- What happens on an ambiguous or low-confidence match — fallback to
  manual, or best guess with an undo?
- Is routing logic local/rule-based, or does it call out to a model?
- How does a user override or lock a routing decision so the AI doesn't
  re-route it later?
