# New-signal cosmetics — qualified Windows 1.19

Executable SHA-256: `fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.

The template parser at RVA `0x4113e0` reads `size` and `rotate` but no lateral-side property. The normal construction builder at `0x7830a0` copies the selected template from editor+0x148, clears its ID, sets its track position and explicitly resets lateral offset +0x58. The placement command at `0x7832e0` calls this same builder and moves its result into the normal command; modifying only this result keeps preview and placement consistent.

Signal offsets: texture hash +0x38, track +0x40, fraction +0x48, signed direction byte +0x50, track orientation byte +0x51, lateral offset int32 +0x58, size int32 +0x5c, rotation int32 +0x60. Rendering at `0x61b580` reverses the lateral offset when the track orientation changes, then passes twice that offset to the track geometry function `0x3898d0`. Its positive normal is (-dy,+dx), the right side in screen coordinates. Left relative to travel therefore uses minus the signed direction. Do not change the direction or orientation bytes.

SDK metadata format 3 declares construction defaults by exact texture catalogue ID. The bridge hashes these IDs with the game's string hash at `0x241780` and applies defaults only for unambiguous ownership, a fresh zero-ID output, a zero-ID source template, a valid track and a signed direction of -1 or +1. Removed or invalid sidecars clear prior registrations. This does not rewrite placed signals, save data, or the repeat tool's copies.

Both the executable identity and the hooked/helper function prologues are checked before installation of hooks. Tests execute the real bridge against owned native-layout fixtures, cover both directions/orientations and preserve every byte outside +0x58. They do not replace visual validation in a running game.
