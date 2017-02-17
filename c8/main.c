
#include <stdio.h>
#include <string.h>

static const unsigned char font[80] =
{ 
	0xF0, 0x90, 0x90, 0x90, 0xF0, //0
	0x20, 0x60, 0x20, 0x20, 0x70, //1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
	0x90, 0x90, 0xF0, 0x10, 0x10, //4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
	0xF0, 0x10, 0x20, 0x40, 0x40, //7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
	0xF0, 0x90, 0xF0, 0x90, 0x90, //A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
	0xF0, 0x80, 0x80, 0x80, 0xF0, //C
	0xE0, 0x90, 0x90, 0x90, 0xE0, //D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
	0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

static unsigned char memory[4096];
static unsigned short pc;

static unsigned short idx;

static unsigned short stack[16];
static unsigned short sp;

static unsigned char v[16];

static unsigned short opcode;

void init_c8(void)
{
	pc = 0x200;
	idx = sp = opcode = 0;

	int i;
	for (i=0;i<sizeof(memory);i++)
	{
		memory[0] = 0;
	}

	for (i=0;i<sizeof(stack)/2;i++)
	{
		stack[0] = 0;
	}

	for (i=0;i<sizeof(v);i++)
	{
		v[0] = 0;
	}

	memcpy(memory+50, font, 80);
}

void load_game(char *filename) {

}

int main(void)
{
	memory[pc] = 0xDE;
	memory[pc+1] = 0xAD;
	for (;;)
	{
		//TODO: verify pc not 4095 or higher!
		opcode = (unsigned short)memory[pc] << 8 | memory[pc+1];
		printf("%x\n", memory[pc]);
		printf("%x\n", memory[pc+1]);
		printf("%hx\n", opcode);


		unsigned char x = opcode & 0x0F00 >> 8;
		unsigned char y = opcode & 0x00F0 >> 4;
		unsigned char nn = opcode & 0x00FF;
		unsigned char nnn = opcode & 0x0FFF;

		switch (opcode & 0xF000)
		{
		case 0x1000:
			pc = nnn;
			break;
		case 0x3000:
			if (nn == v[x])
			{
				pc += 2;
			}
			pc += 2;
			break;
		case 0x4000:
			if (nn != v[x])
			{
				pc += 2;
			}
			pc += 2;
			break;
		case 0x5000:
			// noch gucken ob das letzte 0 ist!
			if ( v[x] == v[y] )
			{
				pc += 2;
			}
			pc += 2;
			break;
		case 0x6000:
			v[x] = nn;
			pc += 2;
			break;
		case 0x7000:
			v[x] += nn;
			pc += 2;
			break;
		case 0x8000:
			switch (opcode & 0x000F)
			{
			case 0x0:
				v[x] = v[y];
				break;
			case 0x1:
				v[x] |= v[y];
				break;
			case 0x2:
				v[x] &= v[y];
				break;
			case 0x3:
				v[x] ^= v[y];
				break;
			case 0x4:
				unsigned short result = (unsigned short)v[x] + (unsigned short)v[y];
				v[15] = result >> 8;
				v[x] = result;
				break;
			case 0x5:
				unsigned char result = v[x] - v[y];
				v[15] = v[x] >= v[y];
				v[x] = result;
				break;
			case 0x6:
				unsigned char result = v[x] >> 1;
				v[15] = v[x] & 0x0001;
				v[x] = result;
				break;
			case 0x7:
				unsigned char result = v[y] - v[x];
				v[15] = v[y] >= v[x];
				v[x] = result;
				break;
			case 0xE:
				unsigned char result = v[x] << 1;
				v[15] = v[x] & 0x8000;
				v[x] = result;
				break;
			default:
				printf("ERROR: UNKNOWN OPCODE!\n");
				break;
			}

			pc += 2;
			break;
		case 0x9000:
			if ( v[x] != v[y] )
			{
				pc += 2;
			}
			pc += 2;
			break;
		case 0xA000:
			idx = nnn;
			pc += 2;
			break;
		case 0xB000:
			pc = v[0] + nnn;
			break;
		default:
			printf("ERROR: UNKNOWN OPCODE!\n");
			break;
		}

		break;
	}
	return 0;
}