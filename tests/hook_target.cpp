// Owned fixture with known signature. Separate translation unit, no LTO.
// These are not instructions, offsets or signatures taken from the game.
extern "C" __declspec(noinline) int __cdecl nimby_test_target(int input) {
    volatile int value = input;
    value = value + 3;
    value = value * 2;
    return value;
}
