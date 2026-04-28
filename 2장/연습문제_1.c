#include<stdio.h>

unsigned int convert_order32(unsigned int before) {
	return ((before >> 24) & 0x000000FF) |
		((before >> 8) & 0x0000FF00) |
		((before << 8) & 0x00FF0000) |
		((before << 24) & 0xFF000000);
}
int main() {
	unsigned int x = 0x12345678;
	unsigned int y = convert_order32(x);

	printf("before = 0x%08X\n", x);
	printf("after = 0x%08X\n", y);

	return 0;
}
hy