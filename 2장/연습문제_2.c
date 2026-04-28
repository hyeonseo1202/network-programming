#include<stdio.h>

unsigned int convert_order32(unsigned int before) {
	return((before &0x000000FF) <<24) |
		((before&0x0000FF00) <<8) |
		((before &0x00FF0000) >>8) |
		((before &0xFF000000) >>24);
}
int main() {
	unsigned int num = 0x12345678;
	unsigned int converted = convert_order32(num);

	printf("first num : 0x%X\n", num);
	printf("converted num : 0x%X\n", converted);

	if(num==converted) {
		printf("this system is hard to figure byte index.\n");
	}else{
		unsigned char *p = (unsigned char *)&num;

		if(p[0] == 0x78) {
			printf("this system is Little Endian.\n");
		}else if(p[0] ==0x12) {
			printf("this system is Big Endian.\n");
		}else{
			printf("byte stream that is not able to know.\n");
		}
	}
	return 0;
}
