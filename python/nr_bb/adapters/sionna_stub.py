"""Optional Sionna adapter stub — does not import Sionna unless installed."""

STATUS = "DOCUMENTED_IMPLEMENTATION"


def compare_bler_placeholder(*_args, **_kwargs):
    return {"status": STATUS, "notes": "Install Sionna separately for offline compare"}
