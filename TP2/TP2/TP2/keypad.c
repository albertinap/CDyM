#include "keypad.h"

static const uint8_t keymap[4][4] = {
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},
	{'7', '8', '9', 'C'},
	{'*', '0', '#', 'D'}
};

void keypad_init(void) {
	ROW0_DDR |= (1 << ROW0_PIN); ROW0_PORT |= (1 << ROW0_PIN);
	ROW1_DDR |= (1 << ROW1_PIN); ROW1_PORT |= (1 << ROW1_PIN);
	ROW2_DDR |= (1 << ROW2_PIN); ROW2_PORT |= (1 << ROW2_PIN);
	ROW3_DDR |= (1 << ROW3_PIN); ROW3_PORT |= (1 << ROW3_PIN);

	COL0_DDR &= ~(1 << COL0_PIN); COL0_PORT |= (1 << COL0_PIN);
	COL1_DDR &= ~(1 << COL1_PIN); COL1_PORT |= (1 << COL1_PIN);
	COL2_DDR &= ~(1 << COL2_PIN); COL2_PORT |= (1 << COL2_PIN);
	COL3_DDR &= ~(1 << COL3_PIN); COL3_PORT |= (1 << COL3_PIN);
}

static void set_row(uint8_t row) {
	ROW0_PORT |= (1 << ROW0_PIN);
	ROW1_PORT |= (1 << ROW1_PIN);
	ROW2_PORT |= (1 << ROW2_PIN);
	ROW3_PORT |= (1 << ROW3_PIN);
	switch(row) {
		case 0: ROW0_PORT &= ~(1 << ROW0_PIN); break;
		case 1: ROW1_PORT &= ~(1 << ROW1_PIN); break;
		case 2: ROW2_PORT &= ~(1 << ROW2_PIN); break;
		case 3: ROW3_PORT &= ~(1 << ROW3_PIN); break;
	}
}

static uint8_t read_col(uint8_t col) {
	switch(col) {
		case 0: return !(COL0_PIN_REG & (1 << COL0_PIN));
		case 1: return !(COL1_PIN_REG & (1 << COL1_PIN));
		case 2: return !(COL2_PIN_REG & (1 << COL2_PIN));
		case 3: return !(COL3_PIN_REG & (1 << COL3_PIN));
	}
	return 0;
}

// Scan crudo: retorna el char de la tecla presionada o KEY_NONE
static uint8_t KepadUpdate(void) {
	for (uint8_t r = 0; r < 4; r++) {
		set_row(r);
		_delay_us(10);
		for (uint8_t c = 0; c < 4; c++) {
			if (read_col(c)) return keymap[r][c];
		}
	}
	return KEY_NONE;
}

// Doble verificación + evita detección múltiple mientras se mantiene presionada
uint8_t KEYPAD_Scan(uint8_t *pkey) {
	static uint8_t Old_key, Last_valid_key = KEY_NONE;
	uint8_t Key;

	Key = KepadUpdate();

	if (Key == KEY_NONE) {
		Old_key = KEY_NONE;
		Last_valid_key = KEY_NONE;
		return 0;
	}
	if (Key == Old_key) {           // 2da verificación: la misma tecla dos llamadas seguidas
		if (Key != Last_valid_key) { // no reportar si ya se reportó esta pulsación
			*pkey = Key;
			Last_valid_key = Key;
			return 1;
		}
	}
	Old_key = Key;  // 1era verificación guardada
	return 0;
}