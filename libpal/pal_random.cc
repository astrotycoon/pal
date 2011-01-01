#include "libpal/SFMT/SFMT.h"

#include "pal_random.h"

void palSeedRandom(uint32_t seed) {
  init_gen_rand(seed);
}

uint32_t palGenerateRandom() {
  return gen_rand32();
}

uint32_t pal_generate_random_int (uint32_t max) {
  return gen_rand32() % max;
}

float palGenerateRandomFloat () {
  return static_cast<float>(genrand_real1());
}
