#include "IncFile1.h"

//from lab 9
void lcd_putValue(unsigned char val) {
	LCD_DPRT &= 0x0F;
	LCD_DPRT |= (val & 0xF0);
	LCD_CPRT |= (1 << LCD_EN);
	_delay_us(1);
	LCD_CPRT &= ~(1 << LCD_EN);
	_delay_us(100);

	LCD_DPRT &= 0x0F;
	LCD_DPRT |= val << 4;
	LCD_CPRT |= (1 << LCD_EN);
	_delay_us(1);
	LCD_CPRT &= ~(1 << LCD_EN);
	_delay_us(100);
}

void lcdCommand(unsigned char cmnd) {
	LCD_CPRT &= ~(1 << LCD_RS);
	lcd_putValue(cmnd);
}

void lcdData(unsigned char data) {
	LCD_CPRT |= (1 << LCD_RS);
	lcd_putValue(data);
}

void lcd_clear() {
	lcdCommand(0x01);
	_delay_us(1700);
}

void lcd_showCursor() {
	lcdCommand(0x0E);
	_delay_us(50);
}

void lcd_hideCursor() {
	lcdCommand(0x0C);
	_delay_us(50);
}

void lcd_init() {
	LCD_DDDR |= 0xF0;
	LCD_CDDR |= (1 << LCD_RS) | (1 << LCD_EN);

	LCD_CPRT &= ~(1 << LCD_EN);
	_delay_us(2000);

	lcdCommand(0x33);
	lcdCommand(0x32);
	lcdCommand(0x28);
	lcdCommand(0x0E);
	lcdCommand(0x06);

	lcd_clear();
}

void lcd_gotoxy(unsigned char x, unsigned char y) {
	const unsigned char firstCharAdr[] = { 0x80, 0xC0, 0x94, 0xD4 };
	lcdCommand(firstCharAdr[y - 1] + x - 1);
	_delay_us(100);
}

void lcd_print(const char* str) {
	for (int i = 0; str[i] != '\0'; i++) {
		lcdData(str[i]);
	}
}
/*==============================================================================*/
// Set layout keyboard
unsigned char keyvalue;
const char keypad[4][4] = {
	{ '1', '2', '3', '+' },
	{ '4', '5', '6', '-' },
	{ '7', '8', '9', '*' },
	{ '=', '0', '^', '/' }
};

void keyboard_init() {
	KEY_DDR = 0xF0;
	KEY_PRT = 0xFF;
}

void keyboard() {
	unsigned char colloc, rowloc;

	do {
		KEY_PRT &= 0x0F;
		asm("NOP");
		colloc = (KEY_PIN & 0x0F);
	} while (colloc != 0x0F);

	do {
		_delay_ms(20);
		colloc = (KEY_PIN & 0x0F);
	} while (colloc == 0x0F);

	_delay_ms(20);
	colloc = (KEY_PIN & 0x0F);

	while (1) {
		KEY_PRT = 0x7F;
		_delay_ms(20);
		colloc = (KEY_PIN & 0x0F);

		if (colloc != 0x0F) {
			rowloc = 0;
			break;
		}

		KEY_PRT = 0xBF;
		_delay_ms(20);
		colloc = (KEY_PIN & 0x0F);

		if (colloc != 0x0F) {
			rowloc = 1;
			break;
		}

		KEY_PRT = 0xDF;
		_delay_ms(20);
		colloc = (KEY_PIN & 0x0F);

		if (colloc != 0x0F) {
			rowloc = 2;
			break;
		}

		KEY_PRT = 0xEF;
		_delay_ms(20);
		colloc = (KEY_PIN & 0x0F);
		rowloc = 3;
		break;
	}

	if (colloc == 0x0E) keyvalue = keypad[rowloc][0];
	else if (colloc == 0x0D) keyvalue = keypad[rowloc][1];
	else if (colloc == 0x0B) keyvalue = keypad[rowloc][2];
	else if (colloc == 0x07) keyvalue = keypad[rowloc][3];
}
/*==============================================================================*/
void lcd_printNumber(long long num) {
	char buffer[MAX_EXPRESSION_LENGTH];
	dtostrf(num, 5, 2, buffer);  // Adjust the parameters based on your needs
	lcd_print(buffer);
}

// Function to handle error printing
void lcd_printError(const char* errorMessage) {
	lcd_clear();
	lcd_gotoxy(1, 4);
	lcd_print(errorMessage);
	_delay_ms(2000);
	lcd_clear();
	lcd_gotoxy(1, 1);
}

int getPrecedence(char op) {
	switch (op) {
		case '^':
		return 3;
		case '*':
		case '/':
		return 2;
		case '+':
		case '-':
		return 1;
		default:
		return 0;  // Default for numbers
	}
}

// Function to perform arithmetic operation
long long performOperation(long long operand1, long long operand2, char operator) {
	switch (operator) {
		case '+':
		return operand1 + operand2;
		case '-':
		return operand1 - operand2;
		case '*':
		return operand1 * operand2;
		case '/':
		if (operand2 != 0) {
			return operand1 / operand2;
			} else {
			int divisionError = 1;
			lcd_printError("Division by 0!");
			return 0;  // Return a dummy value in case of error
		}
		case '^':
		return pow(operand1, operand2); //pow(operand1, operand2);
		//break;
		default:
		return 0;  // Default case for unknown operators
	}
}

void evaluateExpression(char expression[]) {
	long long operands[MAX_EXPRESSION_LENGTH];
	char operators[MAX_EXPRESSION_LENGTH];
	int operandIndex = 0;
	int operatorIndex = 0;
	int isNegative = 0;

	// Clear divisionError flag before starting evaluation
	int divisionError = 0;

	for (int i = 0; expression[i] != '\0'; i++) {
		if (expression[i] >= '0' && expression[i] <= '9') {
			long long number = atof(&expression[i]);

			if (isNegative) {
				number = -number;
				isNegative = 0;
			}

			operands[operandIndex++] = number;

			while (expression[i] >= '0' && expression[i] <= '9') {
				i++;
			}
			i--;
			} else if (expression[i] == '+' || expression[i] == '-' || expression[i] == '*' || expression[i] == '/' || expression[i] == '^') {
			if (i == 0 || (i > 0 && (expression[i - 1] == '+' || expression[i - 1] == '-' || expression[i - 1] == '*' || expression[i - 1] == '/' || expression[i - 1] == '^'))) {
				if (expression[i] == '-') {
					isNegative = 1;
					} else if (expression[i] == '+') {
					// Ignore leading '+' symbol
					} else {
					lcd_printError("Syntax Error");
					return;
				}
				} else {
				while (operatorIndex > 0 && getPrecedence(operators[operatorIndex - 1]) >= getPrecedence(expression[i])) {
					long long operand2 = operands[--operandIndex];
					long long operand1 = operands[--operandIndex];

					// Perform the operation
					long long result = performOperation(operand1, operand2, operators[--operatorIndex]);

					// Handle division by zero error
					if (divisionError) {
						return;
					}

					operands[operandIndex++] = result;
				}

				if ((expression[i] == '*' || expression[i] == '/') && (i == 0 || (i > 0 && (expression[i - 1] == '+' || expression[i - 1] == '-' || expression[i - 1] == '*' || expression[i - 1] == '/' || expression[i - 1] == '^')))) {
					lcd_printError("Syntax Error");
					return;
				}

				operators[operatorIndex++] = expression[i];
				isNegative = 0;
			}
		}
	}

	while (operatorIndex > 0) {
		long long operand2 = operands[--operandIndex];
		long long operand1 = operands[--operandIndex];

		// Perform the operation
		long long result = performOperation(operand1, operand2, operators[--operatorIndex]);

		// Handle division by zero error
		if (divisionError) {
			return;
		}

		operands[operandIndex++] = result;
	}

	// Display the result on the LCD
	lcd_clear();
	lcd_gotoxy(1, 2);
	lcd_print("=:");
	lcd_printNumber(operands[0]);
	_delay_ms(2000);
	lcd_gotoxy(1, 1);
}
/*================================================================*/
int main() {
	lcd_init();
	lcd_hideCursor();
	lcd_gotoxy(1, 1);
	keyboard_init();

	char expression[MAX_EXPRESSION_LENGTH];
	int expressionIndex = 0;

	while (1) {
		keyboard();

		if ((keyvalue >= '0' && keyvalue <= '9') || keyvalue == '.' || keyvalue == '+' || keyvalue == '-' || keyvalue == '*' || keyvalue == '/' || keyvalue == '^') {
			lcdData(keyvalue);
			expression[expressionIndex++] = keyvalue;
		}
		if (keyvalue == '=') {
			expression[expressionIndex] = '\0';
			evaluateExpression(expression);
			expressionIndex = 0;
		}
		_delay_ms(100);
	}
	return 0;
}
