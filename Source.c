
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h> // for bitmap definitions
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// size of picture in pixels
#define WIDTH 1000
#define HEIGHT 900

typedef enum
{
	residence,
	office,
	industry,
	commerce
} house_type;

typedef enum
{
	v, h
} street_type;

typedef struct rgb
{
	int r, g, b;
} RGB;

typedef struct street
{
	int startX;  // beginning of road X coordinate
	int startY;  // beginning of road Y coordinate
	int endX;    // ending of road X coordinate
	int endY;    // ending of road Y coordinate
	street_type type;
} STREET;

typedef struct street_node
{
	STREET astreet;
	struct street_node *next;
} STREET_NODE;

typedef struct house
{
	int middleX;       // middle point coordinate X
	int middleY;       // middle point coordinate Y
	int DimensionX;    // building Dimensions widthwise
	int DimensionY;    // building Dimensions lengthwise
	house_type type;
	RGB color;
} HOUSE;

typedef struct house_node
{
	HOUSE ahouse;
	struct house_node *next;
} HOUSE_NODE;

typedef struct crossroad
{
	int middleX;       // middle point coordinate X
	int middleY;       // middle point coordinate Y
} CROSSROAD;

typedef struct crossroad_node
{
	CROSSROAD acrossroad;
	struct crossroad_node *next;
} CROSSROAD_NODE;

typedef struct map              // used to organize all map data in one place //
{
	STREET_NODE *streets;
	HOUSE_NODE *houses;
	CROSSROAD_NODE *crossroads;
} MAP;

void buildStreetList(MAP *cityMap);
void buildHouseList(MAP *cityMap);
void buildCrossroadsList(MAP *cityMap);
void fillHouseRGB(HOUSE_NODE *node);
void drawMap(MAP *cityMap);
void drawHouses(MAP cityMap, unsigned char* bmp);
void drawStreets(MAP cityMap, unsigned char* bmp);
void drawCrossroads(MAP cityMap, unsigned char* bmp);

void  FreeStreetList(MAP *cityMap);
void  FreeHouseList(MAP *cityMap);
void  FreeCrossroadsList(MAP *cityMap);

void main() {
	MAP cityMap;
	buildStreetList(&cityMap);
	buildHouseList(&cityMap);
	buildCrossroadsList(&cityMap);
	drawMap(&cityMap);


	FreeStreetList(&cityMap);
	FreeHouseList(&cityMap);
	FreeCrossroadsList(&cityMap);
}


void buildStreetList(MAP *cityMap)
{
	STREET_NODE *prevNode = 0, *newNode = 0;
	FILE* dataFile = fopen("dataFile.txt", "r");
	if (dataFile == NULL)
	{
		printf("Failed opening the file. Exiting!\n");
		return;
	}

	int dataType = fgetc(dataFile);
	while (dataType != -1)
	{
		if (dataType == 83)
		{
			newNode = (STREET_NODE*)malloc(sizeof(STREET_NODE));
			fscanf(dataFile, " %i %i %i %i\n", &newNode->astreet.startX, &newNode->astreet.startY,
				&newNode->astreet.endX, &newNode->astreet.endY);

			if (prevNode == 0)
			{
				cityMap->streets = newNode;
			}
			else
			{
				prevNode->next = newNode;
			}
			prevNode = newNode;
		}
		dataType = fgetc(dataFile);
	}
	// needs to set to NULL the last node, built the program with Clion on linux, after dubugging with VS on windows realised it's not automatic.
	// after manually setting last (empty) node to NULL invaild memory accessing bugs has been fixed. same at the next functions.
	prevNode->next = 0;  
	fclose(dataFile);
}


void buildHouseList(MAP *cityMap)
{
	HOUSE_NODE *prevNode = 0, *newNode = 0;
	FILE* dataFile = fopen("dataFile.txt", "r");
	if (dataFile == NULL)
	{
		printf("Failed opening the file. Exiting!\n");
		return;
	}

	int dataType = fgetc(dataFile);
	while (dataType != -1)
	{
		if (dataType == 72)
		{
			newNode = (HOUSE_NODE*)malloc(sizeof(HOUSE_NODE));
			fscanf(dataFile, " %i %i %i %i %u\n", &newNode->ahouse.middleX, &newNode->ahouse.middleY,
				&newNode->ahouse.DimensionX, &newNode->ahouse.DimensionY, &newNode->ahouse.type);
			fillHouseRGB(newNode);

			if (prevNode == 0)
			{
				cityMap->houses = newNode;
			}
			else
			{
				prevNode->next = newNode;
			}
			prevNode = newNode;
		}
		dataType = fgetc(dataFile);
	}
	prevNode->next = 0;
	fclose(dataFile);
}

void buildCrossroadsList(MAP *cityMap)
{
	STREET_NODE *currentNodeStreet = cityMap->streets, *checkNodeStreet;
	CROSSROAD_NODE *prevNodeCross = 0, *newNodeCross = 0;

	while (currentNodeStreet != 0)
	{
		checkNodeStreet = currentNodeStreet->next;
		int startX = currentNodeStreet->astreet.startX, startY = currentNodeStreet->astreet.startY,
			endX = currentNodeStreet->astreet.endX, endY = currentNodeStreet->astreet.endY;
		//check if current road Horizontal or Vertical//
		char currentStreet; 
		if (startY == endY)
		{
			currentStreet = 'h';
			currentNodeStreet->astreet.type = h;
		}
		else
		{
			currentStreet = 'v';
			currentNodeStreet->astreet.type = v;
		}

		while (checkNodeStreet != 0)
		{
			if (currentNodeStreet != checkNodeStreet)
			{
				int startCheckX = checkNodeStreet->astreet.startX, startCheckY = checkNodeStreet->astreet.startY,
					endCheckX = checkNodeStreet->astreet.endX, endCheckY = checkNodeStreet->astreet.endY;
				//check if road for comparison is Horizontal or Vertical//
				char checkStreet = startCheckY == endCheckY ? 'h' : 'v';
				if (currentStreet == 'v' && checkStreet == 'h')
				{
					//check if roads cross each other//
					if (startCheckY< endY && startCheckY > startY && endY != startY)
					{
						newNodeCross = (CROSSROAD_NODE*)malloc(sizeof(CROSSROAD_NODE));
						newNodeCross->acrossroad.middleX = startX;
						newNodeCross->acrossroad.middleY = startCheckY;

						if (prevNodeCross == 0)
						{
							cityMap->crossroads = newNodeCross;
						}
						else
						{
							prevNodeCross->next = newNodeCross;
						}
						prevNodeCross = newNodeCross;
					}
				}
				else if (currentStreet == 'h' && checkStreet == 'v')
				{
					if (startCheckX < endX && startCheckX > startX && endX != startX)
					{
						newNodeCross = (CROSSROAD_NODE*)malloc(sizeof(CROSSROAD_NODE));
						newNodeCross->acrossroad.middleX = startCheckX;
						newNodeCross->acrossroad.middleY = startY;

						if (prevNodeCross == 0)
						{
							cityMap->crossroads = newNodeCross;
						}
						else
						{
							prevNodeCross->next = newNodeCross;
						}
						prevNodeCross = newNodeCross;
					}
				}

			}
			checkNodeStreet = checkNodeStreet->next;
		}
		currentNodeStreet = currentNodeStreet->next;
	}
	prevNodeCross->next = 0;
}

void fillHouseRGB(HOUSE_NODE *node)
{
	if (node->ahouse.type == residence)
	{
		node->ahouse.color.b = 144;
		node->ahouse.color.g = 192;
		node->ahouse.color.r = 250;
	}
	else if (node->ahouse.type == office)
	{
		node->ahouse.color.b = 199;
		node->ahouse.color.g = 162;
		node->ahouse.color.r = 179;
	}
	else if (node->ahouse.type == industry)
	{
		node->ahouse.color.b = 213;
		node->ahouse.color.g = 142;
		node->ahouse.color.r = 85;
	}
	else //if commerce
	{
		node->ahouse.color.b = 51;
		node->ahouse.color.g = 51;
		node->ahouse.color.r = 250;
	}
}


void drawMap(MAP *cityMap)
{
	FILE* pf = fopen("Map.bmp", "wb"); 
	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;
	unsigned char* bmp;
	int size;

	size = WIDTH*HEIGHT * 3;
	bmp = (unsigned char*)malloc(size);

	// 1. define bf
	bf.bfType = 0x4d42; 
	bf.bfSize = size + sizeof(bf) + sizeof(bi); // file size in bytes
	bf.bfOffBits = sizeof(bf) + sizeof(bi); // offset before raw data
	// 2. define bi
	bi.biHeight = HEIGHT;
	bi.biWidth = WIDTH;
	bi.biSize = sizeof(bi);
	bi.biBitCount = 24; // 24 = 8*R+8*G+8*B
	bi.biCompression = BI_RGB;
	// 3. define bmp
	// fill background:
	for (int v = 0; v < HEIGHT; v++)
	{
		for (int h = 0; h < WIDTH; h++)
		{
			bmp[(v*WIDTH + h) * 3] = 80; // blue
			bmp[(v*WIDTH + h) * 3 + 1] = 208; // green
			bmp[(v*WIDTH + h) * 3 + 2] = 146; // red
		}
	}
	drawHouses(*cityMap, bmp);
	drawStreets(*cityMap, bmp);
	drawCrossroads(*cityMap, bmp);
		
	// 4. save the data to file
	fwrite(&bf, sizeof(bf), 1, pf);
	fwrite(&bi, sizeof(bi), 1, pf);
	fwrite(bmp, 1, size, pf);

	fclose(pf);
	free(bmp);
}

void drawHouses(MAP cityMap, unsigned char* bmp)
{ 
	// init parameters for horizontical / vertical coordinates (start to end of each building) 
	int hs, vs, he, ve;
	HOUSE_NODE *currentNode = cityMap.houses;
	
	while (currentNode != 0)
	{
		ve = currentNode->ahouse.middleY + currentNode->ahouse.DimensionY / 2;
		he = currentNode->ahouse.middleX + currentNode->ahouse.DimensionX / 2;

		for (vs = currentNode->ahouse.middleY - currentNode->ahouse.DimensionY / 2; vs < ve; vs++)
		{
			for (hs = currentNode->ahouse.middleX - currentNode->ahouse.DimensionX / 2; hs < he; hs++)
			{
				bmp[(vs*WIDTH + hs) * 3] = currentNode->ahouse.color.b; // blue
				bmp[(vs*WIDTH + hs) * 3 + 1] = currentNode->ahouse.color.g; // green
				bmp[(vs*WIDTH + hs) * 3 + 2] = currentNode->ahouse.color.r; // red
			}
		}
		currentNode = currentNode->next;
	}
}

void drawStreets(MAP cityMap, unsigned char* bmp)
{
	// init parameters for horizontical / vertical coordinates 
	int hs, vs;
	STREET_NODE *currentNode = cityMap.streets;

	while (currentNode != 0)
	{
		if (currentNode->astreet.type == v)
		{
			int h = currentNode->astreet.startX;
			for (vs = currentNode->astreet.startY ; vs < currentNode->astreet.endY ; vs++)
			{
				for (hs = h-5 ; hs < h+5; hs++)
				{
					bmp[(vs*WIDTH + hs) * 3] = 134; // blue
					bmp[(vs*WIDTH + hs) * 3 + 1] = 134; // green
					bmp[(vs*WIDTH + hs) * 3 + 2] = 134; // red
				}
			}
		}

		if (currentNode->astreet.type == h)
		{
			int v = currentNode->astreet.startY;
			for (vs = v-5 ; vs < v+5 ; vs++)
			{
				for (hs = currentNode->astreet.startX ; hs < currentNode->astreet.endX ; hs++)
				{
					bmp[(vs*WIDTH + hs) * 3] = 134; // blue
					bmp[(vs*WIDTH + hs) * 3 + 1] = 134; // green
					bmp[(vs*WIDTH + hs) * 3 + 2] = 134; // red
				}
			}
		}
		currentNode = currentNode->next;
	}
}

void drawCrossroads(MAP cityMap, unsigned char* bmp)
{
	int v, h, left, right, bottom, top, radius1=10, radius2=6;
	double dist;
	CROSSROAD_NODE *currentNode = cityMap.crossroads;

	while (currentNode != 0)
	{
		for (v = currentNode->acrossroad.middleY - radius1; v < currentNode->acrossroad.middleY + radius1; v++)
			for (h = currentNode->acrossroad.middleX - radius1; h < currentNode->acrossroad.middleX + radius1; h++)
			{
				dist = sqrt((v - currentNode->acrossroad.middleY) * (v - currentNode->acrossroad.middleY) +
					(h - currentNode->acrossroad.middleX) * (h - currentNode->acrossroad.middleX));
				if (dist < radius1)
				{
					bmp[(v*WIDTH + h) * 3] = 0; // blue
					bmp[(v*WIDTH + h) * 3 + 1] = 255; // green
					bmp[(v*WIDTH + h) * 3 + 2] = 255; // red
				}
				if (dist < radius2)
				{
					bmp[(v*WIDTH + h) * 3] = 200; // blue
					bmp[(v*WIDTH + h) * 3 + 1] = 182; // green
					bmp[(v*WIDTH + h) * 3 + 2] = 175; // red
				}
			}
		currentNode = currentNode->next;
	}
}


//dist = sqrt((i - c.y)*(i - c.y) + (j - c.x)*(j - c.x));




//////////// ******** Free all dynamic allocated lists ******** ////////////



void  FreeStreetList(MAP *cityMap)
{
	STREET_NODE *currentNode = 0, *tmp = 0;
	currentNode = cityMap->streets;

	while (currentNode != 0)
	{
		tmp = currentNode->next;
		free(currentNode);
		currentNode = tmp;
	}
}

void  FreeHouseList(MAP *cityMap)
{
	HOUSE_NODE *currentNode = 0, *tmp = 0;
	currentNode = cityMap->houses;

	while (currentNode != 0)
	{
		tmp = currentNode->next;
		free(currentNode);
		currentNode = tmp;
	}
}

void  FreeCrossroadsList(MAP *cityMap)
{
	CROSSROAD_NODE *currentNode = 0, *tmp = 0;
	currentNode = cityMap->crossroads;

	while (currentNode != 0)
	{
		tmp = currentNode->next;
		free(currentNode);
		currentNode = tmp;
	}
}

