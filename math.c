
static uint32_t lcg_seed = 1;

uint32_t lcg_rand() {
// linear congruential generator
    lcg_seed = (lcg_seed * 1664525 + 1013904223) % 0xFFFFFFFF;
    return lcg_seed;
}

float my_sqrt(float x) {
// newton-raphson square root
    if (x <= 0) return 0;

    float guess = x / 2.0;
    float epsilon = 0.1; // precision
    while ((guess * guess - x) > epsilon || (x - guess * guess) > epsilon) {
        guess = (guess + x / guess) / 2.0;
    }
    return guess;
}

float my_log(float x) {
// bakhshali's approximation logarithm
    if (x <= 0) return -1; // error for non-positive numbers
    float y = (x - 1) / (x + 1);
    float y2 = y * y;
    float sum = 0.0;
    float term = y;
    int n = 1;

    while (term > 0.1 || term < -0.00001) {
        sum += term;
        term *= y2;
        term *= (2.0 * n - 1) / (2.0 * n + 1);
        n++;
    }

    return 2.0 * sum;
}

float gaussian() {
    float u, v, r;
    do {
        u = ((float)lcg_rand() / 0xFFFFFFFF) * 2.0 - 1.0;
        v = ((float)lcg_rand() / 0xFFFFFFFF) * 2.0 - 1.0;
        r = u * u + v * v;
    } while (r == 0 || r > 1); // avoid zero and out-of-bounds

    float c = my_sqrt(-2.0 * my_log(r) / r);
    return u * c;
}
