#include <msp430.h>

unsigned int seed;

/**
 * Recieve back a random number
 * @return random number
 */
unsigned int random() {
  seed ^= (seed << 13);
  seed ^= (seed >> 9);
  return seed ^= (seed << 7);
}

/**
 * add to the random seed to mix it up, maybe add the switched pushed?
 * @param input some unsigned int number to add to the seed
 */
void random_add(unsigned int input) {
  seed += input;
}

/**
 * TI instruments impltemented random bit generator using the aux clock
 * @return random bits from aux clock
 */
unsigned int random_Gen() {
	int i, j;
	seed = 0;

	/* Save state */
	unsigned int BCSCTL3_old = BCSCTL3;
	unsigned int TACCTL0_old = TACCTL0;
	unsigned int TACTL_old = TACTL;

	/* Halt timer */
	TACTL = 0x0;

	/* Set up timer */
	BCSCTL3 = (~LFXT1S_3 & BCSCTL3) | LFXT1S_2; // Source ACLK from VLO
	TACCTL0 = CAP | CM_1 | CCIS_1;            // Capture mode, positive edge
	TACTL = TASSEL_2 | MC_2;                  // SMCLK, continuous up

	/* Generate bits */
	for (i = 0; i < 16; i++) {
		unsigned int ones = 0;

		for (j = 0; j < 5; j++) {
			while (!(CCIFG & TACCTL0));       // Wait for interrupt

			TACCTL0 &= ~CCIFG;                // Clear interrupt

			if (1 & TACCR0)                   // If LSb set, count it
				ones++;
		}

		seed >>= 1;                         // Save previous bits

		if (ones >= 3)                        // Best out of 5
			seed |= 0x8000;                 // Set MSb
	}

	/* Restore state */
	BCSCTL3 = BCSCTL3_old;
	TACCTL0 = TACCTL0_old;
	TACTL = TACTL_old;

	return seed;
}

/**
 * Sets up the library
 */
void rand_init() {
  random_Gen();
}
