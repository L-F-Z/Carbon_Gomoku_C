//哈希去重相关函数

void HashClear() //Clear Hash Table
{
	hashASize = 0;
}

int HashPresent() //Get Current Status
{
	return currentItem->hashB == hashB && currentItem->hashC == hashC;
}

void HashMove(int x, int y, enum OXPiece who) //Make a Move
{
	int offset = x + (y << 6) + (who << 11);
	hashA = (hashA + hashValA[offset]) % hashASize;
	currentItem = &elem[hashA];
	hashB = (hashB + hashValB[offset]);
	hashC = (hashC + hashValC[offset]);
}

void HashUndo(int x, int y, enum OXPiece who) //Undo a Move
{
	int offset = x + (y << 6) + (who << 11);
	hashA = ((int)(hashA - hashValA[offset]) % (int)hashASize);
	if ((int)hashA < 0)
		hashA += hashASize;
	currentItem = &elem[hashA];
	hashB = (hashB - hashValB[offset]);
	hashC = (hashC - hashValC[offset]);
}

short HashValue() //Return Current Value
{
	return currentItem->value;
}

short HashDepth() //Return Current Depth
{
	return currentItem->depth;
}

short HashMoves() //Return Current Moves
{
	return currentItem->moves;
}

struct OXPoint HashBest() //Return Current Best Move
{
	return currentItem->best;
}

void HashUpdate(short _value, short _depth, short _moves, struct OXMove _best) //Update Table
{
	struct HashRec *c = currentItem;
	c->value = _value;
	c->depth = _depth;
	c->moves = _moves;
	c->hashB = hashB;
	c->hashC = hashC;
	c->best.cx = _best.mx;
	c->best.cy = _best.my;
}

void HashResize(ULONG size) //Resize Hash Table
{
	if (size > hashASize)
	{
		ULONG num;
		ULONG maxBytes = info_max_memory;
		if (maxBytes == 0)
			maxBytes = 1000000000; //1GB
		maxBytes = maximum(maxBytes, 7500000) - 7000000;
		num = maxBytes / sizeof(struct HashRec);
		if (maxSize * 2 < num || maxSize > num)
		{
			elem = (struct HashRec *)realloc(elem, maxBytes);
			maxSize = num;
		}
		if (hashASize < maxSize)
		{
			hashASize = minimum(size * 2, maxSize);
			memset(elem, 0, hashASize * sizeof(struct HashRec));
		}
	}
}

void HashInit() //Initial
{
	int n;
	maxSize = 0;
	hashASize = 0;
	elem = 0;
	for (n = 0; n < 4096; n++)
		assert(hashValA[n] < 2000000000);
}

void HashEnd() //Free the Space
{
	if (elem)
		free(elem);
}